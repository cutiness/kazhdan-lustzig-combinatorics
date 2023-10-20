#ifndef POLYNOMIALS
#define POLYNOMIALS
/*--------------------------------*/
#ifndef PERMUTATION_BASICS
#include "permutation-basics.h"
#endif //!PERMUTATION_BASICS
/*--------------------------------*/
#ifndef BRUHAT_ORDER
#include "bruhat-order.h"
#endif // !BRUHAT_ORDER
/*--------------------------------*/
#ifndef BRUHAT_MATRIX
#include "bruhat-matrix.h"
#endif // !BRUHAT_MATRIX
/*--------------------------------*/
#include <stdexcept> // std::out_of_range
#endif // !POLYNOMIALS

// Type definitions

struct Polynomial
{
    std::map<float /*power*/, float /*coefficient*/> coefficients;
};

/* This is used in the K-L graph, provided some long list of conditions are satisfied.*/
struct k_l_edge
{
    Polynomial greek_mu_bar;
};

// a bidirectional graph where vertices and edges carry extra necessary information
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
        PermtVertex, k_l_edge> k_l_graph;

/*--------------------------Global variables, just their declerations-----------------------*/

// The database that is used to calculate K-L polynomials more efficiently
extern std::vector<std::vector<Polynomial>> k_l_database;

// The temporary database used to store elements that are not in k_l_database, when the program ends
// this should be appended to the specificied database_name file, by default "KL-database.txt"
extern std::vector<std::vector<Polynomial>> temp_database;

// The name of the file containing the database, change this value as required.
extern std::string database_name; /*  = "KL-database<number>.txt" , by default                      */

/*
 This variable is a pair consisting of a bruhat_graph and a map, which includes necessary data to
 analyse the graph, it is used when the same graph needs to be accessed for multiple times
 'polynom_k_l' function utilizes this variable to make operations more efficient
*/
extern std::pair<bruhat_graph, std::map<std::vector<int>, PermtData>> bruhat_data;


/*------------------------------------------------------------------------------------------*/

// function definitions

Polynomial polynom_add(Polynomial poly1, Polynomial poly2);

Polynomial polynom_subtract(Polynomial poly1, Polynomial poly2);

Polynomial polynom_multiply(Polynomial poly1, Polynomial poly2);

void polynom_display(FILE* ifp, Polynomial poly);

void k_l_database_initiate(void);

/*  Default -1 values are just placeholders, negative indexes can't be achieved normally, in this program */
std::pair<bool, Polynomial> k_l_database_check(std::pair<std::vector<int>, std::vector<int>> p, int v1_index = -1, int v2_index = -1);

void k_l_database_append(void);

/* This functions utilizes a global variable 'bruhat_data', look at the source code file for more info */
Polynomial polynom_k_l(std::vector<int> u, std::vector<int> v, PermtData u_data = {-1,-1}, PermtData v_data = {-1,-1}, bool check_database = true);

Polynomial polynom_greek_mu(std::vector<int> u, std::vector<int> v, PermtData u_data = {-1,-1}, PermtData v_data = {-1,-1});

Polynomial polynom_greek_mu_standalone(std::vector<int> u, std::vector<int> v, PermtData u_data = {-1,-1}, PermtData v_data = {-1,-1});

Polynomial polynom_k_l_standalone(std::vector<int> u, std::vector<int> v, PermtData u_data = {-1,-1}, PermtData v_data = {-1,-1}, bool check_database = true);

Polynomial polynom_greek_mu_bar(std::vector<int> u, std::vector<int> v);

k_l_graph k_l_graph_all_sn(int n, std::pair<bruhat_graph, std::map<std::vector<int>, PermtData>> bruhat_data);

void temp_database_append(std::pair<int, int> vec_indexes, Polynomial temp_poly);
