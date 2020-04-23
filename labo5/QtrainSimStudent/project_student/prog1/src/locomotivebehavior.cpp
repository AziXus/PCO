//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Nom Prénom, Nom Prénom
//
#include "locomotivebehavior.h"
#include "ctrain_handler.h"

PcoSemaphore LocomotiveBehavior::mutex = PcoSemaphore(1);

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */

    // Vous pouvez appeler les méthodes de la section partagée comme ceci :
    //sharedSection->request(loco);
    //sharedSection->getAccess(loco);
    //sharedSection->leave(loco);
    int nbTour = 2;
    int vitesse = loco.vitesse();

    while(1) {
        // On attend d'atteindre le contact de début de section

        attendreContact(contactFreinage1);
        loco.fixerVitesse(6);

        attendreContact(contactSectionDepart);
        sharedSection->getAccess(loco, SharedSectionInterface::Priority(loco.priority));
        loco.fixerVitesse(vitesse);

        // On attend d'atteindre le contact de fin de section
        attendreContact(contactSectionFin);
        sharedSection->leave(loco);

        // On attend d'atteindre le contact de départ
        attendreContact(contactDepart);
        loco.afficherMessage("contact passé");

        nbTour--;
        if(nbTour == 0){
            inverserDirection();
            std::swap(contactSectionDepart,contactSectionFin);
            std::swap(contactFreinage1, contactFreinage2);
            nbTour = 2;
        }
    }
}

void LocomotiveBehavior::inverserDirection() {
    loco.afficherMessage("Changement de direction");
    loco.arreter();
    loco.inverserSens();
    loco.demarrer();
}

void LocomotiveBehavior::attendreContact(int contact) {
    loco.afficherMessage(qPrintable(QString("Attente du connecteur %1").arg(contact)));
    attendre_contact(contact);
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
