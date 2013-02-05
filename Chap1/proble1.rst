.. -*- coding: utf-8 -*-
.. Copyright |copy| 2012 by `Olivier Bonaventure <http://inl.info.ucl.ac.be/obo>`_, Christoph Paasch et Gr�gory Detal
.. Ce fichier est distribu� sous une licence `creative commons <http://creativecommons.org/licenses/by-sa/3.0/>`_

Exercices
=========



1. Exercices en bash
--------------------

Pour rappel, quelques commandes de base dans le shell pour g�rer les dossiers et fichiers:

 - `ls(1)`_ : affiche la liste des fichiers et dossiers du r�pertoire sp�cifi�
 - `mv(1)`_ : utilitiaire pour renommer ou d�placer un fichier ou dossier
 - `cp(1)`_ : copie un fichier ou dossier (``-r`` pour les dossiers)
 - `rm(1)`_ : efface un fichier ou dossier
 - `mkdir(1)`_ : cr�e un r�pertoire
 - `rmdir(1)`_ : efface un r�pertoire vide
 - `cd(1posix)`_ : change le r�pertoire courant
 - `pwd(1)`_ : affiche le r�pertoire courant
 - `grep(1)`_ : permet de faire des recherches sur le contenu des fichiers

Pour vous familiarisez avec le shell, faites les petits exercices suivants dans le shell:

#. Cr�ez, dans un nouveau r�pertoire ``my_dir`` le fichier nomm� ``first`` contenant le texte "`Inserer`". V�rifiez chaque �tape en utilisant une des commandes `ls(1)`_, `pwd(1)`_ et `cat(1)`_.
#. Copiez le fichier ``first`` vers ``second`` et ajoutez la ligne "`Retirer`" � ``second``.
#. Comment pourriez-vous afficher la taille des fichiers avec `ls(1)`_?
#. Concat�nez ``first`` et ``second`` dans un nouveau fichier ``third`` (sans utiliser la command `echo(1)`_)
#. Utilisez `grep(1)`_ pour afficher le num�ro de la ligne sur laquelle se trouve le mot "`Retirer`" dans ``second``.
#. Retournez dans le r�pertoire parent de ``my_dir`` et copiez le dossier complet ``my_dir`` dans ``my_dir_2``.
#. Effacez le dossier entier ``my_dir`` en une seule commande (utilisez `rm(1)`_).

2. Questions de base
--------------------

#. Compilez et ex�cutez le code suivant. Expliquez ce que fait l'appel � `printf(3)`_.

        .. code-block:: c

	        int main(int argc, const char *argv[]) 
                {
                        int a = 1252;
                        printf("%d\n", a);
                }

#. Expliquez � quoi sert l'option ``-Werror`` de ``gcc``. Regardez la manpage (`gcc(1)`_).

        .. only:: staff

                .. note::
                        ``-Werror``
                                   Make all warnings into errors.

#. Recompilez le code du point `1` avec l'option ``-Werror``, que se passe-t-il ? Que faut-t-il rajouter dans le code pour qu'il compile sans erreurs ? 

        .. only:: staff

                .. note::
                
                        Le programme ne compile plus. Il faut inclure ``stdio.h`` parce que C n�cessite d'avoir une d�claration des fonctions.

#. (bonus) comment se fait-t-il que ``gcc`` compilait sans probl�me au point `1`?

        .. only:: staff

                .. note::
                
                        ``gcc`` cr�e une d�claration implicite des fonctions de la librairie C. Il n'affiche qu'un warning.

#. Lorsque j'ex�cute le programme corrig� du point `3` dans une console, j'ai acc�s � plusieurs variables, l'une d'elle est ``$?`` qui permet de r�cup�rer la valeur de retour du dernier programme ex�cut�. Expliquez � quoi sert cette valeur.

        .. code-block:: console

                $ gcc -Werror -std=c99 test.c -o test
                $ ./test
                42
                $ echo $?
                0

        .. only:: staff

                .. note::

                        voir note du cours.

#. Expliquez � quoi sert l'option ``-Wall`` de ``gcc``.

        .. only:: staff 

                .. note::
                
                        ``-Wall``
                                This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros.

#. Recompilez le code corrig� du point `3` avec les options ``-Wall`` et ``-Werror``, que ce passe-t'il ? Que faut-t'il rajouter dans le code pour qu'il compile sans erreurs ?

        .. only:: staff 

                .. note::
        
                        ``-Wall``: warning si une fonction ne renvoi pas de valeur. Il faut donc ajouter ``return 0;`` � la fin de la ``main``.

#. Compiler le code suivant (sans les options ``-Wall`` et ``-Werror``). Expliquez ce que sont les arguments de la fonction ``main``. Expliquez ce que fait `atoi(3)`_ (voir `strtol(3)`_ pour une fonction similaire). Ex�cutez ensuite le code avec ou sans arguments. Qu'observez-vous ? Comment se prot�ger du fait qu'un utilisateur ne va pas forc�ment rentrer le bon nombre d'arguments ?

        .. code-block:: c

                #include <stdlib.h>
                int main(int argc, const char *argv[])
                {
                        int a = atoi(argv[1]);
                        printf("%d\n", a);
                }

        .. only:: staff 

                .. note::

                        ``argc`` = nombre d'arguments.
                        ``argv`` = tableau de chaine de caract�re contenant le nom de l'ex�cutable suivi des arguments.
                        ``atoi`` = transforme une chaine de caract�re en un entier.
                        Le programme renvoi une segmentation fault lorsque l'on ne passe pas d'argument. Il faut donc utiliser ``argc`` pour tester que l'on a le bon nombre d'argument.

#. Ecrivez un programme qui va it�rer (avec une boucle ``for`` et une boucle ``while``) et afficher tous les arguments qui lui sont pass�s � la sortie standard. 

        .. only:: staff 

                .. note::
                        
                        .. code-block:: c
                                
                                #include <stdio.h>
                                int main(int argc, const char *argv[]) 
                                {
                                        int i;
                                        for (i = 1; i < argc; i++)
                                                printf("%s\n", argv[i]);
                                        return 0;
                                }

#. Ecrivez deux programmes qui r�agissent de fa�on similaire � `true(1)`_ et `false(1)`_ en utilisant `exit(3)`_. Vous pouvez tester le bon fonctionnement de votre programme en ex�cutant le code suivant dans une console:

        .. code-block:: console

                $ if ./true; then echo "true fonctionne"; fi
                true fonctionne
                $ if ! ./false; then echo "false fonctionne"; fi
                false fonctionne

   Bash permet aussi de faire des ``else``. Trouvez comme faire ceci en regardant ce `lien <http://tldp.org/LDP/abs/html/tests.html>`_.
        .. only:: staff 

                .. note::
                
                        ``true`` retourne toujours 0:
                                .. code-block:: c
                                
                                        #include <stdlib.h>
                                        int main(int argc, const char *argv[]) 
                                        {
                                        exit(0);
                                        }

                        ``false`` retourne toujours quelque chose != 0:
                                .. code-block:: c

                                        #include <stdlib.h>
                                        int main(int argc, const char *argv[]) 
                                        {
                                                exit(1);
                                        }
        

3. Petits programmes
--------------------


#.  Faites l'exercice sur `Pythia <http://pythia.info.ucl.ac.be/module/10/problem/26>`_ sur la commande `test(1)`_.

    Pythia est un environnement qui permet de soumettre du code, et des tests sont automatiquement ex�cut� sur ce code. Cela vous permettra de v�rifier que votre programme fonctionne.

    Vous avez besoin d'un login sur Pythia, que vous recevrez durant l'apr�s-midi du 29 janvier.


#. Faites de m�me pour la commande `expr(1)`_. On vous demande d'impl�menter les expressions suivante: ``+``, ``-``, ``*``, ``/`` et ``%``. Mais cette-fois ci sans utiliser Pythia. Vous devriez tester votre programme vous-m�me et assurez le bon fonctionnement de celui-ci.

   N'oubliez pas de respecter les valeurs de retour comme d�crit dans les man-pages.


#. En utilisant le shell et un programme C, essayez de d�terminer exp�rimentalement le nombre maximum d'arguments que vous pouvez passer � un programme C. Y-a-t-il une limite � ce nombre d'arguments ? Si oui, d'o� vient-elle et de quoi d�pend-elle ?