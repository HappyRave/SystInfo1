#!/bin/bash

# DEBUT création programme C

touch test.c

echo "#include <stdio.h>" >> test.c

echo "int main(int argc, const char *argv[])" >> test.c
echo "{" >> test.c
echo "int i;" >> test.c
echo "for (i=0; i<argc; i++) {" >> test.c
echo "if ((i%100) == 0) {" >> test.c
echo "printf(\"%s\",argv[i]);" >> test.c # Il ne fait que imprimer les arguments
echo "printf(\"\n\");" >> test.c # un argument sur 100 pour être precis
echo "}" >> test.c
echo "}" >> test.c
echo "return(0);" >> test.c
echo "}" >> test.c

# FIN création programme C

gcc -o test.o test.c # Compilation du programme "test.c"

# DEBUT script d'execution arguments

touch exe.sh

echo "#!/bin/bash" >> exe.sh
echo -n "./test.o" >> exe.sh # seul commande au debut: ./test sans arguments
chmod +x exe.sh # donner les bonnes permissions au script

./exe.sh 

j=1

while [ $? -eq 0 ] # tant que return vaut 0 (test s'execute sans erreur)
do
    echo -n " $j" >> exe.sh # on ajoute un argument a test à chaque iteration
    (( j++ ))
    if [ $j -lt 19000 ] # on execute test que tous les 1000 ajouts au début (gain de temps)
    then
        if (( $j % 1000 == 0 ))
            then
                ./exe.sh
        fi
    else # apres 19000 iterations, ont execute a caque ajout (precision ++)
        ./exe.sh
    fi
done

echo "Nombre max d'arguments: $j" # imprime le nombre d'arguments

    
