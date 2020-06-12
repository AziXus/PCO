# Labo 6 : Producteur-Consommateur pour calcul différé

Auteurs: Müller Robin, Teixeira Carvalho Stéphane

## Etape 2

### Conception
Dans un premier temps nous avons dû ajouter les variables suivantes pour mener à bien l'étape 2 :
```cpp
Condition resultsEmpty;
Condition resultsMinId;
std::map<int, Result> results;
int minId;
```
Nous avons ajouter 2 conditions `resultsEmpty` et `resultsMinId` pour que la fonction `getNextResult()` soit bloquante. Une condition va nous permettre d'arrêter le thread lorsque le buffer de résultat est vide et la seconde va nous permettre d'attendre jusqu'à ce que le résultat avec l'id le plus ancien(id minimal) arrive en tant que résultat.

Nous avons ensuite défini une map `results` allant contenir une paire `id->résultat` ainsi, nous aurons tout les résultat dans l'ordre croissant des id. Nous avons utilisé une map car ainsi il est simple de faire une attente sur l'id minium et nous savons alors que cet id sera toujours le premier. Nous pouvons aussi grâce a cette structure lié un résultat à un id.

L'entier `minId` va permettre de savoir quel est le prochain id attendu pour transmettre le résultat.

Voici donc ce que donne la fonction provideResult :
```cpp
void ComputationManager::provideResult(Result result) {
    monitorIn();
    std::cout << "Providing result" << std::endl;
    results.insert(std::make_pair(result.getId(), result));
    // A cause du fait que le thread reveille prend la main
    if(results.size() == 1){
        std::cout << "Sending empty" << std::endl;
        signal(resultsEmpty);
    }
    if(results.begin()->first == minId){
        std::cout << "Sending min" << std::endl;
        signal(resultsMinId);
    }
    monitorOut();
}
```
Comme expliqué au-dessus la map results va nous permettre de maintenir une liaison id->résultat et donc nous allons tout d'abord ajouter le nouveau résultat avec son id dans la map.

Puis, si la taille de results vaut 1 (ce qui veut dire qu'elle était vide auparavant) ou que le résultat obtenu est bien le résultat à l'id attendu nous allons envoyer un signal sur l'une des conditions qui pourrait bloquer un thread dans la fonction getNextResult().

Voici la fonction getNextResult :
```cpp
Result ComputationManager::getNextResult() {
    monitorIn();
    if(results.size() == 0){
        std::cout << "Blocking empty" << std::endl;
        wait(resultsEmpty);
    }
    while(results.begin()->first != minId){
        std::cout << "Blocking min" << std::endl;
        wait(resultsMinId);
    }
    minId++;
    Result result = results.begin()->second;
    results.erase(results.begin());
    monitorOut();

    return result;
}
```
Comme indiqué dans la donnée la fonction getNextResult doit être bloquante. Elle doit bloquer lorsque aucun résultat n'est parvenu(`results.size() == 0`) ou lorsque le résultat n'est pas la suite direct du dernier resulat envoyé(`results.begin()->first != minId`).

Si l'un de ces deux cas s'avère vrai la fonction va attendre sur les conditions que nous avons créer.

Une fois ces 2 conditions passées nous incrémentons `minId` pour spécifier le prochain id que getNextResult devra retourner. Puis, nous enlevons le résultat de la map pour ensuite le retourner.

### Tests

Pour valider cette étape nous avons utiliser les test proposées et des tests avec la GUI expliqué ci-dessous :



## Etape 4

### Conception

Pour cette étape, nous n'avons pas ajouté de variables supplémentaires.

Nous avons effectués les actions suivantes dans la fonctios stop :
```cpp
void ComputationManager::stop() {
    stopped = true;
    monitorIn();
    signal(resultsEmpty);
    signal(resultsMinId);
    int type = 0;
    for(Condition &c : conditionsFull){
        while(nbWaitingFull[type] > 0){
            signal(c);
            nbWaitingFull[type]--;
        }
        type++;
    }
    type = 0;
    for(Condition &c : conditionsEmpty){
        while(nbWaitingEmpty[type] > 0){
            signal(c);
            nbWaitingEmpty[type]--;
        }
        type++;
    }
    monitorOut();
}
```
Nous avons décidé de ne pas mettre stopped en section critique car le seul endroit où la variable est modifiée est dans cette fonction.

Nous envoyons ensuite des signaux à toutes les conditions que nous avons créer auparavant pour libérer tout les threads qui seraient bloquer pour ouvoir les arrêter.

Pour ne pas avoir un code répétitif pour arrêter les threads correctement nous avons ensuite créer 2 fonctions :

```cpp
void ComputationManager::checkStop(){
  if(stopped){
      monitorOut();
      throwStopException();
  }
}
```

Cette fonction permet de vérifier si un stop a été effectué. Si cela est le cas, la fonction libère le moniteur et renvoie l'exception indiqué dans la donnée. Sinon elle ne fait rien. On peut effectuer un monitorOut car cette fonction sera toujours appelé par une fonction ou un monitorIn a déjà été effectué.

```cpp
void ComputationManager::waitCheckStop(Condition &c){
    checkStop();
    wait(c);
    checkStop();
}
```
Nous avons créer cette fonction pour éviter de devoir écrire ces 3 lignes de code partout ou il aurait des wait. Ainsi dans tout les endroits où nous avions des wait nous effectuons maintenant `waitCheckStop(condition)`.

Nous effectuons un checkStop avant le wait pour éviter que l'on ait une famine. Nous aurions ce cas si un thread arrive alors que le programme a déjà effectué la fonction stop et donc il resterait bloquer dans la fonction wait car plus aucun signal ne serait envoyé.

Le deuxième `checkstop` permet, si le programme a été stoppé, de quitter le moniteur et d'arrêter l'exécution du thread en renvoyant une exception si un signal a été reçu.

Nous avons également dû modifier la condition de continueWork pour qu'après un stop celle-ci renvoie toujours false:
```cpp
work = abortedId.find(id) == abortedId.end() && !stopped;
```

Nous avons ajouter la condition `!stopped` ainsi si `stopped` passe a true work sera toujours a false et la fonction continueWork renverra toujours false.


### Tests

Pour valider cette étape nous avons utiliser les test proposées et des tests avec la GUI expliqué ci-dessous :
