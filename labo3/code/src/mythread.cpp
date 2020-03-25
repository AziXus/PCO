/**
  \file mythread.cpp
  \author Müller Robin, Teixeira Carvalho Stéphane
  \date 19.03.2020
  \brief Classe pour permettre de définir ce qu'un thread doit effectuer lors d'un crackage de mot de passe
**/
#include "mythread.h"
#include <QCryptographicHash>
#include <QVector>
#include <QMessageBox>
#include <pcosynchro/pcomutex.h>
#include "threadmanager.h"

/* Utilisation de _ parce que les élèment sont static */
//Tous les threads auront accès aux différentes variables ci-dessous
//Aucun mécanisme de protection n'a été mis en place car nous effectuons du parallélisme

//Les variables ci-dessous seront lu par le différents thread
//car elle permettent de définir le fonctionnement du crackage
static QString _charset;
static QString _salt;
static QString _hash;
static unsigned int _nbChars;
//La variable _password sera accéder en lecture et écriture par les threads
//Elle contiendra le mot de passe si celui-ci est trouvé sinon elle sera vide
static QString _password;

QString getPassword() {
    return _password;
}

void init(const QString& charset, const QString& salt, const QString& hash, unsigned int nbChars) {
    _charset = charset;
    _salt = salt;
    _hash = hash;
    _nbChars = nbChars;
    _password = "";
}

void crack(QVector<unsigned int> currentPasswordArray, long long unsigned int nbToCompute, double increment, ThreadManager* tm) {
    unsigned int i;
    long long unsigned int nbComputed;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars;

    /*
     * Mot de passe à tester courant
     */
    QString currentPasswordString;

    /*
     * Hash du mot de passe à tester courant
     */
    QString currentHash;

    /*
     * Object QCryptographicHash servant à générer des md5
     */
    QCryptographicHash md5(QCryptographicHash::Md5);

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    nbValidChars       = _charset.length();
    nbComputed         = 0;

    currentPasswordString.fill(_charset.at(0),_nbChars);

    while (nbComputed < nbToCompute) {
        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i=0;i<_nbChars;i++)
            currentPasswordString[i]  = _charset.at(currentPasswordArray.at(i));

        /* On vide les données déjà ajoutées au générateur */
        md5.reset();
        /* On préfixe le mot de passe avec le sel */
        md5.addData(_salt.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        /* On calcul le hash */
        currentHash = md5.result().toHex();

        /*
         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
         */
        if (currentHash == _hash)
            _password = currentPasswordString;

        if (_password != "")
            return;

        /*
         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
         * de l'état de notre avancement (pour la barre de progression)
         */
        if ((nbComputed % 1000) == 0) {
            tm->incrementPercentComputed(increment);
        }

        /*
         * On récupère le mot de passe à tester suivant.
         *
         * L'opération se résume à incrémenter currentPasswordArray comme si
         * chaque élément de ce vecteur représentait un digit d'un nombre en
         * base nbValidChars.
         *
         * Le digit de poids faible étant en position 0
         */
        i = 0;

        while (i < (unsigned int)currentPasswordArray.size()) {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars) {
                currentPasswordArray[i] = 0;
                i++;
            } else
                break;
        }

        nbComputed++;
    }
}
