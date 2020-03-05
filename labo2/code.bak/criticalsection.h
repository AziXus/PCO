#ifndef CRITICALSECTION_H
#define CRITICALSECTION_H
#include <vector>

class CriticalSection
{
public:

    /**
     * Destructeur virtuel par défaut
     */
    virtual ~CriticalSection() = default;

    /**
     * @brief Méthode initialisant la section critique
     * @param nbThreads Le nombre de threads maximal géré par la section critique
     * Cette méthode doit être appelée avant l’utilisation de la section critique
     */
    virtual void initialize(unsigned int nbThreads) = 0;

    /**
     * @brief Protocole d’entrée dans la section critique
     * @param index Indice de la tâche appelante
     */
    virtual void lock(unsigned int index) = 0;

    /**
     * @brief Protocole de sortie de la section critique
     * @param index Indice de la tâche appelante
     */
    virtual void unlock(unsigned int index) = 0;
};


class WonderfulCriticalSection : public CriticalSection
{
public:

    /**
     * @brief Méthode initialisant la section critique
     * @param nbThreads Le nombre de threads maximal géré par la section critique
     * Cette méthode doit être appelée avant l’utilisation de la section critique
     */
    void initialize(unsigned int nbThreads) override;

    /**
     * @brief Protocole d’entrée dans la section critique
     * @param index Indice de la tâche appelante
     */
    void lock(unsigned int index) override;

    /**
     * @brief Protocole de sortie de la section critique
     * @param index Indice de la tâche appelante
     */
    void unlock(unsigned int index) override;

private:
    std::vector<unsigned int> tickets;
    std::vector<unsigned char> choosing; //std::vector specialization for bool causes issues
};

#endif // CRITICALSECTION_H

