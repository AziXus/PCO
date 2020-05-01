#include "parcours.h"
#include <algorithm>

Parcours::Parcours(std::vector<int> parcours, int contactDepart, int contactSectionDepart, int contactSectionFin, int contactSectionDepartInverse, int contactSectionFinInverse)
{
    this->parcours                    = parcours;
    this->contactDepart               = contactDepart;
    this->contactSectionDepart        = contactSectionDepart;
    this->contactSectionFin           = contactSectionFin;
    this->contactSectionDepartInverse = contactSectionDepartInverse;
    this->contactSectionFinInverse    = contactSectionFinInverse;
}

int Parcours::getContactSectionDepart() const
{
    return contactSectionDepart;
}

int Parcours::getContactDepart() const
{
    return contactDepart;
}

int Parcours::getContactSectionFin() const
{
    return contactSectionFin;
}

int Parcours::getContactSectionDepartInverse() const
{
    return contactSectionDepartInverse;
}

int Parcours::getContactSectionFinInverse() const
{
    return contactSectionFinInverse;
}

std::vector<int> Parcours::getParcours() const
{
    return parcours;
}

void Parcours::inverserSens(){
    std::reverse(parcours.begin(), parcours.end() - 1);
    std::swap(contactSectionDepart,contactSectionDepartInverse);
    std::swap(contactSectionFin,contactSectionFinInverse);
}

int Parcours::getContact(int numeroContact) const{
    return parcours.at(numeroContact);
}
