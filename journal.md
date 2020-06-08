# Journal de stage

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
