#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/*
• A è una matrice MxN
• B è una matrice NxP
• C è una matrice PxM
*/
#define M 4
#define N 6
#define P 8
#define BLOCK 2 // Numero di blocchi in cui dividere le matrici

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
};

float **allocMatrix(struct Matrix mat);     // Alloca un array bidimensionale
void print(struct Matrix mat);      // Stampa la matrice
float **init(struct Matrix mat);        // Inizializza una matrice con float randomici
void *multiply(void *var);      // Moltiplica due matrici
struct Matrix merge(struct Matrix mat1, struct Matrix mat2);        // Unisce due matrici
struct Matrix decomp(int row, struct Matrix mat, int index);       // Divide una matrice in blocchi
struct Matrix threadCreate(struct Matrix *mat1, struct Matrix *mat2);        // Crea ed esegue i thread
int main();