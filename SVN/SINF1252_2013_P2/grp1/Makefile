#
#  Makefile for Filter
#
#     make all   (construction de l'executable)
#     make clean (effacement des fichiers objets et de l'executable)
#
CC       = gcc  
LD       = gcc
CFLAGS   = -Os -Wall -Werror -pthread -g
LFLAGS   = -Os -Wall -Werror -pthread -g
#
PROG     = filter
LISTEOBJ = \
  filters.o   bitmapio.o   threadpool.o  main.o
#
# compilation
#
.c.o :
	$(CC) -c  $(CFLAGS) -o $@ $<
#
# dependances
#
all           : $(PROG)
filters.o     : filters.c filterBand.h
bitmapio.o    : bitmapio.c bitmap.h
threadpool.o  : threadpool.c threadpool.h filterBand.h
main.o        : main.c filterBand.h
#
# edition de lien
#
$(PROG) : $(LISTEOBJ)
	$(LD) -o $(PROG) $(LFLAGS) $(LISTEOBJ) -lm
#
# effacement des fichiers intermediaires
#
clean :
	rm -vf $(PROG) $(LISTEOBJ)
