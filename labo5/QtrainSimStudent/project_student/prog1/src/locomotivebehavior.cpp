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

    while(1) {
        attendre_contact(contactSectionDepart);
        sharedSection->getAccess(loco, SharedSectionInterface::Priority(loco.priority));
        attendre_contact(contactSectionFin);
        sharedSection->leave(loco);
        attendre_contact(contactDepart);
        loco.afficherMessage("contact passé");
        nbTour--;
        if(nbTour == 0){
            loco.arreter();
            loco.inverserSens();
            loco.demarrer();
            // Attend que la locomtive repart
            attendre_contact(contactDepart);
            std::swap(contactSectionDepart,contactSectionFin);
            nbTour = 2;

        }
    }
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
