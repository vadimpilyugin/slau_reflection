// Standard headers
#include <stdio.h>
// atoi, exit, malloc, free
#include <stdlib.h>
// errno
#include <errno.h>
// memset
#include <string.h>
// fabs
#include <math.h>

// Header
#include "matrix.h"

#define ZERO 0
#define ONE 1
#define EPS 1e-7

// Macros
#define sqr(x) ((x)*(x))

// Functions

Matrix matrix_new (int size) {
	Matrix A;
	A.data = (double *) malloc (size*size*sizeof(double));
	if (A.data == NULL) {
		perror ("matrix_new");
		fprintf (stderr, "Значение size: %d\n", size);
		exit (ERROR);
	}
	A.size = size;
	return A;
}

Vector vector_new (int size) {
	Vector V;
	V.data = (double *) malloc (size*sizeof(double));
	if (V.data == NULL) {
		perror ("vector_new");
		fprintf (stderr, "Значение size: %d\n", size);
		exit (ERROR);
	}
	V.size = size;
	return V;
}

void matrix_free (Matrix A) {
	if (A.data != NULL)
		free (A.data);
}

void vector_free (Vector V) {
	if (V.data != NULL)
		free (V.data);
}

void matrix_vector_new_from_file (const char * filename, Matrix * _A, Vector * _V) {
	FILE * input_file = fopen (filename, "r");
	if (input_file == NULL) {
		perror ("matrix_vector_new_from_file: fopen");
		exit (ERROR);
	}
	
	int size;
	int chars_read = fscanf (input_file, "%d", &size);
	if (chars_read == EOF) {
		errno = EINVAL;
		perror ("matrix_vector_new_from_file: EOF в начале файла!");
		exit (ERROR);
	}

	Matrix A = matrix_new (size);
	Vector V = vector_new (size);

	int row;
	int col;
	for (row = 0; row < size; row++) {
		for (col = 0; col < size; col++) { 
			chars_read = fscanf (
				input_file, "%lf", 
				A.data + INDEX(row,col,size)
			);
			if (chars_read == EOF) {
				errno = EINVAL;
				perror ("matrix_vector_new_from_file: EOF при чтении матрицы!");
				fprintf(stderr, "Строка: %d\nСтолбец: %d\n", row, col);
				exit (ERROR);
			}
		}
		// нужно считывать еще и вектор b
		chars_read = fscanf (input_file, "%lf", V.data + row);
		if (chars_read == EOF) {
			errno = EINVAL;
			perror ("matrix_vector_new_from_file: EOF при чтении b!");
			fprintf(stderr, "Строка: %d\n", row);
			exit (ERROR);
		}
	}
	fclose (input_file);

	*_V = V;
	*_A = A;
}

void vector_fill (Vector V, double (*func)(int)) {
	int row;

	for (row = 0; row < V.size; row++) {
		V.data[row] = func (row);
	}
}

void matrix_fill (Matrix A, double (*func)(int, int)) {
	int row;
	int col;

	for (row = 0; row < A.size; row++) {
		for (col = 0; col < A.size; col++) {
			A.data[INDEX (row, col, A.size)] = func (row,col);
		}
	}
}

Matrix matrix_new_and_fill (int size, double (*func)(int, int)) {
	Matrix A = matrix_new (size);
	matrix_fill (A, func);
	return A;
}

void matrix_print (Matrix A) {
	printf("\n\n");

	int row;
	int col;

	for (row = 0; row < A.size; row++) {
		for (col = 0; col < A.size; col++) {
			printf ("%8.4lf ", A.data[INDEX (row, col, A.size)]);
		}
		printf ("\n");
	}
	printf("\n\n");
}

Vector vector_new_and_fill (int size, double (*func)(int)) {
	Vector V = vector_new (size);
	vector_fill (V, func);
	return V;
}

Matrix matrix_new_copy (Matrix A) {
	Matrix A_copy = matrix_new (A.size);
	memcpy (A_copy.data, A.data, A.size*A.size*sizeof(double));
	return A_copy;
}

void matrix_transform (Matrix A, double (*func)(int,int,double)) {
	int row;
	int col;

	for (row = 0; row < A.size; row++) {
		for (col = 0; col < A.size; col++) {
			int index = INDEX (row, col, A.size);
			A.data[index] = func (row,col,A.data[index]);
		}
	}
}

void vector_print (Vector V) {
	int row;

	printf("\n\n");
	for (row = 0; row < V.size; row++) {
		printf ("%8.4lf\n", V.data[row]);
	}
	printf("\n\n");
}

double matrix_abs_diff (Matrix A, Matrix B) {
	if (A.size != B.size) {
		errno = EINVAL;
		perror ("matrix_abs_diff: размеры не совпадают!");
		exit (ERROR);
	}

	int size = A.size;
	int row;
	int col;
	double diff = 0;

	for (row = 0; row < size; row++) {
		for (col = 0; col < size; col++) {
			int index = INDEX(row,col,size);
			double tmp = fabs (A.data[index]-B.data[index]);
			if (tmp > diff)
				diff = tmp;
		}
	}

	return diff;
}

double vector_abs_diff (Vector U, Vector V) {
	if (U.size != V.size) {
		errno = EINVAL;
		perror ("vector_abs_diff: размеры не совпадают!");
		exit (ERROR);
	}

	int size = V.size;
	int row;
	double diff = 0;

	for (row = 0; row < size; row++) {
		int index = row;
		double tmp = fabs (U.data[index]-V.data[index]);
		if (tmp > diff)
			diff = tmp;
	}

	return diff;
}

void matrix_triagonalize (Matrix A, Vector V) {
	if (A.size != V.size) {
		errno = EINVAL;
		perror ("matrix_triagonalize: размеры не совпадают!");
		exit (ERROR);
	}
	int n = A.size;
	int k;
	int row;
	int col;
	Vector X = vector_new (n);

	for (k = 0; k < n-1; k++) {

		// считаем вектор x матрицы отражения
		// x = (a - ||a||*e) / ||a - ||a||*e||

		// S_k = sum for i = k+1..n (A[i,k]^2)
		double s_k = 0;
		for (row = k+1; row < n; row++)
			s_k += sqr (A.data[INDEX(row,k,n)]);

		// ||a|| = sqrt (S_k + A[k,k]^2)
		double norm_a = sqrt (s_k + sqr(A.data[INDEX(k,k,n)]));
		if (norm_a < EPS) {
			vector_free (X);
			return;
		}

		// X[k] = A[k,k]-norm(A[k])
		X.data[k] = A.data[INDEX(k,k,n)] - norm_a;

		// X[k+1..n] = A[k+1..n,k]
		for (row = k+1; row < n; row++)
			X.data[row] = A.data[INDEX(row,k,n)];

		// ||X|| = sqrt (S_k + X[k]^2)
		double norm_x = sqrt (s_k + sqr (X.data[k]));
		if (norm_x < EPS) {
			// вектор уже повернут
			continue;
		}
		// X = X / norm_x
		for (row = k; row < n; row++) {
			X.data[row] /= norm_x;
		}

		// считаем преобразование матрицы A
		// a = (E - 2xx*)a = a - (2x*a)x
		double dot_product;
		for (col = k; col < n; col++) {
			// dot_product = x*a
			dot_product = 0;
			for (row = k; row < n; row++) {
				dot_product += A.data[INDEX(row,col,n)]*X.data[row];
			}
			// 2x*a
			dot_product *= 2;
			// a = a - (2x*a)x
			for (row = k; row < n; row++) {
				int index = INDEX(row,col,n);
				A.data[index] -= dot_product*X.data[row];
			}
		}
		// считаем преобразование вектора b
		// a = (E - 2xx*)a = a - (2x*a)x
		// dot_product = x*a
		dot_product = 0;
		for (row = k; row < n; row++) {
			dot_product += V.data[row]*X.data[row];
		}
		// 2x*a
		dot_product *= 2;
		// a = a - (2x*a)x
		for (row = k; row < n; row++) {
			V.data[row] -= dot_product*X.data[row];
		}
	}
	vector_free (X);
}

// обратный проход метода Гаусса
Vector gaussian_elimination (Matrix A, Vector V) {
	if (A.size != V.size) {
		errno = EINVAL;
		perror ("gaussian_elimination: размеры не совпадают!");
		exit (ERROR);
	}
	int n = A.size;
	Vector X = vector_new (n);
	int k;
	int i;

	// вычисляем X[n-1]..X[0]
	for (k = n-1; k >= 0; k--) {
		if (fabs (A.data[INDEX(k,k,n)]) < EPS) {
			fprintf (stderr,"\n\n--- Матрица вырожденная\n\n");
			vector_free (X);
			X.data = NULL;
			return X;
		}

		// X[k] = (b[k] - sum for i=(k+1..n-1) (a[k,i]*x[i])) / a[k,k]
		double sum = 0;
		for (i = k+1; i < n; i++)
			sum += A.data[INDEX(k,i,n)]*X.data[i];
		X.data[k] = (V.data[k] - sum) / A.data[INDEX(k,k,n)];
	}

	return X;
}

Vector matrix_vector_mult (Matrix A, Vector X) {
	if (A.size != X.size) {
		errno = EINVAL;
		perror ("gaussian_elimination: размеры не совпадают!");
		exit (ERROR);
	}
	int n = A.size;
	Vector Res = vector_new (n);
	int row;
	int k;

	for (row = 0; row < n; row++) {
		double sum = 0;
		for (k = 0; k < n; k++)
			sum += A.data[INDEX(row,k,n)]*X.data[k];
		Res.data[row] = sum;
	}

	return Res;
}

double vector_norm (Vector X) {
	int row;
	double sum = 0;
	for (row = 0; row < X.size; row++)
		sum += sqr (X.data[row]);
	return sum;
}

double matrix_slau_difference (Matrix A, Vector X, Vector V) {
	// невязка = ||Ax-b||

	int n = A.size;
	int row;
	// V_calc = Ax
	Vector V_calc = matrix_vector_mult (A, X);
	// V_calc = Ax-b
	for (row = 0; row < n; row++)
		V_calc.data[row] -= V.data[row];
	// возвращаем норму
	return vector_norm (V_calc);
}

void vector_write_to_file (Vector X, const char *filename) {
	FILE * file = fopen (filename, "w");
	if (file == NULL) {
		perror ("vector_write_to_file: fopen");
		exit (ERROR);
	}
	
	int row;
	for (row = 0; row < X.size; row++)
		fprintf (file, "%f\n", X.data[row]);
	fclose (file);
}