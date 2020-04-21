//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Robin Muller, Stéphane Teixeira Carvalho

#include "pcocablecar.h"
#include <pcosynchro/pcothread.h>

#include <QDebug>
#include <QRandomGenerator>
constexpr unsigned int MIN_SECONDS_DELAY = 1;
constexpr unsigned int MAX_SECONDS_DELAY = 5;
constexpr unsigned int SECOND_IN_MICROSECONDS = 1000000;

PcoCableCar::PcoCableCar(const unsigned int capacity) : capacity(capacity), cableCarLoad(0), cableCarUnload(0)
{

}

PcoCableCar::~PcoCableCar()
{

}

void PcoCableCar::waitForCableCar(int id)
{
    // Incrémentation du nombre de skieur qui attendent la télécabine
    mutex.acquire();
    nbSkiersWaiting++;
    mutex.release();
    qDebug() << "waitForCableCar(" << id << ")";
    // Bloque le thread et attend que la télécabine autorise le skieur à monter
    // Grâce à la FIFO implémeneter dans les sémaphores ils attenderont dans l'ordre d'arrivée
    cableCarLoad.acquire();
}

void PcoCableCar::waitInsideCableCar(int id)
{
    qDebug() << "waitInsideCableCar(" << id << ")";
    // Attend que la télécabine donne le signal que le skieur peut descendre
    cableCarUnload.acquire();
}

void PcoCableCar::goIn(int id)
{
    // Donne un signal à la télécabine que le skieur à fini de monter dans celle-ci
    skieurInside.release();
    qDebug() << "goIn(" << id << ")";
}

void PcoCableCar::goOut(int id)
{
    // Donne un signal à la télécabine que le skieur à fini de descendre dans celle-ci
    skieurOutside.release();
    qDebug() << "goOut(" << id << ")";
}

bool PcoCableCar::isInService()
{
    return inService;
}

void PcoCableCar::endService()
{
    qDebug() << "Arret du service";

    inService = false;

    // On libère tous les skieurs en attente
    while (nbSkiersWaiting != 0) {
        cableCarLoad.release();
        mutex.acquire();
        nbSkiersWaiting--;
        mutex.release();
    }
}

void PcoCableCar::goUp()
{
    qDebug() << "Le télécabine monte";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
    qDebug() << "Le télécabine atteint le sommet";
}

void PcoCableCar::goDown()
{
    qDebug() << "Le télécabine descend";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
    qDebug() << "Le télécabine atteint le bas";
}

void PcoCableCar::loadSkiers()
{
    // Calcul du nombre de skieurs à charger
    mutex.acquire();
    unsigned nbToLoad = std::min(nbSkiersWaiting, capacity);
    mutex.release();

    // On laisse les skieurs entrer dans la télécabine
    for (unsigned i = 0; i < nbToLoad; ++i) {
        cableCarLoad.release();
        mutex.acquire();
        --nbSkiersWaiting;
        mutex.release();
    }
    
    // On attend que tous les skieurs entrent dans la télécabine
    for (unsigned i = 0; i < nbToLoad; ++i) {
        skieurInside.acquire();
        ++nbSkiersInside; // nbSkiersInside est uniquement utilisé par un thread
    }
}

void PcoCableCar::unloadSkiers()
{
    // Release des skieurs à l'intérieur de la télécabine
    for (unsigned i = 0; i < nbSkiersInside; ++i) {
        cableCarUnload.release();
    }

    // On attend que les skieurs sortent de la télécabine
    while (nbSkiersInside > 0) {
        skieurOutside.acquire();
        nbSkiersInside--;
    }
}
