# Labo 6 : Producteur-Consommateur pour calcul différé

Auteurs: Müller Robin, Teixeira Carvalho Stéphane

## Etape 1
### Conception
La première étape du programme consiste a créé les fonctions requestComputation et getWork.

La méthode `requestComputation` est potentiellement bloquante car il ne peut avoir que 10 computations par type. Nous utilisons donc un `std::vector<Condition> conditionsFull`. Ce vecteur permet de stocker une variable de condition par type de computation. On attends donc s'il y a 10 computations et la méthode `getWork` envoie un signal au bon type lorsque la computation est récupérée.

Afin de stocker les différentes computations, nous avons décidé d'utiliser une `map` en prévision des futures fonctions. La déclaration est la suivante : `std::map<int, Computation> computations`.  
L'index de la map contient l'id afin d'obtenir une computation en fontion de son id rapidement. Ceci sera très utile dans le cas de `abortComputation` .   
L'utilisation d'une `std::map` permet également d'avoir un conteneur trié en fonction de l'id.

Il nous faut encore un conteneur pour stocker les ids pour chaque type de computation et obtenir le prochain id. Nous avons utilisé le type suivant : `std::vector<std::queue<int>> computation`.

Code de `requestComputation` :
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

Pour la méthode getWork, il est aussi nécessaire d'avoir un vecteur de variables de conditions : `    std::vector<Condition> conditionsEmpty`. Ceci permet d'attendre lorsque aucune computation du bon type n'est disponible. Le signal est envoyé par la méthode `requestComputation` lorsqu'une computation du type est ajoutée.  
Cette méthode obtient l'id de la première computation et supprime l'id ainsi que la computation des conteneurs.

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

— int requestComputation(Computation c);
— Request getWork(ComputationType computationType)

## Etape 2

### Conception
Dans un premier temps nous avons dû ajouter les variables suivantes pour mener à bien l'étape 2 :
```cpp
Condition resultsEmpty;
Condition resultsMinId;
std::map<int, Result> results;
int minId;
```
Nous avons ajouter 2 conditions pour que la fonction getNextResult soit bloquante. Une condition va nous permettre de s'arrêter quand le buffer de résultat est vide et la seconde va nous permettre d'attendre que le résultat avec l'id le plus ancien(id minimal) arrive en tant que résultat.

Nous avons ensuite défini une map allant contenir une pair id->résultat ainsi nous aurons tout les résultat dans l'ordre croissant des id. Nous avons utilisé une map car ainsi il est simple de faire une attente sur l'id minium et nous savons alors que l'id minimale sera toujours le premier. Nous pouvons aussi  grâce a cette structure lié un résultat à un id.

L'entier minId va permettre de savoir quel est l'id attendu pour transmettre le résultat.

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
Comme expliqué au-dessus la map results va nous permettre de maintenir une liaison id->résultat et donc nous allons tout d'abord ajouter le nouveau résultat dans la map.

Puis si la taille de results vaut 1 (ce qui veut dire qu'elle était vide auparavant) ou que le résultat obtenu est bien le résultat à l'id attendu nous allons envoyer un signal sur l'une des conditions qui pourrait bloquer un thread dans la fonction getNextResult.

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
Comme indiqué dans la donnée la fonction getNextResult doit être bloquante. Elle doit bloquer lorsque aucun résultat n'est parvenu(`results.size() == 0`) ou lorsque le résultat n'est pas la suite direct du dernier resulat reçu(`results.begin()->first != minId`).

Si l'un de ces deux cas s'avère vrai la fonction va attendre sur les conditions que nous avons créer.

Une fois ces 2 conditions passées nous incrémentons minId pour spécifier le prochain id que getNextResult devra retourner. Puis, nous enlevons le résultat de la map pour ensuite le retourner.

## Etape 4
