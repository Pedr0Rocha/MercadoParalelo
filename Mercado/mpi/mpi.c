#include <mpi.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <time.h> 
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

/* Processo MASTER - Fila Principal */
#define FILA 0

int numClientes;
int numCaixas;
long tempoAtendimento;

int atendeCliente(int rank, int clientesNaFila) {
	printf("Caixa %d - Atendendo Clientes - Fila Atual com %d Clientes.\n", rank, clientesNaFila);

	sleep(tempoAtendimento);
	int clientesAtendidos = (rand() % 13) + 13;
	if (clientesAtendidos > clientesNaFila)
		clientesAtendidos = clientesNaFila;

	printf("Caixa %d - Atendeu %d Clientes - Fila Atual com %d Clientes.\n", rank, 
		clientesAtendidos, clientesNaFila);

	return clientesAtendidos;
}

int logicaMercado(int rank) {
	int numClientesAtual = numClientes;
	if (rank == FILA) { 							/* Logica do Processo FILA*/
		while (numClientesAtual > 0) {
			int menorFila = INT_MAX;
			int rankDoMenor;
			for (int i = 1; i <= numCaixas; i++) { 	/* Escolhe menor fila */
				int menor;
				MPI_Recv(&menor, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if (menor < menorFila) {
					menorFila = menor;
					rankDoMenor = i;
				}
			}
			printf("Processo Fila - Caixa %d tem menor fila com %d clientes.\n\n", rankDoMenor, menorFila);

			int semNovosClientes = 0;
			int fechaCaixa = -1;
			int novosClientes = (numClientes/numCaixas) / 2;

			numClientesAtual -= novosClientes;

			if (novosClientes > numClientesAtual) {
				novosClientes = numClientesAtual;
				numClientesAtual = 0;
			}

			for (int i = 1; i <= numCaixas; i++) { 	/* Envia cliente para as filas ou fecha caixas */
				if (numClientesAtual <= 0) { 
					//printf("Processo Fila - Fechando Caixa %d.\n", i);
					MPI_Send(&fechaCaixa, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
					continue;
				}
				if (i == rankDoMenor)
					MPI_Send(&novosClientes, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				else
					MPI_Send(&semNovosClientes, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			printf("Processo Fila - Fila do Mercado %d.\n", numClientesAtual);
		}
	} else { /* Logica dos Caixas */
		int inicialClientePorFila = (numClientes/2)/numCaixas;
		int clientesNaFila = inicialClientePorFila;
		int caixaAberto = 1;
		
		while (caixaAberto >= 0) {
			if (clientesNaFila > 0) { 				/* Processa clientes na fila do caixa */
				int clientesAtendidos = atendeCliente(rank, clientesNaFila);
				clientesNaFila -= clientesAtendidos;
			}
			MPI_Send(&clientesNaFila, 1, MPI_INT, FILA, 0, MPI_COMM_WORLD); /* Envia tamanho da fila para processo FILA */
			
			int novosClientes;
			MPI_Recv(&novosClientes, 1, MPI_INT, FILA, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Recebe novos clientes */

			if (novosClientes > 0) {
				clientesNaFila += novosClientes;
			} else if (novosClientes == -1) {		/* Acabou a Producao */
				while (clientesNaFila > 0) {		/* Acabou a Producao mas ainda existem clientes na fila do caixa */
					int clientesAtendidos = atendeCliente(rank, clientesNaFila);
					clientesNaFila -= clientesAtendidos;
				}
				printf("Caixa %d -  Fechando Caixa.\n", rank);
				caixaAberto = -1;
			}
		}	
	}
}

void imprimeUsage(){
    printf("Help\n\n");
    printf("Uso: ./mpi <parametro> <valor>\n\n");
    printf("  -c <clientes>       numero de clientes para o mercado\n");
    printf("  -t <tempo>          tempo de atendimento de cada caixa\n\n");
}

int main(int argc, char **argv) {
	char c;
    while ((c = getopt (argc, argv, "c:t:h")) != -1){
	    switch (c){
	        case 'c':
	            numClientes = atoi(optarg);
	            break;
	        case 't':
	            tempoAtendimento = strtoull(optarg, NULL, 10);
	            break;
	        case 'h':
	            imprimeUsage();
	            exit(0);
	        case '?':
	            printf("Erro nos Parametros.\n");
	        default:
	            abort();
	    }
	}

	unsigned long long tempoTotal = time(NULL);

	int totalRanks;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &totalRanks);

	unsigned long long tempoExecucao = time(NULL);

	unsigned long long seed = rank + time(NULL);
	srand(seed);

	numCaixas = totalRanks - 1;
	if (rank == FILA) {
		printf("Numero de Clientes %d\n", numClientes);
		printf("Numero de Caixas %d\n", numCaixas);
		printf("Tempo de Atendimento %lus\n", tempoAtendimento);
		printf("Abrindo Mercado...\n\n");
	}
	logicaMercado(rank);

	if (rank == FILA)
		printf("Processo Fila - Tempo Execucao %llus\n", time(NULL) - tempoExecucao);
	else
		printf("Caixa %d - Tempo Execucao %llus\n", rank, time(NULL) - tempoExecucao);
	

	printf("Processo %d terminou com sucesso.\n", rank);
	MPI_Finalize();
	return 0;
}