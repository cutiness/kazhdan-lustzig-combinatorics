#include "bruhat-matrix.h"

using namespace std;

/* This is where the data obtained by bruhat_matrix_all_sn will be stored
 * 'b_matrix' is a global variable, for more info please look at "bruhat-matrix.h" */
int** b_matrix;

/*
 * Return an n x n matrix containing information about whether or not two permutation is comparable
 * with respect to bruhat order. Comparision is done from left to right, meaning what if u <(B) w
 * then the matrix has the entry '1' on (row(u), column(w)) , otherwise it will be zero.
 *
 */
void bruhat_matrix_all_sn_multi_threaded(int n){
    //auto all_p = permt_all_sn(n);
    //vector<int> all_p_len;
    int n_f = factorial(n);

    /* Create a vector contaning lengths of permutations  */
    //for(int i = 0; i < n_f; i++){
    //    all_p_len.push_back(permt_inversion_amount(all_p[i]));
    //}

    /* Dividing the task into smaller bits */
    thread b_worker1(bruhat_matrix_worker_function, 0, n_f/7);
    thread b_worker2(bruhat_matrix_worker_function, (n_f/7)+1, 2*n_f/7);
    thread b_worker3(bruhat_matrix_worker_function, (2*n_f/7)+1, 3*n_f/7);
    thread b_worker4(bruhat_matrix_worker_function, (3*n_f/7)+1, 4*n_f/7);
    thread b_worker5(bruhat_matrix_worker_function, (4*n_f/7)+1, 5*n_f/7);
    thread b_worker6(bruhat_matrix_worker_function, (5*n_f/7)+1, 6*n_f/7);
    thread b_worker7(bruhat_matrix_worker_function, (6*n_f/7)+1, n_f-1); /* -1 is because of index */
    b_worker1.join();
    b_worker2.join();
    b_worker3.join();
    b_worker4.join();
    b_worker5.join();
    b_worker6.join();
    b_worker7.join();
}

/*
 * Return an n x n matrix containing information about whether or not two permutation is comparable
 * with respect to bruhat order. Comparision is done from left to right, meaning what if u <(B) w
 * then the matrix has the entry '1' on (row(u), column(w)) , otherwise it will be zero.
 *
 * This is the single threaded version of the function bruhat_matrix_all_sn_multi_threaded
 *
 */
void bruhat_matrix_all_sn(int n){
    //auto all_p = permt_all_sn(n);
    //vector<int> all_p_len;
    int n_f = factorial(n);

    bruhat_matrix_worker_function(0, n_f-1);
}

/*
 * This function does not have a meaning on its own, it divides the task af finding bruhat_matrix into
 * smaller parts so that multiple threads can work on it, for now that number is '6'
 */
void bruhat_matrix_worker_function(int start_i, int end_i){
    for (int i = start_i; i <= end_i; i++) {
        for(int j = 0; j < all_p.size(); j++){
            if(bruhat_compare(all_p[i], all_p[j], all_p_len[i], all_p_len[j])) b_matrix[i][j] = 1;
            else b_matrix[i][j] = 0;
        }
    }
}

/*  By default file_name = "bruhat-matrix" */
void bruhat_matrix_write(string file_name){
    ostringstream s; s << file_name << current_sn_group << ".txt";
    int f_n = factorial(current_sn_group);
    FILE* ifp = fopen(s.str().c_str(), "a");
    if(ifp == NULL) ifp = fopen(s.str().c_str(), "w");

    for(int i = 0; i < f_n; i++){
        for(int j = 0; j < f_n; j++){
            fprintf(ifp, "%d", b_matrix[i][j]);
        }
        fprintf(ifp, "\n");
    }
    fclose(ifp);
}

/*  By default file_name = "bruhat-matrix"
 *  It is up to the programmer to make sure that 'b_matrix' has the space to take the data.
 *  Please do it beforehand. */
void bruhat_matrix_initiate(string file_name){
    ostringstream s; s << file_name << current_sn_group << ".txt";
    int f_n = factorial(current_sn_group);
    FILE* ifp = fopen(s.str().c_str(), "r");
    if(ifp == NULL){
        printf("%s%s", "  No previous bruhat matrix data is found, generating for the entire group...\n",
                       "  This might take some time, stand still...\n");
        bruhat_matrix_all_sn(current_sn_group);
        bruhat_matrix_write();
        return;
    }

    for(int i = 0; i < f_n; i++){
        for(int j = 0; j < f_n; j++){
            b_matrix[i][j] = getc(ifp) - 48;
        }
        char temp_c = getc(ifp); /*  This will either be a '\n' of EOF */
    }
    fclose(ifp);
}

/* Using the data stored the global variable 'b_matrix', this function returns indexes of function
 * that stay between u and v. (Endpoints are inclusive) */
vector<int> bruhat_matrix_interval(vector<int> u, vector<int> v, int u_index, int v_index){
    if(u_index == -1) u_index = all_p_data[u].index;
    if(v_index == -1) v_index = all_p_data[v].index;
    int f_n = factorial(current_sn_group);
    vector<int> u_z /* elements z satisfying u <= z*/;
    vector<int> z_v /* elements z satisfying z <= v*/;
    vector<int> intersection_vec; /* The place where both of these conditions hold*/

    /* Note, the end points u and z will be added at the end, later on */
    for(int i = 0; i < f_n; i++){
        if(b_matrix[u_index][i] == 1) u_z.push_back(i);
        if(b_matrix[i][v_index] == 1) z_v.push_back(i);
    }
    /* The algorith below uses sorted pieces of data to take their intersection. As u_z, z_v already
     * contains sorted index data by default, we shall not perform that extra step here.*/
    set_intersection(u_z.begin(), u_z.end(), z_v.begin(), z_v.end(), back_inserter(intersection_vec));
    /* Adding indexes of u and v itself here, at the end */
    intersection_vec.push_back(u_index); intersection_vec.push_back(v_index);
    return intersection_vec;
}
