# Objectifs du stage

## Phase 0 : Familiarisation avec RISC-V (environ une semaine)

- [x] Lire la spécification du RISC-V (le bloc 32I seulement)
- [x] Comprendre l'encodage des instructions en binaire
- [x] Installer un environnement RISC-V (Cross-compiler + simulateur)
- [ ] Transcrire du code C simple en instructions assembleur RISC et le tester.(Reprendre un cours classique d'assembleur et l'adapter pour couvrir : arithmétique entière, expressions, tests, boucles, fonctions et tableaux)
- [ ] Faire une ébauche de décodage d'instructions binaires

### Liste de programmes à coder en assembleur

- [x] Arithmétique :
  - [x] Calcul de `x -> (0x5e301a8b + x) <<< 5 - (x & 0x3309)`
  - [x] Expression pour des nombres aléatoires avec un Linear Congruential Generator

- [x] Tests :
  - [x] Un simple match sur les valeurs de `x` : `x < 0`, `0 <= x < 100`, `100 <= x`

- [ ] Boucles :
  - [x] Calcul du miroir d'un registre
  - [ ] Calculer la suite de Fibonacci en temps $\mathcal{O}(n)$ et espace $\mathcal{O}(1)$
  - [ ] Un jeu de + ou -

- [ ] Fonctions :
  - [x] Appel à `scanf` et `printf` pour récupérer un entier et afficher un entier
  - [ ] Initialisation d'un tableau par une fonction du second ordre
  - [ ] Somme d'un tableau d'une taille donnée
  - [ ] Fibonacci récursif et non optimal
  - [ ] Calcul de la factorielle en récursif

- [ ] Tableaux :
  - [ ] Comparaison de chaînes de caractères
  - [ ] Calcul de la suite de Fibonacci avec mémoïsation

## Phase 1 : Étude de la micro-architecture avec pipeline out-of-order

Bible sur la micro-architecture : « Computer architecture, a quantitative approach » de Hennessy et Patterson.

Étages du pipeline :

- Fetch : Récupérer l'instruction
- Decode. Il faut regarder les registres sources et leur état : sont-ils stables ou en cours d'écriture par d'autres instructions. On renomme aussi le registre de destination pour le faire correspondre à un registre micro-architecturale, cela évite une dépendance supplémentaire. Les instructions jouant avec la mémoire doivent voir les adresses renommées également. Placer l'instruction dans une table pour qu'elle attende son tour.
- Issue : Observation des instructions sur la table : exécuter une parmi celles qui sont prêtes, c'est-à-dire dont les sources sont prêtes.
- Writeback : Récupérer le résultat et notifier que le registre de destination est prêt aux étages du pipeline concernés.
- Commit : Disparition de l'instruction. ⚠ Il y a des choses à faire, notamment pour les instructions jouant avec la mémoire.

## Phase 2 : Implémentation d'un cœur RISC-V 32 bits scalaire out-of-order

Essayer dans un premier temps de programmer un petit additionneur pour prendre en main les outils.
Il y a un petit tuto sur comment faire avec une Pynq-Z1 (on utilisera directement l'interface filaire plutôt que le protocole IP avec Python).

Logiciels :

- Vivado HLS : implémentation de la micro-architecture en C / C++
- Vivado : construction du FPGA à partir de la netlist fournie par Vivado HSL
- Vitis :  interface avec le FPGA : transfère du bitstream à la carte, envoi et récupération des données.

## Phase 3 : Implémentation de l'extension XPAR (ambitieuse, non prioritaire car l'objectif est d'abord d'apprendre)

Pour rajouter du parallélisme déterministe.
