# Journal de stage

## 12/06/2020

Programmation de trois programmes utilisant OpenMP :

- Un "hello world" affiché par plusieurs threads simultanément
- Un programme pour calculer la somme d'un tableau (ici la somme des premiers entiers naturels)
- Un calcul de la suite de Fibonacci de manière naïve. Seule les valeurs au-dessus de 20 ont été parallélisées pour permettre d'éviter la création de trop nombreuses *tasks*. Grâce à cela, l'amélioration des performances est nette pour `n = 40`.

Lecture plus en profondeur de la documentation de *X_PAR*.

Conversion du programme "hello world" en assembleur RISC-V avec l'extension *X_PAR*.

## 11/06/2020

Début du brouillon pour construire un pipeline complet. Seul l'étape de fetch et decoding a vraiment été traitée. L'issue, le writeback et le commit ont été traitées seulement superficiellement pour l'instant.

Lecture de cours sur OpenMP, comme la parallélisation est organisée par le compilateur lorsque l'information est disponible statiquement.

> Je n'ai rien eu de concret à commit sur le répertoire git :
>
> En effet, j'ai passé la matinée à faire des brouillons pour schématiser le pipeline out-of-order, mais je pense être encore loin d'avoir quelque chose de potable.
> Sinon, l'après-midi, je me suis documenté sur OpenMP. Je vais essayer de produire du code aujourd'hui avec GCC classique pour tester ce que j'ai appris hier, et je vais essayer de voir comment faire le lien avec les différentes instructions spécifiques au LBP. Je ne pourrai pas vérifier mon code directement cependant.

## 10/06/2020

Reprise de la trace de `fib(3)` : construction du diagramme des dépendances en LaTeX des dépendances avec dans un premier temps le diagramme complet, puis le diagramme ne prenant en compte que les instructions autres que `sp ± k` ou modifiant `ra`. Enfin, le diagramme ne prenant en compte aucune instruction pour gérer le stack et la sauvegarde / le chargement des valeurs sur celui-ci.
Correction mineure de la trace par ailleurs.

> Ce que j'ai fait m'a permis de bien comprendre l'intérêt d'avoir plusieurs hart pour des appels successifs :
>
> on voit clairement que le nombre d'instructions à exécuter diminue drastiquement dès qu'on ne se charge plus du stack. (Du moins pour cette fonction simple. Je me doute qu'elle n'est pas représentative de ce qui est habituellement exécuté, mais ça donne une bonne idée de ce qu'il est possible de faire et de son intérêt).
> J'ai passé pas mal de temps à éplucher aussi certaines références que vous m'avez données, notamment la solution proposée par Tomasulo, nom que j'ai vu revenir plusieurs fois dans mes recherches d'ailleurs.
> J'ai commencé ce matin à faire une synthèse du pipeline out-of-order tel que je l'implémenterai, malheureusement sans pouvoir utiliser plusieurs harts puisque l'on reste sur du monocœur. Je vais essayer, je pense, d'en faire une rapide implémentation en python avec une architecture très restreinte : saut, load, store, addition de deux registres et d'un registre avec un immédiat.

## 09/06/2020

Conversion en assembleur du programme calculant la suite de Fibonacci récursivement, puis construction de la trace du code assembleur avec renommage et table d'association lors du calcul de `fib(3)`.

- [ ] Fonctions :
  - [x] Fibonacci récursif

> Aujourd'hui, j'ai continué le travail sur les traces, notamment en faisant la trace d'un appel de fibonacci récursif.
> J'en ai profité pour rapidement coder l'ensemble du programme en assembleur afin de vérifier que c'était correct.
> Mes conclusions sont que les instructions après un saut ne devraient pas être renommées et réserver un registre physique dans le fichier de registres : cela peut conduire à un blocage du pipeline si les instructions après le call sont renommées, remplissant au fur et à mesure le fichier de registre. Un saut, conditionnel ou non, et un appel apparaissent donc pour moi comme des instructions bloquant tout renommage, afin d'en garantir la séquentialité ainsi que celle des dépendances.
> J'ai réfléchi aussi à la phase du commit dans le cas où le fichier de registres est limité : il apparaît évident qu'il faille désallouer certains registres physiques lorsqu'il n'y en a plus besoin. La règle que j'ai déduite est qu'un registre peut être désalloué lorsqu'aucune instruction dans la liste d'attente ou en cours d'exécution n'a besoin de ce registre en entrée, et que le registre physique n'est pas présent dans la table d'association. Par exemple, si les registres utilisés sont r1, r2, r3, r4, et que la table d'association est { x1 -> r1, x2 -> r3, x3 -> r4 }, alors r2 peut-être désalloué s'il n'est pas requis par une instruction en attente / en cours d'exécution.
> Enfin, j'ai essayé de m'amuser un peu avec Vivado HLS en regardant les exemples et essayer de les simuler. Malheureusement, j'ai un problème : aucun fichier csim.tcl n'existe dans les solutions, ce qui semble empêcher la co-simulation. J'ai rapidement cherché sur internet, sans trouver de réponses. J'ai l'impression qu'il s'agit d'un module que je n'ai pas coché lors de l'installation.

## 08/06/2020

Renommage des registres dans l'exemple `appel_asm.s` et création d'un graphe de dépendance entre les différentes instructions. Ces dépendances sont soit issues des registres, soit de le mémoire, soit d'un contexte différent.

Conversion en assembleur du programme de calcul de la suite de Fibonacci :

- [ ] Tableaux :
  - [x] Calcul de la suite de Fibonacci avec mémoïsation

Ceci a été fait pour illustrer le problème des différents types de dépendances lors du renommage.

Quelques recherches supplémentaires ont été faites sur le renommage des registres et les différents problèmes que cela soulève. Quelques exemples :

- Les écritures en mémoire doivent se faire dans le bon ordre. Un modèle (quoique complexe à mettre en place à première vue) est de renommer les adresses mémoires lors du décodage : les adresses doivent donc être calculées à cette étape puis rangées dans une structure capable de les retrouver rapidement
- De la même manière, une écriture après lecture doit se faire dans le bon ordre, ainsi qu'une lecture après écriture.
- Si une exception est levée par le processeur (instruction non reconnue, division par zéro, ...), la bonne table d'association (*i.e.* les bons registres) doit être rétablie pour qu'elle corresponde au programme s'il avait été fait dans l'ordre. Par exemple

    ```asm
    addi a0, x0, 10
    addi t1, x0, 0
    addi t2, x0, 4
    div t1, t2, t1 # t1 <- 4/0 lève une exception
    add a0, t2, 1 # indépendant de l'instruction précédente, et peut-être calculée avant. Lorsque l'exception est levée, a0 doit être rétabli à sa valeur précédente
    ```

> Je me suis concentré aujourd'hui sur le renommage des registres et leurs conséquences sur le réordonnancement des instructions, dans quels cas peut-on effectivement changer l'ordre, ... en faisant l'hypothèse d'une infinité de registres physiques.
> J'ai donc fait un exemple de renommage avec table d'association sur un programme relativement simple (celui d'appel de deux fonctions de la bibliothèque standard) et j'ai fait un graphe des dépendances entre instructions, prenant en compte les dépendances de registres, mémoire, et en prenant en compte l'exécution de code inconnu. Ce n'est pas le cas du processeur bien sûr, mais c'était pour me poser les questions sur les branches, et comment elles doivent être gérées. Pour l'instant, à moins d'avoir un prédicteur de branche, je ne vois que deux solutions possibles :
>
> - soit l'exécution est mise en pause le temps de résoudre le branchement
> - soit les deux branches sont exécutées en "parallèle", avec deux tables d'associations différentes, et lors du branchement, la bonne est conservée.
>
> J'ai également converti un programme C vers assembleur en faisant exprès de faire des écritures et lectures successives en mémoire pour faire ressortir les problèmes de WAR et RAW. Je n'ai pas fait de graphe de dépendance au propre cependant. Pour l'instant la seule idée qui me vient pour éviter d'arrêter l'exécution à chaque lecture / écriture est soit de faire un renommage des adresses mémoire, soit d'avoir recours à une file stockant les adresses occupées (cela me semble plus simple, mais n'enlève pas les dépendances). Dans les deux cas cependant, cela signifie pouvoir calculer les adresses lors du décodage.
> J'ai noté aussi, même si ce n'est pas prioritaire, que les exceptions peuvent poser problème car la table d'association après la levée doit correspondre à celle attendue par le code non réordonné.
> Je me concentrerai demain sur le cas avec nombre limité de registres physiques.

## 05/06/2020

Création d'un décodeur d'instructions RISC-V selon l'architecture `rv32im`.
Le décodage des *immediates* est incorrect cependant. L'opération effectuée semble être celle d'encodage plutôt que de décodage. De plus, la conversion de valeurs hexadécimales en binaire ne semble pas correspondre.

Note du lendemain matin : À part l'extension de signe des immédiats qui n'était pas réalisée, finalement, tout est correct. L'immédiat donné pour les branches et les jumps dans *objdump* sont les adresses absolues, mais dans le binaire, les adresses sont **relatives**.

> J'ai effectivement embrayé sur le décodeur hier, comme conseillé.
> J'ai choisi de le faire en C++ avec une approche très fortement typée pour éviter des erreurs bêtes et d'être sûr de bien comprendre les détails. Ça a été payant, car le compilateur m'a évité pas mal d'erreurs.
> Cependant, dans ma version d'hier, il restait deux problèmes, que j'ai résolu ce matin : le premier était que je n'effectuais pas l'extension de signe des immédiats, c'est corrigé maintenant. Le second était que mon programme était correct, mais que je ne comprenais pas pourquoi : il se trouve que objdump affiche (et c'est logique) les addresses des sauts et branchement en mode absolu, alors que l'encodage se fait avec du relatif. Je ne voyais donc pas pourquoi les valeurs données ne correspondaient pas.
> Je referai peut-être certaines fonctions pour les rendre plus légères.
> (merci d'ailleurs pour la typo, c'est effectivement Fibonacci)

## 04/06/2020

Conversion en assembleur de certains programmes C :

- [x] Arithmétique :
  - [x] Calcul de `x -> (0x5e301a8b + x) <<< 5 - (x & 0x3309)`
  - [x] Expression pour des nombres aléatoires avec un Linear Congruential Generator

- [x] Tests :
  - [x] Un simple match sur les valeurs de `x` : `x < 0`, `0 <= x < 100`, `100 <= x`

- [ ] Boucles :
  - [x] Calcul du miroir d'un registre

- [ ] Fonctions :
  - [x] Appel à `scanf` et `printf` pour récupérer un entier et afficher un entier

> J'ai mis en place le git, disponible ici : [lien](https://github.com/Clematrics/Stage-L3)
> Ma journée se résume à la mise en place de celui-ci et d'écrire les documents pour qu'il soit lisible.
> J'ai aussi commencé à convertir les programmes C en assembleur. Ce que je fais, c'est que je code un programme en C qui fait ce que j'ai écrit dans le fichier ojectifs.md, je vérifie qu'il se comporte bien, puis je code l'assembleur. Suite à mes entraînements d'hier, je ne ressens plus le besoin de regarder ce que le compilateur crée comme assembleur pour le comparer au mien. Je vérifie ensuite que mon code se comporte comme celui du programme C.

## 03/06/2020

Pour compiler en 32 bits sur la toolchain **64 bits** : `-march=rv32im -mabi=ilp32`

L'installation principale a été refaite pour travailler exclusivement en 32 bits en suivant l'ISA `rv32im`.

Réinstallation d'une toolchain en **32 bits** (`rv32im`) :

- Changer l'export de `PATH` :

    ```bash
    export RISC=/opt/riscv32
    path+=("$RISCV/bin")
    path+=("$RISCV/riscv32-unknown-elf/bin")
    export PATH
    ```

- installer la toolchain avec les instructions suivantes :

    ```bash
    ./configure --prefix=$RISC --with-arch=rv32im --with-abi=ilp32
    make
    ```

- installer `pk` avec les instructions suivantes :

    ```bash
    ../configure --prefix=$RISCV --host=riscv32-unknown-elf --with-arch=rv32im
    make
    make install
    ```

- installer `spike` :
d'abord, modifier le fichier `fesvr/htif.cc` et changer la valeur de la macro `TARGET_ARCH` de `riscv64-unknown-elf` vers `riscv32-unknown-elf`. Puis instructions identiques que pour 64 bits.

Pour utiliser `spike`, préciser l'option `--isa=rv32im`.

Quelques ressources supplémentaires :

- Cours du MIT sur l'assembleur RISC-V, le pipeline, et tout un tas d'autres trucs : [lien](https://www.youtube.com/channel/UC1DcxXg6GkAcp2zk2w7U6qQ/videos)
- Document officiel sur les conventions d'appel : [lien](github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md)

> J'ai pu dans un premier temps lire l'ensemble de la spécification de base.
> J'ai également réinstallé une autre version de la toolchain : celle que j'avais installée utilisait l'architecture 64bits avec de nombreuses extensions et cela ne s'avère pas pratique pour comparer du code C avec ce que je produis en assembleur. De plus j'aurais dû la réinstaller de toute façon pour avoir la librairie standard en 32 bits pour ensuite programmer mon petit processeur. Comme ça, s'est fait.
> J'ai ensuite compilé divers programmes C pour voir ce que ça donnait en RISC, me familiariser avec les conventions d'appels, les registres... J'ai trouvé aussi quelques ressources supplémentaires pour m'aider à m'y familiariser.
> Enfin, j'ai commencé à me faire une liste de petits programmes à faire en assembleur. J'ai commencé à en faire un, et je pense que je vais m'imposer comme contrainte de ne pas utiliser les pseudo-instructions disponibles (dans la mesure du possible) pour vraiment me familiariser avec les subtilités (j'ai déjà été confronté à quelques unes, pour le chargement de constantes notamment).

## 02/06/2020

Visio pour définir les objectifs du stage.
Quelques notes et conseils :

- Phase 0 :
  - Pour GCC, utiliser l'indice d'optimisation `-O1` ou `-O2`
- Phase 2 :
  - Le testbench se fait avec des `printf` dans Vivado
  - C++ : restrictions drastiques : les seules choses publiques sont le constructeur et la fonction exposant les broches entrées-sorties. Une seule classe par étage du pipeline.
  - Faire un développement très incrémental et avoir un code *très* propre.

Installation des outils pour travailler avec l'architecture RISC-V : ont été installés

- `riscv-gnu-toolchain`
- `riscv-pk`
- `riscv-isa-tools`

Le `PATH` a été modifié dans `.zshrc` ainsi :

```bash
    export RISC=/opt/riscv
    path+=("$RISCV/bin")
    path+=("$RISCV/riscv64-unknown-elf/bin")
    export PATH
```

Notes pour l'installation **64 bits** :

- installer la toolchain avec les instructions suivantes (une fois les prérequis installés) :

  ```bash
  ./configure --prefix=$RISC
  make
  ```

- installer `pk` avec les instructions suivantes :

  ```bash
  ../configure --prefix=$RISCV --host=riscv64-unknown-elf
  make
  make install
  ```

- installer `spike` avec les instructions suivantes (après les prérequis):

  ```bash
  ../configure --prefix=$RISCV
  make
  sudo make install
  ```

L'ensemble de l'environnement fonctionne à merveille.

La spécification du RISC-V a commencé à être lue : l'introduction a été entièrement lue.

> Je suis sur Ubuntu 18.04, et tout n'a pas fonctionné du premier coup cependant. Il m'a fallu trois essais :
> J'ai commencé par installer riscv-gcc mais j'ai eu des erreurs de linkage lors de la compilation du repo, et je n'ai pas trouvé comment les résoudre. J'ai vu ensuite que spike nécessitait riscv-gnu-toolchain, donc j'ai installé la toolchain, qui contient de toute façon gcc, en espérant ne pas avoir d'erreurs de linkage cette fois. Et effectivement, la compilation s'est bien déroulée. J'ai installé ensuite riscv-pk pour spike. Malheureusement, en essayant de compiler et émuler du code pour vérifier que l'installation était correcte, j'ai eu des erreurs avec spike, et je me suis rendu compte que j'avais utilisé une mauvaise option lors de l'installation de la toolchain. J'ai donc tout recommencé avec la bonne option cette fois, et tout marche désormais très bien.
