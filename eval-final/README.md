Évaluation du compilateur 2018/2019
-----------------------------------

### Préparation

Compilez votre compilateur. Assurez-vous qu'il est à jour et qu'il 
compile sans problèmes. Vérifiez que vous avez téléchargé les dernières 
versions des modules fournis, notamment `c3a2nasm.{c,h}`, `code3a.{c,h}`
et `affiche_arbre_abstrait.c` qui ont été mis à jour.

Ensuite, téléchargez le package d'évaluation `autotest-2018-2019.zip` et
décompressez-le. Ouvrez le script `testAll.sh` et configurez-le comme 
indiqué dans les commentaires. Notamment, éditez la variable `MYCOMPILO`
pour qu'elle pointe vers l'exécutable de votre compilateur. Modifiez 
éventuellement les options pour générer les différentes sorties (`-l`, 
`-a`, etc).


### Tests connus/nouveaux

Exécutez le script de test :
```
./testAll.sh
```

Le script va exécuter votre compilateur et comparer les sorties obtenues
avec les sorties de référence attendues. Pour le code machine x86, il ne 
compare pas directement le code, mais plutôt l'exécution de ce code
après assemblage. 

Vérifiez que votre compilateur passe les tests pour les exemples connus
et pour les nouveaux exemples. Si besoin, modifiez les variables
`EXITONFAIL` et `VERBOSE` pour que le script s'arrête à la première 
erreur et pour qu'il affiche plus ou moins de détails sur chaque test.


### Ajout de la boucle `pour`

On désire ajouter à notre langage une boucle de type `pour`.
Les mots-clef POUR (`pour`) et FAIRE (`faire`) sont des unités lexicales
(terminaux) reconnues par l'analyseur lexical. La syntaxe de cette 
instruction est donnée par la règle suivante

```
instrPour -> POUR instrAffect exp; instrAffect FAIRE instrBloc
```

Où `instrAffect` est une instruction d'affectation, `instrBloc` est un 
bloc d'instructions délimité par des accolades, et `exp` correspond à
une expression.

Une telle instruction exécute d'abord la première affectation, contenue 
dans le premier `instrAffect`, pour l'initialisation. Ensuite, tant que 
la valeur de l'expression `exp` est vraie (différente de zéro) elle 
exécute le bloc d'instructions dans `instrBloc` suivi de l'instruction 
d'incrément contenue dans le deuxième `instrAffect`. Cette boucle est 
similaire à la boucle `for` en C. Cependant, il n'est pas possible de 
mettre autre chose qu'une affectation dans les parties d'initialisation 
et d'incrément (en C, elles peuvent être vides ou contenir n'importe 
quelle instruction-expression).

Modifiez l'analyseur lexical, l'analyseur syntaxique, la structure de
l'arbre abstrait, et la génération de code trois adresses afin de 
prendre en compte cette nouvelle instruction.

Ensuite, vous testerez votre nouveau compilateur sur les programmes 
de test fournis `pour.l`, `pourien.l`, `pourtab.l` et `pourri.l`. Le
dernier ne doit pas être accepté, car il contient une erreur de syntaxe.
