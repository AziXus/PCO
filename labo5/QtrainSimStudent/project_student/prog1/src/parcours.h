#ifndef PARCOURS_H
#define PARCOURS_H
#include <vector>
//parcours = {22, 21, 20, 19, 13, 15, 14, 7, 6, 1, 31, 30, 29};
//this->contactSectionDepart = 19;
//this->contactSectionFin = 1;
//this->contactSectionDepartInverse = 31;
//this->contactSectionFinInverse = 13;
class Parcours
{
public:
    Parcours(std::vector<int> parcours, int contactDepart, int contactSectionDepart, int contactSectionFin, int contactSectionDepartInverse, int contactSectionFinInverse);


    int getContactSectionDepart() const;

    int getContactDepart() const;

    int getContactSectionFin() const;

    int getContactSectionDepartInverse() const;

    int getContactSectionFinInverse() const;

    std::vector<int> getParcours() const;

    void inverserSens();

    int getContact(int numeroContact) const;

private:
    std::vector<int> parcours;
    int contactDepart;
    int contactSectionDepart;
    int contactSectionFin;
    int contactSectionDepartInverse;
    int contactSectionFinInverse;
};

#endif // PARCOURS_H
