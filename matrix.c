#include "matrix.h"

/*
• A è una matrice MxN
• B è una matrice NxP
• C è una matrice PxM
*/

#define M 400
#define N 800
#define P 1800
#define BLOCK 1000 // Numero di blocchi in cui dividere le matrici

pthread_barrier_t barrier;

struct Matrix *createMatrix(struct Matrix *mat, int rows, int cols)
{
    mat = malloc(sizeof(struct Matrix));
    mat->rows=rows;
    mat->cols=cols;
	mat->data = malloc(rows * sizeof(float *));
	for (int i = 0; i < rows; i++)
		mat->data[i] = (float *)calloc(cols, sizeof(float));

	return mat;
}

struct toMult *createResultMatrix(struct toMult *result, struct Matrix *mat1, struct Matrix *mat2, struct Matrix *mat3)
{
    result = malloc(sizeof(struct toMult));
    result->mat1=mat1;
    result->mat2=mat2;
    result->mat3=mat3;
    result->partialRes=createMatrix(result->partialRes, M, P);
    result->res=createMatrix(result->res, P, P);
    result->threadNum=-1;
    return result;
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
    int volatile threadNumber=result->threadNum;
    

    int offset=threadNumber*result->mat1->rows/BLOCK;
	
    for (int i = 0; i < result->mat1->rows/BLOCK; i++)
	{
		for (int j = 0; j < result->mat2->cols; j++)
		{
			for (int k = 0; k < result->mat1->cols; k++)
				result->partialRes->data[i+offset][j] += result->mat1->data[i+offset][k] * result->mat2->data[k][j];
		}
	}
	pthread_barrier_wait(&barrier);

    offset=threadNumber*result->mat3->rows/BLOCK;
    
    for (int i = 0; i < result->mat3->rows/BLOCK; i++)
	{
		for (int j = 0; j < result->partialRes->cols; j++)
		{
			for (int k = 0; k < result->mat3->cols; k++)
				result->res->data[i+offset][j] += result->mat3->data[i+offset][k] * result->partialRes->data[k][j];
		}
	}
}

void threading(struct toMult *arg)
{
    pthread_barrier_init(&barrier, NULL, BLOCK);
    pthread_t *threads = (pthread_t *)calloc(BLOCK, sizeof(pthread_t *));
    
    int index = 0;
	int count = 0;

    for (count = 0; count < BLOCK; count++)
	{
        arg->threadNum+=1;
		if (pthread_create(&threads[count], NULL, &multiply, arg) != 0)
		{
			fprintf(stderr, "error: Cannot create thread # %d\n", count);
			break;
		}
        sleep(1);
	}
	for (int i = 0; i < count; i++)
	{
		if (pthread_join(threads[i], NULL) != 0)
		{
			fprintf(stderr, "error: Cannot join thread # %d\n", i);
		}
	}
    pthread_barrier_destroy(&barrier);
}

int main()
{
	srand(time(NULL));
	clock_t start = clock();

	struct Matrix *mat1 = createMatrix(mat1, M, N);
    init(mat1);
	printf("La matrice A è:\n");
	print(mat1);
    
	struct Matrix *mat2 = createMatrix(mat2, N, P);
    init(mat2);
	printf("La matrice B è:\n");
	print(mat2);
    
	struct Matrix *mat3 = createMatrix(mat3, P, M);
    init(mat3);
	printf("La matrice C è:\n");
	print(mat3);

    struct toMult *result = createResultMatrix(result, mat1, mat2, mat3);
    threading(result);
    clock_t end = clock();
	printf("La matrice C*(A*B) è:\n");
    print(result->res);

    printf("Tempo di esecuzione =  %f secondi \n", ((double)(end - start)) / CLOCKS_PER_SEC);
}