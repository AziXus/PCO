#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QString>
#include "threadmanager.h"

void init(const QString& charset, const QString& salt, const QString& hash, unsigned int nbChars);
void crack(QVector<unsigned int> currentPasswordArray, long long unsigned int nbToCompute, double increment, ThreadManager* tm);
QString getPassword();

#endif // MYTHREAD_H
