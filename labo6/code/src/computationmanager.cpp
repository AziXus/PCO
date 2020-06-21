//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Müller Robin, Teixeira Carvalho Stéphane


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include "computationmanager.h"
#include <iostream>
#include <algorithm>


ComputationManager::ComputationManager(int maxQueueSize): MAX_TOLERATED_QUEUE_SIZE(maxQueueSize), conditionsEmpty(3), conditionsFull(3)
                                        , computation(3), nbWaitingFull(3), nbWaitingEmpty(3) {
    minId = 0;
}

int ComputationManager::requestComputation(Computation c) {
    monitorIn();
    checkStop();
    // Stock le type de computation en int afin de simplifier le code
    int cType = (int)c.computationType;
    // Si le nombre de compuation Max est égal au max autorisé pour le type donné une attente est effetuée
    if(computation[cType].size() == MAX_TOLERATED_QUEUE_SIZE){
      // Augmentation du nombre de computation en attente
        nbWaitingFull[cType]++;
        // Vérification que le programme n'est pas stoppé
        waitCheckStop(conditionsFull[cType]);
    }
    // Passe au prochain id
    int id = nextId++;
    // Ajout de la paire id->computaion dans la map
    computations.insert(std::make_pair(id, c));

    // Ajoute l'id de la computation dans la deque
    computation[cType].insert(id);

    // Si un computeEngine était en attente on le signal qu'une computation est disponible
    if(nbWaitingEmpty[cType] > 0){
        signal(conditionsEmpty[cType]);
    }
    monitorOut();
    return id;
}

void ComputationManager::abortComputation(int id) {
    monitorIn();
    // Effacer de la liste des computations
    auto itComputation = computations.find(id);
    // Effacer de la liste des résultat
    auto itResult = results.find(id);

    // Si l'id a été trouvé dans la liste des computations
    if(itComputation != computations.end()){
        int cType = (int)itComputation->second.computationType;
        // Trouve l'id a supprimer du set
        auto it = std::find(computation[cType].begin(), computation[cType].end(), id);
        // Efface la computation trouvée de la map et du set
        computations.erase(itComputation);
        computation[cType].erase(it);
        // Si un thread était en attente, on le libère
        if(nbWaitingFull[cType] > 0){
            nbWaitingFull[cType]--;
            signal(conditionsFull[cType]);
        }
    // Si l'id a été trouvé dans la liste des résultats
    } else if(itResult != results.end()){
        if(itResult->first == minId){ // Si on supprime le minId, on l'incrémente et on libère un thread
            minId++;
            signal(resultsMinId);
        }
        results.erase(itResult);
    }

    // On ajoute uniquement ids valide
    if(id < nextId || id >= minId){
        abortedId.insert(id);
    }

    monitorOut();
}

Result ComputationManager::getNextResult() {
    monitorIn();
    // Si le taille de la map result est égal à 0 cela signifie qu'elle est vide donc attente
    if(results.size() == 0){
        waitCheckStop(resultsEmpty);
    }

    // Supprime les id abortés et augmente minId
    while (abortedId.size() > 0 && *abortedId.begin() == minId) {
        abortedId.erase(abortedId.begin());
        minId++;
    }

    // Si l'id qui se trouve en premier sur la map n'est pas équivalent à l'id attendu on attend
    // Nous pouvons vérifier avec .begin car les id seront triés par ordre croissant
    if(results.begin()->first != minId){
        waitCheckStop(resultsMinId);
    }
    // Suppression du résultat dans la map
    Result result = results.begin()->second;
    results.erase(results.begin());
    // Incrémentation de minId pour prendre le prochain Id
    minId++;
    while(abortedId.find(minId) != abortedId.end()){
        abortedId.erase(abortedId.find(minId));
        minId++;
    }
    std::cout << minId << std::endl;
    monitorOut();
    return result;
}

Request ComputationManager::getWork(ComputationType computationType) {
    monitorIn();
    int id;
    // Stock le type de computation en int afin de simplifier le code
    int cType = (int)computationType;
    // Si la liste des computations est vide on attend
    if(computation[cType].size() == 0){
        std::cout << "Waiting because empty " << cType << std::endl;
        nbWaitingEmpty[cType]++;
        waitCheckStop(conditionsEmpty[cType]);
    }
    // Prend le premier élément du set
    auto it = computation[cType].begin();
    id = *it;
    // Efface la computation
    computation[cType].erase(it);
    auto itComputation = computations.find(id);
    // Ajoute la computation dans la liste des requêtes
    Request request = Request(itComputation->second, id);
    // Efface la computation de la map car c'est maintenant une requête
    computations.erase(itComputation);
    // Envoie un signal si une computation est en attente car le buffer est plein
    if (nbWaitingFull[cType] > 0) {
        nbWaitingFull[cType]--;
        signal(conditionsFull[cType]);
    }
    std::cout << "Going out " << cType << std::endl;
    monitorOut();
    return request;
}

bool ComputationManager::continueWork(int id) {
    bool work;
    monitorIn();
    work = abortedId.find(id) == abortedId.end() && !stopped;
    monitorOut();
    return work;
}

void ComputationManager::provideResult(Result result) {
    monitorIn();
    results.insert(std::make_pair(result.getId(), result));
    // A cause du fait que le thread reveille prend la main
    if(results.size() == 1){
        signal(resultsEmpty);
    }
    // Si l'id du résultat reçu est bien celui attendu on le signal
    if(results.begin()->first == minId){
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
