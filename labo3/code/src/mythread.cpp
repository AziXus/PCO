#include "mythread.h"
#include <QCryptographicHash>
#include <QVector>
#include <QMessageBox>
#include <pcosynchro/pcomutex.h>
#include "threadmanager.h"

static QString CHARSET;
static QString SALT;
static QString HASH;
static QString password;
static unsigned int NB_CHARS;

QString getPassword() {
    return password;
}

void init(const QString& charset, const QString& salt, const QString& hash, unsigned int nbChars) {
    CHARSET = charset;
    SALT = salt;
    HASH = hash;
    NB_CHARS = nbChars;
    password = "";
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
    nbValidChars       = CHARSET.length();
    nbComputed         = 0;

    currentPasswordString.fill(CHARSET.at(0),NB_CHARS);

    while (nbComputed < nbToCompute) {
        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i=0;i<NB_CHARS;i++)
            currentPasswordString[i]  = CHARSET.at(currentPasswordArray.at(i));

        /* On vide les données déjà ajoutées au générateur */
        md5.reset();
        /* On préfixe le mot de passe avec le sel */
        md5.addData(SALT.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        /* On calcul le hash */
        currentHash = md5.result().toHex();

        /*
         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
         */
        if (currentHash == HASH)
            password = currentPasswordString;

        if (password != "")
            return;

        /*
         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
         * de l'état de notre avancement (pour la barre de progression)
         */
        if ((nbComputed % 1000) == 0) {
            tm->incrementPercentComputed(increment);
        }

        /*
         * On récupère le mot de pass à tester suivant.
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

