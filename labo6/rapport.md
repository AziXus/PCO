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
