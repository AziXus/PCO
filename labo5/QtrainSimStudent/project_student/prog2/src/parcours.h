#ifndef PARCOURS_H
#define PARCOURS_H
#include <vector>
class Parcours
{
public:
    Parcours(std::vector<int> parcours, int contactDepart, int contactSectionDepart, int contactSectionFin,
             int contactSectionDepartInverse, int contactSectionFinInverse, int contactRequest, int contactRequestInverse);

    int getContactRequest() const;

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
    int contactRequest;
    int contactRequestInverse;
};

#endif // PARCOURS_H
