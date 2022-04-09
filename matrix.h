#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define R1 4	// Numero di righe della matrice A
#define C1 5	// Numero di colonne della matrice A
#define R2 7	// Numero di righe della matrice B
#define C2 4	// Numero di colonne della matrice B
#define R3 4	// Numero di righe della matrice C
#define C3 4	// Numero di colonne della matrice C
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
struct Matrix threadCreate(struct Matrix *mat1, struct Matrix *mat2, int block);        // Crea ed esegue i thread
int main();