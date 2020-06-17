//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Prénom Nom, Prénom Nom


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include "computationmanager.h"
#include <iostream>
#include <algorithm>


ComputationManager::ComputationManager(int maxQueueSize): MAX_TOLERATED_QUEUE_SIZE(maxQueueSize), conditionsEmpty(3), conditionsFull(3), computation(3), nbWaitingFull(3), nbWaitingEmpty(3) {
    minId = 0;
}

int ComputationManager::requestComputation(Computation c) {
    monitorIn();
    while(computation[(int)c.computationType].size() == MAX_TOLERATED_QUEUE_SIZE){
        nbWaitingFull[(int)c.computationType]++;
        waitCheckStop(conditionsFull[(int)c.computationType]);
    }
    int id = nextId++;
    computations.insert(std::make_pair(id, c));
    computation[(int)c.computationType].insert(id);
    if(nbWaitingEmpty[(int)c.computationType] > 0){
        signal(conditionsEmpty[(int)c.computationType]);
    }
    monitorOut();
    checkStop();
    return id;
}

void ComputationManager::abortComputation(int id) {
    monitorIn();
    // Effacer de la liste des computations
    auto itComputation = computations.find(id);
    // Effacer de la liste des résultat
    auto itResult = results.find(id);

    if(itComputation != computations.end()){
        int cType = (int)itComputation->second.computationType;
        auto it = std::find(computation[cType].begin(), computation[cType].end(), id);
        // Remove computation
        computations.erase(itComputation);
        computation[cType].erase(it);
        // Si une thread était en attente, on la libère
        if(nbWaitingFull[cType] > 0){
            nbWaitingFull[cType]--;
            signal(conditionsFull[cType]);
        }
    } else if(itResult != results.end()){
        if(itResult->first == minId){
            minId++;
            if(results.begin()->first == minId)
                signal(resultsMinId);
        }
        results.erase(itResult);
    } else if(id < nextId || id >= minId){
        // Evite d'ajouter de id invalide
        abortedId.insert(id);
    }
    monitorOut();
    checkStop();
}

Result ComputationManager::getNextResult() {
    monitorIn();
    // Si le taille de la map result est égal à 0 cela signifie qu'elle est vide donc attente
    if(results.size() == 0){
        std::cout << "Blocking empty" << std::endl;
        waitCheckStop(resultsEmpty);
    }
    // Si l'id qui se trouve en premier sur la map n'est pas équiavalent à l'id attendu on attend
    // Nous pouvons vérifier avec .begin car les id serotn triés par ordre croissant
    if(results.begin()->first != minId){
        std::cout << "Blocking min" << std::endl;
        waitCheckStop(resultsMinId);
    }
    // Incrémentation de minId pour prendre le prochain Id
    minId++;
    // Suppression du résultat dans la map
    Result result = results.begin()->second;
    results.erase(results.begin());
    monitorOut();
    return result;
}

Request ComputationManager::getWork(ComputationType computationType) {
    monitorIn();
    int id;
    if(computation[(int)computationType].size() == 0){
        std::cout << "Waiting because empty " << (int)computationType << std::endl;
        nbWaitingEmpty[(int)computationType]++;
        waitCheckStop(conditionsEmpty[(int)computationType]);
    }
    auto it = computation[(int)computationType].begin();
    id = *it;
    computation[(int)computationType].erase(it);
    auto itComputation = computations.find(id);
    Request request = Request(itComputation->second, id);
    computations.erase(itComputation);
    nbWaitingFull[(int)computationType]--;
    signal(conditionsFull[(int)computationType]);
    std::cout << "Going out " << (int)computationType << std::endl;
    monitorOut();
    return request;
}

bool ComputationManager::continueWork(int id) {
    bool work;
    monitorIn();
    work = abortedId.find(id) == abortedId.end() && !stopped;
    // Efface l'id a abort si celui ci est présent dans la liste
    if(abortedId.find(id) != abortedId.end()){
        abortedId.erase(abortedId.find(id));
    }
    monitorOut();
    return work;
}

void ComputationManager::provideResult(Result result) {
    monitorIn();
    std::cout << "Providing result" << std::endl;
    results.insert(std::make_pair(result.getId(), result));
    // A cause du fait que le thread reveille prend la main
    if(results.size() == 1){
        std::cout << "Sending empty" << std::endl;
        signal(resultsEmpty);
    }
    if(results.begin()->first == minId){
        std::cout << "Sending min" << std::endl;
        signal(resultsMinId);
    }
    monitorOut();
}

void ComputationManager::waitCheckStop(Condition &c){
    checkStop();
    wait(c);
    checkStop();
}

void ComputationManager::checkStop(){
    // Si le programme a été stoppé quitte le moniteur et renvoie une exception
    if(stopped){
        monitorOut();
        throwStopException();
    }
}

void ComputationManager::stop() {
    stopped = true;
    monitorIn();
    // Envoie un signal pour un thread qui est potentiellement bloquer sur resultsEmpty ou resultsMinId
    signal(resultsEmpty);
    signal(resultsMinId);
    // Initialisation d'un entier permettant de parcourir tout les type de computation possible
    int type = 0;
    for(Condition &c : conditionsFull){
        // Envoie un signal a tout les threads en attente sur la condition conditionFull
        while(nbWaitingFull[type] > 0){
            signal(c);
            nbWaitingFull[type]--;
        }
        // Passe au type suivant
        type++;
    }
    type = 0;
    for(Condition &c : conditionsEmpty){
        // Envoie un signal a tout les threads en attente sur la condition conditionEmpty
        while(nbWaitingEmpty[type] > 0){
            signal(c);
            nbWaitingEmpty[type]--;
        }
        type++;
    }
    monitorOut();
}
