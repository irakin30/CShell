all: compile run

main.o: main.c
	gcc -c main.c

shell.o: shell.c shell.h
	gcc -c shell.c

compile: main.o shell.o
	gcc -o shell.out main.o shell.o

run: ./shell.out
	./shell.out
