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