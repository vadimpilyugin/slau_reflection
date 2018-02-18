// Standard headers
#include <stdio.h>
// exit, atoi
#include <stdlib.h>
// clock
#include <time.h>

// Dependencies
#include "matrix.h"

// Definitions
#define N_ARGS_FILE 3
#define ARG_PROG 0
#define ARG_INPUT_FILE 1
#define ARG_OUTPUT_FILE 2

#define N_ARGS_FORMULA 2
#define ARG_SIZE 1

#define DEFAULT_OUTPUT_FILE "matrices/answer"
#define SUCCESS 0
#define MAX_SIZE_OUTPUT 20
#define EPS 1e-4

// Измерение времени
clock_t sum_time = 0, tmp_time;

void go()
{
	tmp_time = clock();
}
double stop()
{
	tmp_time = clock() - tmp_time;
	sum_time += tmp_time;
	double t_sec = sum_time / (double) CLOCKS_PER_SEC;
	sum_time = 0;
	return t_sec;
}

double matrix_filler (int row, int col) {
	// row и col от 0
	if (row == col)
		return 1*(row+1);
	else
		return 0;
}
double vector_filler (int row) {
	// row от 0
	return row+1;
}

// Functions
int main (int argc, char *argv[]) {
	Matrix A;
	Vector V;

	// если задан входной и выходной файл
	if (argc == N_ARGS_FILE) {
		matrix_vector_new_from_file (argv[ARG_INPUT_FILE], &A, &V);
	} else if (argc == N_ARGS_FORMULA) {
		// иначе по формуле
		int size = atoi (argv[ARG_SIZE]);
		A = matrix_new_and_fill (size, matrix_filler);
		V = vector_new_and_fill (size, vector_filler);
	} else {
		fprintf (
			stderr, "\n\nUsage: %s <input file> <output file>\n\tOR\n%s <size>\n", 
			argv[0],argv[0]
		);
		exit (ERROR);
	}
	int on_screen = A.size <= MAX_SIZE_OUTPUT;
	printf ("--- Размер системы: %d\n", A.size);
	if (on_screen) {
		printf ("\n--- Матрица A:");
		matrix_print (A);
		printf ("\n--- Вектор b:");
		vector_print (V);
	}
	// приведение к треугольному виду
	go ();
	matrix_triagonalize (A, V);
	double triag_time = stop ();
	printf ("--- Приведение к треугольному виду: %lf сек\n", triag_time);
	// решение обратным ходом
	go ();
	Vector X = gaussian_elimination (A, V);
	double gauss_time = stop ();
	printf ("--- Обратный ход метода Гаусса: %lf сек\n", gauss_time);
	if (X.data == NULL) {
		// матрица вырожденная
		exit (SUCCESS);
	}
	if (on_screen) {
		printf ("--- Решение:");
		vector_print (X);
	} else {
		vector_write_to_file (X, DEFAULT_OUTPUT_FILE);
	}

	// считаем невязку
	double diff = matrix_slau_difference (A, X, V);
	printf ("--- Невязка = %f\n\n", diff);
	if (diff > EPS)
		exit (ERROR);
	matrix_free (A);
	vector_free (X);
	vector_free (V);
	exit (SUCCESS);
}