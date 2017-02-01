#include <stdlib.h> 
#include <stdio.h> 
#include <time.h> 
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

#define FILA 0

int numClientes;
int numCaixas;
long tempoAtendimento;
int numClientesAtual;

int *filas;

void atendeCliente(int rank) {
	printf("\nCaixa %d - Atendendo Clientes - Fila Atual com %d Clientes.\n", rank, filas[rank]);

	sleep(tempoAtendimento);
	int clientesAtendidos = (rand() % 13) + 13;
	if (clientesAtendidos > filas[rank])
		clientesAtendidos = filas[rank];

	filas[rank] -= clientesAtendidos;
	printf("Caixa %d - Atendeu %d Clientes - Fila Atual com %d Clientes.\n\n", rank, 
		clientesAtendidos, filas[rank]);
}

void printFilasCaixas() {
	for (int i = 1; i <= numCaixas; i++)
		printf("Fila do Caixa %d esta com %d clientes\n", i, filas[i]);
	printf("\n");
}

int filasVazias() {
	for (int i = 1; i <= numCaixas; i++)
		if (filas[i] > 0) return -1;
	return 1;
}

void inicializaFilas() {
	filas = malloc(sizeof(int) * numCaixas);

	for (int i = 1; i <= numCaixas; i++)
		filas[i] = (numClientes/2)/numCaixas;	
}

int logicaMercado(int rank) {
		if (rank != FILA) {
			if (filas[rank] > 0)
				atendeCliente(rank);	
			
		} else {
			if (numClientesAtual <= 0) 
				return 0;

			int menorFila = INT_MAX;
			int rankDoMenor;
			for (int i = 1; i <= numCaixas; i++) { 	/* Escolhe menor fila */
				if (filas[i] < menorFila) {
					menorFila = filas[i];
					rankDoMenor = i;
				}
			}
			printFilasCaixas();
			printf("\nFila - Caixa %d tem menor fila com %d clientes.\n\n", rankDoMenor, menorFila);
			
			int novosClientes = (numClientes/numCaixas) / 2;
			numClientesAtual -= novosClientes;

			if (novosClientes > numClientesAtual) {
				novosClientes = numClientesAtual;
				numClientesAtual = 0;
			}
			filas[rankDoMenor] += novosClientes;
			printf("\nFila do Mercado %d.\n", numClientesAtual);			
			printFilasCaixas();
		}
}

void imprimeUsage(){
    printf("Help\n\n");
    printf("Uso: ./seq <parametro> <valor>\n\n");
    printf("  -a <caixas>         numero de caixas para o mercado\n");
    printf("  -c <clientes>       numero de clientes para o mercado\n");
    printf("  -t <tempo>          tempo de atendimento de cada caixa\n\n");
}

int main(int argc, char **argv) {
	char c;
    while ((c = getopt (argc, argv, "a:c:t:h")) != -1){
	    switch (c){
	        case 'a':
	            numCaixas = atoi(optarg);
	            break;
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
	unsigned long long tempoExecucao = time(NULL);

	unsigned long long seed = time(NULL);
	srand(seed);


	numClientesAtual = numClientes;
	inicializaFilas();
	
	printf("Numero de Clientes %d\n", numClientes);
	printf("Numero de Caixas %d\n", numCaixas);
	printf("Tempo de Atendimento %lus\n", tempoAtendimento);
	printf("Abrindo Mercado...\n\n");

	while (numClientesAtual > 0 || filasVazias() < 0) {
		for (int i = 0; i < numCaixas + 1; i++) 
				logicaMercado(i);
	}

	printf("Tempo Execucao %llus\n", time(NULL) - tempoExecucao);
	return 0;
}