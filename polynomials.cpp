#include "polynomials.h"
using namespace std;

/* ---------------------------- GLOBAL VARIABLES ------------------------------------------------------ */

// The database that is used to calculate K-L polynomials more efficiently
// The dummy polynomial that is just used as a placeholder is -q^-1 , which is theoratically not a polynomial
std::vector<std::vector<Polynomial>> k_l_database(100, vector<Polynomial>(100));

// The temporary database used to store elements that are not in k_l_database, when the program ends
// this should be appended to the specified database_name file, by default "KL-database.txt"
// The dummy polynomial that is just used as a placeholder is -q^-1 , which is theoratically not a polynomial
std::vector<std::vector<Polynomial>> temp_database(100, vector<Polynomial>(100));

// The name of the file containing the database, change this value as required.
// The program will add the number 'n' and the file extension '.txt' at the end of this, each S_n group will
// have its own database file. For example 'KL-database4.txt' for S_4 etc.
// The number 'n' comes from the global variable 'current_sn_group' defined in "permutation-basics.h"
string database_name = "KL-database";

// This variable is a pair consisting of a bruhat_graph and a map, which includes necessary data to
// analyse the graph, it is used when the same graph needs to be accessed for multiple times
// 'polynom_k_l' function utilizes this variable to make operations more efficient
pair<bruhat_graph, map<std::vector<int>, PermtData>> bruhat_data = {};

/* -----------------------------------------------------------------------------------------------------*/

// Returns poly1 + poly2
Polynomial polynom_add(Polynomial poly1, Polynomial poly2){
    for(auto itr = poly2.coefficients.begin(); itr != poly2.coefficients.end(); itr++){
        auto fitr = poly1.coefficients.find(itr->first);

        if(fitr != poly1.coefficients.end()) fitr->second += itr->second;
        else poly1.coefficients[itr->first] = itr->second;
    }
    return poly1; // changes are made on poly1, so this becomes the result
}

// Returns poly1 - poly2
Polynomial polynom_subtract(Polynomial poly1, Polynomial poly2){
    // multiplying poly2 with -1
    for(auto itr = poly2.coefficients.begin(); itr != poly2.coefficients.end(); itr++){
        itr->second *= -1;
    }
    return polynom_add(poly1, poly2);
}

// Returns poly1 * poly2
Polynomial polynom_multiply(Polynomial poly1, Polynomial poly2){
    Polynomial result;
    for(auto itr = poly1.coefficients.begin(); itr != poly1.coefficients.end(); itr++){
        pair<int, int> cur_element = {itr->first, itr->second};
        Polynomial poly_temp2;
        for(auto itr2 = poly2.coefficients.begin(); itr2 != poly2.coefficients.end(); itr2++){
            poly_temp2.coefficients.clear();
            // The power adds up , the coefficient is multiplied
            poly_temp2.coefficients[cur_element.first + itr2->first] = cur_element.second * itr2->second;
            result = polynom_add(result, poly_temp2);
        }
    }
    return result;
}

void polynom_display(FILE* ifp, Polynomial poly){
    int i = 0;
    for(auto itr = poly.coefficients.begin(); i < poly.coefficients.size(); i++){
        if(i != poly.coefficients.size() - 1) fprintf(ifp, "%.1fq^%.1f + ", itr->second, itr->first);
        else fprintf(ifp, "%.0fq^%.0f", itr->second, itr->first);
        itr++;
    }
}

// This function takes a valid file_name input, and stores the data inside the file
// using the k_l_database global variable
void k_l_database_initiate(void){
    ostringstream s; s << database_name << current_sn_group << ".txt";
    k_l_database.clear();
    FILE* ifp = fopen(s.str().c_str(), "r"); char c = 0;

    /* If the file does not exist yet, just quit */
    if(ifp == NULL){printf("  K-L polynomial database file does not exits, creating a new one...\n"); return;}

    c = getc(ifp);
    string unit_element;
    vector<float> temp_vec_float;
    pair<int, int> temp_pair; Polynomial temp_poly;
    while(c != EOF){
        if(c == '\n'){
            c = getc(ifp); continue;
        }
        else if(c == '='){
            temp_pair.second = stoi(unit_element);
            unit_element.clear();
        }
        else if(c == ':'){
            temp_pair.first = stoi(unit_element);
            unit_element.clear();
        }
        // k-l polynomial is put between curly brackets, it is handled here in a separate while loop
        else if(c == '{'){
            c = getc(ifp);
            while(c != '}'){
                if(c == ' '){
                    temp_vec_float.push_back(stof(unit_element));
                    unit_element.clear();
                }
                else unit_element.push_back(c); c = getc(ifp);
            }
            // the last element is added here
            temp_vec_float.push_back(stof(unit_element));
            unit_element.clear();

            // Now using the data in temp_vec_float we can construct our polynomial
            /* Example template -> 1 2 3 4:4 3 2 1={0 1} */
            for(int i = 0; i < temp_vec_float.size() - 1; i +=2){
                temp_poly.coefficients[temp_vec_float[i]] = temp_vec_float[i+1];
            }
            // We map the pair of permutations to their corresponding k-l polynomial
            try {
                k_l_database.at(temp_pair.first).at(temp_pair.second) = temp_poly;
            } catch (const out_of_range& error) {
                /* If out of range is returned, make sure vector allocates enough space*/
                if(k_l_database.size() < temp_pair.first + 1) k_l_database.resize(temp_pair.first+1); /* +1 for index*/
                if(k_l_database[temp_pair.first].size() < temp_pair.second + 1){
                    k_l_database[temp_pair.first].resize(temp_pair.second+1); /* +1 for index*/
                }
                k_l_database.at(temp_pair.first).at(temp_pair.second) = temp_poly;
            }
            // Resetting necessary variables
            temp_poly.coefficients.clear(); temp_vec_float.clear(); /* temp_pair = {};  Already overwritten*/
        }
        else{
            unit_element.push_back(c);
        }
        c = getc(ifp);
    }
    fclose(ifp);
}
/*
 This function accepts a pair of permutations, and checks whether or not k-l polynomial
 for those permutations is inside the database
 Notice that the order of these permutations is important!
 The return value is a pair containing a boolean, and the pointer to the wanted element (if found)
 If the function is not able to find necessary value in k_l_database it will check for temp_database as well
 Providing v1_index or v2_index as an argument is not necessary, but in case it is provided, it will be used
*/
pair<bool, Polynomial> k_l_database_check(pair<vector<int>, vector<int>> p, int v1_index, int v2_index){
    if(v1_index == -1) v1_index = all_p_data[p.first].index;
    if(v2_index == -1) v2_index = all_p_data[p.second].index;
    Polynomial p_wanted, p_dummy;

    try {
        p_wanted = k_l_database.at(v1_index).at(v2_index);
    } catch (const out_of_range& error) {
        p_wanted = p_dummy;
    }

    // if not found in k_l_database
    if(p_wanted.coefficients == p_dummy.coefficients){
        try {
            p_wanted = temp_database.at(v1_index).at(v2_index);
        } catch (const out_of_range& error) {
            p_wanted = p_dummy;
        }
        // if found in the temp_database
        if(p_wanted.coefficients != p_dummy.coefficients) return {true, p_wanted};
        else return {false, p_wanted};
    }
    // if found in k_l_database
    else return {true, p_wanted};
}

/*
 This function does what you think it does
 Given a pair of permutations (order is important!) and their k-l polynomial
 this function will append it to the database for later use
 -- The filename for the database is taken from the global variable 'database_name' --
 An example database entry looks like the following:
         1 2 3 4 5:5 4 3 2 1={0 1}
 Format: permt1_with_whitespaces:permt2_with_whitespace={k_l_poly_coefficients}
 For more info consider looking at the definition for 'Polynomial' in polynomails.h
 The function appends the data in 'temp_database' variable at the end of the file.
*/
void k_l_database_append(){
    ostringstream s; s << database_name << current_sn_group << ".txt";
    FILE* ifp = fopen(s.str().c_str(), "a");
    /*If the file does not exist*/
    if(ifp == NULL) ifp = fopen(s.str().c_str(), "w");

    Polynomial p_dummy; /*The impossible polynomial, just a placeholder*/
    if(!temp_database.empty())
        for(int i = 0; i < temp_database.size(); i++){
            for(int j = 0; j < temp_database[i].size(); j++){
                /* Do not write the placeholder data to the file*/
                if(temp_database[i][j].coefficients == p_dummy.coefficients) continue;
                else{
                    fprintf(ifp, "%d", i); fprintf(ifp, ":"); fprintf(ifp, "%d={", j);
                    int k = 0; auto itr = temp_database[i][j].coefficients.begin();
                    for( ; k < temp_database[i][j].coefficients.size() - 1; k++){
                        fprintf(ifp, "%.0f %.0f ", itr->first, itr->second); itr++;
                    }
                    fprintf(ifp, "%.0f %.0f}\n", itr->first, itr->second);
                }
            }
        }
    fclose(ifp); temp_database.clear();
}

/* A convenient way to deal with temp_database variable while program is executing.*/
void temp_database_append(pair<int, int> vec_indexes, Polynomial temp_poly){
    try {
        temp_database.at(vec_indexes.first).at(vec_indexes.second) = temp_poly;
    } catch (const out_of_range& error) {
        /* If out of range is returned, make sure vector allocates enough space*/
        if(temp_database.size() < vec_indexes.first + 1) temp_database.resize(vec_indexes.first+1); /* +1 for index*/
        if(temp_database[vec_indexes.first].size() < vec_indexes.second + 1){
            temp_database[vec_indexes.first].resize(vec_indexes.second+1); /* +1 for index*/
        }
        temp_database.at(vec_indexes.first).at(vec_indexes.second) = temp_poly;
    }
}

/*
 This stands for the Kazhdan-Lustzig polynomial
 Before using this please initiate the database with k_l_database_initiate function defined above
 Calculating the k-l polynomial is a recursive process, and if the k-l polynomial for to permutations is
 already known, then the function will *not* calculate it again. Instead it will use the value in the database.
 For more information check the functions above and the 'k_l_database' global variable.

 This function also makes use of the following global variables:
 ** 'b_matrix', for more info please look at "bruhat-matrix.h", it has to be initialized
 manually by the programmer, BEFOREHAND
 ** 'all_p_data', for more info please look at "permutation-basics.h"
*/                                                                                      /* True by default ~~ */
Polynomial polynom_k_l(vector<int> u, vector<int> v, PermtData u_data, PermtData v_data, bool check_database){
    // By definition, if u = v then P(u, v) = 1
    if(u == v) return {{{0,1}}}; // this is 1*q^0 = 1

    if(u_data.length == -1 ) u_data = all_p_data[u];
    if(v_data.length == -1 ) v_data = all_p_data[v];

    int u_index = u_data.index;
    int v_index = v_data.index, v_len = v_data.length;

    // By definition, if u and v are not comparable with respect to bruhat order , than P(u, v) = 0
    if(b_matrix[u_index][v_index] == 0) return {{{0,0}}}; // this is 0*q^0 = 0

    int max_len = ((v.size() * (v.size() - 1)) / 2);
    if(v_len == max_len) return {{{0, 1}}}; // if this is the case then v is reverse identity, which means P(u,v)=1

    // dummy variable to be used on database checking operations
    pair<bool, Polynomial> dummy;

    if(check_database){
        // as the name suggests, this is a dummy variable, it is not used in the definition of the polynomial
        dummy = k_l_database_check({u,v}, u_index, v_index);
        // if we have the answer already in the database, we may return here
        if(dummy.first) return dummy.second;
    }
    // finding the first 'i' where v(i) > v(i + 1)
    int i = permt_first_right_descent(v) - 1, c; /* -1 is for index*/

    // the variable 'c' in the definition is set up here
    if(u[i] > u[i+1]) c = 1;
    else              c = 0;

    /* The elements that are between u and v with respect to bruhat order will be important later on
     * we will handle it here, we say u <= z <= v , variable z_map will contain indexes of permutations
     * that stay between u and v with respect to bruhat order. */
    vector<int> z_map = bruhat_matrix_interval(u, v, u_index, v_index);

    pair<int, int> s_i = {i+1, i+2}; // +1 is added, because -1 was subtracted from i above
    Polynomial result, poly_temp, poly_temp2; vector<int> temp_vec, temp_vec2;
    temp_vec = permt_multp_right(u, s_i); temp_vec2 = permt_multp_right(v, s_i);

    PermtData temp_vec_data = all_p_data[temp_vec], temp_vec2_data = all_p_data[temp_vec2];
    int temp_vec_index = temp_vec_data.index;
    int temp_vec2_index = temp_vec2_data.index;

    // as the name suggest, this is a dummy variable, it is not used in the definition of the polynomial
    dummy = k_l_database_check({temp_vec, temp_vec2}, temp_vec_index, temp_vec2_index);

    if(dummy.first){ // if we already have the k-l polynomial in the database, we directly use it here
        poly_temp = polynom_multiply({{{1-c, 1}}}, dummy.second);
        result = polynom_add(result, poly_temp);
    }
    else{ // otherwise more calculation is needed
        /* Calling the function again with checkted_database = false */
        poly_temp = polynom_k_l(temp_vec, temp_vec2, temp_vec_data, temp_vec2_data, false);

        /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
         * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
         * For that reason, this part is commente out for now, might change later. */
        //temp_database_append({temp_vec_index, temp_vec2_index}, poly_temp);

        // adding q^(1-c) * P(u*s_i , v*s_i)
        poly_temp = polynom_multiply({{{1-c, 1}}}, poly_temp);
        result = polynom_add(result, poly_temp);
    }

    temp_vec = permt_multp_right(v, s_i);
    temp_vec_data = all_p_data[temp_vec];
    temp_vec_index = temp_vec_data.index;

    dummy = k_l_database_check({u, temp_vec}, u_index, temp_vec_index);

    // Here, we apply a very similar procedure to the one above
    if(dummy.first){
        poly_temp = polynom_multiply({{{c, 1}}}, dummy.second);
        result = polynom_add(result, poly_temp);
    }
    else{
        /* Calling the function again with checkted_database = false */
        poly_temp = polynom_k_l(u, temp_vec, u_data, temp_vec_data, false);

        /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
         * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
         * For that reason, this part is commente out for now, might change later. */
        //temp_database_append({u_index, temp_vec_index}, poly_temp);

        // adding q^c * P(u, v*s_i)
        poly_temp = polynom_multiply({{{c, 1}}}, poly_temp);
        result = polynom_add(result, poly_temp);
    }

    // This is where it gets really spicy, for any u < z < v with respect to bruhat order, if z(i) > z(i+1)
    // according to the definition, one needs to subtract μ(z, v*s_i) * q^[(l_v - l_z)/2] * P(u,z)
    // this operation should be done for any permutation z, satisfying the conditions above

    for(auto zitr = z_map.begin() ; zitr != z_map.end(); zitr++){
        vector<int> z = all_p[*zitr]; poly_temp.coefficients.clear();
        if(z[i] > z[i+1]){
            int z_len = all_p_len[*zitr];
            poly_temp = polynom_greek_mu(z, permt_multp_right(v, s_i));
            poly_temp = polynom_multiply(poly_temp, {{{(v_len - z_len)/2, 1}}});
            // This checks if poly_temp is zero polynomial, in that case further calculation
            // is unnecessary, at the end we would just subtract 0, so we may omit it
            if(polynom_add(poly_temp, poly_temp).coefficients == poly_temp.coefficients) continue;

            // 'dummy' variable is also used above, it does the same thing here
            dummy = k_l_database_check({u,z}, u_index, *zitr);

            if(dummy.first){
                poly_temp = polynom_multiply(poly_temp, dummy.second);
                result = polynom_subtract(result, poly_temp);
            }
            else{
                PermtData z_data = {all_p_len[*zitr], *zitr};

                /* Calling the function again with checkted_database = false */
                poly_temp2 = polynom_k_l(u, z, u_data, z_data, false);

               /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
                * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
                * For that reason, this part is commente out for now, might change later. */
                //temp_database_append({u_index, *zitr}, poly_temp2);

                poly_temp = polynom_multiply(poly_temp, poly_temp2);
                result = polynom_subtract(result, poly_temp);
            }
        }
    }
    // addding the result to the database for later use
    temp_database_append({u_index, v_index}, result);
    return result;
}

// This corresponds to the μ(u,v) function in the definition
Polynomial polynom_greek_mu(vector<int> u, vector<int> v, PermtData u_data, PermtData v_data){
    if(u_data.length == -1) u_data = all_p_data[u];
    if(v_data.length == -1) v_data = all_p_data[v];
    int u_index = u_data.index, v_index = v_data.index;

    if(b_matrix[u_index][v_index] == 0) return {{{0,0}}}; // this corresponds to just zero

    int len_u = u_data.length, len_v = v_data.length;
    // if the difference between their length is odd, we may directly return 0, check theory later
    if(len_v - len_u % 2 == 0) return {{{0,0}}};

    Polynomial k_l_poly;
    auto dummy = k_l_database_check({u, v}, u_index, v_index);
    // if the wanted polynomial is already in the database, no need to calculate it
    if(dummy.first) k_l_poly = dummy.second;
    // otherwise we calculate it
    else{
        k_l_poly = polynom_k_l(u, v, u_data, v_data);
        /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
         * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
         * For that reason, this part is commente out for now, might change later. */
        //temp_database_append({u_index, v_index}, k_l_poly);
    }
    // This corresponds to q^[(len_v - len_u - 1) / 2] * P(u, v)
    auto wanted_coefficient = k_l_poly.coefficients.find((len_v - len_u- 1) / 2.0);

    if(wanted_coefficient != k_l_poly.coefficients.end()) return {{{0, wanted_coefficient->second}}};
    else return {{{0,0}}};
}

// The same function as greek_mu, but this is designed to be used with 'polynom_k_l_standlaone'
// No prior length data is assumed
Polynomial polynom_greek_mu_standalone(vector<int> u, vector<int> v, PermtData u_data, PermtData v_data){
    if(u_data.length == -1) u_data = all_p_data[u];
    if(v_data.length == -1) v_data = all_p_data[v];
    int u_index = u_data.index, v_index = v_data.index;

    int len_u = permt_inversion_amount(u), len_v = permt_inversion_amount(v);

    if(!bruhat_compare(u, v, len_u, len_v)) return {{{0,0}}}; // this corresponds to just zero

    // if the difference between their length is odd, we may directly return 0, check theory later
    if(len_v - len_u % 2 == 0) return {{{0,0}}};

    Polynomial k_l_poly;
    auto dummy = k_l_database_check({u, v}, u_index, v_index);
    // if the wanted polynomial is already in the database, no need to calculate it
    if(dummy.first) k_l_poly = dummy.second;
    // otherwise we calculate it
    else{
        k_l_poly = polynom_k_l_standalone(u, v, u_data, v_data, false);
        /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
         * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
         * For that reason, this part is commente out for now, might change later. */
        //temp_database_append({u_index, v_index}, k_l_poly);
    }
    // This corresponds to q^[(len_v - len_u - 1) / 2] * P(u, v)
    auto wanted_coefficient = k_l_poly.coefficients.find((len_v - len_u- 1) / 2.0);

    if(wanted_coefficient != k_l_poly.coefficients.end()) return {{{0, wanted_coefficient->second}}};
    else return {{{0,0}}};
}

/* This is the standalone version of "polynom_k_l" above , it finds the KL-polynomial without using the global
 * variable b_matrix, it is aimed for individual calculations, rather than big applications */
Polynomial polynom_k_l_standalone(vector<int> u, vector<int> v, PermtData u_data, PermtData v_data, bool check_database){
    // By definition, if u = v then P(u, v) = 1
    if(u == v) return {{{0,1}}}; // this is 1*q^0 = 1

    if(u_data.length == -1 ) u_data = {permt_inversion_amount(u), all_p_data[u].index};
    if(v_data.length == -1 ) v_data = {permt_inversion_amount(v), all_p_data[v].index};

    int u_index = u_data.index, u_len = u_data.length;
    int v_index = v_data.index, v_len = v_data.length;

    // By definition, if u and v are not comparable with respect to bruhat order , than P(u, v) = 0
    if(!bruhat_compare(u, v, u_len, v_len)) return {{{0,0}}}; // this is 0*q^0 = 0

    int max_len = ((v.size() * (v.size() - 1)) / 2);
    if(v_len == max_len) return {{{0, 1}}}; // if this is the case then v is reverse identity, which means P(u,v)=1

    // dummy variable to be used on database checking operations
    pair<bool, Polynomial> dummy;

    if(check_database){
        // as the name suggests, this is a dummy variable, it is not used in the definition of the polynomial
        dummy = k_l_database_check({u,v}, u_index, v_index);
        // if we have the answer already in the database, we may return here
        if(dummy.first) return dummy.second;
    }
    // finding the first 'i' where v(i) > v(i + 1)
    int i = permt_first_right_descent(v) - 1, c; /* -1 is for index*/

    // the variable 'c' in the definition is set up here
    if(u[i] > u[i+1]) c = 1;
    else              c = 0;

    vector<int> z_map;
    int num_vertices = boost::num_vertices(bruhat_data.first);
    /* The elements that are between u and v with respect to bruhat order will be important later on
     * we will handle it here, we say u <= z <= v , variable z_map will contain indexes of permutations
     * that stay between u and v with respect to bruhat order. */
    if(num_vertices == 0){
        bruhat_data = bruhat_graph_between_permt(u, v, u_data, v_data, false);
        for(auto itr = bruhat_data.second.begin(); itr != bruhat_data.second.end(); itr++){
            z_map.push_back(all_p_data[itr->first].index);
        }
    }

    else{
        auto temp_data = bruhat_subgraph_between_permt(u, v, bruhat_data.first, bruhat_data.second[u].index,
                                                       bruhat_data.second[v].index);
        for(auto itr = temp_data.begin(); itr != temp_data.end(); itr++){
            z_map.push_back(all_p_data[itr->first].index);
        }
    }

    pair<int, int> s_i = {i+1, i+2}; // +1 is added, because -1 was subtracted from i above
    Polynomial result, poly_temp, poly_temp2; vector<int> temp_vec, temp_vec2;
    temp_vec = permt_multp_right(u, s_i); temp_vec2 = permt_multp_right(v, s_i);

    PermtData temp_vec_data = {permt_inversion_amount(temp_vec), all_p_data[temp_vec].index},
              temp_vec2_data = {permt_inversion_amount(temp_vec2), all_p_data[temp_vec2].index};
    int temp_vec_index = temp_vec_data.index;
    int temp_vec2_index = temp_vec2_data.index;

    // as the name suggest, this is a dummy variable, it is not used in the definition of the polynomial
    dummy = k_l_database_check({temp_vec, temp_vec2}, temp_vec_index, temp_vec2_index);

    if(dummy.first){ // if we already have the k-l polynomial in the database, we directly use it here
        poly_temp = polynom_multiply({{{1-c, 1}}}, dummy.second);
        result = polynom_add(result, poly_temp);
    }
    else{ // otherwise more calculation is needed
        /* Calling the function again with checkted_database = false */
        poly_temp = polynom_k_l_standalone(temp_vec, temp_vec2, temp_vec_data, temp_vec2_data, false);

        /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
         * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
         * For that reason, this part is commente out for now, might change later. */
        //temp_database_append({temp_vec_index, temp_vec2_index}, poly_temp);

        // adding q^(1-c) * P(u*s_i , v*s_i)
        poly_temp = polynom_multiply({{{1-c, 1}}}, poly_temp);
        result = polynom_add(result, poly_temp);
    }

    temp_vec = permt_multp_right(v, s_i);
    temp_vec_data = {permt_inversion_amount(temp_vec), all_p_data[temp_vec].index};
    temp_vec_index = temp_vec_data.index;

    dummy = k_l_database_check({u, temp_vec}, u_index, temp_vec_index);

    // Here, we apply a very similar procedure to the one above
    if(dummy.first){
        poly_temp = polynom_multiply({{{c, 1}}}, dummy.second);
        result = polynom_add(result, poly_temp);
    }
    else{
        /* Calling the function again with checkted_database = false */
        poly_temp = polynom_k_l_standalone(u, temp_vec, u_data, temp_vec_data, false);

        /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
         * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
         * For that reason, this part is commente out for now, might change later. */
        //temp_database_append({u_index, temp_vec_index}, poly_temp);

        // adding q^c * P(u, v*s_i)
        poly_temp = polynom_multiply({{{c, 1}}}, poly_temp);
        result = polynom_add(result, poly_temp);
    }

    // This is where it gets really spicy, for any u < z < v with respect to bruhat order, if z(i) > z(i+1)
    // according to the definition, one needs to subtract μ(z, v*s_i) * q^[(l_v - l_z)/2] * P(u,z)
    // this operation should be done for any permutation z, satisfying the conditions above

    for(auto zitr = z_map.begin() ; zitr != z_map.end(); zitr++){
        vector<int> z = all_p[*zitr]; poly_temp.coefficients.clear();
        if(z[i] > z[i+1]){
            int z_len = permt_inversion_amount(z);
            poly_temp = polynom_greek_mu_standalone(z, permt_multp_right(v, s_i));
            poly_temp = polynom_multiply(poly_temp, {{{(v_len - z_len)/2, 1}}});
            // This checks if poly_temp is zero polynomial, in that case further calculation
            // is unnecessary, at the end we would just subtract 0, so we may omit it
            if(polynom_add(poly_temp, poly_temp).coefficients == poly_temp.coefficients) continue;

            // 'dummy' variable is also used above, it does the same thing here
            dummy = k_l_database_check({u,z}, u_index, *zitr);

            if(dummy.first){
                poly_temp = polynom_multiply(poly_temp, dummy.second);
                result = polynom_subtract(result, poly_temp);
            }
            else{
                PermtData z_data = {z_len, *zitr};

                /* Calling the function again with checkted_database = false */
                poly_temp2 = polynom_k_l_standalone(u, z, u_data, z_data, false);

               /* Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
                * When we call 'polynom_k_l' above, it will already try to add it for us, on its own stack
                * For that reason, this part is commente out for now, might change later. */
                //temp_database_append({u_index, *zitr}, poly_temp2);

                poly_temp = polynom_multiply(poly_temp, poly_temp2);
                result = polynom_subtract(result, poly_temp);
            }
        }
    }
    // addding the result to the database for later use
    temp_database_append({u_index, v_index}, result);
    return result;
}

// Very similar to the one above, but for K-L cells, an alternative definition is used
//Polynomial polynom_greek_mu_bar(vector<int> u, vector<int> v){
//    if(!bruhat_compare(u, v) && !bruhat_compare(v, u)) return {{{0,0}}};
//    else if(!bruhat_compare(u,v)){
//        vector<int> temp_vec = v; v = u; u = temp_vec;
//    } // just change places of u and v
//
//    int len_u = permt_inversion_amount(u), len_v = permt_inversion_amount(v);
//    // if the difference between their length is odd, we may directly return 0, check theory later
//    if(len_v - len_u % 2 == 0) return {{{0,0}}};
//
//    Polynomial k_l_poly;
//    auto dummy = k_l_database_check({u, v});
//    // if the wanted polynomial is already in the database, no need to calculate it
//    if(dummy.first) k_l_poly = dummy.second->second;
//    // otherwise we calculate it
//    else{
//        k_l_poly = polynom_k_l(u, v);
//        //Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
//        temp_database[{u, v}] = k_l_poly;
//    }
//    // This corresponds to q^[(len_v - len_u - 1) / 2] * P(u, v)
//    auto wanted_coefficient = k_l_poly.coefficients.find((len_v - len_u- 1) / 2.0);
//
//    if(wanted_coefficient != k_l_poly.coefficients.end()) return {{{0, wanted_coefficient->second}}};
//    else return {{{0,0}}};
//}
/*
 This functions constructs the K-L graph for an entire symmetric group, here is the definition
 Let E = {(x,y) , x < y or y > x and μ_bar(x,y) =/= 0}
 Now, K-L graph is a bidirectional graph with vertices from S_n , and there are edges x --> y
 if the pair (x,y) satisfies the following condition              --NOTE: D_r(x) means right descent of x
 (i)  - x =/=y, (x,y) is in E, and there is a transposition 's' that is in D_r(x) but not in D_r(y)
 If there is an edge from x to y, it also carries the value of μ_bar(x,y) with it
 One could say that we 'colour' our edges, that is another way to put it
 Caution: For these operations a bruhat order graph that is calculated BEFOREHAND is needed
 The pair returned by bruhat_graph_all_sn function can be used as a SECOND argument here
*/
//k_l_graph k_l_graph_all_sn(int n, pair<bruhat_graph, map<vector<int>, PermtData>> bruhat_data){
//
//    /*
//       A general overview of what we are doing here
//       For any value x inside the group, the function will calculate the permutations that are smaller
//       than x, and also bigger than x. These value have the _chance_ to create a pair (x,y) that is an element
//       of the set E, defined above.
//
//       For possible pairs (x,y) necessary conditions will be checked, for those conditions see defn. above.
//       Keep in mind that loops can be created in this graph, as it is BIDIRECTIONAL.
//    */
//
//    auto permt_map = bruhat_data.second; auto bruhat_g = bruhat_data.first;
//
//    // identity and reverse identity permutations
//    // id = {1,2,3 ... , n} , rid = {n, n-1 ,n-2 , ...... 1}
//    vector<int> id, rid;
//    for(int i = 1; i <= n; i++){
//        id.push_back(i); rid.push_back(n-i+1);
//    }
//
//    k_l_graph result;
//    // initialisation of the result K-L graph
//    for(auto itr = permt_map.begin(); itr != permt_map.end(); itr++){
//        /*
//         * At this stage of the development, k_l_graph DOES contain additional info on graph vertices
//         * might be removed later, if there is no use for it
//         * note, to myself
//        */
//        PermtVertex temp = {itr->first, itr->second.length, itr->second.index};
//        boost::add_vertex(temp, result);
//    }
//
//    std::map<std::vector<int>, PermtData> permt_lower, permt_upper;
//    for(auto itr = permt_map.begin(); itr != permt_map.end(); itr++){
//        /* Set of elements that are lower and bigger than x, respectively */
//        permt_lower = bruhat_subgraph_between_permt(id, itr->first, bruhat_g, permt_map[id].index, itr->second.index);
//        permt_upper = bruhat_subgraph_between_permt(itr->first, rid, bruhat_g, itr->second.index, permt_map[rid].index);
//
//        for(auto itr2 = permt_lower.begin(); itr2 != permt_lower.end(); itr2++){
//            Polynomial mu_bar_x_y = polynom_greek_mu_bar(itr->first, itr2->first);
//            // We check if μ_bar(x,y) is zero here
//            if(polynom_add(mu_bar_x_y, mu_bar_x_y).coefficients == mu_bar_x_y.coefficients) continue;
//
//            // Here, we check the difference between right descents of x and y, D_r(x) \ D_r(y)
//            vector<int> diff_vec, x_sorted = permt_right_descent(itr->first),
//                                  y_sorted = permt_right_descent(itr2->first);
//
//            sort(x_sorted.begin(), x_sorted.end());
//            sort(y_sorted.begin(), y_sorted.end());
//            set_difference(x_sorted.begin(), x_sorted.end(), y_sorted.begin(), y_sorted.end(),
//                           inserter(diff_vec, diff_vec.begin()));
//
//            // If there is no element that is in D_r(x) \ D_r(y) then we don't do anything
//            if(diff_vec.empty()) continue;
//
//            // If the signal has reached here, it means the pair (x,y) satisfies all conditions
//            k_l_edge temp_edge = {mu_bar_x_y};
//            // Construction of the edge x ---> y , with the value mu_bar_x_y
//            boost::add_edge(boost::vertex(itr->second.index, result), boost::vertex(itr2->second.index, result),
//                            temp_edge, result);
//        }
//
//        for(auto itr2 = permt_upper.begin(); itr2 != permt_upper.end(); itr2++){
//            Polynomial mu_bar_x_y = polynom_greek_mu_bar(itr->first, itr2->first);
//            // We check if μ_bar(x,y) is zero here
//            if(polynom_add(mu_bar_x_y, mu_bar_x_y).coefficients == mu_bar_x_y.coefficients) continue;
//
//            // Here, we check the difference between right descents of x and y, D_r(x) \ D_r(y)
//            vector<int> diff_vec, x_sorted = permt_right_descent(itr->first),
//                                  y_sorted = permt_right_descent(itr2->first);
//
//            sort(x_sorted.begin(), x_sorted.end());
//            sort(y_sorted.begin(), y_sorted.end());
//            set_difference(x_sorted.begin(), x_sorted.end(), y_sorted.begin(), y_sorted.end(),
//                           inserter(diff_vec, diff_vec.begin()));
//
//            // If there is no element that is in D_r(x) \ D_r(y) then we don't do anything
//            if(diff_vec.empty()) continue;
//
//            // If the signal has reached here, it means the pair (x,y) satisfies all conditions
//            k_l_edge temp_edge = {mu_bar_x_y};
//            // Construction of the edge x ---> y , with the value mu_bar_x_y
//            boost::add_edge(boost::vertex(itr->second.index, result), boost::vertex(itr2->second.index, result),
//                            temp_edge, result);
//        }
//    }
//    return result;
//}

// int main(void){
//     // Polynomial poly_test, poly_test2; poly_test.coefficients = {{0,1}, {1,2}, {2,1}};
//     // poly_test2.coefficients = {{0,1}, {1,-2}, {2,1}};
//     // polynom_display(polynom_add({}, poly_test2), stdout);
//     // polynom_display(polynom_subtract(poly_test, poly_test2), stdout);
//     // polynom_display(polynom_multiply(poly_test, poly_test2), stdout);
//     // polynom_display(polynom_multiply(poly_test2, poly_test), stdout);
//     // polynom_display(polynom_multiply(poly_test, {}), stdout);
//     // polynom_display(polynom_multiply(poly_test, {{{0,0}}}), stdout);
//     // polynom_display(polynom_multiply(poly_test, {{{5,-33}}}), stdout);
//     // polynom_display(polynom_multiply({{{5,-33}}}, poly_test), stdout);
//     //vector<int> permt1 = permt_prompt(), permt2 = permt_prompt();
//     //printf("\n K-L polynomial: ");
//     //polynom_display(stdout, polynom_k_l({11,9,2,7,6,4,5,10,8,3,1,12},{12,10,7,2,11,8,6,5,4,3,1,9}));
//     // polynom_display(polynom_k_l({2,1,4,7,5,6,3}, {6,1,5,7,2,4,3}), stdout);
//     // k_l_database_initiate();
//     // for(auto itr = k_l_database.begin(); itr != k_l_database.end(); itr++){
//     //     print1d(stdout, itr->first.first); printf(":"); print1d(stdout, itr->first.second);
//     //     printf("={"); polynom_display(stdout, itr->second); printf("}\n");
//     // }
//     //temp_database = {{{{3,7,13,465},{9,8,7}}, {{{13, -84}, {1, 1}, {0, -3}}}}, {{{3,7,12,467},{9,8,6}}, {{{13, -86}, {1, 1}, {0, -3}}}}};
//     //k_l_database_append();
//     k_l_database_initiate();
//     auto lulw = polynom_k_l({1,2,3,4,5}, {4,5,2,1,3});
//     k_l_database_append();
//     k_l_database_initiate();
//     for(auto itr = k_l_database.begin(); itr != k_l_database.end(); itr++){
//         print1d(stdout, itr->first.first); printf(":"); print1d(stdout, itr->first.second);
//         printf("={"); polynom_display(stdout, itr->second); printf("}\n");
//     }
// }
