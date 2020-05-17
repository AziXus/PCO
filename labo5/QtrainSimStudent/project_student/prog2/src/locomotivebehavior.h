//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Müller Robin, Teixeira Carvalho Stéphane
//
#ifndef LOCOMOTIVEBEHAVIOR_H
#define LOCOMOTIVEBEHAVIOR_H

#include "locomotive.h"
#include "launchable.h"
#include "parcours.h"
#include "sharedsectioninterface.h"

#include <pcosynchro/pcosemaphore.h>

/**
 * @brief La classe LocomotiveBehavior représente le comportement d'une locomotive
 */
class LocomotiveBehavior : public Launchable
{
public:
    /*!
     * \brief locomotiveBehavior Constructeur de la classe
     * \param loco la locomotive dont on représente le comportement
     * \param sharedSection la section partagée entre les locomotives
     * \param parcours objet Parcours étant le parcours effectuée par la locomotive
     * \param priority priorité de la locomotive
     */
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SharedSectionInterface> sharedSection, Parcours parcours, SharedSectionInterface::Priority priority)
                       : loco(loco), sharedSection(sharedSection), parcours(parcours), priority(priority) {
        // Eventuel code supplémentaire du constructeur
    }

protected:
    /*!
     * \brief run Fonction lancée par le thread, représente le comportement de la locomotive
     */
    void run() override;

    /*!
     * \brief printStartMessage Message affiché lors du démarrage du thread
     */
    void printStartMessage() override;

    /*!
     * \brief printCompletionMessage Message affiché lorsque le thread a terminé
     */
    void printCompletionMessage() override;

    /**
     * @brief loco La locomotive dont on représente le comportement
     */
    Locomotive& loco;

    /**
     * @brief sharedSection Pointeur sur la section partagée
     */
    std::shared_ptr<SharedSectionInterface> sharedSection;

    /*
     * Vous êtes libres d'ajouter des méthodes ou attributs
     *
     * Par exemple la priorité ou le parcours
     */
public:
    void static stopAllBehaviors(){
        stop = true;
    }

protected:
    void attendreContact(int numero);

    /**
     * @brief inverserSens Permet à la locomotive d'inverser de sens
     */
    void inverserSens();

    Parcours parcours;
    SharedSectionInterface::Priority priority;
    bool static stop;
};

#endif // LOCOMOTIVEBEHAVIOR_H
