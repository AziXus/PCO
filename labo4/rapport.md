# Labo 4 : Telecabine

Auteurs: Müller Robin, Teixeira Carvalho Stéphane

## Description des fonctionnalités du logiciel

Lors de ce laboratoire, il nous a été demandé d'implémenter de la synchronisation à l'aide de sémaphore dans une application de simulation de télécabine.  

Il a fallu implémenter cela car le système qui nous a été donné est construit de telle sorte à avoir un ensemble de `N` thread qui seront les skieurs et d'un thread qui représentera la télécabine.  

La cabine fera monter des skieurs sur la piste et ils descendront alors la piste chacun à leur rythme. Les threads n'arriveront donc pas forcéement en même temps. La télécabine, une fois tous les skieurs déchargés, effectuera le trajet de retour à la station pour prendre plus de skieurs. Ce trajet s'effectue avec la cabine vide il est impossible de prendre des skieurs.

La télécabine ne pourra pas supportée plus de `M` skieurs.
Une fois toutes les places prises les skieurs attendront la prochaine remontée. Ils devront donc attendre leur tour.

Il est possible de voir d'après les explications ci-dessus qu'il faut effectivement implémenter de la synchronisation entre les threads de par le fait que les skieurs ne peuvent pas descendre la montagne sans avoir pris la télécabine ou encore qu'il ne peuvent pas monter à plus de `M` personnes etc...

## Choix d'implémentation

En prenant en compte les fonctionnalitésque le problème expliqué ci-dessus comporte voici comment nous avons implémentés la solution à ce problème.

Dans un premier temps nous avons décider du comportement de la télécabine et des skieurs. Voici 2 workflow expliquant le fonctionnement.

![Comportement programme](./images/Behaviors.png)

Si nous commençons par la `télécabine` son comportement est très basique s'il est en service elle fait monter et descendre des skieurs. Même si aucun skieurs n'est présent lors de l'arrivée de la télécabine celle-ci montera quand même.

Une fois que la `télécabine` a fini la montée effectué la cabine doit faire descendre tous ces skieurs avant de pouvoir redescendre. Une fois tout les skieurs hors de la télécabine celle-ci descend et le cycle recommmence.

Si nous prenons maintenant les `skieurs`, on sait que si la cabine est en service il doit attendre la télécabine car celle-ci pourrait être en haut de la montagne ou effectue une montée ou une descente.  
Une fois que la télécabine arrive le `skieur` doit à nouveau vérifier que la télécabine est en service car cela pouvait être sa dernière remonter ou il pourrait y avoir un problème technique dans ce cas le comportement du skieurs est stoppé.

Par contre la télécabine est en service celui-ci va alors monter dans celle-ci puis attendre à l'intérieur(les skieurs qui montent dans la télécabine ainsi que le temps de la montée). Une fois la montée terminée il va sortir de la télécabine et descendre la montagne.

Nous n'avons bien sûr pas encore expliqué les besoins de synchronisation car ici le but est d'expliquer simplement le comportement que nous voulons des 2 classes.

Une fois les comportements mis en place nous avons implémenté la classe `PcoCableCar.cpp`.

Pour l'implémentation des différents fonctionnements nous utiliser 4 sémaphores.

```cpp
PcoSemaphore cableCarLoad;
PcoSemaphore cableCarUnload;
PcoSemaphore skieurInside;
PcoSemaphore skieurOutside;
PcoSemaphore mutex = PcoSemaphore(1);
```

La première sémaphore `cableCarLoad` va permettre à un skieurs de faire une demande(`acquire`) pour monter dans la télécabine lorsque un skieur attendra la télécabine(`waitForCableCar`). Cette sémaphore nous permet également de maintenir les demandes d'accès des skieurs dans l'ordre grâce à la FIFO mise en place dans la conception de sémaphore.  
Les demandes seront ensuite accepté(`release`) lorsque que la télécabine fera monter les skieurs.

La deuxième sémaphore `cableCarUnload` permet à un skieur de faire une demande pour descendre de la télécabine. Ainsi tant que la télécabine ne permet pas au skieur de descendre(`release`) celui-ci attends à l'intérieur de la télécabine(`waitInsideCableCar`).

La sémaphore `skieurInside` permet à la télécabine de faire monter un skieur à la fois. La télécabine va indiquer au skieurs que celui-ci peut monter(`acquire`) et une fois que celui-ci a terminé de monter(`release`) un autre skieur peut monter. Ainsi la télécabine ne peut pas partir tant que les skieurs ne sont as tous complètement montés.

La sémaphore `skieurOutside` permet de faire descendre les skieurs qui sont dans la télécabine 1 par 1. Elle a le même fonctionnement que `skieurInside` mais sauf que cette fois-ci celle permet de ne pas faire descendre une télécabine si tout les skieurs ne sont pas tous descendus.

La sémaphore `mutex` comme son nom l'indique va nous permettre d'avoir un verrou dans notre programme car il y a beacoup d'action qui doivent être mise dans une zone critique. Comme par exemple les variables `nbSkiersWaiting` et `nbSkiersInside` qui vont souvent être incrémentée ou décrementée


## Tests effectués
