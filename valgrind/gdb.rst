.. -*- coding: utf-8 -*-
.. Copyright |copy| 2012 by `Olivier Bonaventure <http://inl.info.ucl.ac.be/obo>`_, Christoph Paasch et Grégory Detal
.. Ce fichier est distribué sous une licence `creative commons <http://creativecommons.org/licenses/by-sa/3.0/>`_

.. _gdb-ref:

GDB
---

`gdb(1)`_ permet de débugger vos programmes plus facilement en permettant d'analyser l'état de l'exécution. A titre d'exemple, essayez de lancer le programme :download:`src/gdb.c`. Ce programme contient plusieurs bugs que vous pourrez détecter plus facilement avec `gdb(1)`_.

Pour pouvoir analyser votre exécutable avec `gdb(1)`_, vous devez ajouter les symboles de débuggage lors de la compilation en utilisant l'option ``-g`` de `gcc(1)`_:

	.. code-block:: console

		gcc -g gdb.c -o my_program


Lancez gdb avec la commande ``gdb my_program``. Ceci va vous ouvrir la console de gdb qui vous permet de lancer, le programme et de l'analyser. Pour démarrer le programme, tapez ``run``. gdb va arrêter l'exécution au  premier problème trouvé. Votre programme tourne encore pour l'instant. Arrètez-le avec la commande ``kill``.

Pour analyser un programme, vous pouvez y placer des breakpoints. Un breakpoint permet de mettre en pause l'exécution d'un programme à un endroit donné pour pouvoir afficher l'état des variables et faire une exécution pas-à-pas. Pour mettre un breakpoint, vous avez plusieurs choix:
 
	* ``break [function]`` met en pause l'exécution à l'appel de la fonction argument de la commande 
	* ``break [filename:linenumber]`` spécifie le fichier du code source et la ligne à laquelle l'exécution doit s'arrêter
	* ``delete [numberbreakpoint]`` supprime le breakpoint spécifié

Une fois un breakpoint placé, il est très utile de pouvoir afficher la valeur des variables lorsque le programme est en pause :
	
	* ``print [variablename]`` affiche la valeur de la variable dans son format de base. Il est possible de connaitre la valeur pointé en utilisant ``*`` ainsi que l'adresse de la variable avec ``&``. Pour afficher une registre utilisez ``$[registre]``.

	.. code-block:: console

		Il est interressant de noter qu'il est possible d'afficher la variable sous le format spécifié. Pour cela, remplacer ``print`` par :
		* ``p/x`` - affiche en format hexadécimal la variable spécifiée
		* ``p/d`` - en format d'un entier signé
		* ``p/f`` - en format floating point
		* ``p/c`` - affiche un caractère.

	* ``backtrace`` ou ``bt`` affiche la pile des appels de fonctions. 
	
	.. code-block:: console

		Il est possible de naviguer dans la pile des appels à l'aide de ``up`` et ``down``. Ces deux commandes montent et descendent respectivement dans la pile. Cela est très utile car il permet de modifier le contexte dans lequel on se trouve pour afficher les variables. 

	* ``list`` affiche les lignes de codes entourant le break. On peut donc facilement voir le code posant un problème ou analyser le code avant de faire une avancée pas à pas.

	* ``show args`` affiche les arguments passé au programme.		
	* ``info breakpoints`` affiche les breakpoints
	* ``info diplays`` affiche les displays
	* ``info func [fonctionname] affiche le prototype d'une fonction

Quand vous avez acquis suffisament d'information sur le programme, vous avez plusieurs choix pour continuer l'exécution : 
 
	* ``next`` exécute la prochaine instruction de votre code source, mais sans rentrer dans des fonctions externes.
	* ``step`` exécute la prochaine instruction de votre code source, mais en entrant dans le code des fonctions appelées.
	* ``continue`` continue le reste de l'exécution.
	* ``finish`` qui termine l'exécution d'une fonction.

Lors d'un débuggage long et fastidieux, il est parfois indispensable d'effectuer certaines commandes à chaque breakpoint.

	* ``commands`` definit une liste de commande à effectuer à un breakpoint. Il suffit de taper les commandes à effectuer les unes après les autres et de terminer par ``end``.
	* ``display [variablename]`` affiche la variable à chaque breakpoint.

Il est aussi possible de modifier une variable avec ``set variable [nom_variable] = [expression]`` et de façon similaire avec ``print [nom_variable] = [expression]``.

Pour arrêter la console de gdb, tappez ``quit``.



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
