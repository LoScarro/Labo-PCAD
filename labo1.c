#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

struct Matrix
{
	int rows;	// number of rows
	int cols;	// number of columns
	int **data; // <- note that this is a pointer to one dim array
};

struct toMult
{
	struct Matrix* mat1;
	struct Matrix* mat2;
};

int **allocMatrix(struct Matrix mat)
{
	mat.data = malloc(mat.rows * sizeof(int *));
	for (int i = 0; i < mat.rows; i++)
		mat.data[i] = (int *)calloc(mat.cols, sizeof(int));

	return mat.data;
}

void print(struct Matrix mat)
{
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
			printf("%d ", mat.data[i][j]);
		printf("\n");
	}
}

void *multiply(void *var)
{
	struct toMult *mat = (struct toMult *)var;
	struct Matrix result = {mat->mat1->rows, mat->mat2->cols};
	result.data = allocMatrix(result);

	print(*mat->mat2);

	for (int i = 0; i < mat->mat1->rows; i++)
	{
		for (int j = 0; j < mat->mat2->cols; j++)
		{
			// find product
			for (int k = 0; k < mat->mat1->cols; k++)
				result.data[i][j] += mat->mat1->data[i][k] * mat->mat2->data[k][j];
		}
		//print(mat->mat1);
	}
	return &result;
}

int **init(struct Matrix mat)
{
	srand(time(NULL)); // Initialization, should only be called once.

	int i, j, count;

	// Note that arr[i][j] is same as *(*(arr+i)+j)
	count = 0;
	for (i = 0; i < mat.rows; i++)
		for (j = 0; j < mat.cols; j++)
			mat.data[i][j] = (rand() % 10); // Or *(*(arr+i)+j) = ++count

	// print(mat);

	return mat.data;
}

int **merge(struct Matrix mat1, struct Matrix mat2)
{

	struct Matrix res = {mat1.rows + mat2.rows, mat2.cols};
	res.data = allocMatrix(res);

	for (int i = 0; i < mat1.rows; i++)
	{
		for (int j = 0; j < mat1.cols; j++)
		{
			res.data[i][j] = mat1.data[i][j];
		}
	}

	for (int i = mat1.rows; i < res.rows; i++)
	{
		for (int j = 0; j < res.cols; j++)
		{
			res.data[i][j] = mat2.data[i - mat1.rows][j];
		}
	}

	return res.data;
}

struct Matrix decomp(int row, struct Matrix mat1, int index)
{
	struct Matrix temp = {row, mat1.cols};
	temp.data = allocMatrix(temp);

	int l = 0;

	for (int i = index; i < row; i++)
	{
		for (int j = 0; j < mat1.cols; j++)
		{
			temp.data[i][j] = mat1.data[l][j];
		}
		l++;
	}
	return temp;
}

void threadCreate(struct Matrix *mat1, struct Matrix *mat2, int block)
{
	int row = mat1->rows / block;

	struct Matrix temp = {row, mat1->cols};
	temp.data = allocMatrix(temp);

	pthread_t *threads = (pthread_t *)calloc(block, sizeof(pthread_t *));
	struct Matrix *retvals = (struct Matrix *)calloc(block, sizeof(struct Matrix *));

	struct toMult args;

	struct Matrix res = {0, mat2->cols};
	res.data = allocMatrix(res);

	struct Matrix toMerge = {row, mat2->cols};
	toMerge.data = allocMatrix(toMerge);

	int index = 0;
	args.mat2 = mat2;

	int count = 0;
		
		//-----------------------------------------------------------------------------------------------------------

		for (count = 0; count < block; ++count)
		{
			temp=decomp(row, *mat1, index);
			args.mat1 = &temp;
			if (pthread_create(&threads[count], NULL, multiply, &args) != 0)
			{
				fprintf(stderr, "error: Cannot create thread # %d\n", count);
				break;
			}
			index += row;
		}
		for (int i = 0; i < count; ++i)
		{
			if (pthread_join(threads[i], &retvals[i]) != 0)
			{
				fprintf(stderr, "error: Cannot join thread # %d\n", i);
			}
		}
		//-------------------------------------------------------------------------------------------------------------

		/*toMerge.data=multiply(temp, mat2);
		res.data = merge(res, toMerge);*/
		//------------------------------ threading
		// res.rows+=toMerge.rows;
		
		print(retvals[0]);
		//printf("%d", retvals[j].rows);
	

	printf("----------------\n");
}

int main()
{
	// int **mat1, **mat2, **mat3, **res;
	int row1, col1, row2, col2, row3, col3, blockNumber, blockSize;

	printf("Quante righe ha la prima matrice? ");
	scanf("%d", &row1);
	printf("Quante colonne ha la prima matrice? ");
	scanf("%d", &col1);

	struct Matrix mat1 = {row1, col1};
	mat1.data = allocMatrix(mat1);
	mat1.data = init(mat1);

	//print(mat1);

	printf("Quante righe ha la seconda matrice? ");
	scanf("%d", &row2);
	printf("Quante colonne ha la seconda matrice? ");
	scanf("%d", &col2);

	struct Matrix mat2 = {row2, col2};
	mat2.data = allocMatrix(mat2);
	mat2.data = init(mat2);

	//print(mat2);
	printf("-------------------------\n");
	threadCreate(&mat1, &mat2, 2);
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
