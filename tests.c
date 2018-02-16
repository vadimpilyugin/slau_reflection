// Standard headers
#include <stdio.h>
// exit
#include <stdlib.h>

// Libraries
#include "matrix.h"

#define FAIL "FAIL!"
#define OK NULL
#define FALSE 0
#define HUGE 23
#define BIG 8
#define MEDIUM 5
#define SMALL 3

int g_n; // для некоторых генераторов

double matrix_filler_natural_medium (int row, int col) {
	return row*MEDIUM+col+1;
}
double vector_filler_natural (int row) {
	return row+1;
}
double matrix_filler_small (int row, int col) {
	return row*SMALL+col+1;
}
double matrix_filler_identity (int row, int col) {
	return row == col;
}
double matrix_filler_antidiagonal (int row, int col) {
	return row+col == g_n-1;
}
double vector_filler_countdown (int row) {
	return g_n - row;
}
double vector_filler_small (int row) {
	return row*10+1;
}
double vector_filler_ones (int row) {
	return 1;
}
double matrix_filler_ones (int row, int col) {
	return 1;
}
double matrix_filler_slau (int row, int col) {
	double a [MEDIUM][MEDIUM] = {
		{ 45.763030, 44.269726, 31.264127, 24.050056, 82.452115 },
		{ 12.769607, 29.755301, 50.650619, 95.263367, 1.872111 },
		{ 73.776901, 45.171356, 40.387992, 31.608756, 53.029842 },
		{ 1.276430, 2.474284, 78.646368, 75.875657, 9.722007 },
		{ 82.509788, 58.273965, 55.112964, 9.725793, 56.041680 }
	};
	return a[row][col];
}
double vector_filler_slau (int row) {
	double a [MEDIUM] = {
		53.629906, 91.537775, 3.781911, 14.883654, 42.956326
	};
	return a[row];
}
double vector_filler_slau_solution (int row) {
	double a [MEDIUM] = {
		-2.295191,4.060002,0.270309,-0.137051,-0.318066
	};
	return a[row];
}
double vector_m_v_mult_solution (int row) {
	double a [MEDIUM] = {
		15, 40, 65, 90, 115
	};
	return a[row];
}
double matrix_transformer_triagonalizer (int i, int j, double elem) {
	if (i > j)
		return 0;
	else
		return elem;
}
double matrix_filler_huge (int row, int col) {
	// от 1 до HUGE^2
	return row*HUGE+col+1;
}

void print_test (const char *test) {
	if (test != NULL)
		printf ("%s...",test);
	else
		printf ("pass!\n");
}

void assert (int expr, const char *msg) {
	if (!expr) {
		print_test (FAIL);
		if (msg != NULL)
			fprintf (stderr, "--- %s\n", msg);
		exit (ERROR);
	}
}

void test_hello_world () {
	print_test ("test_hello_world");
	// ======= test code ==========

	printf ("\n\nHello, World!\n\n");
	
	// ======= test code ==========
	print_test (OK);
}

void test_create_and_fill_matrix () {
	print_test ("test_create_and_fill_matrix");
	// ======= test code ==========

	Matrix A = matrix_new_and_fill (MEDIUM, matrix_filler_natural_medium);
	matrix_print (A);
	matrix_free (A);
	
	// ======= test code ==========
	print_test (OK);
}

void test_create_and_fill_vector () {
	print_test ("test_create_and_fill_vector");
	// ======= test code ==========

	Vector V = vector_new_and_fill (MEDIUM, vector_filler_natural);
	vector_print (V);
	vector_free (V);
	
	// ======= test code ==========
	print_test (OK);
}


void test_read_file () {
	print_test ("test_read_file");
	// ======= test code ==========
	
	const char * filename = "matrices/matrix3x3";
	// сначала запишем данные
	FILE * file = fopen (filename, "w");
	assert (file != NULL, "fopen не удался");

	Matrix A_init = matrix_new_and_fill (SMALL, matrix_filler_small);
	Vector V_init = vector_new_and_fill (SMALL, vector_filler_small);

	int row;
	int col;
	fprintf (file, "%d\n", SMALL);
	for (row = 0; row < SMALL; row++) {
		for (col = 0; col < SMALL; col++) { 
			fprintf (file, "%lf ", A_init.data[INDEX(row,col,A_init.size)]);
		}
		fprintf (file, "%lf\n", V_init.data[row]);
	}
	fclose (file);

	// теперь прочитаем эти же данные
	Matrix A;
	Vector V;
	matrix_vector_new_from_file (filename, &A, &V);
	
	double eps = 1e-7;
	if (matrix_abs_diff (A, A_init) > eps) {
		fprintf (stderr, "Матрицы отличаются на %lf\n", matrix_abs_diff (A, A_init));
		assert (FALSE, NULL);
	}
	if (vector_abs_diff (V, V_init) > eps) {
		fprintf (stderr, "Векторы отличаются на %lf\n", vector_abs_diff (V, V_init));
		assert (FALSE, NULL);
	}
	matrix_free(A_init);
	vector_free(V_init);
	matrix_free(A);
	vector_free(V);
	// ======= test code ==========
	print_test (OK);
}


void test_triagonalization () {
	print_test ("test_triagonalization");
	// ======= test code ==========

	Matrix A = matrix_new_and_fill (MEDIUM, matrix_filler_slau);
	Vector V = vector_new_and_fill (MEDIUM, vector_filler_slau);
	Vector True_X = vector_new_and_fill (MEDIUM, vector_filler_slau_solution);
	
	matrix_triagonalize (A, V);
	matrix_print (A);

	Matrix A_copy = matrix_new_copy (A);
	matrix_transform (A_copy, matrix_transformer_triagonalizer);
	assert (matrix_abs_diff (A, A_copy) < 1e-7, "Матрица не треугольная!");

	matrix_free (A);
	vector_free (V);
	vector_free (True_X);

	// ======= test code ==========
	print_test (OK);
}

void test_triagonalization_identity () {
	print_test ("test_triagonalization_identity");
	// ======= test code ==========

	Matrix A = matrix_new_and_fill (MEDIUM, matrix_filler_identity);
	Matrix A_copy = matrix_new_copy (A);
	Vector V = vector_new (A.size);
	matrix_triagonalize (A, V);

	matrix_print (A);

	assert (matrix_abs_diff (A, A_copy) < 1e-7, "Матрица не диагональная!");

	matrix_free (A);
	matrix_free (A_copy);
	vector_free (V);

	// ======= test code ==========
	print_test (OK);
}

void test_slau_solving () {
	print_test ("test_slau_solving");
	// ======= test code ==========

	Matrix A = matrix_new_and_fill (MEDIUM, matrix_filler_slau);
	Vector V = vector_new_and_fill (MEDIUM, vector_filler_slau);
	Vector True_X = vector_new_and_fill (MEDIUM, vector_filler_slau_solution);
	
	matrix_triagonalize (A, V);
	Vector X = gaussian_elimination (A, V);
	vector_print (X);

	double eps = 1e-5;
	assert (vector_abs_diff (X, True_X) < eps, "Решения не совпадают!");
	printf("--- Невязка: %f\n\n", matrix_slau_difference (A, X, V));

	matrix_free (A);
	vector_free (V);
	vector_free (True_X);
	vector_free (X);

	// ======= test code ==========
	print_test (OK);
}

void test_slau_antidiagonal () {
	print_test ("test_slau_antidiagonal");
	// ======= test code ==========

	g_n = MEDIUM;
	Matrix A = matrix_new_and_fill (MEDIUM, matrix_filler_antidiagonal);
	Vector V = vector_new_and_fill (MEDIUM, vector_filler_natural);
	Vector True_X = vector_new_and_fill (MEDIUM, vector_filler_countdown);

	matrix_triagonalize (A, V);
	Vector X = gaussian_elimination (A, V);
	vector_print (X);

	double eps = 1e-5;
	assert (vector_abs_diff (X, True_X) < eps, "Решения не совпадают!");
	printf("--- Невязка: %f\n\n", matrix_slau_difference (A, X, V));

	matrix_free (A);
	vector_free (V);
	vector_free (True_X);
	vector_free (X);

	// ======= test code ==========
	print_test (OK);
}

void test_slau_degenerate_case () {
	print_test ("test_slau_degenerate_case");
	// ======= test code ==========

	Matrix A = matrix_new_and_fill (MEDIUM, matrix_filler_natural_medium);
	Vector V = vector_new_and_fill (MEDIUM, vector_filler_natural);
	
	matrix_triagonalize (A, V);
	matrix_print (A);
	Vector X = gaussian_elimination (A, V);
	assert (X.data == NULL, "Не освободили память!");

	matrix_free (A);
	vector_free (V);
	vector_free (X);

	// ======= test code ==========
	print_test (OK);
}


void test_matrix_vector_mult () {
	print_test ("test_matrix_vector_mult");
	// ======= test code ==========

	Matrix A = matrix_new_and_fill (MEDIUM, matrix_filler_natural_medium);
	Vector V = vector_new_and_fill (MEDIUM, vector_filler_ones);
	Vector Res = matrix_vector_mult (A, V);
	Vector True_Res = vector_new_and_fill (MEDIUM, vector_m_v_mult_solution);

	vector_print (Res);
	assert (vector_abs_diff (Res, True_Res) < 1e-7, NULL);

	matrix_free (A);
	vector_free (V);
	vector_free (Res);
	vector_free (True_Res);

	// ======= test code ==========
	print_test (OK);
}


int main(int argc, char **argv) {

	// =================== Tests ======================

	test_hello_world ();
	test_create_and_fill_matrix ();
	test_create_and_fill_vector ();
	test_read_file ();
	test_triagonalization ();
	test_triagonalization_identity ();
	test_slau_solving ();
	test_slau_antidiagonal ();
	test_slau_degenerate_case ();
	test_matrix_vector_mult ();

	// =================== Tests ======================

	return 0;
}
