//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Nom Prénom, Nom Prénom
//
#include "locomotivebehavior.h"
#include "ctrain_handler.h"

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
    int numeroContact = 0;

    while(1) {
        // Prend le contact qui doit être attendu sur le parcours
        int contactCourant = parcours.getContact(numeroContact);
        loco.afficherMessage(qPrintable(QString("Attente du connecteur %1").arg(numeroContact)));
        // Attend le contact
        attendre_contact(contactCourant);
        // Une fois le contact passé. Prend le prochain contact sur la liste
        numeroContact++;
        if(contactCourant == parcours.getContactRequest()){
            sharedSection->request(loco, priority);
        }
        // Si le contact courant(qui vient d'être passé) est le contact de départ de la section on effectue un getAccess
        else if(contactCourant == parcours.getContactSectionDepart()){
            sharedSection->getAccess(loco, priority);
        } else if(contactCourant == parcours.getContactSectionFin()){
            sharedSection->leave(loco);
        } else if(contactCourant == parcours.getContactDepart()){
            loco.afficherMessage("contact de départ passé");
            nbTour--;
            if(nbTour == 0){
                inverserSens();
                nbTour = 2;
            }
            numeroContact = 0;
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
