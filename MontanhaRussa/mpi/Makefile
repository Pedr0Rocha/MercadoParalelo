ALL:
	mpicc -o mpi mpi.c -lm -O3 -std=c99
clear:
	rm mpi
run:
	mpirun -n 10 ./mpi -c 5 -v 3 -t 5
