// Standard headers
#include <stdio.h>
// exit
#include <stdlib.h>

// Dependencies
#include "matrix.h"

// Definitions
#define INPUT_FILE "matrices/slau"
#define OUTPUT_FILE "matrices/answer"
#define SUCCESS 0
#define MAX_SIZE_OUTPUT 20
#define EPS 1e-4

// Functions
int main () {
	Matrix A;
	Vector V;
	matrix_vector_new_from_file (INPUT_FILE, &A, &V);
	int on_screen = A.size < MAX_SIZE_OUTPUT;
	printf ("--- Размер системы: %d\n", A.size);
	if (on_screen) {
		printf ("\n--- Матрица A:");
		matrix_print (A);
		printf ("\n--- Вектор b:");
		vector_print (V);
	}
	// приведение к треугольному виду
	matrix_triagonalize (A, V);
	// решение обратным ходом
	Vector X = gaussian_elimination (A, V);
	if (X.data == NULL) {
		// матрица вырожденная
		exit (SUCCESS);
	}
	if (on_screen) {
		printf ("--- Решение:");
		vector_print (X);
	} else {
		vector_write_to_file (X, OUTPUT_FILE);
	}

	// считаем невязку
	double diff = matrix_slau_difference (A, X, V);
	printf ("--- Невязка = %f\n\n", diff);
	if (diff > EPS)
		exit (ERROR);
	exit (SUCCESS);
}