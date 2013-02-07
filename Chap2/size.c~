#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, const char *argv[])
{
    int i;
    int sizeInt = sizeof i;
    printf("La taille d'un int est de : %d octets soit de : %2d bits\n", sizeInt, sizeInt*8);
    
    long l;
    int sizeLong = sizeof l;
    printf("La taille d'un long est de : %d octets soit de : %2d bits\n", sizeLong, sizeLong*8);
    
    void *v;
    int sizeVoid = sizeof v;
    printf("La taille d'un void* est de : %d octets soit de : %2d bits\n", sizeVoid, sizeVoid*8);
    
    char *c;
    int sizeChar = sizeof c;
    printf("La taille d'un char* est de : %d octets soit de : %2d bits\n", sizeChar, sizeChar*8);
    
    size_t *s;
    int sizeSizeT = sizeof s;
    printf("La taille d'un size_t est de : %d octets soit de : %2d bits\n", sizeSizeT, sizeSizeT*8);
    
    uint64_t *u;
    int sizeU = sizeof u;
    printf("La taille d'un uint64_t est de : %d octets soit de : %2d bits\n", sizeU, sizeU*8);
    
    exit(EXIT_SUCCESS);
}

/*
Sur MAC OSX MOUNTAIN LION (x64)

 La taille d'un int est de : 4 octets soit de : 32 bits
 La taille d'un long est de : 8 octets soit de : 64 bits
 La taille d'un void* est de : 8 octets soit de : 64 bits
 La taille d'un char* est de : 8 octets soit de : 64 bits
 La taille d'un size_t est de : 8 octets soit de : 64 bits
 La taille d'un uint64_t est de : 8 octets soit de : 64 bits

*/
