//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Müller Robin, Teixeira Carvalho Stéphane
//
#include "locomotivebehavior.h"
#include "ctrain_handler.h"

#define NB_TOURS 2

bool LocomotiveBehavior::stop = false;

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */

    // Nombre de tour que la locomotive doit effectuée avant d'inverser de sens
    int nbTour = NB_TOURS;
    int numeroContact = 0;

    while(1) {
        if (stop){
            loco.afficherMessage("Je m'arrête");
            return;
        }

        // Prend le contact qui doit être attendu sur le parcours
        int contactCourant = parcours.getContact(numeroContact);

        // Attend le contact courant
        attendreContact(contactCourant);

        if (stop){
            loco.afficherMessage("Je m'arrête");
            return;
        }

        // Une fois le contact passé. Augmente la valeur du numero de contact pour prendre le prochain contact sur la liste
        numeroContact++;

        // Si le contact courant(qui vient d'être passé) est le contact de départ de la section on effectue un getAccess
        if(contactCourant == parcours.getContactSectionDepart()){
            sharedSection->getAccess(loco, SharedSectionInterface::Priority(loco.priority));
        } else if(contactCourant == parcours.getContactSectionFin()){ // Si on sort de la section, on effectue un leave
            sharedSection->leave(loco);
        } else if(contactCourant == parcours.getContactDepart()){ // Si on atteint le contact de départ
            loco.afficherMessage("Contact de départ passé");
            nbTour--;
            // Check si le nombre de tour est égal à 0 pour savoir si les 2 tours ont été effectués
            if(nbTour == 0){
                inverserSens();
                nbTour = NB_TOURS;
            }
            // Réinitialiser le numero de contact car un nouveau tour débute
            numeroContact = 0;
        }
    }
}

void LocomotiveBehavior::inverserSens(){
    loco.afficherMessage("Changement de direction");
    loco.arreter();
    // Inverse le sens de la locomotive et du parcours
    loco.inverserSens();
    parcours.inverserSens();
    loco.demarrer();
}

void LocomotiveBehavior::attendreContact(int numeroContact) {
    loco.afficherMessage(qPrintable(QString("Attente du connecteur %1").arg(numeroContact)));
    attendre_contact(numeroContact);
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
