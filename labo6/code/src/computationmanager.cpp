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
        auto it = std::find(computation[(int)itComputation->second.computationType].begin(), computation[(int)itComputation->second.computationType].end(), id);
        computation[(int)itComputation->second.computationType].erase(it);
        if(computation[(int)itComputation->second.computationType].size() + 1 == MAX_TOLERATED_QUEUE_SIZE){
            nbWaitingFull[(int)itComputation->second.computationType]--;
            signal(conditionsFull[(int)itComputation->second.computationType]);
        }
        computations.erase(itComputation);
    } else if(itResult != results.end()){
        if(itResult->first == minId){
            minId++;
            if(results.begin()->first == minId)
                signal(resultsMinId);
        }
        results.erase(itResult);
        return;
    } else if(id < nextId || id >= minId){
        // Evite d'ajouter de id invalide
        abortedId.insert(id);
    }
    monitorOut();
    checkStop();
}

Result ComputationManager::getNextResult() {
    // Replace all of the code below by your code

    // Filled with some code in order to make the thread in the UI wait
    monitorIn();
    if(results.size() == 0){
        std::cout << "Blocking empty" << std::endl;
        waitCheckStop(resultsEmpty);
    }
    if(results.begin()->first != minId){
        std::cout << "Blocking min" << std::endl;
        waitCheckStop(resultsMinId);
    }
    minId++;
    Result result = results.begin()->second;
    results.erase(results.begin());
    monitorOut();
    return result;
}

Request ComputationManager::getWork(ComputationType computationType) {
    // Filled with arbitrary code in order to make the callers wait
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
    std::cout << "We'll be wiaitng" << std::endl;
    wait(c);
    std::cout << "I'm out" << std::endl;
    checkStop();
}

void ComputationManager::checkStop(){
    if(stopped){
        std::cout << "Throwing exception" << std::endl;
        monitorOut();
        throwStopException();
    }
}

void ComputationManager::stop() {
    stopped = true;
    monitorIn();
    std::cout << "I'm stopping" << std::endl;
    signal(resultsEmpty);
    signal(resultsMinId);
    int type = 0;
    for(Condition &c : conditionsFull){
        while(nbWaitingFull[type] > 0){
            signal(c);
            nbWaitingFull[type]--;
        }
        type++;
    }
    type = 0;
    for(Condition &c : conditionsEmpty){
        while(nbWaitingEmpty[type] > 0){
            signal(c);
            nbWaitingEmpty[type]--;
        }
        type++;
    }
    monitorOut();
}
