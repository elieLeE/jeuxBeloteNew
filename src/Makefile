#Makefile
#cible: dependances
#	commandes
#$@ : cible
#$< : nom de la premiere dependance
#$^ : liste des dependances
#$? : liste des dependances plus recentes que la cible
#$* : nom du fichier sans le suffixe

CC=gcc
CFLAGS=-std=c2x -Wextra -Wall# -ansi -pedantic
LDFLAGS=
EXEC=prog
LIB_C=../../libC/src
SRC= $(wildcard *.c */*.c $(LIB_C)/logger/logger.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC) clean

debug: CFLAGS += -DDDEBUG -g
debug: LDFLAGS += -DDDEBUG -g
debug: all

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean mr

clean:
	rm -rf $(OBJ)

mr: clean
	rm -rf $(EXEC)
