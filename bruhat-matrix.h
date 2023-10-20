#ifndef BRUHAT_MATRIX
#define BRUHAT_MATRIX
/* ------------------------------ */
#ifndef PERMUTATION_BASICS
#include "permutation-basics.h"
#endif // !PERMUTATION_BASICS
/* ------------------------------ */
#ifndef BRUHAT_ORDER
#include "bruhat-order.h"
#endif // !BRUHAT_ORDER
/* ------------------------------ */
//#ifndef POLYNOMIALS
//#include "polynomials.h"
//#endif // !POLYNOMIALS
/* ------------------------------- */
#include <thread>
#endif // !BRUHAT_MATRIX

/* Global variables */

/*  This matrix contains the data about bruhat_order for the group specified by 'current_sn_group' global variable 
 *  In order to initialize it, look at 'bruhat_matrix_all_sn' to create it from scratch, or 'bruhat_matrix_initiate' 
 *  to read data from a file, 'bruhat-matrix(number).txt' by default. */
extern int** b_matrix;



/*  Function declarations */

void bruhat_matrix_all_sn_multi_threaded(int n);

void bruhat_matrix_all_sn(int n);

void bruhat_matrix_worker_function(int start_i, int end_i);

void bruhat_matrix_write(std::string file_name = "bruhat-matrix");

void bruhat_matrix_initiate(std::string file_name = "bruhat-matrix");

/* Indexes '-1' are just placeholder values, they are just there to let the program know that no special index
 * output is provided. Normally, negative indexes are not used with the program. */
std::vector<int> bruhat_matrix_interval(std::vector<int> u, std::vector<int> v, int u_index = -1, int v_index = -1);
