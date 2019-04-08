CC = gcc

LIBS = -lm 
CCFLAGS = -Wall -ggdb

all: compare_arbres_xml

compare_arbres_xml: compare_arbres_xml.c analyseur_xml.o
	$(CC) $(CCFLAGS) -o compare_arbres_xml compare_arbres_xml.c analyseur_xml.o

analyseur_xml.o: analyseur_xml.c
	$(CC) $(CCFLAGS) -c analyseur_xml.c

.PHONY : clean

clean:
	- rm -rf *.o compare_arbres_xml output/
