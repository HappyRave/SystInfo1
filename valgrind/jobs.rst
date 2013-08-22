.. -*- coding: utf-8 -*-
.. Copyright |copy| 2013 by Maxime De Mol
.. Ce fichier est distribué sous une licence `creative commons <http://creativecommons.org/licenses/by-sa/3.0/>`_


.. _jobs:

Jobs
----

La plupart des commandes en consoles sont exécutées instantanément, mais ce n'est pas le cas de toutes. Certaines commandes, que l'on va appeler job, prennent plus de temps (comme par exemple copier un très gros fichier), et d'autres encore tournent indéfiniment (c'est le cas par exemple de yes).
Evidemment, quand un job est en cours d'exécution à la console, nous ne pouvons plus rien faire sur celle ci à part attendre. Unix nous vient en aide dans ce cas là avec les le raccourci ``Ctrl+z`` et les commandes `jobs(1)`_, `bg(1)`_ et `fg(1)`_.



.. _`jobs(1)`: http://www.manpagez.com/man/1/jobs/
.. _`bg(1)`: http://linux.die.net/man/1/bg
.. _`fg(1)`: http://linux.die.net/man/1/fg
