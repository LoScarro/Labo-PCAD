#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define R1 4		//Numero di righe della matrice A
#define C1 4		//Numero di colonne della matrice A
#define R2 4		//Numero di righe della matrice B
#define C2 4		//Numero di colonne della matrice B
#define R3 4		//Numero di righe della matrice C
#define C3 4		//Numero di colonne della matrice C
#define BLOCK 2		//Numero di blocchi in cui dividere le matrici

pthread_barrier_t barrier;

struct Matrix
{
	int rows;	// number of rows
	int cols;	// number of columns
	float **data; // <- note that this is a pointer to one dim array
};

struct toMult
{
	struct Matrix* mat1;
	struct Matrix* mat2;
};

float **allocMatrix(struct Matrix mat)
{
	mat.data = malloc(mat.rows * sizeof(float *));
	for (int i = 0; i < mat.rows; i++)
		mat.data[i] = (float *)calloc(mat.cols, sizeof(float));

	return mat.data;
}

void print(struct Matrix mat)
{
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
			printf("%f ", mat.data[i][j]);
		printf("\n");
	}
}

void *multiply(void *var)
{
	struct toMult *mat = (struct toMult *)var;
	struct Matrix *result = malloc(sizeof(struct Matrix));
	
	result->rows=mat->mat1->rows;
	result->cols = mat->mat2->cols;
	result->data = allocMatrix(*result);

	//print(*mat->mat1);

	for (int i = 0; i < result->rows; i++)
	{
		for (int j = 0; j < result->cols; j++)
		{
			// find product
			for (int k = 0; k < mat->mat1->cols; k++)
				result->data[i][j] += mat->mat1->data[i][k] * mat->mat2->data[k][j];
		}
		//print(mat->mat1);
	}
	//print(*result);
	pthread_barrier_wait(&barrier);
	return (void*) result;
}

float **init(struct Matrix mat)
{
	srand(time(NULL));

	int i, j, count;

	count = 0;
	for (i = 0; i < mat.rows; i++)
		for (j = 0; j < mat.cols; j++)
			mat.data[i][j] = ((float)rand()/RAND_MAX)*(float)(10.0);

	return mat.data;
}

struct Matrix merge(struct Matrix mat1, struct Matrix mat2)
{
	struct Matrix res = {mat1.rows + mat2.rows, mat2.cols};
	res.data = allocMatrix(res);

	for (int i = 0; i < mat1.rows; i++){
		for (int j = 0; j < mat1.cols; j++){
			res.data[i][j] = mat1.data[i][j];
		}
	}

	for (int i = mat1.rows; i < res.rows; i++){
		for (int j = 0; j < res.cols; j++){
			res.data[i][j] = mat2.data[i - mat1.rows][j];
		}
	}
	return res;
}

struct Matrix decomp(int row, struct Matrix mat1, int index){
	struct Matrix temp = {row, mat1.cols};
	temp.data = allocMatrix(temp);

	int l = 0;

	for (int i = index; i < index+row; i++){
		for (int j = 0; j < mat1.cols; j++){
			temp.data[l][j] = mat1.data[i][j];
		}
		l++;
	}
	//print(temp);
	return temp;
}

struct Matrix threadCreate(struct Matrix *mat1, struct Matrix *mat2, int block)
{
	pthread_barrier_init(&barrier, NULL, block);
	int row = mat1->rows / block;

	struct Matrix temp = {row, mat1->cols};
	temp.data = allocMatrix(temp);

	pthread_t *threads = (pthread_t *)calloc(block, sizeof(pthread_t *));
	//struct Matrix *retvals = (struct Matrix *)calloc(block, sizeof(struct Matrix *));
	struct Matrix *retvals[block];
	
	struct toMult args;
	args.mat2 = mat2;

	struct Matrix res = {0, mat2->cols};
	res.data = allocMatrix(res);

	struct Matrix toMerge = {row, mat2->cols};
	toMerge.data = allocMatrix(toMerge);

	int index = 0;


	int count = 0;
		
		//-----------------------------------------------------------------------------------------------------------

		for (count = 0; count < block; count++)
		{
			temp=decomp(row, *mat1, index);
			index += row;
			args.mat1 = &temp;
			
			if (pthread_create(&threads[count], NULL, &multiply, &args) != 0)
			{
				fprintf(stderr, "error: Cannot create thread # %d\n", count);
				break;
			}
			
		}
		for (int i = 0; i < count; ++i)
		{
			if (pthread_join(threads[i], (void*) &retvals[i]) != 0)
			{
				fprintf(stderr, "error: Cannot join thread # %d\n", i);
			}
			//print(*retvals[i]);
			res=merge(res, *retvals[i]);
		}
		//-------------------------------------------------------------------------------------------------------------

		/*toMerge.data=multiply(temp, mat2);
		res.data = merge(res, toMerge);*/
		//------------------------------ threading
		// res.rows+=toMerge.rows;
		//printf("%d", retvals[j].rows);
	
	pthread_barrier_destroy(&barrier);
	return(res);
	printf("----------------\n");
}

int main()
{
	clock_t start = clock();
	// int **mat1, **mat2, **mat3, **res;
	int row1, col1, row2, col2, row3, col3;

	/*printf("Quante righe ha la prima matrice? ");
	scanf("%d", &row1);
	printf("Quante colonne ha la prima matrice? ");
	scanf("%d", &col1);*/

	struct Matrix mat1 = {R1, C1};
	mat1.data = allocMatrix(mat1);
	mat1.data = init(mat1);

	printf("La matrice A è:\n");
	print(mat1);

	/*printf("Quante righe ha la seconda matrice? ");
	scanf("%d", &row2);
	printf("Quante colonne ha la seconda matrice? ");
	scanf("%d", &col2);*/

	struct Matrix mat2 = {R2, C2};
	mat2.data = allocMatrix(mat2);
	mat2.data = init(mat2);

	printf("La matrice B è:\n");
	print(mat2);
	
	struct Matrix mat3 = {R3, C3};
	mat3.data = allocMatrix(mat3);
	mat3.data = init(mat3);

	printf("La matrice C è:\n");
	print(mat3);

	struct Matrix res1 = {R1, C2};
	res1.data = allocMatrix(res1);

	res1=threadCreate(&mat1, &mat2, BLOCK);
	printf("\n");
	printf("Il risultato del calcolo è:\n");
	print(threadCreate(&mat3, &res1 , BLOCK));
	printf("\n");

	clock_t end = clock();

	printf("Tempo di esecuzione =  %f secondi \n", ((double)(end - start)) / CLOCKS_PER_SEC);
	// multiply(mat1, mat2);

	/*struct Matrix RES = {A.rows, B.cols, res};
	printf("-------------------------\n");
	RES.data = multiply(A, B);
	print(RES);*/

	/*printf("Quante righe ha la terza matrice? ");
	scanf("%d", &row3);
	printf("Quante colonne ha la terza matrice? ");
	scanf("%d", &col3);
	int **c = init(row3, col3);

	// la mtrice risultante ha il numero di righe della prima e il numero delle colonne della seconda

	int** res = multiply(a, b, row1, col1, row2, col2);
	printf("-------------------------\n");
	print(res, row1, col2);
	res = multiply(c, res, row3, col3, row1, col2);
	printf("-------------------------\n");
	print(res, row3, col2);

	printf("Quanti blocchi? ");
	scanf("%d", &blockNumber);

	split(a, b, blockNumber, row1, col1);*/
}
