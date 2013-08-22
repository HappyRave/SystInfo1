.. -*- coding: utf-8 -*-
.. Copyright |copy| 2013 by Maxime De Mol
.. Ce fichier est distribué sous une licence `creative commons <http://creativecommons.org/licenses/by-sa/3.0/>`_


.. _bash:
	
Bash
----

Tapez des commandes dans la console est inévitable lors d'opérations avancées sur un système Unix, et peut devenir très vite répétitif et fastidieux pour l'utilisateur. Le Bash est justement là pour éviter ces répétitions et automatiser certaines tâche à l'aide de scripts, qui sont des fichiers texte composés de différentes commandes Unix, lus, interprétés et exécutés par Bash.


Premier script
^^^^^^^^^^^^^^

Nous allons écrire un premier script bash pour présenter la manière générale de procéder avec un tel outils. Les scripts commencent toujours par la ligne ``#!/bin/bash`` qui indique à l'exécution qu'il s'agit d'un script et avec quel interpréteur le lire (ici bash).

    .. code-block:: bash

      #!/bin/bash
      echo "Hello, 1252"

Nous allons enregistrer ce texte sous le nom `hello.sh <https://raw.github.com/HappyRave/SystInfo1/master/valgrind/hello.sh>`_, puis changer ses permissions pour le rendre exécutable.

    .. code-block:: console

      $ chmod 700 hello.sh

Après il ne reste plus qu'à l'exécuter et observer le résultat.

    .. code-block:: console

      $ ./hello.sh
      Hello, 1252

Les variables
^^^^^^^^^^^^^

Bash permet l'utilisation de variables dans les scripts. Il peut s'agir de simples variables ou, de tableaux. Bash n'est pas un langage typé, des Int ou des String n'existe pas, toutes les variables sont traitées de la même façon. Pour illustrer ceci nous allons écrire le script `variables.sh <https://raw.github.com/HappyRave/SystInfo1/master/valgrind/variables.sh>`_

    .. code-block:: bash

      #!/bin/bash

      bonjour='Hello, '
      #il est important de ne pas mettre d'espace autour du =
      nombre[0]=12
      nombre[1]=52

      echo $bonjour${nombre[0]}${nombre[1]}
      #on accède à une variable simple avec un $ devant son nom
      #on accède à un élément d'un tableau avec un $ devant et des {} autour
      echo $bonjour${nombre[*]}
      #le caractère * indique qu'on veut utiliser tout les éléments du tableau (séparer
      #par un espace à chaque fois)

Ce script produit comme résultat

    .. code-block:: console

      $ ./variables.sh
      Hello,1252
      Hello,12 52

Les structures de contrôles
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Comme dans chaque langage de programmation, bash offre les structures de contrôles habituelles telles que les boucles if, for ou encore while que nous allons démontrer maintenant.

Comme dit précédemment, il n'y a pas de type en bash, true et false n'existe pas. Les conditions que les boucles vont utiliser seront les valeurs renvoyées par l'exécution d'une commande. Un 0 renvoyé correspond à un true, tandis que tout le reste est considéré comme un false.

Dans le but de tester ces boucles nous utiliserons un petit programme en C, return.c, qui va renvoyer la valeur qu'il reçoit en argument.





