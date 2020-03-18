# Labo 3 : Cracking md5

Auteurs: Müller Robin, Teixeira Carvalho Stéphane

## Description des fonctionnalités du logiciel

Lors de ce laboratoire, il nous a été demandés d'implémenter du multi-threading dans une application de crackage de mot de passe md5.  
 Il a fallu implémenter cela car l'application était fonctionnel mais avait des problèmes de performances et donc avec un programme multi-threadé il était possible de fortement améliorer son fonctionnement.

 Au lieu d'avoir un seul thread pour cracker nous aurions plusierus thread se partageant les essais pour effectuer le crackge plus rapidement.

Le logiciel fonctionne en donnant la taille du mot de passe ainsi que le sel et hash correspondant à celui-ci. Il est ensuite possible de définir le nombre de threads que nous voulons utiliser pour cracker le mot de passe.

## Choix d'implémentation

Nous avons initialement implémenter une première version avec comme optique de faire que chaque threads effectue l'un après l'autre le test de hash. Avec ce choix, nous avons du mettre en place un mutex pour permettre aux threads de ne pas rentrer dans la zone critique en même temps. La zone critique est l'incrémentation du vecteur d'entier contenant le mot de passe, ainsi que la transformation en chaine de caractères.

![](img/ZoneCritique.PNG)

Étant donné que la section est grande et contient deux boucles, le crackage de hash était de plus en plus lent, en augmentant le nombre de threads. Car le zone ne pouvait être accedée que par un thread à la fois.

Nous avons donc changé d'optique et nous nous sommmes concentrés sur de la parallélisation plutôt que d'avoir une section critique. Pour cela, nous avons décidé de séparer l'espace de crackage de chaque thread. Si nous avons 1000 différentes possibilitées de mot de passe ainsi que 5 threads, la thread 1 crackera les mots de passe aux indexes 0 à 199, puis 200 à 399, etc...

Le problème de cette optique est de trouver en un temps raisonnable l'état du mot passe à un certain index.
Afin de déterminer cela, nous nous sommes rendu compte que c'était simplement un problème de conversion de base, de la base 10 à la base nbValidChars. nbValidChars correspond aux nombres de caractères du charset, qui est 66 (abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!$~\*). La complexité de l'algorithme est log<sub>nbValidChars</sub>(n).

Notre solution augmente la vitesse d'un facteur `n`, n étant le nombre de threads, si l'on ne trouve pas le mot de passe et que les threads vont à la même vitesse.

Nous avons remarqué que si l'on essaie de cracker le même hash avec un nombre de threads différents, il est possible que le temps augmente ou diminue. Cette variation est causée par la séparation de l'espace de crackage. Exemple...........

## Tests effectués
Nous avons utilisé le mot de passe `test`, sans sel, qui correspond au hash suivant : `098f6bcd4621d373cade4e832627b4f6`.

1. Choix d'un mot de passe de 4 caractères et tentative de cassage avec 1 thread. Cela permet de vérifier que notre implémentation fonctionne comme le programme de base.

  Résultat : Le résultat reste identique au programme de base. Le mot de passe est trouvé au même moment(29%).

  ![](img/R1.png)

2. Choix d'un mot de passe de 4 caractères et tentative de cassage avec plusieurs thread.

  Résultat : Le mot de passe est trouvé plus rapidement que lors du test précédent.

  ![](img/R2.png)

  1. Nous augmentons ici le nombre de thread à 8

    Résultat : Nous avons ici le problème que nous avions cité auparavant ou il est possible que le temps de crackge soit plus élévé avec plus de thread. Ici le mot de passe doit se situé vers la millieu de test d'un des threads.

    ![](img/R2_2.png)

  2. Nous augmentons ici le nombre de thread à 5

      Résultat : Tout comme le test précédent nous avons une augmenation du temps de recherche.

      ![](img/R2_3.png)

3. Choix d'un mot de passe de 4 caractères avec un hash faussé.

  Résultat : Le mot de passe n'est effectivement pas trouvé et lorsque 100% est atteint le programme affiche le message directement. Cela nous confirme donc que tout ne thread se termine correctement.

  ![](img/R3.png)

4. Choix d'un mot de passe de 4 caractères avec un sel.
  Le sel utilisé est `SALTY`, le mot de passe `test` et le hash corresondant `e13c4944e60f4451974b2e9cb784d0f1`

  Résultat : Dans ce test, le crackage est en peut plus lent mais cela est dû au sel. Le mot de passe est trouvé en un temps raisonnable.

  ![](img/R4.png)

5. Choix d'un mot de passe de 3 caractères.
  Le mot de passe est `tes` et le hash correspondant est `28b662d883b6d76fd96e4ddc5e9ba780`.

  Résultat : Le mot de passe est trouvé encore plus rapidement qu'auparavant.

  ![](img/R5.png)
