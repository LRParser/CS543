256: memory_manager.c
	gcc -g -o memory_manager memory_manager.c

128: memory_manager_modified.c
	gcc -g -o memory_manager_modified memory_manager_modified.c

rw: memory_manager_rw.c
	gcc -g -o memory_manager_rw memory_manager_rw.c

run_256: 256
	./memory_manager

run_128: 128
	./memory_manager_modified

run_rw: rw
	./memory_manager_rw
