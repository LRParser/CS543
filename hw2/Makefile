
all: my_shell.c
	gcc -g -o my_shell my_shell.c

run: all
	./my_shell

debug: all
	valgrind --track-origins=yes ./my_shell
