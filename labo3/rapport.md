# Titre du laboratoire

Auteurs: Müller Robin, Teixeira Carvalho Stéphane

## Description des fonctionnalités du logiciel

Lors de ce laboratoire, il nous a été demandés d'implémenter du multi-threading dans une application de crackage de mot de passe md5.  
 Il a fallu implémenter cela car l'application était fonctionnel mais avait des problèmes de performances et donc avec du multi-threadé il était possible de fortement améliorer son fonctionnement.

Le logiciel fonctionne en donnant la taille du mot de passe ainsi que le sel et hash correspondant à celui-ci. Il est ensuite possible de définir le nombre de threads que nous voulons utiliser pour cracker le mot de passe.

## Choix d'implémentation


Comment avez-vous abordé le problème, quels choix avez-vous fait, quelle
décomposition avez-vous choisie, quelles variables ont dû être protégées, ...



## Tests effectués

1. Choix d'un mot de passe de 4 caractères et tentative de cassage avec 1 thread. Cela permet de vérifier que notre implémentation fonctionne comme le programme de base.

  Résultat :


2. Choix d'un mot de passe de 4 caractères et tentative de cassage avec plusieurs thread.

  Résultat :

3. Choix d'un mot de passe de 4 caractères avec un hash faussé.

  Résultat :

4. Choix d'un mot de passe de 4 caractères avec un sel.

  Résultat :

Description de chaque test, et information sur le fait qu'il ait passé ou non
