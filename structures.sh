#!/bin/bash

if ./return 0; then
#la valeur de renvoi sera 0 dans la boucle sera exécutée
echo "Hello"
fi

if ./return 1; then
#ici c'est la condition else qui sera remplie
echo "Hello"
else
echo "Bye"
fi

for i in 1 2 5 2
#les boucles for peuvent s'écrire de cette façon
do
echo $i
done

echo Hello again!

for (( j=1; j<=5; j++))
#ou encore utiliser la synthaxe classique comme en C ou Java
do
echo $j
done

k=4
while ((k>0))
do
echo $k
k=$((k-1))
done