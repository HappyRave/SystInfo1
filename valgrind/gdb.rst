.. -*- coding: utf-8 -*-
.. Copyright |copy| 2012 by `Olivier Bonaventure <http://inl.info.ucl.ac.be/obo>`_, Christoph Paasch, Grégory Detal et Nicolas Houtain
.. Ce fichier est distribué sous une licence `creative commons <http://creativecommons.org/licenses/by-sa/3.0/>`_

.. _gdb-ref:

GDB
---

`gdb(1)`_ permet de débugger vos programmes plus facilement en permettant d'analyser l'état de l'exécution. Pour pouvoir analyser votre exécutable avec `gdb(1)`_, vous devez ajouter les symboles de débuggage lors de la compilation en utilisant l'option ``-g`` de `gcc(1)`_:

	.. code-block:: console

		gcc -g gdb.c -o my_program


Lancez gdb avec la commande ``gdb my_program``. Celle ci ouvre la console de gdb et vous permet de lancer le programme et de l'analyser. Pour démarrer le programme, tapez ``run``. Gdb va arrêter l'exécution au  premier problème trouvé. Votre programme tourne encore pour l'instant. Arrètez-le avec la commande ``kill``.

Pour analyser un programme, vous pouvez y placer des breakpoints. Un breakpoint permet de mettre en pause l'exécution d'un programme à un endroit donné pour afficher l'état des variables et continuer l'exécution pas-à-pas. Pour mettre un breakpoint, vous avez plusieurs choix:
 
	* ``break [function]`` met en pause l'exécution à l'appel de la fonction argument de la commande 
	* ``break [filename:linenumber]`` spécifie le fichier du code source et la ligne à laquelle l'exécution doit s'arrêter
	* ``delete [numberbreakpoint]`` supprime le breakpoint spécifié

Une fois un breakpoint placé, plusieurs informations peuvent être extraite via gdb :
	
	* ``print [variablename]`` affiche la valeur de la variable dans son format de base. Utilisez ``*``pour connaitre la valeur pointé, ``&`` pour l'adresse de la variable et ``$[registre]`` pour afficher un registre.

	.. code-block:: console

		Il est interressant de noter qu'il est possible d'afficher la variable sous le format voulu. Pour cela, remplacer ``print`` par :
		* ``p/x`` - affiche en format hexadécimal la variable spécifiée
		* ``p/d`` - en format d'un entier signé
		* ``p/f`` - en format floating point
		* ``p/c`` - affiche un caractère.

	* ``backtrace`` ou ``bt`` affiche la pile des appels de fonctions. 
	
	.. code-block:: console

		Naviguez dans la pile des appels de fonction avec ``up`` et ``down`` qui montent et descendent dans la pile.
		Naviguer dans les différents contexte permet d'afficher des variables inaccessibles qui étaient innaccessibles.
		Typiquement, nous l'utiliserons pour voir une variable local dans une autre fonction.

	* ``list`` affiche les lignes de codes. On peut donc facilement voir le code posant un problème ou analyser le code avant de faire une avancée pas à pas. 

	* ``show args`` affiche les arguments passé au programme.		
	* ``info breakpoints`` affiche les breakpoints
	* ``info diplays`` affiche les displays
	* ``info func [fonctionname] affiche le prototype d'une fonction

Quand vous avez acquis suffisament d'information sur le programme, vous avez plusieurs choix pour continuer l'exécution : 
 
	* ``next`` exécute la prochaine instruction de votre code source, mais sans rentrer dans des fonctions externes.
	* ``step`` exécute la prochaine instruction de votre code source, mais en entrant dans le code des fonctions appelées.
	* ``continue`` continue le reste de l'exécution.

Lors d'un débuggage long et fastidieux, il est parfois indispensable d'effectuer certaines commandes à chaque breakpoint.

	* ``commands [numerobreakpoint]`` definit une liste de commande associé à un breakpoint. Celle ci seront exécuté quand on s'arretera sur ce breakpoint. Il suffit de taper les commandes à effectuer les unes après les autres et de terminer par ``end``. Si vous ne fournissez pas de numero, les commandes sont assignés au dernier breakpoint créé.
	* ``display [variablename]`` affiche la variable à chaque breakpoint.

Il est aussi possible de modifier une variable avec ``set variable [nom_variable] = [expression]`` et de façon similaire avec ``print [nom_variable] = [expression]``.

Pour arrêter la console de gdb, tappez ``quit``.



A titre d'exemple, telechargez cette archive :download:`src/gdb.c`. L'archive contient un makefile qui vous permettra de compiler vos programmes. 

	Le premier programme est ``calc``. Executez le et observez le bug. A priori peu ou pas d'information sur l'erreur. Lancez donc gdb à l'aide de ``gdb calc`` puis lancez le programme avec ``run``.

	.. code-block:: console

		Program received signal SIGFPE, Arithmetic exception.	=> Exception arithmetique
		0x0000000000400553 in calc (a=165, b=4) at calc.c:10	=> Dans la fonction calc du fichier calc.c à la ligne 10

		10			res = (a*5 -10) / (b-i);	=> Affichage de la ligne problématique

	Le premier réflexe doit être la commande``list`` pour observer le code. Puisque le problème provient de la boucle, faites un ``break 9`` et relancer le programme. 
	Le programme va donc s'arrêter avant le début de la boucle. ``print a`` et ``print b`` pour connaitre la valeur des arguments reçus par ``calc``.
	
	.. code-block:: console	

		Il est interressant de noter une particularité du language C par rapport à java : une variable déclaré n'est pas initialisé à 0 par défault, elle reprend juste la valeur de la mémoire avant son affectation. ``print i`` et ``print res`` vous donnerons donc des résultats aléatoires.
	
	Puisque le problème vient du calcul arithmetique, placez un break sur cette ligne pour pouvoir observer à chaque itération les variables. ``break 10`` puis ``commands`` qui permet d'automatiser des commandes. Ces commandes sont automatiquement attribué au dernier breakpoint créé. Rajoutez ces commandes :
		* ``echo i : ``
		* ``print i``
		* ``echo b : ``
		* ``print b``
		* ``echo numerateur : ``
		* ``print a*5 -10``
		* ``echo denominateur : ``
		* ``print b-i``
		* et enfin ``end`` pour terminer la liste de commande.
	
	Avancez avec ``continue`` pour aller de breakpoint en breakpoint et observez les variables pour comprendre le problème. Vous pourrez donc deviner que le problème vient d'un dénominateur nulle. Pour résoudre ce problème, il faut envoyer une valeur plus grande que 6 à ``calc`` lors de son appel depuis la main. 
``list main`` suivi de plusieurs ``list`` permet de visualiser la main. L'appel de la fonction ``calc`` est à la ligne 18. 
	Supprimez les anciens break avec ``delete [numerobreakpoint]``, le numero du breakpoint étant connu via ``info break``. Rajoutez ``break 18`` et relancez le programme. Une fois sur le break, assignez la valeur 10 à m avec``set variable m = 10``. Puis continuez le programme. Celui se terminera normalement puisque il n'y a plus de division par zéro.



	Le deuxieme programme est ``recursive``. Celui ne présente aucun bug et se déroulera normalement. Toutefois, il est interressant d'utiliser gdb pour bien comprendre les différents contextes au sein d'un programme. Mettez un break sur la fonction factTmp avec ``break factTmp`` et ajouter automatiquement à ce breakpoint la commande ``backtrace``, via ``commands``, puis lancez le programme.
	``backtrace`` permet de visualiser les appels de fonction effectué. La fonction ``factTmp`` a donc été appellé par ``factTerminal``, elle même appellé par ``main``.

	.. code-block:: console
			
			#0  factTmp (acc=1, nbr=6) at recursive.c:8
			#1  0x000000000040057d in factTerminal (a=6) at recursive.c:17
			#2  0x0000000000400598 in main (argc=1, argv=0x7fffffffe1b8) at recursive.c:23

	Essayez d'afficher la variable ``globalVar`` puis``localVar``. Vous remarquerez qu'il n'est pas possible d'afficher ``localVar`` puisque cette variable ne fait pas partie de l'environement contextuel de ``factTmp``. Pour afficher cette variable, il faut remonter la liste des appels. ``up`` permettra de remonter les appels pour pouvoir afficher ``localVar``.
	Une fois la variable affiché, redescendez avec ``down`` et continuez de breakpoint en breakpoint. Vous remarquerez que la liste des appels s'allongent à chaque appel récursif, ce qui est tout à fait normal. 
Naviguez dans les appels recursif de ``factTmp`` en affichant les valeur de ``globalTmp``, ``tmp``, ``acc`` et ``nbr``. 

	.. code-block:: console
	
	La variable static ``globalTmp`` est commune à tous les appels de la fonction ``factTmp``. Les variables pointent vers la même adresse mémoire. Un changement de cette variable dans un des appels va donc modifier la variable dans les autres appels. 
	A contrario, la variable local ainsi que les arguments sont indépendant à chaque appels puisqu'ils pointent vers une adresse mémoire distincte.


	Vous pouvez maintenant terminer le programme.


	Le troisième programme est ``tab``. Ce programme se déroule correctement, et pourtant, il y a une erreur. Lancez le programme sur gdb et mettez un breakpoint sur la première instruction, à la ligne 9. Pour comprendre un problème sans savoir où commencer, il est important de suivre les variables. 

	.. code-block:: console
			
		``print`` ainsi que ``display`` comprend les expressions telque :
			* tab[1], tab[i],..
			* &i, *i,..
			* +, -, *, /, %
			* ..

	Avancez instruction par instruction, avec ``step`` ou ``next`` et portez attention aux valeurs de tab[i] par rapport à i. Une fois le problème trouvé avec gdb, solutionner le.


	Le quatrième programme est ``malloc``.


Plus d'informations sur `gdb(1)`_ peuvent être trouvées sur:
 
	* http://www.cprogramming.com/gdb.html
	* http://www.ibm.com/developerworks/library/l-gdb/
	* https://www.rocq.inria.fr/secret/Anne.Canteaut/COURS_C/gdb.html


Débuggage des threads avec GDB
------------------------------

`gdb(1)`_ est aussi utile pour débugger des programmes avec des threads. Il permet de faire les opérations suivantes sur les threads:

        * Notifier lors de la création d'un nouveau thread.
        * Afficher la liste complète des threads avec ``info threads``.
        * Mettre un breakpoint dans un thread. En effet, si vous placez un breakpoint dans une certaine fonction, et un thread passe lors de son exécution à travers de ce breakpoint, ``gdb`` va mettre l'exécution de tous les threads en pause et changer le contexte de la console `gdb(1)`_ vers ce thread.
        * Lorsque les threads sont en pause, vous pouvez manuellement donner la main à un thread en faisant ``thread [thread_no]`` avec ``thread_no`` étant l'indice du thread comme indiqué par ``info threads``

D'autres commandes pour utiliser `gdb(1)`_ avec les threads:
        * http://sourceware.org/gdb/current/onlinedocs/gdb/Threads.html
