# Journal de stage

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