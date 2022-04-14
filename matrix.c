#include "matrix.h"

/*
• A è una matrice MxN
• B è una matrice NxP
• C è una matrice PxM
*/

#define M 400
#define N 600
#define P 800
#define BLOCK 20	// Numero di blocchi in cui dividere le matrici

pthread_barrier_t barrier;

struct Matrix *createMatrix(struct Matrix *mat, int rows, int cols)
{
	mat = malloc(sizeof(struct Matrix));
	mat->rows = rows;
	mat->cols = cols;
	mat->data = malloc(rows * sizeof(float *));		//Alloco lo spazio per il contenuto della matrice
	for (int i = 0; i < rows; i++)
		mat->data[i] = (float *)calloc(cols, sizeof(float));
	return mat;
}

struct toMult *createResultMatrix(struct toMult *result, struct Matrix *mat1, struct Matrix *mat2, struct Matrix *mat3)
{
	result = malloc(sizeof(struct toMult));
	result->mat1 = mat1;
	result->mat2 = mat2;
	result->mat3 = mat3;
	result->partialRes = createMatrix(result->partialRes, M, P);		//Creo la matrice che conterrà il risultato di A*B
	result->res = createMatrix(result->res, P, P);		//Creo la matrice che conterrà il risultato di C*(A*B)
	result->threadNum = 0;		//Identificatore del thread, viene usato per posizionare i risultati delle moltiplicazioni nelle righe giuste della matrice finale
	return result;
}

void multiplyNoThreading(struct toMult *arg){

	for (int i = 0; i < arg->mat1->rows; i++)		//A*B
	{
		for (int j = 0; j < arg->mat2->cols; j++)
		{
			for (int k = 0; k < arg->mat1->cols; k++)
				arg->partialRes->data[i][j] += arg->mat1->data[i][k] * arg->mat2->data[k][j];
		}
	}

	for (int i = 0; i < arg->mat3->rows; i++)		//C*R
	{
		for (int j = 0; j < arg->partialRes->cols; j++)
		{
			for (int k = 0; k < arg->mat3->cols; k++)
				arg->res->data[i][j] += arg->mat3->data[i][k] * arg->partialRes->data[k][j];
		}
	}
}

void print(struct Matrix *mat)
{
	for (int i = 0; i < mat->rows; i++)
	{
		for (int j = 0; j < mat->cols; j++)
			printf("%f ", mat->data[i][j]);
		printf("\n");
	}
}

void init(struct Matrix *mat)
{

	for (int i = 0; i < mat->rows; i++)
		for (int j = 0; j < mat->cols; j++)
			mat->data[i][j] = ((float)rand() / RAND_MAX) * (float)(10.0);
}

void *multiply(void *arg)
{
	struct toMult *result = (struct toMult *)arg;
	int threadNumber = result->threadNum;
	
	int offset = threadNumber * result->mat1->rows / BLOCK;		//Indica da quale riga cominciare a scrivere/moltiplicare

	for (int i = 0; i < result->mat1->rows / BLOCK; i++)		//blocco di A*B
	{
		for (int j = 0; j < result->mat2->cols; j++)
		{
			for (int k = 0; k < result->mat1->cols; k++)
				result->partialRes->data[i + offset][j] += result->mat1->data[i + offset][k] * result->mat2->data[k][j];
		}
	}

	pthread_barrier_wait(&barrier);		//Aspetto che anche l'altro thread abbia finito di moltiplicare il suo blocco dellla matrice A

	offset = threadNumber * result->mat3->rows / BLOCK;

	for (int i = 0; i < result->mat3->rows / BLOCK; i++)		//Blocco di C*R
	{
		for (int j = 0; j < result->partialRes->cols; j++)
		{
			for (int k = 0; k < result->mat3->cols; k++)
				result->res->data[i + offset][j] += result->mat3->data[i + offset][k] * result->partialRes->data[k][j];
		}
	}
}

void threading(struct toMult *arg)
{
	//Per evitare che l'identificatore dei thread (threadNum) venga modificato all'interno del for
	//mi creo BLOCK struct che puntatno alle stesse matrici ma possiedono identificatori diversi
	struct toMult args[BLOCK];
	int tNum=0;
	for (int i=0;i<BLOCK;i++){
		struct toMult temp=*arg;
		temp.threadNum=tNum;
		args[tNum]=temp;
		tNum++;
	}

	pthread_barrier_init(&barrier, NULL, BLOCK);
	pthread_t *threads = (pthread_t *)calloc(BLOCK, sizeof(pthread_t *));
	int count = 0;

	for (count = 0; count < BLOCK; count++)
	{

		if (pthread_create(&threads[count], NULL, &multiply, &args[count]) != 0)
		{
			fprintf(stderr, "Errore: impossibile creare thread #%d\n", count);
			break;
		}	
	}
	for (int i = 0; i < count; i++)
	{
		if (pthread_join(threads[i], NULL) != 0)
		{
			fprintf(stderr, "Erore : join #%d\n fallita", i);
		}
	}
	pthread_barrier_destroy(&barrier);
}

int main()
{
	srand(time(NULL));
	struct timeval t0, t1, dt;		//Struct per cronometrare i tempo di esecuzione

	struct Matrix *mat1 = createMatrix(mat1, M, N);
	init(mat1);
	/*printf("La matrice A è:\n");
	print(mat1);		DEBUG*/

	struct Matrix *mat2 = createMatrix(mat2, N, P);
	init(mat2);
	/*printf("La matrice B è:\n");
	print(mat2);		DEBUG*/

	struct Matrix *mat3 = createMatrix(mat3, P, M);
	init(mat3);
	/*printf("La matrice C è:\n");
	print(mat3);		DEBUG*/

	struct toMult *result = createResultMatrix(result, mat1, mat2, mat3);
	
	gettimeofday(&t0, NULL);		//Tempo di partenza
	
	threading(result);
	
	gettimeofday(&t1, NULL);		//Tempo di fine
	timersub(&t1, &t0, &dt);

	//print(result->res);		DEBUG

	fprintf(stderr, "Tempo di esecuzione con i thread = %ld.%06ld sec\n",dt.tv_sec, dt.tv_usec);

	result = createResultMatrix(result, mat1, mat2, mat3);		//Reinizzializzo la struct contenente i risultati
	
	gettimeofday(&t0, NULL);
	
	multiplyNoThreading(result);
	
	gettimeofday(&t1, NULL);
	timersub(&t1, &t0, &dt);

	//print(result->res);		DEBUG

	fprintf(stderr, "Tempo di esecuzione senza i thread = %ld.%06ld sec\n",dt.tv_sec, dt.tv_usec);
}