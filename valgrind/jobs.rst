.. -*- coding: utf-8 -*-
.. Copyright |copy| 2013 by Maxime De Mol
.. Ce fichier est distribué sous une licence `creative commons <http://creativecommons.org/licenses/by-sa/3.0/>`_


.. _jobs:

Jobs
----

La plupart des commandes en consoles sont exécutées instantanément, mais ce n'est pas le cas de toutes. Certaines commandes, que l'on va appeler job, prennent plus de temps (comme par exemple copier un très gros fichier), et d'autres encore tournent indéfiniment (c'est le cas par exemple de yes).
Evidemment, quand un job est en cours d'exécution à la console, nous ne pouvons plus rien faire sur celle ci à part attendre. Unix nous vient en aide dans ce cas là avec les le raccourci ``Ctrl+z`` et les commandes `jobs(1)`_, `bg(1)`_ et `fg(1)`_.

    * ``Ctrl+z`` permet de suspendre le job actuellement en cours d'exécution à la console. Le job suspendu est mis en pause et placé en arrière plan (``background``)
    * ``jobs`` affiche à la console tous les jobs présent en arrière plan
    * ``bg`` passe un job présent en arrière plan de l'état ``suspended`` à l'état ``running``. Le job va donc rester en arrière plan mais va continuer à s'exécuter.
    * ``fg`` permet de faire passer un job de l'arrière plan à nouveau en avant plan

Démonstration du fonctionnement des jobs avec la série de commandes suivantes:

    .. code-block:: console

      $ yes > \dev\null
      #nous lançons la commande yes

      ^Z
      #nous la suspendons avec Ctrl+z
      [1]+  Stopped                 yes > \dev\null
      #elle est placé en arrière plan

      $ jobs
      #nous regardons la liste des jobs en arrière plan
      [1]+  Stopped                 yes > \dev\null
      #chaque job à un numéro qui lui est attribué. ici 1

      $ bg 1
      #nous relançons yes en arrière plan. on peu utiliser son nom comme son numéro avec la commande bg et fg
      [1]+ yes > \dev\null &
      #yes set remix en route

      $ jobs
      #nous vérifions le statut de yes avec jobs
      [1]+  Running                 yes > \dev\null &
      #il est en cours d'exécution

      
      $ fg yes
      #nous remettons yes en avant plan
      yes > \dev\null

      ^Z
      #nous le suspendons à nouveau
      [1]+  Stopped                 yes > \dev\null
      
      $ kill %1
      #nous terminons yes avec la commande kill %[numJob]
      [1]+  Stopped                 yes > \dev\null

      $ jobs
      #nous vérifions les jobs
      [1]+  Terminated: 15          yes > \dev\null
      #yes set marqué Terminated

      $ jobs
      #un deuxième appel à jobs nous affiche une liste vide




.. _`jobs(1)`: http://www.manpagez.com/man/1/jobs/
.. _`bg(1)`: http://linux.die.net/man/1/bg
.. _`fg(1)`: http://linux.die.net/man/1/fg
