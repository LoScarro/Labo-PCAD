#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Matrix
{
	int rows;	// number of rows
	int cols;	// number of columns
	int **data; // <- note that this is a pointer to one dim array
};

void print(struct Matrix mat)
{
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
			printf("%d ", mat.data[i][j]);
		printf("\n");
	}
}

int **multiply(struct Matrix mat1, struct Matrix mat2)
{
	int **result = malloc(mat1.rows * sizeof(int *));
	for (int i = 0; i < mat1.rows; i++)
		result[i] = (int *)calloc(mat2.cols, sizeof(int));

	for (int i = 0; i < mat1.rows; i++)
	{
		for (int j = 0; j < mat2.cols; j++)
		{
			// find product
			for (int k = 0; k < mat1.cols; k++)
				result[i][j] += mat1.data[i][k] * mat2.data[k][j];
		}
	}

	return result;
}

int **init(struct Matrix mat)
{
	srand(time(NULL)); // Initialization, should only be called once.

	int i, j, count;

	mat.data = malloc(mat.rows * sizeof(int *));
	for (i = 0; i < mat.rows; i++)
		mat.data[i] = (int *)calloc(mat.cols, sizeof(int));

	// Note that arr[i][j] is same as *(*(arr+i)+j)
	count = 0;
	for (i = 0; i < mat.rows; i++)
		for (j = 0; j < mat.cols; j++)
			mat.data[i][j] = (rand() % 10); // Or *(*(arr+i)+j) = ++count

	print(mat);

	return mat.data;
}

int **split(struct Matrix mat1, struct Matrix mat2, int block)
{
	int row = mat1.rows / block;

	int **temp = malloc(row * sizeof(int *));
	for (int i = 0; i < row; i++)
		temp[i] = (int *)calloc(mat1.cols, sizeof(int));

	struct Matrix res = {row, mat1.cols, temp};

	for (int i = 0; i < row; i++)
		for (int j = 0; j < mat1.cols; j++)
			res.data[i][j] = mat1.data[i][j];

	print(res);
}

int main()
{
	int **mat1, **mat2, **mat3, **res;
	int row1, col1, row2, col2, row3, col3, blockNumber, blockSize;

	printf("Quante righe ha la prima matrice? ");
	scanf("%d", &row1);
	printf("Quante colonne ha la prima matrice? ");
	scanf("%d", &col1);
	struct Matrix A = {row1, col1, mat1};
	A.data=init(A);

	printf("Quante righe ha la seconda matrice? ");
	scanf("%d", &row2);
	printf("Quante colonne ha la seconda matrice? ");
	scanf("%d", &col2);
	struct Matrix B = {row2, col2, mat2};
	B.data=init(B);

	split(A, B, 5);

	struct Matrix RES = {A.rows, B.cols, res};
	RES.data = multiply(A, B);
	print(RES);



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
