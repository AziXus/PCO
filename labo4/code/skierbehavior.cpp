//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Robin Muller, Stéphane Teixeira Carvalho

#include "skierbehavior.h"

#include <QRandomGenerator>

constexpr unsigned int MIN_SECONDS_DELAY = 2;
constexpr unsigned int MAX_SECONDS_DELAY = 10;
constexpr unsigned int SECOND_IN_MICROSECONDS = 1000000;

int SkierBehavior::nextId = 1;

void SkierBehavior::run()
{
    while(cableCar->isInService()){
        cableCar->waitForCableCar(id);

        //Test une seconde fois car le skieur pourrait être en attente de la télécabine quand celle-ci termine son service
        if (!cableCar->isInService())
            return;

        cableCar->goIn(id);
        cableCar->waitInsideCableCar(id);
        cableCar->goOut(id);
        goDownTheMountain();
    }
    //Message de debug
    qDebug() << "Skieur " << id << " se stop";
}

void SkierBehavior::goDownTheMountain()
{
    qDebug() << "Skieur" << id << "est en train de skier et descend de la montagne";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
    qDebug() << "Skieur" << id << "est arrivé en bas de la montagne";
}
