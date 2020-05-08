//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Müller Robin, Teixeira Carvalho Stéphane
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

    // Nombre de tour que la locomotive doit effectuée avant d'inverser de sens
    int nbTour = 2;
    int numeroContact = 0;

    while(1) {
        if (PcoThread::thisThread()->stopRequested())
            return;

        // Prend le contact qui doit être attendu sur le parcours
        int contactCourant = parcours.getContact(numeroContact);
        loco.afficherMessage(qPrintable(QString("Attente du connecteur %1").arg(numeroContact)));

        // Attend le contact courant
        attendre_contact(contactCourant);
        // Une fois le contact passé. Augmente la valeur du numero de contact pour prendre le prochain contact sur la liste
        numeroContact++;
        // Si le contact courant(qui vient d'être passé) est le contact ou getRequest doit être effectuée on effectue un request
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
            // Check si le nombre de tour est égal à 0 pour savoir si les 2 tours ont été effectués
            if(nbTour == 0){
                inverserSens();
                nbTour = 2;
            }
            // Réinitialiser le numero de contact car un nouveau tour débute
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
