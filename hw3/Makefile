all: memory_manager.c
	gcc -g -o memory_manager memory_manager.c

run: all
	./memory_manager

debug: all
	valgrind --track-origins=yes ./memory_manager
