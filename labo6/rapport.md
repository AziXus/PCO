# Labo 6 : Producteur-Consommateur pour calcul différé

Auteurs: Müller Robin, Teixeira Carvalho Stéphane

## Etape 1
### Conception
La première étape du programme consiste a créé les fonctions requestComputation et getWork.

La méthode `requestComputation` est potentiellement bloquante car il ne peut avoir que 10 computations par type(A, B, C). Nous utilisons donc un `std::vector<Condition> conditionsFull`. Ce vecteur permet de stocker une variable de condition par type de computation. On attends donc s'il y a 10 computations et la méthode `getWork` envoie un signal au bon type lorsque la computation est récupérée.

Afin de stocker les différentes computations, nous avons décidé d'utiliser une `map` en prévision des futures fonctions. La déclaration est la suivante : `std::map<int, Computation> computations`.  
L'index de la map contient l'id afin d'obtenir une computation en fontion de son id rapidement. Ceci sera très utile dans le cas de `abortComputation` .   
L'utilisation d'une `std::map` permet également d'avoir un conteneur trié en fonction de l'id.

Il nous faut encore un conteneur pour stocker les ids pour chaque type de computation et obtenir le prochain id. Nous avons utilisé le type suivant : `std::vector<std::queue<int>> computation`.

Voici le code de la fonction `requestComputation` :
```C
int ComputationManager::requestComputation(Computation c) {
    monitorIn();
    while(computation[(int)c.computationType].size() == MAX_TOLERATED_QUEUE_SIZE){
        wait(conditionsFull[(int)c.computationType]);
    }
    int id = nextId++;
    computations.insert(std::make_pair(id, c));
    computation[(int)c.computationType].push_back(id);
    signal(conditionsEmpty[(int)c.computationType]);
    monitorOut();
    return id;
}
```
Cette fonction va être bloquante si la taille de computation max a été atteinte. Si c'est le cas le thrad va se mettre en attente sur la condition correspondant à son type. Sinon l'id va augmenter et la compuation va ensuite être ajoutée dans la queue liée à son type.

Pour la méthode getWork, il est aussi nécessaire d'avoir un vecteur de variables de conditions : `    std::vector<Condition> conditionsEmpty`. Ceci permet d'attendre lorsque aucune computation du bon type n'est disponible. Le signal est envoyé par la méthode `requestComputation` lorsqu'une computation du type est ajoutée.  
Cette méthode obtient l'id de la première computation(Computation la plus ancienne) et supprime l'id ainsi que la computation des conteneurs.

Code de `getWork` :
```C
Request ComputationManager::getWork(ComputationType computationType) {
    monitorIn();
    int id;
    if(computation[(int)computationType].size() == 0){
        wait(conditionsEmpty[(int)computationType]);
    }
    id = computation[(int)computationType].front();
    computation[(int)computationType].pop_front();
    auto itComputation = computations.find(id);
    Request request = Request(itComputation->second, id);
    computations.erase(itComputation);
    signal(conditionsFull[(int)computationType]);
    monitorOut();
    return request;
}
```

## Etape 2

### Conception
Dans un premier temps nous avons dû ajouter les variables suivantes pour mener à bien l'étape 2 :
```cpp
Condition resultsEmpty;
Condition resultsMinId;
std::map<int, Result> results;
int minId;
```
Nous avons ajouter 2 conditions `resultsEmpty` et `resultsMinId` pour que la fonction `getNextResult()` soit bloquante. La condition `resultsEmpty` va nous permettre d'arrêter le thread lorsque le buffer de résultat est vide et la seconde va nous permettre d'attendre jusqu'à ce que le résultat avec l'id le plus ancien(id plus petit) arrive en tant que résultat.

Nous avons ensuite défini une map `results` allant contenir une paire `id->résultat` ainsi, nous aurons tout les résultat dans l'ordre croissant des id. Nous avons utilisé une map car cela facilte l'implémentation de l'attente sur l'id minium, nous savons alors que cet id sera toujours le premier. Nous pouvons aussi grâce a cette structure lié un résultat à un id ce qui nous est fortement utile.

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
    if(results.begin()->first != minId){
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

#### Test 1

Dans un premier temps nous avions testé le cas où nous envoyons une tâche de chaque type. Mais ce test ne fonctionnait pas car il y avait un problème dans le ComputeEngine des computation A et B.

Pour corriger cela il fallait modifier la valeur de retour de la fonction continueWork et mettre true.

Voici donc le résultat après la modification

![](./images/Etape2_Test1.PNG)

On voit alors que même si les computation B et C se terminent avant A le programm attend que A se termine avant d'afficher les requests.

#### Test 2

Dans ce cas, nous avons augmenter le nombre de computation en variant les computations.

![](./images/Etape2_Test2.PNG)

Le test est correct car les id sont dans l'ordre comme désiré

#### Test supplémentaires

Nous avons ensuite continuer d'augmenter le nombre de computations et vérifier si les id restait dans l'ordre.

| # | Nb computations | Computation par type | Ordre correct   |
|---|------------|----------|------------|
| 1 | 15         | 5        | OK      |
| 2 | 30         | 10       | OK      |
| 3 | 45         | 15       | OK      |
| 4 | 60         | 20       | OK      |
| 5 | 75         | 25       | OK      |

## Etape 3
Cette étape implémente les fonctions `abortComputation` et `continueWork`.  

Afin de les faire fonctionner proprement, plusieurs structures ont été ajoutées :
- `std::vector<int> nbWaitingFull` : Ce vecteur permet de maintenir un compteur pour chaque type de computation lorsque le buffer du type est plein. On l'incrémente juste avant le wait lorsque le buffer est plein pour le type de computation demandé.
- `std::vector<int> nbWaitingEmpty` : Ce vecteur permet de maintenir un compteur pour chaque type de computation lorsque le buffer du type est vide. On l'incrémente juste avant le wait lorsque le buffer est vide pour le type de computation demandé.
- `std::set<int> abortedId` : Afin de maintenir une liste des ids annulés, un set est utilisé pour supprimer et ajouter des id en `log(n)`.

Le code de notre fonction abortComputation est le suivant :
```C
void ComputationManager::abortComputation(int id) {
    monitorIn();
    // Effacer de la liste des computations
    auto itComputation = computations.find(id);
    // Effacer de la liste des résultat
    auto itResult = results.find(id);

    if(itComputation != computations.end()){
        int cType = (int)itComputation->second.computationType;
        auto it = std::find(computation[cType].begin(), computation[cType].end(), id);
        // Remove computation
        computations.erase(itComputation);
        computation[cType].erase(it);
        // Si une thread était en attente, on la libère
        if(nbWaitingFull[cType] > 0){
            nbWaitingFull[cType]--;
            signal(conditionsFull[cType]);
        }
    } else if(itResult != results.end()){
        if(itResult->first == minId){ // Si l'on supprime le minId, on l'incrémente et l'on libère une thread
            minId++;
            signal(resultsMinId);
        }
        results.erase(itResult);
    } else if(id < nextId || id >= minId){
        // Evite d'ajouter de id invalide
        abortedId.insert(id);
    }
    monitorOut();
}
```
La fonction commence par rechercher l'id de la computation à annuler dans `computations` et dans `results`. Il y a ensuite trois possibilités :

1. Si la computation est trouvée dans la liste de computations, cela signifie qu'aucun `computeengine` effectue actuellement des calculs. On supprime donc simplement l'id des structures et on libère une thread si le buffer était plein.

2. Si 1 est faux et que l'id à annuler se trouve dans les résultats, on regarde tout d'abord si l'id est égal au `minId`. Dans ce cas là, on incrémente l'id et envoie un signal à la fonction `getNextResult` si une thread attendait. On supprime ensuite le résultat dans tout les cas.

3. Si 1 et 2 sont faux et que l'id est valide, on ajoute l'id aux ids annulés. Il sera utilisé plus tard dans la fonction `continueWork`.

Fonction `continueWork` :
```C
bool ComputationManager::continueWork(int id) {
    bool work;
    monitorIn();
    work = abortedId.find(id) == abortedId.end();
    // Efface l'id a abort si celui ci est présent dans la liste
    if(abortedId.find(id) != abortedId.end()){
        abortedId.erase(abortedId.find(id));
    }
    monitorOut();
    return work;
}
```
La fonction `continueWork` retourne vrai si l'id n'a pas été annulé, faux sinon. On supprime également l'id du set s'il a été trouvé.

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

Nous envoyons ensuite des signaux à toutes les conditions que nous avons créer auparavant pour libérer tout les threads qui seraient bloquer pour pouvoir les arrêter.

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

Nous avons également ajouter un checkStop en tout début de la fonction car si le programme est arrêté plus aucune computation de doit être acceptée.

```cpp
int ComputationManager::requestComputation(Computation c) {
    monitorIn();
    checkStop();
...
```

Nous avons également dû modifier la condition de continueWork pour qu'après un stop celle-ci renvoie toujours false:
```cpp
work = abortedId.find(id) == abortedId.end() && !stopped;
```

Nous avons ajouter la condition `!stopped` ainsi si `stopped` passe a true work sera toujours a false et la fonction continueWork renverra toujours false.


### Tests

Pour valider cette étape nous avons utilisé les test proposés et des tests avec la GUI expliqué ci-dessous :

#### Test 1

Avec la GUI nous avons tout d'abord tester le fonctionnement de base qui est d'arrêter le programme au bout d'un certain temps alors que des computation sont en cours.

![](./images/Etape4_Test1.PNG)

Dans un premier temps on peut voir que le programme arrête bien les computations directement dès que le signal stop est envoyé. Le siganl stop a été envoyé juste après la demande de computation 8.

Si on regarde dans le terminal on voit également que les compute engines sont bien stopper.
```
GUI Enabled : 0
[STOP] Compute Engine A - 0
GUI Enabled : 1
[STOP] Compute Engine A - 1
GUI Enabled : 2
[STOP] Compute Engine B - 0
GUI Enabled : 3
[STOP] Compute Engine C - 0
```
#### Test 2

Nous avons ensuite effectué un 2ème test pour vérifier que le programme n'accepte plus de computation après un arrêt.

![](./images/Etape4_Test2.PNG)

Le test est fonctionnel car on voit que les computations B, A, C sont grisés ce qui indique qu'il n'est plus possible d'effectuer une computation.
