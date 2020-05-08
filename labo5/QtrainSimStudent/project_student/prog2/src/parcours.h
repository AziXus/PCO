/**
  \file parcours.h
  \author Müller Robin, Teixeira Carvalho Stéphane
  \date 08.05.2020
  \brief Classe pour permettre de définir le parcours d'une locomotive et les différents contacts important du parcours

  Ce fichier contient la définition de la classe Parcours, qui permet de spécifier le parcours d'une locomotive
  avec ses contacts importants
*/
#ifndef PARCOURS_H
#define PARCOURS_H
#include <vector>
class Parcours
{
public:
    /**
     * \brief Parcours Constructeur de la classe Parcours permettant de définir le parcours et ses contacts
     * \param parcours vecteur de int représentant le parcours complet de la locomotive.
     * \param contactDepart int étant le contact de départ du parcours
     * \param contactSectionDepart int étant le contact indiquant le début de la section partagée
     * \param contactSectionFin int étant le contact indiquant la fin de la section partagée
     * \param contactSectionDepartInverse int étant le contact indiquant le début de la section partagée lorsque le train inverse de sens
     * \param contactSectionFinInverse int étant le contact indiquant la fin de la section partagée lorsque le train inverse de sens
     * \param contactRequest int étant le contact où la fonction getRequest de la section partagée doit être appelée
     * \param contactRequestInverse int étant le contact d'appel à la fonction getRequest de la section partagée mais lorsque le train change de sens
     */
    Parcours(std::vector<int> parcours, int contactDepart, int contactSectionDepart, int contactSectionFin,
             int contactSectionDepartInverse, int contactSectionFinInverse, int contactRequest, int contactRequestInverse);

    /**
     * \brief getContactRequest Renvoie la valeur du contact où request doit être appelée
     * \return  un int étant le contact où effetuer l'appel de request
     */
    int getContactRequest() const;

    /**
     * \brief getContactDepart Renvoie la valeur du contact de départ du circuit
     * \return un int étant le contact de départ
     */
    int getContactDepart() const;

    /**
     * \brief getContactSectionDepart Renvoie la valeur du contact indiquant le début de la section partagée
     * \return un int étant le contact de départ de la section partagée
     */
    int getContactSectionDepart() const;

    /**
     * \brief getContactSectionFin Renvoie la valeur du contact de fin de section critique
     * \return un int étant le contact de fin de section partagée
     */
    int getContactSectionFin() const;

    /**
     * @brief inverserSens Permet d'inverser le sens du parcours ainsi que mettre à jour les différents contacts suite à l'inversement
     */
    void inverserSens();

    /**
     * @brief getContact Permet d'avoir un contact du parcours selon son numéro sur le parcours
     * @param numeroContact int étant le numéro du contact désiré
     * @return un int étant le contact à la position du numero de contact donnée
     */
    int getContact(int numeroContact) const;

private:
    std::vector<int> parcours;
    int contactDepart;
    int contactSectionDepart;
    int contactSectionFin;
    int contactSectionDepartInverse;
    int contactSectionFinInverse;
    int contactRequest;
    int contactRequestInverse;
};

#endif // PARCOURS_H
