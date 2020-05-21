/**
  \file parcours.cpp
  \author Müller Robin, Teixeira Carvalho Stéphane
  \date 08.05.2020
  \brief Classe pour permettre de définir le parcours d'une locomotive et les différents contacts important du parcours
*/
#include "parcours.h"
#include <algorithm>

Parcours::Parcours(std::vector<int> parcours, int contactDepart, int contactSectionDepart, int contactSectionFin,
                   int contactSectionDepartInverse, int contactSectionFinInverse, int contactRequest, int contactRequestInverse)
{
    this->parcours                    = parcours;
    this->contactDepart               = contactDepart;
    this->contactSectionDepart        = contactSectionDepart;
    this->contactSectionFin           = contactSectionFin;
    this->contactSectionDepartInverse = contactSectionDepartInverse;
    this->contactSectionFinInverse    = contactSectionFinInverse;
    this->contactRequest              = contactRequest;
    this->contactRequestInverse       = contactRequestInverse;
}

int Parcours::getContactRequest() const
{
    return contactRequest;
}

int Parcours::getContactDepart() const
{
    return contactDepart;
}

int Parcours::getContactSectionDepart() const
{
    return contactSectionDepart;
}

int Parcours::getContactSectionFin() const
{
    return contactSectionFin;
}

void Parcours::inverserSens()
{
    // Inverse toute les valeurs du vecteur sauf le dernier contact qui est le contact de départ du parcours
    std::reverse(parcours.begin(), parcours.end() - 1);
    // Swap les valeurs des contacts entre les contacts du parcours en sens inverse et normal
    std::swap(contactSectionDepart,contactSectionDepartInverse);
    std::swap(contactSectionFin,contactSectionFinInverse);
    std::swap(contactRequest, contactRequestInverse);
}

int Parcours::getContact(int numeroContact) const
{
    return parcours.at(numeroContact);
}
