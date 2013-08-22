.. -*- coding: utf-8 -*-
.. Copyright |copy| 2013 by Maxime De Mol
.. Ce fichier est distribué sous une licence `creative commons <http://creativecommons.org/licenses/by-sa/3.0/>`_


.. _bash:
	
Bash
----

Tapez des commandes dans la console est inévitable lors d'opérations avancées sur un système Unix, et peut devenir très vite répétitif et fastidieux pour l'utilisateur. Le Bash est justement là pour éviter ces répétitions et automatiser certaines tâche à l'aide de scripts, qui sont des fichiers texte composés de différentes commandes Unix, lus, interprétés et exécuté par Bash.


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

