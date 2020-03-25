/**
  \file threadmanager.cpp
  \author Yann Thoma
  \modified Müller Robin, Teixeira Carvalho Stéphane
  \date 19.03.2020
  \brief Classe pour reverser un hash md5 par brute force.
*/
#include <QCryptographicHash>
#include <QVector>
#include <pcosynchro/pcothread.h>
#include "threadmanager.h"
#include "mythread.h"

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow (
        long long unsigned int number,
        long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}

ThreadManager::ThreadManager(QObject *parent) :
        QObject(parent)
{}


void ThreadManager::incrementPercentComputed(double percentComputed)
{
    emit sig_incrementPercentComputed(percentComputed);
}

/**
 * Cette fonction permet de trouver l'état du mot de passe après un certain nombre d'itération
 * @param iteration long long unsigned int indiquant l'itération du mot de passse voulu
 * @param nbChars le nombre de caractère du mot de passe à bruteforcer
 * @param nbValidChars nombre de caractère valides pour le mot de passe (nombre de charset)
 * @return un vecteur de int étant l'état du mot de passe après x itération
 */
QVector<unsigned int> getPasswordState(long long unsigned int iteration, long long unsigned int nbChars, long long unsigned int nbValidChars) {
    //Initisalisation du vecteur allant contenir l'état du mot de passe
    QVector<unsigned int> state(nbChars, 0);

    unsigned int i = 0;
    //Effectue une conversion de base 10 vers la base de nbValidChars
    while (iteration != 0) {
        state[i] = iteration % nbValidChars;
        iteration /= nbValidChars;
        ++i;
    }

    return state;
}

/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        unsigned int nbThreads
)
{
    std::vector<std::unique_ptr<PcoThread>> threadList;
    long long unsigned int nbToCompute;
    unsigned long long start;
    QVector<unsigned int> initialPassword;
    long long unsigned int nbToComputePerThread;
    double percentageIncrement;

    init(charset, salt, hash, nbChars);

    /*
     * Calcul du nombre de hash à générer
     */
    nbToCompute = intPow(charset.length(), nbChars);
    initialPassword.fill(0, nbChars);

    nbToComputePerThread = nbToCompute / nbThreads;
    percentageIncrement = (double)1000 / nbToCompute;

    /* Crée les threads, en ajoutant leur pointeur à la liste.
       Les threads sont immédiatement lancés par le constructeur. */
    for (long unsigned int i=0; i<nbThreads; i++)
    {
        //Calcul de l'itération ou doit commencer le thread que nous allons créer
        start = nbToCompute / nbThreads * i;
        //Trouve l'état du mot de passe à l'itération trouvée aupravant
        initialPassword = getPasswordState(start, nbChars, charset.length());

        PcoThread *currentThread = new PcoThread(crack, initialPassword, nbToComputePerThread, percentageIncrement, this);
        threadList.push_back(std::unique_ptr<PcoThread>(currentThread));
    }

    /* Attends la fin de chaque thread et libère la mémoire associée.
     * Durant l'attente, l'application est bloquée.
     */
    for (long unsigned int i=0; i<nbThreads; i++)
    {
        threadList[i]->join();
    }
    /* Vide la liste de pointeurs de threads */
    threadList.clear();

    /*
     * Si on arrive ici, cela signifie que tous les mot de passe possibles ont
     * été testés
     */
    return getPassword();
}
