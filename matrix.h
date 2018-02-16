#ifndef __MY_MATRIX_H
#define __MY_MATRIX_H

// Definitions
#define SUCCESS 0
#define ERROR 2

// Macros
#define INDEX(row, col, size) ((row)*(size)+(col))

typedef struct {
	double *data;
	int size;
} Matrix;

typedef struct {
	double *data;
	int size;
} Vector;

// creation
Matrix matrix_new (int size);
Matrix matrix_new_and_fill (int size, double (*func)(int, int));
void matrix_free (Matrix);
void matrix_vector_new_from_file (const char * filename, Matrix * _A, Vector * _V);
Vector vector_new (int size);
void vector_free (Vector);
Vector vector_new_and_fill (int size, double (*func)(int));
Matrix matrix_new_copy (Matrix A);

void matrix_print (Matrix);
void vector_print (Vector);

double matrix_abs_diff (Matrix A, Matrix B);
double vector_abs_diff (Vector U, Vector V);
void matrix_triagonalize (Matrix A, Vector V);
Vector gaussian_elimination (Matrix A, Vector V);
void matrix_transform (Matrix A, double (*func)(int,int,double));
Vector matrix_vector_mult (Matrix A, Vector X);
double matrix_slau_difference (Matrix A, Vector X, Vector V);
void vector_write_to_file (Vector X, const char *fn);

#endif