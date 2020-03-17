/**
  \file mythread.h
  \author Müller Robin, Teixeira Carvalho Stéphane
  \date 19.03.2020
  \brief Classe pour permettre de définir ce qu'un thread doit effectuer lors d'un crackage de mot de passe

  Ce fichier contient la définition de la classe mythread, qui permet de spécifier le fonctionnement
  d'un thread pour brute-forcer un mot de passe md5.
*/
#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QString>
#include "threadmanager.h"

/**
 * Cette fonction permet d'initaliser certaine constante pour les threads du programme
 * @param charset string constante indiquant les charsets utilisés pour trouver le mot de passe
 * @param salt string constante indiquant le sel du mot de passe a trouvé
 * @param hash string constante indiquant le hash du mot de passe a trouvé
 * @param nbChars unsigned int étant la taille du mot de passe
 */
void init(const QString& charset, const QString& salt, const QString& hash, unsigned int nbChars);

/**
 * Permet de lancer un reverse de hash md5 sur un mot de passe
 * @param currentPasswordArray vecteur de int réprésentant l'état du mot de passe à un certain état
 * @param nbToCompute long long unsigned int étant le nombre d'essai à effectuer pour trouver le mot de passe
 * @param increment double étant le pourcentage a augmenté pour la barre de progression du programme
 * @param tm pointeur vers le threadManager du thread
 */
void crack(QVector<unsigned int> currentPasswordArray, long long unsigned int nbToCompute, double increment, ThreadManager* tm);

/**
 * Retourne le mot de passe trouvé
 * @return une chaîne vide si celui-ci n'est pas trouvé, sinon une chaîne étant le mot de passe
 */
QString getPassword();

#endif // MYTHREAD_H
