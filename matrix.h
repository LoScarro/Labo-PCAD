#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

struct Matrix
{
	int rows;		// Numero di righe della matrice
	int cols;	  	// Numero di colonne della matrice
	float **data; 	// Puntatore alla matrice
};

struct toMult
{
	struct Matrix *mat1;		// Puntatore alla prima matrice
	struct Matrix *mat2;		// Puntatore alla seconda  matrice
	struct Matrix *mat3;
    struct Matrix *partialRes;		// Contiene il risultato di A*B
	struct Matrix *res;		// Contiene il risultato di C*(A*B)
	int volatile threadNum;		// Inidce del thread
};

struct Matrix *createMatrix(struct Matrix *mat, int rows, int cols);		// Crea e alloca lo spazio per le matrici
struct toMult *createResultMatrix(struct toMult *result, struct Matrix *mat1, struct Matrix *mat2, struct Matrix *mat3);		// Gestisce la struct che passerò alla funzione multiply (dato che posso passarle solo un parametro all'interno del thread)
void print(struct Matrix *mat);		// Stampa la matrice
void init(struct Matrix *mat);		// Inizializza la matrice con float randomici
void *multiply(void *arg);		// Si occupa di decomporre e moltiplicare le matrici
void threading(struct toMult *arg);		// Avvia i thread
int main();