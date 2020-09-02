# Journal de stage

## 31/07/2020

Implémentation d'un nouveau buffer pour la sortie standard, permettant des échanges synchrones avec le PC.
Réécriture du système de débogage avec la bibliothèque JSON, abandon du système de macros.
Il n'y a plus de problèmes.

> J'ai poursuivi un peu mes tests hier et ce matin, et j'ai développé un petit système pour écrire des informations sur la sortie standard sans les problèmes que j'avais avant.
> J'ai les chiffres exacts d'ailleurs de la pile et du tas : ils sont en fait affichés dans la console XSCT lors du téléchargement du programme vers la carte. Résultat : par défaut, le tas fait 8Ko, mais la pile est en fait très limitée, et fait 14Ko. Cela ne laisse donc pas beaucoup de marge.
> J'ai fini par trouver cependant comment augmenter leurs tailles respectives : dans l'explorateurde Vitis, en faisant clic droit  sur l'application ou le système, il est possible de générer le linker script. Dans la boite de dialogue, il y a alors l'option de changer leur taille.  Il y a aussi un fichier lscript.ld (pour moi dans le dossier des sources), qui permet de les modifier.
> Ce que j'ai développé reste cependant utile pour éviter de saturer et réécrire sur le buffer de la sortie standard.
> J'ai fait un commit avec la structure, mais il me reste encore à rendre tout ça un peu plus propre.
>
> Petite correction par rapport à mon mail d'hier : les valeurs erronées que j'avais sont en fait les caractères ascii non supportés par la sortie standard. Cela ne pose pas de problème puisque ce sont tous des symboles de la table étendue.

> J'ai pu boucler aujourd'hui le système de débuggage, avec tout qui fonctionne de bout en bout.
> J'ai donc pu tester mon IP itération par itération avec le status interne de chaque étage.
> Il y a encore quelques améliorations à faire cependant, notamment sur le temps : comme j'attends une seconde entre 2Ko de données pour m'assurer que tout soit transmis en intégralité au PC, cela commence à faire long quand le rapport fait 500Ko.

## 30/07/2020

Poursuite des tests. Tous les problèmes possibles ont été identifiés, il reste à développer des solutions.

> Je vais essayer de faire un rapport simple mais le plus complet possible pour expliquer comment je procède pour débugger mon IP, quels problèmes j'ai rencontrés, et les tests que j'ai effectués pour ensuite les corriger.
>
> L'observation de l'IP ne se fait pas cycle par cycle mais plutôt itération après itération.
> Originellement, ma fonction top ressemblait à :
>
> ```cpp
> void processor(memory_t memory, bit_t* stopped) {
>     #pragma HLS interface s_axilite port=memory
>     #pragma HLS interface s_axilite port=stopped
>     #pragma HLS interface ap_ctrl_none port=return
>
>     bit_t stop = false;
>
>     while (!stop) {
>         #pragma HLS pipeline
>         #pragma HLS dependence variable=stop inter false
>
>         pipeline(memory, &stop);
>     }
>     *stopped = true;
> }
> ```
>
> Je l'ai adaptée ainsi :
>
> ```cpp
> typedef uint32_t tw_large_bool[1]
> void processor(memory_t memory, tw_large_bool run, bit_t* stopped) {
>     #pragma HLS interface s_axilite port=run
>     #pragma HLS interface s_axilite port=memory
>     #pragma HLS interface s_axilite port=stopped
>     #pragma HLS interface ap_ctrl_none port=return
>
>     while (!stop) {
>         #pragma HLS pipeline
>         #pragma HLS dependence variable=stop inter false
>
>         if (*run) {
>             pipeline(memory, &stop);
>             *run = false;
>         }
>     }
>     *stopped = true;
> }
> ```
>
> J'ai ici une variable run, qui est un tableau d'entiers 32bits de taille 1. L'entier sert ici à simuler un booléen,
> car j'ai eu des problèmes en faisant des tableaux de booléens (notamment des bus de données de largeur nulle ou négative dans le code
> Verilog généré). Cela reprend votre conseil de faire un petit tableau pour communiquer dans les deux sens entre l'IP et le cœur ARM.
>
> Du côté du programme de test, j'ai à peu près ce code dans le main:
>
> ```cpp
> *run = true;
> do {
>     while (*run); // j'attends que l'IP finisse une itération
>
>     // récupération des informations
>
>     *run = true; // indication à l'IP de continuer
> } while (!(*stopped));
> ```
>
> Le FPGA tourne en fait en permanence, et je ne synchronise pas vraiment les cycles FPGA avec ceux du cœur ARM, mais je triche en n'autorisant le FPGA à effectuer le contenu du if qu'une seule fois, quand je lui demande : lorsque run est mis à vrai dans le programme de test, le FPGA effectue une seule fois le corps du if dans la boucle, donc une itération du pipeline, et met run à false, ce qui l'empêche d'effectuer une itération supplémentaire.
> Le programme de test attend de son côté que run passe à faux, indiquant que l'IP est en train d'effectuer un cycle. Je devrais mettre un délai entre l'instant où run passe à faux et la récupération des informations, mais je n'en ai jusque là pas eu besoin, l'itération se finissant très rapidement.
> Pendant que je traite les informations issues de l'itération effectuée, l'IP continue de tourner, mais ne fait pas ce qu'il y a dans le if.
> Dès que j'ai récupéré et affiché ce qui m'intéressait, je remets run à vrai pour lancer une nouvelle itération du pipeline.
> Je continue ainsi jusqu'à ce que l'IP indique avoir fini.
>
> Maintenant, pour récupérer les informations de l'IP, je définis une structure DebugInfo que j'adjoins à la définition de la fonction top, de la fonction du pipeline, et des fonctions de chaque étage.
> Pour simplifier, je vais juste l'écrire seulement pour l'étage de Fetch.
>
> ```cpp
> struct FetchStatus {
>     program_counter_t pc;
>     bit_t             did_smth;
> };
>
> struct DebugInfo {
>     FetchStatus fetch_status;
> };
> ```
>
> Ma fonction top est maintenant :
>
> ```cpp
> typedef uint32_t tw_large_bool[1]
> void processor(memory_t memory, tw_large_bool run, bit_t* stopped, DebugInfo* dbg) {
>     #pragma HLS interface s_axilite port=dbg
>     #pragma HLS interface s_axilite port=run
>     #pragma HLS interface s_axilite port=memory
>     #pragma HLS interface s_axilite port=stopped
>     #pragma HLS interface ap_ctrl_none port=return
>
>     while (!stop) {
>         #pragma HLS pipeline
>         #pragma HLS dependence variable=stop inter false
>
>         if (*run) {
>             pipeline(memory, &stop, dbg);
>             *run = false;
>         }
>     }
>     *stopped = true;
> }
> ```
>
> Ma fonction pipeline (simplifiée, avec seulement l'étage de fetch) :
>
> ```cpp
> void pipeline(memory_t memory, bit_t* stopped, DebugInfo* dbg) {
>     fetch_stage.interface(memory, dbg);
> }
> ```
>
> Et enfin l'étage de fetch :
>
> ```cpp
> class FetchStage {
>     program_counter_t pc;
> public:
>     FetchStage();
>     void interface(memory_t memory, DebugInfo* dbg) {
>         bit_t do_smth = /* conditions pour activer l'étage de fetch */;
>
>         if (do_smth) {
>             // fait ce qu'il y a à faire
>         }
>         else {
>             // ...
>         }
>
>         // Transmission des informations vers la structure de débuggage.
>         dbg->fetch_status.program_counter = program_counter;
>         dbg->fetch_status.did_smth        = do_smth;
>     }
> };
> ```
>
> J'ai aussi tendance à rajouter, en plus de l'état interne des étages, les informations échangée entre étages.
> Bien évidemment, cela impacte beaucoup les performances de l'IP et du pipeline, puisque beaucoup plus d'accès à la mémoire
> est nécessaire : l'II est augmenté à 2 dès que j'utilise le système pour controler les itérations du pipeline.
> Il reste à 2 si je ne récupère que les informations échangées entre les étages, mais peut grimper à 30 si je récupère
> tout (le plus coûteux étant la table d'issue).
> Comme l'objectif est d'avoir un processeur sans tout ce lourd système de débuggage, j'ai une macro qui permet de contrôler
> si c'est activé et désactivé. Cela me permet notamment de contrôler que l'II reste à 1, que tout se passe correctement dans l'onglet d'analyse
> de Vivado HLS, etc.
>
> Jusque là, c'est quelque chose que j'avais fait la semaine dernière relativement rapidement. Il m'a suffit d'un ou deux jours pour avoir tout cela qui fonctionne correctement, ou presque.
> L'interruption du pipeline est une des premières choses que j'ai essayé, après avoir implémenté l'étage de fetch et de décodage, pour m'assurer qu'ils alternaient bien entre eux, et cela marche bien. J'ai ensuite implémenté la récupération des informations échangées entre étages, et adapté le programme de test dans Vitis pour afficher ces informations dans la console. Cela a bien marché aussi, mais il y avait très peu d'informations à afficher par itération.
> Là où cela se corse, c'est lorsque je veux récupérer des informations plus conséquentes comme la table d'issue.
>
> Le premier souci est que la console de débuggage de Vitis est limitée en nombre de lignes : une bonne partie des informations disparaissent, et le paramètre pour demander à Vitis un buffer illimité n'affecte pas cette console. J'ai donc cherché à rediriger les informations directement vers un fichier. J'ai vu des options qui semblaient faire ça dans les options de configuration, mais rien n'a fonctionné, et aucune documentation n'explique ce que font vraiment ces paramètres. J'ai fini par trouver des commandes sur cette page (https://www.xilinx.com/html_docs/xilinx2018_1/SDK_Doc/xsct/use_cases/xsdb_using_jtag_uart.html) qui permettaient de rediriger vers un fichier (les commandes importantes étant 'set fp [open <chemin vers le fichier> w]' et 'readjtaguart -handle $fp')
> J'ai donc retrouvé les scripts tcl des configurations, et j'y ai rajouté ces commandes. Comme les scripts sont regénérés à chaque fois que l'IP est lancée depuis Vitis, j'ai copié le script a un endroit, et pour déclencher l'IP, je déclenche simplement le script depuis la console XSCT.
>
> Pour afficher les informations de débuggage, j'ai voulu reprendre la librairie de JSON que j'utilise déjà pour la simulation dans Vivado HLS.
> C'est du C++11, ça marche bien, et ça permet de bien visualiser les différents états, itérations après itérations, de masquer ce qui ne m'intéresse pas ou de rechercher rapidement des choses. Bref, cela semblait être l'outil parfait.
> Sauf qu'en utilisant cette bibliothèque dans Vitis, je me retrouvais en permanence avec des exceptions 'std::bad_alloc'.
> J'ai d'abord pensé que cela venait de la bibliothèque, qui ne marchait pas sur ARM.
> J'ai donc abandonné la bibliothèque et fait mon propre petit système JSON, bien qu'il soit moins élégant et pratique.
> J'utilisais extensivement des macros pour imprimer sur la sortie standard les informations, en passant par des structures et des surcharges d'opérateurs, rien d'interdit en C++ donc, même pour de l'ARM, et en testant mon code sur des simulateurs pour ARM, cela marchait très bien.
> Mais j'ai encore eu des exceptions 'std::bad_alloc' en testant sur le FPGA.
>
> J'ai donc décidé hier d'investiger pour comprendre le modèle mémoire de la carte.
> Le principale souci, c'est qu'il est difficile de savoir comment sont allouées les ressources mémoires sur ce genre d'appareil, surtout quand les spécifications du langage ne sont pas claires à ce sujet, laissant les compilateurs libres de choisir.
> La première chose que j'ai essayée, c'est de comprendre quelle est la taille maximale que peut prendre un std::vector.
> Comme je suis incertain de la granularité de la mémoire, j'ai fait un std::vector d'entiers 64bits en faisant l'hypothèse que les éléments sont consécutifs dans la mémoire.
> Avec le code suivant :
>
> ```cpp
> const uint32_t size = 1 << 9;
> std::vector<uint64_t> vect(size);
> ```
>
> je n'ai pas de souci. Par contre, avec size = 1 << 10, j'ai l'exception std::bad_alloc.
> Comme les données d'un vecteur sont censées être stockées sur le tas, j'en déduis que je ne peux pas faire d'allocations supérieures à 4Ko.
> En essayant de faire deux vecteurs avec size = 1 << 9, j'ai aussi l'exception. Donc j'en déduis que c'est en fait le tas qui est limité à une taille totale de 4Ko.
> Cela se confirme en essayant d'allouer d'autres structures sur le tas, comme des std::string de taille conséquente, ou des tableaux dynamiques avec 'new uint64_t[size]'.
> Chose étrange : la taille maximale varie selon la structure : pour les vecteurs, c'est 4Ko, avec size = 1 << 9 ; pour les tableaux alloués dynamiquement (new uint64_t[size]), c'est 2Ko avec size = 1 << 8 ; pour les strings, c'est aussi 2Ko.
> Pour confirmer d'ailleurs qu'il s'agisse bien du tas, et pas du stack, j'ai essayé d'allouer un std:array, qui lui devrait être alloué sur le stack. Résultat : je peux allouer la taille que je veux, même si je dépasse les limites physiques de la mémoire, sans avoir d'exceptions, donc std::array est bien sur la pile (ce qui se confirme d'ailleurs en regardant le code assembleur généré), et le reste sur le tas.
>
> Il reste enfin un dernier problème, qui est celui de l'échange d'informations.
> Lorsque j'essaye d'envoyer des informations de débuggage sur la sortie standard, j'ai régulièrement des informations complètement tronquées, ou seule la fin apparait.
> La première chose que j'ai essayée est de flusher régulièrement la sortie standard pour voir si je récupérais plus d'informations, notamment celles du début, ce qui n'a pas été le cas. Le flush n'a en fait l'air d'avoir aucun effet sur la transmission du contenu du buffer vers le PC. En particulier, il n'a pas l'air de bloquer l'ajout de données au buffer, même quand celui-ci est plein.
> J'ai donc essayé de déterminer sa taille en envoyant des nombres croissants et en regardant si les zéros initiaux étaient effacés au profit de valeurs plus grandes. J'en ai déduit que le buffer est au moins de 2Ko, comme pour les std::string, mais pas plus de 4Ko.
> La solution auquelle je suis arrivé est d'envoyer dans la sortie standard les informations par paquet de 2Ko, et entre chaque paquet d'attendre 1 seconde. Cela laisse le temps au buffer d'être envoyé vers le PC avant de réécrire dedans.
> En effectuant quelques tests (le code ci-dessous par exemple), j'ai bien obtenu 32 fois le contenu de 'string'.
>
> ```cpp
> const uint32_t size = 1 << 11;
> std::string string;
> string.resize(size);
>
> const uint16_t markers = 256;
> for (uint32_t i = 0; i < size; i++) {
>     string[i] = (char)(i * markers / size);
> }
>
> for (uint16_t i = 0; i < 32; i++) {
>     std::cout << string;
>     sleep(1); // sleep for 1 second
> }
> ```
>
> En modifiant la ligne 'std::cout << string;' par 'std::cout << string << string;', je n'obtiens par contre pas le bon résultat : j'obtiens 20 paquets de 2Ko, avec des valeurs erronées au milieu, alors que je devrais recevoir 64 paquets. Cela semble donc confirmer mes hypothèses.

> Voilà ce que j'ai trouvé en tout cas ces derniers jours. Je vais essayer de monter un petit système permettant un échange fiable de données. J'ai quelques idées déjà de comment faire.
> J'espère que ce que j'ai décrit est assez clair, n'hésitez pas si vous avez des questions ou des remarques. Je vais aussi essayer de documenter ça encore plus clairement, en poursuivant aussi quelques tests de mon côté.


## 29/07/2020

Tests pour comprendre d'où vient les erreurs vis-à-vis du rapport. Le tas semble être très limité, ce qui pourrait être la source du problème. Il y a aussi un problème de taille du buffer de la sortie standard, et d'asynchronicité.

>  J'ai trouvé la motivation pour poursuivre le débuggage de l'IP, et j'ai donc continuer mon investigation.
> Comme la documentation sur le sujet (ou du moins ce que j'ai pu trouver) est très maigre ou évasive, j'ai fait moi-même des expérimentations pour comprendre comment se comporte la carte. Je me suis surtout concentré sur les allocations mémoires (le stack et le heap) et comment les informations sont renvoyées vers le PC, car c'est je pense ce qui me bloque le plus. Ce que j'ai trouvé m'a d'ailleurs très surpris : l'allocation sur le stacksemble illimitée, quitte à dépasser les capacités physiques : il n'y a aucune vérification puisque pas d'OS je pense.
> Par contre l'allocation sur le tas semble limitée à environ 4 kilo-octets au total, et il y a cette fois des vérifications effectuées par le langage, puisque j'obtiens des exceptions. Enfin, pour ce qui est de l'échange de données (via std::cout), il y a un buffer cyclique d'environ 2Ko, sans sécurité, donc pour remonter les informations de débuggage, je dois le faire par paquets de 2Ko, puis attendre 1 seconde le temps que la transmission ait eu lieu, avant de réécrire dans le buffer.
>
> Bref, il faut maintenant que je trouve un moyen d'automatiser cela pour extraire les données pertinentes sans me soucier de ses soucis de taille. Mais au moins je sais ce qui me limite et où aller !

## 28/07/2020

Tentative d'amélioration du système de débogage de l'IP. Les macros seules sont difficiles à utiliser pour exporter un fichier au format JSON. Utilisation de structures templatées pour compléter. Rajout de macros obscures pour compléter le tout. Il reste cependant des problèmes : le rapport est incomplet, erroné par endroits. Problème de taille du stack ?

> Ces deux derniers jours ont été très frustrants (c'est d'ailleurs la raison pour laquelle je n'ai rien push lundi : j'ai tout effacé pour recommencer).
> Étant donné que j'essaie de vérifier le bon fonctionnement de l'étage d'issue et de sa table d'instruction, mon objectif est d'extraire l'état de la table pour l'afficher cycle après cycle. Au niveau de l'IP et de la synthèse, je n'ai eu aucun souci.
> Je me suis cependant heurté à de nombreux problèmes ensuite :
> - soit c'est du côté de Vitis : la console ne permet pas d'afficher la totalité des informations affichées, à cause d'un buffer limité et non réglable, et le renvoi vers un fichier ne fonctionne qu'à moitié
> - soit c'est du côté de la carte : le stack semble trop limité pour faire trop d'appels récursifs, et stocker simultanément un buffer de données
> - soit c'est du côté du langage : faire un programme en C++11 qui évite les erreurs pour extraire des données, mêmes partielles, tout en respectant les contraintes de la carte ci-dessus, est ... compliqué.
>
> Bref, je vais peut-être revoir ma copie. Je devrais normalement m'assurer que tout fonctionne correctement avant d'avancer, mais je suis peu motivé par tous ces problèmes, donc j'hésite à continuer plutôt l'implémentation du processeur, et revoir tout le débuggage plus tard.

## 25/07/2020

Quelques optimisations et simplification de l'étage de commit. Première implémentation, encore non testée, de l'étage d'issue.

## 24/07/2020

Résolution du problème de dépendence dans l'étage de commit. L'II de l'IP passe à 1 !

> Je pense avoir compris l'idée : l'étage de commit transmet à lui-même au cycle suivant ce qu'il aimerait dépiler. La décision du dépilement se fait au début du cycle suivant, et donc se base sur la première entrée en fin de cycle précédent (qui donc est lue en fin de cycle seulement). Et dans ce cas, j'ai bien la lecture qui précède l'écriture. Voilà comment j'ai adapté mon code (simplifié ici) :
>
> ```cpp
> // Structure transmettant les informations depuis l'étage de commit vers lui-même pour le cycle suivant
> struct CommitToCommit {
>     bool rob_was_empty;
>     bool first_entry_done;
> };
>
> // Fonction principale de l'étage de décodage
> void decode(CommitToCommit from_previous_cycle, CommitToCommit* to_next_cycle, /* d'autres > paramètres */) {
>     bool can_dequeue        = !from_previous_cycle.rob_was_empty && from_previous_cycle.> first_entry_done;
>     bool can_queue          =  /* ... */
>     /* mise à jour des pointeurs, empilage de la nouvelle entrée, ...
>     *  rob_head est donc potentiellement modifié
>     */
>
>     to_next_cycle->rob_was_empty    = rob_empty;
>     to_next_cycle->first_entry_done = rob[rob_head].done;
> }
> ```
>
> Cela a éliminé la dépendance que j'avais entre la lecture et l'écriture sur 'rob_head'.
> Cependant, cela a amené une nouvelle dépendance, plus obscure, que j'ai eu du mal à comprendre, entre le 'load' de 'rob[rob_head].done' et le '&&' en première ligne  de 'decode'.
> À force d'essayer des #pragma dans tous les sens, j'ai fini par saisir que ça ne servait à rien, et que le souci vient semble-t-il de la façon dont doivent être ordonnancés les appels dynamiques vers la mémoire : en observant l'ordonnancement, j'ai remarqué que les chargements de la mémoire étaient systématiquement placés en tout début de cycle, et en duraient plus d'un. Comme le '&&' et la modification de 'rob_head' prenaient trop de temps sur le premier cycle, j'ai compris que Vivado n'avait d'autre choix que de mettre la lecture du ROB dans un deuxième cycle, causant l'II=2. Ai-je vu juste ?
>
> Toujours est-il que j'ai adapté ma méthode pour ne lire qu'une seule entrée du ROB (la deuxième pour être précis), et au début de l'étage, pour forcer l'ordonnancement au début de cycle. Cela a été un peu casse-tête dans un premier temps pour garantir que ça fonctionne correctement, mais après plusieurs essais et tests sur FPGA, je pense y être parvenu avec un système simple.
> Et j'ai bien un II=1, victoire donc ! Je pense que je vais avoir quelques soucis lorsqu'il va falloir mettre à jour les instructions terminées par contre. J'ai une petite idée cependant, je verrai si elle fonctionne lorsque j'aurai implémenté l'étage de write back.
>
> Petite note d'ailleurs : j'ai eu un warning indiquant que la période de 10ns ne pouvait pas être satisfaite. Je l'ai passée à 15ns et Vivado a arrêté de râler. Dois-je viser aussi une période de 10ns pour mon implémentation ? Ou est-ce difficile à tenir ?
>
> J'entame demain l'étage d'issue et le système de blocage.

## 23/07/2020

Implémentation en grande partie du ROB. Il y a toujours des problèmes de dépendences empêchant d'avoir un II=1.

> La #pragma dependence résout effectivement le problème que j'avais, merci !
>
> J'ai implémenté aujourd'hui une bonne partie de l'étage de commit, notamment pour avoir le ROB de prêt.
> J'ai un code qui fonctionne bien, mais je me suis heurté une nouvelle fois à des soucis de dépendances, autour du ROB.
>
> Pour mettre dans le contexte, je l'implémente avec une liste circulaire : un tableau de taille fixe, et un indice de début et de fin de liste. L'indice de fin pointe sur la prochaine cellule disponible. L'indice auquel une instruction est rajoutée correspond à l'identifiant unique donné par l'étage de décodage (appelé 'token' dans mon code). Cela permet notamment, si une instruction est indiquée comme terminée par le write back, de faire `ROB[id].ready = true`.
>
> La première dépendance arrivait sur le tableau en lui-même, puisque dans le même cycle je lis une entrée (celle en début de liste, pour vérifier si elle peut-être enlevée) et j'en écris potentiellement une nouvelle si l'étage de décodage en transmet une.
> Si je comprends bien cette dépendance, c'est parce que Vivado HLS n'a aucun moyen de vérifier que je lis et j'écris toujours à deux endroits différents (ce qui est bien le cas, car si c'était le cas, le ROB serait plein, ce qui bloque alors le pipeline, et donc empêche l'envoi d'informations depuis l'étage de décodage vers le ROB). J'ai donc ajouté une #pragma dependence sur le tableau servant de ROB, et la dépendance a disparu. Cela m'a par contre imposé de réimplémenter mon buffer circulaire directement dans la classe du Commit,
diminuant la généricité que j'avais.
>
> La seconde dépendance concerne le pointeur de début de la liste, et je n'arrive pas à l'enlever : en effet, je dois lire au début du cycle cette valeur pour lire la première entrée du ROB et décider si elle peut-être enlevée, mais je dois aussi l'incrémenter si la première entrée est effectivement retirée, et ce dans le même cycle.
>  Je me demande donc : est-ce que mon architecture pour le ROB est correcte ?
>
> J'ai pensé ce soir à une autre méthode pour construire le ROB, qui consiste à créer un certain nombre de registres r1, r2, ... rN, avec N fixé, où r1 contient l'entrée la plus ancienne du ROB, et rN la plus ancienne, et à chaque fois qu'une instruction est retirée du ROB, r2 est transféré vers r1, r3 vers r2, ... un peu à la manière d'un hls::stream, mais cela me paraît terriblement inefficace et coûteux.
> De plus, cette méthode a l'inconvénient de ne pas pouvoir mettre à jour une instruction comme terminée en accédant directement le registre où elle est.
>
> Une autre idée que j'ai eu serait de ne pouvoir faire qu'une seule des deux opérations entre empiler et dépiler du ROB.
> Mais cela signifie que dans un premier temps le ROB va se remplir sans extraire d'instructions, puis une fois plein, il bloquera le pipeline et en dépilera une seule. Comme le ROB ne sera plus plein, le pipeline sera débloqué et une nouvelle instruction sera ajoutée au ROB, bloquant à nouveau le pipeline, etc. Une instruction n'est donc commit qu'un cycle sur 2. J'en conclut qu'avec cette approche, c'est comme si j'avais un pipeline deux
fois moins efficace, équivalent à un II=2, ce qui ne me paraît pas très intéressant.
>
> Auriez-vous une indication pour implémenter le ROB correctement ? Est-ce que ce genre de compromis est obligatoire ? Ou alors celui-ci est acceptable si
d'autres harts peuvent reprendre la main les cycles suivants ?

> Je n'avais pas pensé à séparer le pointeur de tête et de queue dans des étages différents, et cela résout effectivement quelques soucis, notamment au niveau du blocage. Je vais revoir l'intégralité de mon code pour renforcer la propriété "l'étage qui doit écrire dans une variable doit en être propriétaire".
> Cependant, je n'arrive pas à voir comment cela résout la dépendance du pointeur de tête.
> J'ai un code comme cela :
>
> ```cpp
> rob_entry_t first_entry = rob[rob_head];                  // Lecture de rob_head, pour ensuite accéder à la case du tableau contenant l'entrée en tête du ROB
> bool can_dequeue        = !rob_empty && first_entry.done; // vérification si l'on peut dépiler la première entrée
> bool can_queue          =                                 // vrai si l'étage de décodage a fait la requête d'empiler une instruction
> if      (!can_queue && !can_dequeue)  ;                // rien à faire
> else if (!can_queue &&  can_dequeue)  rob_head++;      // il faut seulement dépiler : j'incrémente le pointeur de tête pour enlever la première entrée
> else if ( can_queue && !can_dequeue)  /* des choses */ // il faut seulement empiler : j'empile l'instruction transmise par l'étage de décodage
> else   /* can_queue &&  can_dequeue*/ rob_head++; /* et d'autres choses */ // il faut à la fois dépiler la première entrée et empiler celle donnée par l'étage de décodage. Je dois donc entre autre incrémenter le pointeur de tête
> ```
>
> Le problème, c'est que Vivado HLS m'indique qu'il lit depuis 'rob_head' à la première ligne, puis qu'il écrit dans 'rob_head' lors du deuxième et du dernier cas du if-else, et il se passe plus d'un cycle entre les deux car il doit d'abord extraire 'first_entry' de la mémoire à la première ligne, pour ensuite déterminer si 'first_entry.done' autorise à retirer l'instruction.
> Comme une seule variable n'est concernée, je ne vois pas comment faire pour résoudre cette dépendance. En faisant passer l'écriture avant la lecture peut-être ? Mais je vois mal comment faire ici, car c'est le même étage.

## 22/07/2020

Il y a des problèmes de dépendences visiblement, empêchant d'avoir un pipeline avec II=1. Tentative de déplacer dans le code le lieu ou les variables qui posent problème sont lues et écrites.

> J'ai pu implémenter la quasi-totalité de l'étage de décodage aujourd'hui, et tout fonctionne correctement.
> Cela reprend en grande partie le code que j'avais produit précédemment, mais avec des optimisations supplémentaires et une réorganisation de certains calculs, pour que tout ceux du même genre soient au même endroit, et ce pour éviter les erreurs de programmation.
> J'ai par contre à nouveau quelques soucis avec le pipelining et l'II, et je ne suis pas sûr de comment améliorer ça.
> En particulier, la gestion du booléen 'stop' pour indiquer quand le processeur doit s'arrêter.
> Je dispose grosso-modo d'un code dans ma fonction top comme cela :
>
> ```cpp
> FetchToDecode fetch_to_decode;
> DecodeToFetch decode_to_fetch;
> bool stop;
> while (!stop) {
>     #pragma HLS PIPELINE
>     fetch_stage.interface(decode_to_fetch, &fetch_to_decode);
>     decode_stage.interface(fetch_to_decode, &decode_to_fetch, &stop);
> }
> ```
>
> Cependant, comme le décodage écrit dans 'stop' à la fin du pipeline mais que 'stop' est lu au début, il y a un souci, c'est ce que vous m'expliquiez dans le mail précédent si j'ai bien compris.
> J'ai alors essayé avec une variable temporaire, pour déferrer l'écriture au début, mais cela n'a pas amélioré la situation :
>
> ```cpp
> FetchToDecode fetch_to_decode;
> DecodeToFetch decode_to_fetch;
> bool stop;
> bool stop_temp;
> while (!stop) {
>     #pragma HLS PIPELINE
>     stop = stop_temp;
>     fetch_stage.interface(decode_to_fetch, &fetch_to_decode);
>     decode_stage.interface(fetch_to_decode, &decode_to_fetch, &stop_temp);
> }
> ```
>
> Aussi, Vivado HLS semble obstiné à vouloir mettre dans la même itération l'étage de fetch et de décodage, ce qui conduit à un II de 3, et je ne sais pas comment améliorer cela.
> J'ai essayé avec des variables intermédiaires, et j'obtiens un II de 2, avec les deux étages découplés, mais ça me paraît très peu élégant et cela empêche potentiellement des optimisations :
>
> ```cpp
> // Connexion pour fetch
> FetchToDecode fetch_out;
> DecodeToFetch fetch_in;
> // Connexion pour decode
> FetchToDecode decode_in;
> DecodeToFetch decode_out;
> bool stop;
> bool stop_temp;
> while (!stop) {
>     #pragma HLS PIPELINE
>     stop = stop_temp;
>     fetch_stage.interface(fetch_in, &fetch_out);
>     decode_stage.interface(decode_in, &decode_out, &stop_temp);
>
>     fetch_in = decode_out;
>     decode_in = fetch_out;
> }
> ```
>
> Enfin, j'ai poursuivi ma réflexion sur les exceptions et les blocages, puisque j'ai dû considérer plus sérieusement le cas où une instruction est invalide (en tout cas si l'étage de décodage est en mesure de le détecter) et le cas où le ROB est plein.
> J'ai pour l'instant l'idée de considérer une instruction invalide comme une instruction normale, qui s'insère dans le ROB, etc, et qui une fois arrivée au commit et extraite de ROB, provoque l'arrêt du processeur. Cela a l'avantage de conserver les exceptions in-order je pense. Y a-t-il une autre façon de faire, ou est-ce que l'idée est bonne ?
> Pour les blocages par contre, je coince à nouveau dessus. Voici en particulier un scénario sur lequel je bloque :
>
> si, lors d'une itération i du pipeline :
> - le ROB reçoit une instruction et celle-ci le remplit, il doit donc bloquer les étages du dessus
> - l'étage de décodage décode une instruction
> - l'étage de fetch prend une instruction de la mémoire
Dès lors, à l'étage i+1 :
> - l'étage de fetch est bloqué et ne fait rien
> - l'étage de décodage reçoit les infos du fetch de l'itération i, mais il doit arrêter ce qu'il fait. Lorsque le pipeline redémarre, il aura oublié ce qu'il a reçu, non ? J'ai l'impression qu'il faut un registre pour stocker temporairement cette information, comme une espèce de minibuffer entre étages, au cas où il y a ce genre de blocage.
> - enfin, le ROB reçoit ce qu'a décodé l'étage de décodage lors de l'itération précédente, mais il ne peut rien en faire, car il est à priori plein aussi. La seule chose que je vois est qu'il stocke aussi dans une salle d'attente de taille 1 cette information.
>
> Avec ma méthode, il faut par contre détecter quand tous les blocages sont terminés, et plutôt que de reprendre le cycle normal, il faut d'abord consommer ce qu'il y a dans la salle d'attente, puis reprendre normalement, mais tout cela commence à me paraître compliqué.
> J'avais aussi pensé à faire le blocage lorsqu'il ne reste qu'une seule place dans le ROB, comme ça la dernière place peut absorber l'information de l'itération précédente, puis bloquer le pipeline, mais le problème demeure toujours pour les autres étages.
> Y a-t-il une autre façon de faire ?

## 21/07/2020

Création d'un système de macros pour extraire les informations au format JSON sans passer par la bibliothèque.

> J'ai poursuivi mon investigation aujourd'hui. J'ai fait pas mal de tests pour éliminer les hypothèses au fur et à mesure.
> Ce n'est effectivement ni dû à l'inlining, ni à un problème de valeurs mal initialisées. > J'en ai déduit que cela venait du pipelining, et je pense avoir raison :
après plusieurs péripéties dans l'obscure documentation et plusieurs tests sur de toutes petites fonctions, je me suis rendu compte que je comprenais mal comment la pragma PIPELINE se comporte :
> - elle était premièrement mal placée dans mon programme, et ne servait à rien là où je l'avais mise
> - elle ne cherche pas à pipeliner une instruction après l'autre, contrairement à ce que je croyais. C'est en fait Vivado qui décide comment ordonnancer les instructions. Donc en l'occurence si les deux étages se suivent dans la même itération du pipeline, c'est normal, et c'est ce qui m'a causé quelques maux de tête en regardant l'onglet d'analyse.
>
> Est-ce que ce que j'ai déduit est correct ?
> Je faisais aussi une dernière erreur, qui était de mélanger deux méthodes pour décider si un étage devait se verrouiller.
> J'en ai profité pour faire plus de tests sur comment éliminer les dépendances pour réduire le II au maximum.
> En mode debug, l'IP n'a pas un super II car je dois interrompre l'IP, ce qui est coûteux, mais je vérifie maintenant systématiquement que sans cela, l'II est bien de 1.
> Enfin, j'ai aussi continué d'améliorer mon système de débuggage côté IP pour accélérer les sessions de test.

## 20/07/2020

Reprise à zéro du code du processeur pour recommencer pas à pas l'implémentation et tester petit à petit chaque partie du code. Mise en place du système de débogage pour contrôler manuellement quand les cycles se déclenchent. Attention : il faut passer par un tableau / pointeurpour que le circuit et le programme puissent avoir les droits d'écriture des deux côtés, mais pour des booléens, il faut que le tableau ait des éléments de taille au moins 32bits (visiblement) pour que la synthèse ne plante pas.

Problèmes au niveau du pipelining des étages.

Le système de décodage avec la bibliothèque JSON ne fonctionne pas. Ressources trop limités ?

> Mon premier objectif a été de mettre en place les outils pour faciliter le débuggage de l'IP, et je suis assez content de ce que j'ai fait :
> Je dispose d'un système qui me permet d'exécuter une itération du pipeline à la fois, afin d'étudier la mémoire et autres signaux entre chaque cycle, ce qui devrait bien aider à débugger. J'utilise pour cela un booléen indiquant lorsque l'IP a fini une itération et attend. Il suffit alors au programme dans Vitis d'attendre que ce booléen soit vrai, d'extraire les infos nécessaires, et de remettre le booléen à faux pour que l'IP continue.
> J'ai eu du mal à mettre en place un port de communication dans les deux sens : j'ai essayé avec des tableaux de booléens, de taille 1, 2, 3 ou 4, mais soit la synthèse dans Vivado échouait (car le code Verilog produit contenait des fils avec un nombre de bits négatifs) soit les valeurs n'étaient pas mises à jour correctement dans Vitis. J'ai fini par contourner le problème en utilisant des faux booléens d'une largeur de 32bits dans un tableau de taille 1.
>
> J'ai aussi unifié mon système de débuggage pour permettre de simuler le code C en imitant le fonctionnement du pipeline sur l'IP : les sorties des étages sont dissociées des entrées, donc tous peuvent être exécutés "simultanément", et je transvase ensuite les sorties vers les entrées correspondantes avant le cycle suivant. Cela n'impacte d'ailleurs pas la synthèse grâce à un système de macros.
>
> Par contre, je bloque sur la première implémentation du pipeline :
> j'ai fait en sorte que l'étage de fetch soit verrouillé tant que l'étage de décodage n'a pas le prochain pc, et vice-versa : l'étage de décodage est bloqué tant que l'étage de fetch n'a pas extrait une instruction de la mémoire.
> Je devrais donc avoir un cycle ou seul fetch est actif, le suivant ou seul decode est actif, etc. (en tout cas au niveau où j'en suis dans l'implémentation, car seuls ces deux étages existent)
> Cependant, lorsque je teste mon IP, les deux sont actifs simultanément. J'ai pensé que cela pourrait venir :
> - soit de l'inlining des étages (mais il semblerait que ce ne soit pas le cas, j'ai essayé avec et sans, et cela n'a rien changé)
> - soit c'est un problème de valeurs mal initialisées (après plusieurs tests, j'ai du mal à y croire, mais j'ai encore quelques hypothèses à vérifier)
> - soit cela vient du pipelining qui fait en sorte que les deux étages sont exécutés l'un après l'autre dans le même cycle, auquel cas je ne vois pas comment je pourrais résoudre ça
>
> Je vais continuer demain à investiguer.

## 17/07/2020

Essais pour comprendre comment intéragir avec le circuit.

Ce qu'il faut retenir : l'IP démarre dès qu'elle est uploadée sur la carte, et même si le programme sur ARM n'a pas encore été lancé. Il faut donc faire attention.

## 16/07/2020

Prise en main plus approfondie de Vitis. La mise à jour de la spécification hardware n'a pas l'air de fonctionner cependant.

> J'ai commencé à prendre en main Vitis, la création de projet, les liens avec la plateforme, etc. J'ai par contre eu des diffcultés à afficher un hello world dans la console : je me suis perdu dans les configurations de débuggage inutilement et autres fenêtres, mais j'ai fini par trouver un tutoriel qui a résolu mes différents problèmes : il fallait modifier les ports de communication puisque j'utilise le câble USB, et aussi écouter le bon cœur ARM.
> J'ai pu modifier et lire la mémoire et les arguments de mon IP.
> Par contre, j'ai encore quelques problèmes lorsque j'essaie de mettre à jour mon code : une fois dans Vitis, je n'arrive pas à mettre à jour la plateforme via le fichier xsa sans avoir une erreur (il m'indique qu'elle est out-of-date, donc j'utilise l'option "Update Hardware Specification" car je n'en trouve pas d'autre). Cela a même l'air de la supprimer complètement, ce qui fait que je ne peux plus compiler l'application. La seule méthode que j'ai trouvée est de supprimer le workspace et de tout recommencer.

## 15/07/2020

Réception de la carte Xilinx Pynq-Z1.
Le transfert du bitstream depuis Vitis fonctionne correctement.
Il reste à trouver comment programmer le micro-controlleur pour intéragir avec le circuit.

## 13/07/2020

Amélioration de la sélection d'une instruction prête dans la table d'issue.
Poursuite de l'implémentation des différentes unités de calcul.
Amélioration de l'étage de décodage.

> J'ai aujourd'hui commencé et presque fini l'implémentation des différentes unités de calcul.
> Il me reste cependant encore des tests à effectuer pour vérifier que tout colle au niveau des spécifications de RISC-V.
> Je me rends compte qu'il y a encore beaucoup d'optimisations à faire dans le décodage, notamment comment les informations à propos de l'instruction sont passés d'un étage à l'autre. Pour l'instant je me contente d'un type Kind, que l'étage de décodage déduit et qui indique si l'instruction est une instruction arithmétique, si c'est un load, un store, un branch, ...
> Cependant comme les tests s'effectuent sur ce type, les comparaisons sont encore coûteuses.
> Je pense améliorer cela en ne déduisant que l'unité de calcul concernée et la sous-catégorie de cette unité de calcul.
>
> Je me suis également posé beaucoup de questions :
>
> La première concerne la méthode de simulation du pipeline : ce que l'on cherche à simuler est ce comportement, où chaque étage fonctionne en parallèle des autres pendant chaque cycle
>
> | cycle 1    | cycle 2       | cycle 3 |
> |:-----------|:--------------|:--------|
> | fetch      | -> decode     | ->      |
> | decode     | -> issue      | ->      |
> | issue      | -> write back | ->  ... |
> | write back | -> commit     | ->      |
> | commit     | -> fetch      | ->      |
>
> Dans ce scénario, l'information que déduit le fetch au cycle 1 n'est accessible au decode qu'au cycle suivant, et de même pour tous les autres étages.
> Cependant, j'ai l'impression qu'à cause de l'aspect séquentiel du C, le decode reçoit ce qu'a calculé le fetch lors du même cycle.
> Ne faudrait-il pas procéder ainsi : lors d'un cycle, chaque étage prend les informations du cycle précédent, fait ce qu'il a à faire, met son output dans une variable temporaire, puis à la fin de chaque cycle, le pipeline transfert le contenu des variables temporaires aux autres étages concernés ?
>
> La deuxième question porte sur le calcul en parallèle de toutes les unités de calcul.
> Étant donné qu'il y a des calculs très coûteux (je pense notamment à la division, qui sur x86 peut prendre je crois jusqu'à 300 cycles) est-ce qu'il n'y aurait pas intérêt à empêcher ce genre de calcul en propageant le signal uniquement jusqu'à l'unité concernée ?
> Ou alors, dans l'unité, peut-être vérifier si l'instruction lui est destinée, et si elle ne l'est pas, ne rien faire.
> Aussi, si un calcul est coûteux, et qu'une unité bloque toutes les autres, n'y aurait-il pas intérêt à les séparer, c'est à dire à avoir un étage d'issue par unité de calcul, un peu comme les reservations stations de Tomasulo ? J'imagine que le coût de telles instructions peut être gommé par la présence de plusieurs harts dans le LBP, mais 300 cycles me paraît difficile à tant remplir que ça, surtout si les autres harts font aussi des divisions.
> Également, si toutes les unités tournent en même temps, est-ce que cela n'impacterai pas la consommation énergétique du processeur ?
> Enfin, dans le cadre de LBP, comment cela se passe-t-il ? Y a-t-il un booléen busy passé à true lors d'un long calcul pour poursuivre sur d'autres harts ? Ou alors l'approche est complètement différente ?
J'ai l'impression de poser des questions basiques, mais passer à l'implémentation des unités aujourd'hui a fait naître beaucoup de doutes sur ma façon de voir le pipeline et de gérer les instructions longues.
>
> La troisième porte sur les instructions fence, ecall et ebreak. Dois-je les implémenter ?
Si je dois les implémenter, je ne vois pas trop comment. Je ne suis même pas sûr de bien comprendre ce que fait fence d'ailleurs. Faudrait-il faire quelque chose qui intéragit avec le FPGA et ses I/O, avec des commandes spéciales ?
>
> Enfin, je suis en train de réfléchir aussi au blocage des différents étages, et ce qui me bloque reprend un peu la problématique de ma première question.
> Je considère ici seulement l'étage de fetch et decode.
> Soit un cycle c, supposons que l'étage de décode reçoive de l'étage de fetch une instruction, lue lors du cycle c-1. Le decode alloue un registre physique pour la destination, mais après cette allocation, il n'y a plus de registres physiques disponibles. Le decode l'indique donc au fetch. En parallèle, fetch lit une nouvelle instruction.
> Au cycle c+1, on a donc l'étage de fetch qui reçoit de decode un signal de blocage et ne lit rien, mais decode reçoit l'instruction lue au cycle c, et ne peut rien faire avec.
> Avec un tableau, cela donnerait cette situation :
> cycle c-1          cycle c                                                                                                          cycle c+1
> decode : ...
> fetch : lit i1  -> decode : decode i1, après décodage, plus de place, envoie un blocage -> fetch : reçoit le blocage
>                         fetch : lit i2                                                                                                -> decode : reçoit i2 mais ne peut rien faire
> Faut-il donc mettre un buffer entre les deux étages pour retenir cette instruction ? Ou y a-t-il une manière plus élégante de faire ?

## 10/07/2020

Le code a continué à être revu, pour être adapté aux contraintes de langage de la synthèse.
Les unités de calcul ont commencées à être implémentées. L'étage d'issue a bien avancé et le schéma du pipeline a été mis à jour.

> J'ai poursuivi l'implémentation de l'issue et du pipeline en général, pour voir ce qui est requis par chaque étage comme information.
>
> Concernant la sélection de l'instruction dans la table, j'ai utilisé pour le calcul du logarithme l'algorithme proposé ici : https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog. Parmis ceux qui étaient proposés, c'est celui qui me semblait le moins coûteux en ressources.
>
> J'ai une question concernant les unités de calcul : est-ce que, d'un point de vue de l'encapsulation, elles appartiennent bien au write back, puisque c'est cet étage qui récupère les résultats ?
>
> Je me suis aussi demandé comment était fait l'association entre une instruction terminée et ce qu'il y a dans le ROB, pour la marquer comme terminée. La première chose auquelle j'ai pensé est de comparer les registres de destination. Seulement, si une instruction n'en utilise pas (comme les branch), comment faire pour la retrouver dans le ROB ? Sinon, j'ai pensé à utiliser une sorte de token unique pour chaque instruction : l'issue en génère un pour l'instruction prête à être exécutée (en incrémentant un compteur entre [0, #ROB - 1]), et ce token est transmis au ROB et aux unités de calcul. Lors du write back, c'est ce token qui est comparé pour indiquer que l'exécution est terminée.
>
> Sinon, j'ai passé l'après-midi à essayer d'adapter le code pour la synthèse, car je ne l'ai pas fait depuis la remise à zéro. J'ai appris que le compilateur pour la synthèse n'est pas le même du tout que celui utilisé pour la simulation. Notamment, la synthèse ne supporte pas les 'enum struct' que j'utilisais pour encapsuler proprement des constantes. J'ai donc dû essayer différentes choses pour contourner le soucis tout en gardant l'aspect type-safe et sans devoir tout recommencer. J'ai fini par reprendre ce que je faisais pour les constantes de décodage, même si ça rend le code un peu moins lisible.

## 09/07/2020

Poursuite de la réécritude du code. Les constantes symboliques ont été revues, surtout du côté de la reconstruction des immédiats. Le décodage a été réduit au strict minimum.

> J'ai poursuivi aujourd'hui la réimplémentation du pipeline.
> C'est surtout la partie décodage qui a été revue : j'ai complètement réorganisé les constantes symboliques, ce qui me permet d'en utiliser partout au décodage, sans m'emmêler les pinceaux, tout en restant facilement étendable. J'ai notamment rajouté les constantes pour extraire l'immédiat compressé en sortie du décodage, pour les étages futurs.
> J'ai aussi réduit le décodage au strict minimum (du moins comme je le vois pour l'instant) : il y a le type (R, I, S, ...), les registres et s'ils sont utilisés (selon le type), le func3, quelques résultats de tests sur le func7, et la catégorie de l'instruction (alu, branch, load, store, ...) pour rediriger directement vers la bonne unité de calcul. Il se peut même que le type ne soit pas utile.
> Sinon, j'en suis rendu à peu près au même niveau qu'avant. Il me reste juste certaines choses à organiser plus proprement puisque j'ai dû faire quelques copié-collés pour vérifier que tout fonctionnait. Je devrais pouvoir finir l'étage d'issue demain.

## 08/07/2020

Reprise à zéro du code pour faire quelque chose de plus propre.

> J'ai commencé à reprendre le code de zéro.
> Cela consiste pour l'instant à réorganiser les types et les classes, à ajouter un peu de généricité par-ci par-là et à me battre contre le C++ pour faire des choses type safe tout en gardant le code lisible et maintenable, contrairement à la version précédente. J'ai l'impression de marcher dans la bonne direction cependant. J'ai les idées plus claires sur comment organiser les choses, ça devrait aller vite.
> J'en ai profité pour réorganiser les dossiers des sources et includes. Un nouveau projet vivado a été créé pour l'occasion, que j'ai simplement appelé 'processor', disponible dans le dossier 'Phase_2'.

## 07/07/2020

La communication entre étages a été revue et factorisée.
Début de l'implémentation de l'étage d'issue, de write back et de commit.
Refactorisation du fichier de registres.
Mise à jour du schéma du pipeline avec ce qui a été compris.

> J'ai essayé de poursuivre l'implémentation de l'étage d'issue et de réorganiser le décodage.
> Je me rends compte que j'ai des difficultés à implémenter tout ça avec l'approche que j'ai commencée et que je suis beaucoup moins efficace : je commence à bien comprendre ce que chaque étage fait individuellement, mais mettre ensemble les pièces du puzzle me fait poser beaucoup
de questions et donc ralentit l'implémentation. Notamment, j'ai du mal à me rendre compte de ce qui est vraiment nécessaire à chaque étage tant que tous les autres (unités de calcul, write back et commit) ne sont pas implémentés.
>
> Comme je commence aussi à m'y perdre sur comment les étages échangent les informations (il commence à y avoir trop de paramètres dans les fonctions) je pense que je vais changer d'approche et utiliser une structure spécifique pour transférer des informations d'un étage A à un autre étage B : par exemple, pour tout ce qui va de l'étage de décodage à l'étage d'issue, une structure DecodeToIssue serait utilisée pour regrouper toutes les informations.
>
> Concernant la table d'instructions, l'étage d'issue doit sélectionner une instruction prête, s'il y en a une, et l'envoyer à l'unité de calcul concernée. Je me demande cependant comment doit s'effectuer la sélection si plusieurs instructions sont prêtes : prendre la première que l'on rencontre ne me paraît pas efficace, notamment si l'étage de décodage remplit juste après la case libérée : s'il s'agit d'instructions sans opérandes, celles-ci vont être exécutées et remplir le ROB, tandis que les autres ne seront pas lancées et resteront bloquées dans le ROB, jusqu'à ce que celui-ci soit rempli. Ne peut-on pas simplement utiliser le ROB comme outil de sélection, en prennant l'instruction prête la plus ancienne afin d'éviter ce scénario ? Cela évite aussi d'implémenter une autre table pour stocker les instructions. J'aurais alors plutôt tendance à mettre le ROB dans l'issue.
>
> Autre question que je me suis posé : vaut-il mieux stocker l'état des registres prêts dans le banc de registres, et tout est stocké une seule fois, ou alors stocker à côté de chaque source si celle-ci est prête, et tout remettre à jour lorsque le writeback a lieu ? J'ai l'impression que la deuxième méthode a l'avantage d'être moins coûteuse en lecture de mémoire lors de la sélection d'une l'instruction, mais que l'étape du writeback complexifie beaucoup les choses.
>
> Dans tous les cas, il me paraît nécessaire de remanier beaucoup de code, je pense que tout reprendre à zéro évitera de me perdre entre le nouveau code et le legacy code. Ça me permettra aussi de revoir complètement le nommage de mes variables, constantes, l'organisation du décodage et de séparer le mappage du banc des registres.


## 06/07/2020

Mise à jour du schéma du *pipeline*.

Factorisation des buffers circulaires dans une unique classe.
Factorisation du débogage des buffers circulaires.

Correction de quelques bugs dans le fichier de registres.

## 03/07/2020

Le système de reconstruction des immédiats a été séparé dans des fonctions externes qui permettent le packetage et le dépacketage de ces derniers.

> - j'ai optimisé le décodage des immédiats : seuls 20 bits sont transmis aux étages suivant, chaque étage pourra ensuite dépaqueter selon ses besoins. Cependant cela conduit à un code peu propre et où il devient difficile d'utiliser des constantes symboliques sans que je m'emmêle les pinceaux. Je pense que je réécrirai tout ceci une nouvelle fois lorsque j'implémenterai petit à petit sur le FPGA.
> - j'ai légèrement optimisé les requêtes de renommage des registres en les séparant du switch
> - j'ai corrigé un souci d'unrolling dans mon fichier de registre : une boucle infinie était créée, comme vous me l'expliquiez dans un mail précédent
> - j'ai commencé à implémenter le ROB et l'étage d'issue (ceux-ci ne sont cependant pas encore branchés proprement). Après plusieurs essais, la version la plus simple que j'ai trouvée est en fait de reprendre le même système que le fichier de registre : une table stockant les instructions en attente, et une liste circulaire d'indices libres dans la table. Je suis cependant confronté à des problèmes pour déterminer si cette liste est vide ou pleine : j'ai une idée, je verrai ce qu'elle vaut demain.
> - j'ai mis à jour le schéma TikZ du pipeline tel que je le conçois pour l'instant. Le système de blocage n'est cependant pas encore implémenté. Je vais essayer de le détailler plus au fur et à mesure que j'avance.
> - J'ai complété le débuggage pour le fichier de registre

## 29/06/2020

Première implémentation du fichier de registres dans le décodeur. Ce dernier procède maintenant au renommage des registres après le décodage de l'instruction. Les buffers circulaires sont implémentés de manière naïve. Le nombre de registres architecturaux est facilement paramétrable, et ne nécessite de changer une seule valeur, sans avoir besoin de changer la taille du bus d'adressage des registres.

> J'ai implémenté le fichier de registres en vue du renommage.
> Il supporte les requêtes :
> - de création d'un alias pour un registre architectural
> - d'obtention de l'alias d'un registre
> - la libération d'un registre physique
> - l'écriture et la lecture d'une valeur d'un registre architectural
>
> J'ai procédé en stockant tout d'abord les registres physiques en eux-même, une liste d'association entre chaque registre architectural et un registre physique, et un buffer circulaire stockant la liste des registres physiques disponibles.
> J'ai implémenté ce dernier de manière naïve, mais je me suis demandé s'il n'y avait pas une structure fournie par Xilinx pour faire exactement ceci. J'ai cru comprende que `hls::stream` ou `ap_axis` permet ce genre de chose, mais je n'ai pas trouvé de documentation sur leur interface C++ dans la jungle de documentation de DocNav.
> Par ailleurs, j'ai mis de manière arbitraire 64 registres physiques et implémenté un système pour changer ce nombre rapidement sans devoir changer à chaque fois le nombre de bits minimaux pour indexer ces derniers.
> Il me reste à implémenter correctement le débuggeur au fichier de registres et à effectuer les tests. J'essaierai aussi de prendre en compte du côté du décodage le blocage lorsqu'il n'y a plus de registres physiques disponibles (du moins pour le hart courant).

## 26/06/2020

Le système de débogage a été amélioré et est maintenant plus facile à lire.
L'organisation des constantes de débogage a été changée pour être plus lisible.
La reconstruction des immédiats utilise maintenant des constantes symboliques, ajoutées pour l'occasion.
La version 4 du décodeur a été supprimée, après quelques tests dans Vivado sur les ressources utilisées.

> J'ai rajouté plus de constantes pour le décodage. J'ai préféré ne pas en mettre à gauche de l'égalité par contre et garder des instructions comme `immediate(10,  1)   = instruction(Slicing::J::imm_10_1_high, Slicing::J::imm_10_1_low);` car dès que j'en mettais, je m'emmêlais les pinceaux avec celles de droite
>
> J'ai séparé le décodage des immédiats dans des fonctions auxiliaires, il y en a une par type (sauf pour le type R qui n'a pas d'immédiat). J'ai conservé le second switch sur le type à la toute fin de la fonction décode pour décoder les immédiats.
>
> J'en ai aussi profité pour améliorer le débuggeur et son rapport pour qu'il soit plus lisible, et j'ai commencé à réflechir pour construire des tests efficaces qui permettent de détecter le plus de edges cases pour tout ce qui concerne le décodage.
> J'ai aussi fait quelques tests d'utilisation de ressources avec Vivado, ce qui m'a ammené à conserver seulement la dernière itération que j'ai faite. Les autres ont été supprimées.

## 25/06/2020

Ajout d'un débogueur et d'un désassembleur pour observer cycle après cycle ce que fait chaque étage. Le fichier produit est au format JSON et permet d'accélérer le débogage de la simulation par rapport à une session classique dans l'éditeur. Le désassembleur est cependant très différent du décodeur et le code est difficile à maintenir : il devrait être revu entièrement.

La mémoire est maintenant un tableau de valeurs 32 bits plutôt que d'octets.

> J'ai poursuivi l'implémentation du débuggeur en faisant un système très générique, basé sur du JSON, pour non seulement afficher les instructions décodées mais aussi pour afficher un compte-rendu cycle par cycle de chaque étage, avec le code désassemblé tout à la fin (je dois encore améliorer la façon dont c'est intégré dans le rapport). J'ai bien sûr accomodé le débuggeur pour qu'il soit désactivé lors de la synthèse. J'en ai profité pour refactoriser deux trois trucs et enlever les anciennes itérations
du décodage. J'ai laissé la 4ème et la 5ème pour effectuer d'autres comparaisons. À terme, l'une des deux sera supprimée.

## 24/06/2020

Implémentation de deux nouvelles itérations de l'étage de décodage.

La version 4 rajoute les optimisations suivantes :
- Des `switch` sont utilisés plutôt que des `if-else`
- Les opcodes ont été séparés en trois parties : les deux bits les plus bas, valant toujours `0b11`, un opcode bas de 3 bits, et un opcode haut de 2 bits
- De nouvelles constantes symboliques sont utilisées

La version 5 factorise le calcul des immédiats à la fin et en fonction du type de l'instruction.

Les instructions sont affichées plus joliment dans la console lors du décodage, pour faciliter le débogage.

L'import de l'IP dans Vivado est maintenant maîtrisé : il fonctionne et toute mise à jour du RTL est facilement répercutable dans le projet Vivado. Les *boards* des cartes *Pynq-Z1* et *Pynq-Z2* ont été installés.

> J'ai fait deux nouvelles itérations (n°4 et 5) de l'étage de décodage. Pour l'itération 5, j'ai préféré indiquer le type de l'instruction pour ensuite faire un switch dessus et en déduire le format de l'immédiat. Cela permet d'éviter les répétitions de code, mais dans le rapport, par rapport à l'itération précédente, certaines ressources sont plus sollicités tandis que d'autres moins. Même si intuitivement je dirais que c'est un bon changement, je ne saurais pas trop dire si cela fait vraiment gagner quelque chose.
> J'ai rajouté aussi un petit système pour afficher les instructions lues lors de la simulation, car deux erreurs se sont glissées dans mon implémentation et débugger à la main commençait à être long. Je compte l'étendre en reprenant ce que j'ai fait au début du stage et en l'adaptant un peu. Il me reste également un peu de factorisation à effectuer pour avoir un code plus propre et maintenable.
> J'ai passé l'après-midi sur Vivado en appliquant la procédure donnée. J'ai bloqué lors de l'ajout de mon IP car celle-ci ne s'affichait pas, mais j'ai fini par comprendre pourquoi : j'oubliais d'exporter le RTL depuis Vivado HLS. Je me suis aussi familiarisé avec les rapports et les outils de mise à jour automatique de l'IP. J'ai installé les boards Pynq-Z1 et Pynq-Z2, et les adresses de l'IP sont bien les mêmes.

## 23/06/2020

Optimisation de l'étage de décodage :
- L'opcode est décodé plus efficacement en isolant directement les deux premiers bits
- Les conditions sur `func7` sont calculées en amont
- Utilisation de constantes symboliques pour clarifier le code

Tout le décodage est inclus dans le même étage pour l'instant, il sera décalé au fur et à mesure de l'implémentation de l'étage d'issue.

Début de la prise en main des logiciels Vivado et Vitis. Pour l'instant, leur utilisation reste un mystère.

> J'ai essayé de suivre les conseils pour optimiser un peu la phase de décodage. Je conserve pour l'instant le décodage complet mais je décalerai le décodage au fur et à mesure de l'implémentation. J'en ai également profité pour implémenter dans un fichier des constantes symboliques, même s'il y a pas mal de choses non utilisées dedans finalement. Cela m'a permis de gagner pas mal de ressources (le rapport est disponible sur le git).
> J'ai également tenté de faire un Block Design dans Vivado tout en farfouillant dans le logiciel pour voir ce qu'il est possible de faire.
> J'ai cependant quelques questions :
> - lors de l'import de sources, j'ai supposé que l'on devait importer le code synthétisé dans Vivado HLS, et plus précisément du dossier solution1/syn. J'ai ensuite supposé que l'on pouvait importer indifféremment du Verilog ou du VHDL. Cependant j'ignore quel fichier est le bon parmi "pipeline.v", "pipeline_1.v" et "pipeline_AXILiteS_s_axi.v". J'ai l'impression, au vu des entrées et sorties des blocs, que le bon est le dernier car il ne dispose que des entrées et sorties de ma fonction top, mais je ne vois pas à quoi les brancher.
> - J'ai quand même lancé une synthèse pour voir ce que ça donnerait, et les chiffres sont sensiblement plus bas. Qu'est-ce qui explique cela ?

## 22/06/2020

Implémentation de plusieurs versions de l'étage de décodage et comparaison des ressources utilisées à la synthèse dans Vivado HLS.

> Je me suis concentré aujourd'hui sur de l'expérimentation et le décodage.
> Dans un premier temps, j'ai essayé de changer les types natifs par ceux fournis dans ap_cint.h. Cependant, même si tout se passait bien à la simulation, il m'était impossible de faire la synthèse : il semble que ce soit hard-codé dans la bibliothèque dès que du C++ est utilisé. J'ai vu qu'il existait ap_uint.h, compatible avec le C++. La simulation et la synthsèe fonctionnent bien avec, cependant j'ai des warnings pénibles à cause de #define redéfinis et j'ai perdu du temps à essayer de résoudre ce problème (sans succès malheureusement).
> L'expérimentation a surtout consisté à comparer les résultats d'analyse de la synthèse, entre un programme avec et sans un pragma spécifique ou entre deux implémentations différentes. J'ai laissé quelques comparaisons sur le git.  J'ai également essayé de comprendre le principe des ports et comment ruser pour accéder à quatres valeurs en mémoire simultanément (ou en tout cas sans warnings).
> J'ai aussi implémenté le décodage, et j'ai gardé deux versions : la première est plus proche de celle que j'avais faite en C++ au début du stage, tandis que la deuxième est plus concise et cherche à minimiser la latence. Cette dernière utilise un poil plus de ressources dans le rapport cependant, ce qui je pense s'explique par l'absence de résolution du nom de l'instruction. Je rajouterai ça demain pour vérifier mes dires.
D'ailleurs, avec l'implémentation du décodage, je me retrouve avec un nombre de cycles doublés. Je comprend pourquoi mais je n'ai pas d'idées pour l'instant de comment le résoudre (ni de savoir si c'est possible).

## 19/06/2020

Recherche de l'origine du problème empêchant de synthétiser.
L'installation de la version antérieure 2019.2 a résolu le problème, permettant la synthèse. Comme cette version ne supporte pas le standard C++14, le code du pipeline a été converti au standard C++11 pour être simulé et synthétiser.

## 18/06/2020

Première implémentation des deux premiers étages de pipeline. La simulation fonctionne mais pas la synthèse (la fonction top `pipeline` n'est pas détectée).

> Je rencontre plusieurs difficultés :
> - la première est que je n'arrive pas à sélectionner comme fonction top une fonction membre d'une classe : Vivado me sort une erreur lors de la synthèse, et n'arrive pas à faire une simulation C. J'ai donc fait une fonction auxiliaire 'pipeline' pour cela : elle me permet de lancer la simulation C. Cependant, je n'arrive toujours pas à faire de synthèse : "Cannot find the top function 'pipeline' in the design. Possible causes are: (1) the top function name is misspelled; (2) the top function is nonexistent or declared as static." est l'erreur qu'il me donne, cependant la fonction existe bien, et dans les paramètres de synthèse du projet, c'est bien cette fonction qui est sélectionnée.
> - la deuxième difficulté est sur les timings : dans mon testbench, le nombre de cycles est deux fois plus grand que ce qu'il devrait être, le souci étant je pense dans la fonction 'Pipeline::pipeline' : j'ai rajouté des pragmas, notamment la pragma PIPELINE pour essayer d'exécuter les deux étages en parallèle, seulement les valeurs des variables ne se mettent pas à jour comme je me l'imaginais (je pense que je reste trop attaché à la philosophie SystemC) : en effet, je crois que les valeurs 'instructions' et 'program_counter', même si elles sont mises à jour par 'fetch_stage', ce sont les anciennes valeurs de ces deux variables qui sont prises dans 'decode_stage'. Je pensais que ces variables agiraient comme un fil reliant les deux blocs, mais il semblerait qu'elles servent plus de registre tampon, et je ne trouve pas en ligne comment résoudre ce problème (ou a minima je ne cherche pas au bon endroit).
>
> Je crois que j'ai compris par contre le coup de la boucle while à l'intérieur de l'IP : il faudrait que je la mette à l'intérieur de ma fonction top 'pipeline' ? Donc aussi la variable 'cycles' ? Et dans ce cas le testbench se contente seulement d'initialiser la mémoire et d'appeler la fonction 'pipeline'.

## 17/06/2020

Modification du programme assembleur "Hello world" parallélisé pour répartir les harts par groupe de quatre par cœurs. Comme les fonctions appelées dans le corps utilisent le stack, un système de pile locale par hart a été implémenté. Note : l'appel aux fonctions `printf` ou autres fonctions d'*input/output* ne seront de toute façon pas disponible sur le FPGA.

Implémentation en assembleur du programme "sum" pour sommer en parallèle les valeurs d'un tableau.

Essais sur Vitis HLS avec un mini-projet d'additionneur pour s'initier aux outils et vérifier que la simulation et la synthèse fonctionnent.

> Aujourd'hui j'ai surtout continué à implémenter les programmes avec x_par :
> - j'ai amélioré "hello" pour qu'il occupe en priorité les harts sur le même cœur avant de passer au suivant, en faisant l'hypothèse de 4 harts par cœurs. Je me suis d'ailleurs rendu compte hier que mes appels de printf sont caduques car ils utilisent la pile, et ce sur tous les harts, ce qui est problématique. De plus, je me suis dit que même si printf est modifiée pour ne pas utiliser le stack, il reste fort probable qu'elle ait besoin d'appeler d'autres fonctions. J'ai donc palié au problème en donnant à chaque hart un pointeur de pile différent, en soustrayant au sp du hart appelant une valeur qui est la taille maximale supposée du stack utilisé par un hart (en y pensant, je me rends compte que dans mon code, j'ajoute à sp alors que je devrais soustraire à sp). Étant donné qu'on a pas de virtualisation de la mémoire, je vois difficilement comment faire autrement.
> - j'ai aussi implémenté "sum" pour remplir un tableau avec 0, 1, 2, ... et ensuite calculer la somme des valeurs. Je découpe pour cela le tableau en autant de morceaux que de threads, et chaque thread se charge de calculer la somme de sa portion puis de la ranger dans un tableau (afin d'éviter les écritures au même emplacement). Le thread principal se charge ensuite de calculer la somme de ces sommes partielles, avant d'afficher le résultat. Ici aussi, l'allocation des harts se fait en priorité sur le même cœur.
>
> Enfin, je me suis familiarisé avec Vivado HLS (qui d'ailleurs chez moi s'appelle Vitis HLS), en faisant un mini-projet d'additionneur. J'ai pu résoudre les problèmes de la dernière fois, et j'arrive maintenant à simuler et synthétiser un projet : le souci était simplement qu'il n'aimait pas les accents dans les chemins de fichier.
Je commence demain à implémenter les deux premiers étages du pipeline.

## 15/06/2020

Étude de la méthde typique dont sont dispatchés les harts avec les instructions *X_PAR* et comment se propagent les identifiants des threads (celui du premier thread se propageant sur toute la chaîne) et l'adresse de retour. Étude du mécanisme de join grâce à l'instruction `p_ret`.

Reprise du programme assembleur "hello world" parallélisé pour corriger l'utilisation des instructions *X_PAR* et la façon dont les threads sont alloués.

> J'ai repris aujourd'hui le code assembleur de la dernière fois, afin d'y apporter les corrections nécessaires.
> J'ai eu des difficultés lorsque j'ai essayé de respecter la convention utilisant ra, t0 et t6 : comme ra, t0 et t6 ne sont pas sauvegardés lors des appels, j'essayai de les empiler sur la pile, sauf que chaque hart devait s'en charger pour appeler printf, ce qui conduisait à des accès concurrents à la pile. J'ai fini par résoudre le problème en utilisant plutôt s1, s2 et s3, mais cela m'a perturbé dans l'utilisation des instructions de x_par, me forçant à réécrire plusieurs fois le programme.
> J'ai l'impression par contre que le -1 ne sert que dans certains cas, car si je comprends bien la description de p_jalr, quand rd = 0, rs1 = 0 et rs2 = -1, il y a un exit, que j'interpète comme l'arrêt simple du programme tout entier (de la même façon que le exit(0); en C). J'ai donc précisé en commentaire les modifications à effectuer selon l'environnement d'exécution pour utiliser le -1 ou non.
> Merci d'ailleurs pour le programme, comme je n'avais pas lu correctement le papier, j'ai pu y voir plus clair grâce à lui.
> Je me rends compte aussi que ma question à propos du stack avait effectivement une réponse évidente, j'aurais dû penser aux contraintes de l'implémentation empêchant ce genre de comportement. Petite question par curiosité d'ailleurs, quelle est la taille du buffer pour transférer des valeurs entre harts avec p_swcv, ... ?
>
> Je prévois comme prochaines tâches de convertir les deux programmes sum (pour répartir des harts sur une boucle for) et fibonacci (pour répartir plusieurs tâches récursives entre des harts), mais aussi d'adapter hello afin qu'il favorise d'abord les harts d'un même cœur avant d'allouer un hart sur le cœur suivant. Enfin, je simulerai les fetchs sur ces programmes.


## 12/06/2020

Programmation de trois programmes utilisant OpenMP :

- Un "hello world" affiché par plusieurs threads simultanément
- Un programme pour calculer la somme d'un tableau (ici la somme des premiers entiers naturels)
- Un calcul de la suite de Fibonacci de manière naïve. Seule les valeurs au-dessus de 20 ont été parallélisées pour permettre d'éviter la création de trop nombreuses *tasks*. Grâce à cela, l'amélioration des performances est nette pour `n = 40`.

Lecture plus en profondeur de la documentation de *X_PAR*.

Conversion du programme "hello world" en assembleur RISC-V avec l'extension *X_PAR*.

> J'ai publié sur le git ce que j'ai fait aujourd'hui :
>
> J'ai codé trois programmes simples avec OpenMP en essayant de les rendre réellement plus efficace que leur contre-partie single-threaded.
> J'ai ensuite essayé de coder celui affichant "hello world" par plusieurs threads en assembleur avec X_PAR.
> J'ai eu quelques difficultés à comprendre certaines instructions car les exemples de code dans le papier ne suivaient pas le tableau donné sur le site (ici : http://perso.numericable.fr/bernard.goossens/riscv_xpar_as.html), j'ai notamment mis du temps à comprendre que les opérandes rs1 et rs2 étaient inversées entre les deux versions. À part cela, ça s'est bien passé.
> Quelques notes et questions :
> - Dans mon programme, je me suis basé sur la syntaxe du site plutôt que sur celle du papier.
> - J'ai seulement considéré les forks sur les cœurs suivants, par simplicité.
> - Au vu des exemples du papier et de la sémantique de p_jalr x0, ra, t0, j'en suis venu à interpréter p_merge comme un constructeur de couple (id du thread père, id du thread fils) pour se souvenir du lien entre deux threads. J'en ai d'ailleurs déduit que les ids sont sur les 16 lower bits du mot uniquement. Est-ce correct ?
> - Puisqu'il est possible d'envoyer des valeurs via le stack, cela signifie-t-il que la totalité du stack de chaque hart est copié lors d'un fork ? Ou alors le mot stack est utilisé plutôt pour désigner un buffer, plus petit, entre les deux harts ?
> - Si le stack est copié, est-ce que le transfère de valeurs sur le stack via p_swcv se fait par rapport à sp ? J'ai considéré que p_swcv t1, 0(t6) faisait l'équivalent d'un sw t1, 0(sp) sur le thread alloué si c'était le cas.
>
> J'essaierai de convertir les deux autres programmes prochainement.

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
