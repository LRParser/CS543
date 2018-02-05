
all: my_shell_refactored.c
	gcc -g -o my_shell_refactored my_shell_refactored.c


run: all
	./my_shell_refactored

debug: all
	valgrind --track-origins=yes ./my_shell_refactored