# Labo 5 : Gestion de ressources

Auteurs: Müller Robin, Teixeira Carvalho Stéphane

## Description des fonctionnalités du logiciel

Lors de ce laboratoire, il nous a été demandé d'implémenter de la gestion de ressources entre thread à l'aide de sémaphore dans une application de simulation de parcours de locomotive.  

Cela était nécessaire car, les locomotives font chacunes un parcours circulaire et il était demandé qu'un tronçon soit le même pour les 2 locomotives de la simulation nous appelerons cela la section partagée. Les locomotives ne pouvant pas être en même temps sur le tronçon elles devront se partager la ressource.

Pour le premier programme, nous devons faire rouler les locomotives pendant 2 tours dans un sens puis lors du deuxième tour celle-ci doivent repartir dans le sens inverse et effectuer à nouveau deux tours pour rechanger de sens et cela à l'infine.

Comme les deux locomotives partgent un tronçon en commun elle devront demander l'accès au tronçon avant d'arriver dans celui-ci. Si une locomotive est déjà dans le tronçon la locomotive ayant fait la demande d'accès s'arrête et attend la sortie de celle-ci du tronçon. Sinon elle rentre dans la section et la section se retrouve alors fermé tant que celle-ci se trouve à l'intérieur.

Dans la deuxième partie, il était demandé d'effectuer un second programme sur la base du premier en implémentant cette fois-ci une notion de priorité. Pour cela les locomotives devront effectuées une requête avant de faire la demande d'accès faites dans le premier programme. Ainsi, si la locomotive lors de la demande d'accès n'a pas une plus haute priorité que la locomotive venant de faire une requête d'accès celle-ci se retrouve alors arrêtée et doit attendre que la locomotive de priorité plus haute finisse de passer dans la section partagée avant de pouvoir y entrer.

Le tronçon partagé que nous avons choisi pour les deux programmes est celui tracé en rouge dans l'image ci-dessous:
![Parcours \label{my_label}](./images/SharedSection_WithPoints.PNG)

## Choix d'implémentation

En prenant en compte les fonctionnalités que le problème expliqué ci-dessus comporte, voici comment nous avons implémenté la solution.

Dans un premier temps nous avons décidé de schématiser le comportement des locomotives. Voici le flowchart expliquant le fonctionnement.

![Comportement programme \label{my_label}](./images/Behaviors.png)

Sur le schéma nous pouvons voir qu'une `locomotive` démarre en initialisant 2 variables : le nombre de tour que la locomotive doit effectuer avant d'inverser de sens et le numéro du contact que nous désirons sur le parcours(un parcours sera une liste de contact  et donc il nous faut un index pour parcourir la liste).

La `locomotive` va ensuite effectuer son parcours tant que son comportement ne doit pas être stoppé. S'il est stoppée le thread se termine.

Si le stop n'est pas actif la `locomotive` va tout d'abord prendre le contact désiré de la liste et attendre que celui-ci soit atteint. Une fois le contact passé la variable avec le numéro de contact sera incrémentée pour prendre le contact suivant dans la liste lors de la prochaine itération.

Nous effectuons ensuite plusieurs tests pour savoir si le contact passé est un contact pour lequel une action doit être effectuée. Les 3 premiers tests permettent d'appeler les fonctions de la section partagée(Dans l'ordre `request`, `getAccess` et `leave`).

Le dernier test permet de savoir si un tour a été effectué par la `locomotive`. Si cela est le cas on va décrementer le nombre de tour et si celui-ci se retrouve à 0 nous allons inverser le sens du parcours ainsi que celui de la `locomotive`. Puis, Le nombre de tour sera alors remis à deux et le numéro de contact remis à zéro. Sinon, si le nombre de tour est différent de 0, nous allons simplement réinitialiser la variable des numéro de contact du parcours car dans tout les cas le parcours sera recommencé.

Lors de la conception du programme nous nous sommes rendus compte qu'il serait intéressant d'ajouter une classe supplémentaire se nommant `Parcours`. Elle nous permettrait d'avoir tout les contacts qu'une locomotives doit passer ainsi que les contacts importants lors du tour effectué.

Voici un schéma UML de notre classe Parcours :

![UML Parcours \label{my_label}](./images/UML.png)

Comme indiqué auparavant cette classe permet de définir toutes les spécifités d'un parcours effectuée par une locomotive. Le constructeur de cette classe initialise tous les attributs privés. Nous définissons également tous les contacts spéciaux pour le parcours inverse car les contacts sont différents et ainsi il est plus simple pour nous d'échanger les valeurs lorsqu'il y a un changement de sens. Il nous suffit de faire un swap entre les valeurs.

```cpp
void Parcours::inverserSens()
{
    // Inverse toute les valeurs du vecteur sauf le dernier contact qui est le contact de départ du parcours
    std::reverse(parcours.begin(), parcours.end() - 1);
    // Swap les valeurs des contacts entre les contacts du parcours en sens inverse et normal
    std::swap(contactSectionDepart,contactSectionDepartInverse);
    std::swap(contactSectionFin,contactSectionFinInverse);
    std::swap(contactRequest, contactRequestInverse);
}
```
 La classe propose également des getteurs pour les différents contacts importants lors du parcours.  
 Ces contacts sont les contacts :
 1. Où la requête doit être effectuée
 2. Où la demande d'accès doit être effectuée
 3. Sortie de la section partagée
 4. Celui du contact indiquant la fin d'un tour

Ainsi, si nous reprenons le flowchart du comportement d'une locomotive lorsque nous allons tester les actions à faire selon le contact passé, nous allons simplement faire appel aux getteurs de la classe Parcours. Il va de soit que la classe `LocomotiveBehavior` aura alors un objet Parcours en tant qu'attribut pour appeler ces getteurs.

Voici le constructeur de `LocomotiveBehavior`:
```cpp
LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SharedSectionInterface> sharedSection,
                   Parcours parcours, SharedSectionInterface::Priority priority)
                   : loco(loco), sharedSection(sharedSection), parcours(parcours), priority(priority)
```

Lors de la conception de la classe SharedSection nous avons utilisé les attributs privés suivants :
```cpp
bool accesPrioritaire;
bool sectionOccupee;
bool locoEnAttente;

PcoSemaphore wait;
PcoSemaphore mutex = PcoSemaphore(1);

static bool stop;
static PcoSemaphore mutexStatic;
```
Le booléean `accesPrioritaire` va nous permettre de savoir si une locomotive avec un accès prioritaire a demandé accès au tronçon.  
Elle sera mise à `true` lors d'un getRequest d'une locomotive ayant une priorité élevée(HighPriority) et ensuite à false quand une locomotive de haute priorité quitte(`leave`) la section partagée.

Nous avons ensuite défini un booléean `sectionOccupee` qui va permettre de savoir si une locomotive se trouve sur le tronçon partagée car si cela est le cas la variable sera à `true` et toute les locomotives qui essayeront de rentrer dans la section seront alors en mise en attente. Cette variable sera alors remis à `false` que lorsque la locomotive se trouvant dans le tronçon en sort.

Une troisième variable booléenne se nommant `locoEnAttente` va permettre au programme se savoir si une locomotive a été mise en attente la variablesera mise a `true` dans ce cas là. Pour rappel, une locomotive est mise en attente si le tronçon est occupée ou que une locomotive de priorité supérieur a demandée accès au tronçon. Ainsi lorsque la locomotive qui était dans le tronçon en sort elle saura si elle doit relâche une locomotive ou non.

Nous avons pu mettre en place cette variable booléenne car il nous a été demandé de faire une implémentation pour les 2 locomotives qui nous était données et non pas une implémentation pour N locomotives car dans sinon cette variable aurait du être remplacer un par un int

Ensuite la sémaphore `wait` va permettre aux locomotives d'attendre(`acquire`) ou de libérer(`release`) la section partagée.  
Un `mutex` a été défini car les threads devront se partagées des variables, comme par exemple `nbWaiting`, et nous devons donc mettre en place de l'exclusion mutuelle pour éviter d'avoir des problèmes de concurrences.

La variable statique booléenne `stop` va nous permettre d'indiquer depuis le main à la section partagé de se terminer et donc de ne plus prendre de locomotive dans le tronçon.

Nous avons également créer la même variable statique `stop` dans le `LocomotiveBehavior` pour permettre au main d'arrêter le comportement des locomotives.

Les 2 classes auront alors une fonction statique publique se nommant void static `stopAllBehaviors()` pour LocomotiveBehavior et `stopShared()` pour SharedSection pour stopper les threads depuis le main.

Nous avons dû implémeneter des fonctions statiques car la fonction `emergency_stop` dans le main ne connaissait pas d'objet instancié sous la forme de LocomotiveBehavior ou encore SharedSection. Et comme la fonction ne prennait également aucun paramètre le seul moyen que nous avions de stopper les threads était de créer ces fonctions statiques.

Malheureseument, même avec le stop implémenté nous avons toujours des problèmes a terminer le programme correctement car comme la fonction `attendre_contact` dans le LocomotiveBehavior est bloquant il se pourrait que nous arrêtons le parcours(`emergency_stop`) alors que la locomotive se situe dans la fonction attendre_contact et donc dans ce cas le thread ne se termine pas car il est bloqué dans la fonction. Nous n'avons pas réussi à trouver de solution à ce problème.


## Tests effectués

Nous avons tester les 2 programmes de la manière suivante :

### Test programme 1

### Test programme 2
