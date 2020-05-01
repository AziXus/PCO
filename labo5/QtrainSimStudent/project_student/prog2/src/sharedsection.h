//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Nom Prénom, Nom Prénom
//
#ifndef SHAREDSECTION_H
#define SHAREDSECTION_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "sharedsectioninterface.h"

/**
 * @brief La classe SharedSection implémente l'interface SharedSectionInterface qui
 * propose les méthodes liées à la section partagée.
 */
class SharedSection final : public SharedSectionInterface
{
public:

    /**
     * @brief SharedSection Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    SharedSection() : aiguillageDebut(9), aiguillageFin(2), numeroPrioLoco(-1), wait(0), isInSection(false) {
        priorityLoco = SharedSectionInterface::Priority(0);
    }

    /**
     * @brief request Méthode a appeler pour indiquer que la locomotive désire accéder à la
     * section partagée (deux contacts avant la section partagée).
     * @param loco La locomotive qui désire accéder
     * @param priority La priorité de la locomotive qui fait l'appel
     */
    void request(Locomotive& loco, Priority priority) override {
        mutex.acquire();
        if(numeroPrioLoco == -1){
            numeroPrioLoco = loco.numero();
            priorityLoco = priority;
        } else if(priorityLoco < priority){
            numeroPrioLoco = loco.numero();
        }
        mutex.release();

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 requested the shared section.").arg(loco.numero())));
    }

    /**
     * @brief getAccess Méthode à appeler pour accéder à la section partagée, doit arrêter la
     * locomotive et mettre son thread en attente si la section est occupée ou va être occupée
     * par une locomotive de plus haute priorité. Si la locomotive et son thread ont été mis en
     * attente, le thread doit être reveillé lorsque la section partagée est à nouveau libre et
     * la locomotive redémarée. (méthode à appeler un contact avant la section partagée).
     * @param loco La locomotive qui essaie accéder à la section partagée
     * @param priority La priorité de la locomotive qui fait l'appel
     */
    void getAccess(Locomotive &loco, Priority priority) override {
        loco.afficherMessage("Début de getAccess");

        mutex.acquire();
        if(isInSection || numeroPrioLoco != loco.numero()){
            loco.afficherMessage("La section est occupé... Arrêt de la loco.");
            loco.arreter();
            nbTrainWaiting++;
            mutex.release();

            loco.afficherMessage("Attente avant d'accéder à la section.");
            wait.acquire();

            loco.afficherMessage("Section libre... Redémarrage de la loco");
            loco.demarrer();
            mutex.acquire();
            nbTrainWaiting--;
        }
        isInSection = true;
        mutex.release();

        loco.afficherMessage("Aiguillage en cours");
        if(loco.numero() == 42){
            diriger_aiguillage(aiguillageDebut, TOUT_DROIT, 0);
            diriger_aiguillage(aiguillageFin, TOUT_DROIT, 0);
        } else if(loco.numero() == 7){
            diriger_aiguillage(aiguillageDebut, DEVIE, 0);
            diriger_aiguillage(aiguillageFin, DEVIE, 0);
        }

        // Exemple de message dans la console globale
        loco.afficherMessage("La loco a accéder à la section");
        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section
     * partagée. (reveille les threads des locomotives potentiellement en attente).
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive& loco) override {
        loco.afficherMessage("La loco sort de la section");

        mutex.acquire();
        isInSection = false;
        if(nbTrainWaiting > 0){
            afficher_message("Une autre loco peut partir");
            wait.release();
        }
        if(loco.numero() == numeroPrioLoco){
            numeroPrioLoco = -1;
        }
        mutex.release();

        loco.afficherMessage("La section est libre");
        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
    }

    /* A vous d'ajouter ce qu'il vous faut */

private:
    // Méthodes privées ...
    int aiguillageDebut;
    int aiguillageFin;
    Priority priorityLoco;
    int numeroPrioLoco;
    PcoSemaphore wait;
    PcoSemaphore mutex = PcoSemaphore(1);
    bool isInSection;
    int nbTrainWaiting = 0;
};


#endif // SHAREDSECTION_H
