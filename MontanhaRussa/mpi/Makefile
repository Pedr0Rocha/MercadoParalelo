ALL:
	mpicc -o mpi mpi.c -lm -O3 -std=c99
clear:
	rm mpi
run:
	mpirun -n 5 ./mpi -c 600 -t 3
