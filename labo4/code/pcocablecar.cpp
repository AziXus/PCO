//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Prénom Nom, Prénom Nom

#include "pcocablecar.h"
#include <pcosynchro/pcothread.h>

#include <QDebug>
#include <QRandomGenerator>
constexpr unsigned int MIN_SECONDS_DELAY = 1;
constexpr unsigned int MAX_SECONDS_DELAY = 5;
constexpr unsigned int SECOND_IN_MICROSECONDS = 1000000;

// A vous de remplir les méthodes ci-dessous

PcoCableCar::PcoCableCar(const unsigned int capacity) : capacity(capacity), cableCarLoad(), cableCarUnload()
{

}

PcoCableCar::~PcoCableCar()
{

}

void PcoCableCar::waitForCableCar(int id)
{
    mutex.acquire();
    nbSkiersWaiting++;
    mutex.release();
    qDebug() << "waitForCableCar(" << id << ")";
    cableCarLoad.acquire();
}

void PcoCableCar::waitInsideCableCar(int id)
{
    cableCarUnload.acquire();
}

void PcoCableCar::goIn(int id)
{
    qDebug() << "goIn(" << id << ")";
    skieurInside.release();
}

void PcoCableCar::goOut(int id)
{
    skieurOutside.release();
}

bool PcoCableCar::isInService()
{
    return inService;
}

void PcoCableCar::endService()
{
    qDebug() << "Arret du service";

    inService = false;

    while (nbSkiersWaiting != 0) {
        cableCarLoad.release();
        nbSkiersWaiting--;
    }
}

void PcoCableCar::goUp()
{
    qDebug() << "Le télécabine monte";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::goDown()
{
    qDebug() << "Le télécabine descend";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::loadSkiers()
{
    bool plein = false;
    while(nbSkiersWaiting != 0 && !plein){
        mutex.acquire();
        if(nbSkiersInside < capacity){
            cableCarLoad.release();
            nbSkiersInside++;
            nbSkiersWaiting--;
        }else{
            plein = true;
        }
        mutex.release();
    }
    for (int i = 0; i < nbSkiersInside; ++i)
        skieurInside.acquire();
}

void PcoCableCar::unloadSkiers()
{
    while(nbSkiersInside != 0){
        cableCarUnload.release();
        skieurOutside.acquire();
        nbSkiersInside--;
    }
}
