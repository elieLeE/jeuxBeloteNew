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
SRC= $(wildcard *.c */*.c ../libC/src/*.c ../libC/src/*/*.c)
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
