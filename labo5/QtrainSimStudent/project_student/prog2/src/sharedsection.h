//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Müller Robin, Teixeira Carvalho Stéphane
//
#ifndef SHAREDSECTION_H
#define SHAREDSECTION_H

#include <QDebug>
#include <queue>

#include <pcosynchro/pcosemaphore.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "sharedsectioninterface.h"
#define PRIOUNDEFINED -1

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
    SharedSection() : accesPrioritaire(false), sectionOccupee(false),
        locoEnAttente(false), wait(0) {
    }

    /**
     * @brief request Méthode a appeler pour indiquer que la locomotive désire accéder à la
     * section partagée (deux contacts avant la section partagée).
     * @param loco La locomotive qui désire accéder
     * @param priority La priorité de la locomotive qui fait l'appel
     */
    void request(Locomotive& loco, Priority priority) override {
        afficher_message(qPrintable(QString("La loco no. %1 demande la section partagée.").arg(loco.numero())));
        loco.afficherMessage("Début de request...");

        // Une seule locomotive à la fois peut faire un request
        mutex.acquire();

        // Informe qu'un accès prioritaire est nécessaire.
        if (priority == Priority::HighPriority)
            accesPrioritaire = true;

        mutex.release();

        loco.afficherMessage("Fin de request.");
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
        afficher_message(qPrintable(QString("La loco no. %1 tente d'accéder à la section partagée.").arg(loco.numero())));
        loco.afficherMessage("Début de getAccess...");

        mutex.acquire();

        // On attend si une locomotive est dans la section partagée ou qu'une
        // demande d'accès prioritaire a été faite alors que cette loco n'a
        // pas une priorité haute.
        if (sectionOccupee || (accesPrioritaire && priority != Priority::HighPriority)) {
            loco.afficherMessage("La section est occupé... Arrêt de la loco.");
            loco.arreter();
            locoEnAttente = true;
            mutex.release();

            wait.acquire(); // Mutex transmis

            // Sort de la fonction si un arrêt est demandé
            if (stop) {
                mutex.release();
                return;
            }

            loco.afficherMessage("Redémarrage de la loco.");
            loco.demarrer();
            locoEnAttente = false;
        }

        // On a maintenant obtenu l'accès, on occupe donc la section.
        sectionOccupee = true;

        // Si on est la locomotive prioritaire, on met la variable à false.
        if (priority == Priority::HighPriority)
            accesPrioritaire = false;

        mutex.release();

        dirigerAiguillage(loco.numero());

        loco.afficherMessage("La loco a accéder à la section");
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section
     * partagée. (reveille les threads des locomotives potentiellement en attente).
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive& loco) override {
        afficher_message(qPrintable(QString("La loco no. %1 quitte la section partagée.").arg(loco.numero())));
        loco.afficherMessage("Début de leave...");

        mutex.acquire();
        // La locomotive sort de la section partagée
        sectionOccupee = false;

        // Si une locomotive attendait on la relâche
        if(locoEnAttente){
            afficher_message("La loco en attente peut démarrer");
            wait.release();
        } else {
            mutex.release();
        }

        loco.afficherMessage("La section est libre");
        loco.afficherMessage("Fin de leave...");
    }

    /* A vous d'ajouter ce qu'il vous faut */
    /**
     * @brief stopShared permet d'arrêter le fonctionnement de la section partagée
     */
    void static stopShared(){
        stop = true;
    }

private:
    // Méthodes privées ...

    /**
     * @brief dirigerAiguillage permet de mettre en place l'aiguillage en fonction du numéro de locomotive passé en paramètre
     * @param numeroLoco entier étant le numéro de la locomotive
     */
    void dirigerAiguillage(int numeroLoco) {
        afficher_message(qPrintable(QString("Aiguillage de la section pour la loco %1").arg(numeroLoco)));

        // Selon l'id de la loco modification de l'aiguillage dans une certaine direction
        if(numeroLoco == 42){
            diriger_aiguillage(9, TOUT_DROIT, 0);
            diriger_aiguillage(2, TOUT_DROIT, 0);
        } else if(numeroLoco == 7){
            diriger_aiguillage(9, DEVIE, 0);
            diriger_aiguillage(2, DEVIE, 0);
        }
    }

    // Permet de savoir si une locomtive avec un accès prioritaire a fait une demande
    bool accesPrioritaire;
    // Permet de savoir si la section est occupée
    bool sectionOccupee;
    // Permet de savoir si une locomtive est en attente de la libération du tronçon
    bool locoEnAttente;

    // Permet à une locomotive d'attendre la libération de la section partagée
    PcoSemaphore wait;
    // Verrou permettant d'éviter d'avoir des problèmes de concurrences sur les variables booléene
    PcoSemaphore mutex = PcoSemaphore(1);

    // Permet d'indiquer à la section partagée de d'arrêter
    static bool stop;
};

// Initialisation des variables statique
bool SharedSection::stop = false;

#endif // SHAREDSECTION_H
