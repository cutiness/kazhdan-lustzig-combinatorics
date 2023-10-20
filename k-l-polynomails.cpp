#include "k-l-polynomials.h"

using namespace std;

/*
 This stands for the Kazhdan-Lustzig polynomial
 Before using this please initiate the database with k_l_database_initiate function defined above
 Calculating the k-l polynomial is a recursive process, and if the k-l polynomial for to permutations is
 already known, then the function will *not* calculate it again. Instead it will use the value in the database.
 For more information check the functions above and the 'k_l_database' global variable.
*/
Polynomial polynom_k_l(vector<int> u, vector<int> v){
    // By definition, if u = v then P(u, v) = 1
    if(u == v) return {{{0,1}}}; // this is 1*q^0 = 1
    
    // By definition, if u and v are not comparable with respect to bruhat order , than P(u, v) = 0
    if(!bruhat_compare(u,v)) return {{{0,0}}}; // this is 0*q^0 = 0
     
    int max_len = ((v.size() * (v.size() - 1)) / 2); int v_len = permt_inversion_amount(v);
    if(v_len == max_len) return {{{0, 1}}}; // if this is the case then v is reverse identity, which means P(u,v)=1
    // finding the first 'i' where v(i) > v(i + 1)
    
    // as the name suggest, this is a dummy variable, it is not used in the definition of the polynomial
    auto dummy = k_l_database_check({u,v});
    // if we have the answer already in the database, we may return here
    if(dummy.first) return dummy.second->second;

    int i = permt_first_right_descent(v) - 1, c;

    // the variable 'c' in the definition is set up here
    if(u[i] > u[i+1]) c = 1;
    else              c = 0;

    // The elements that are between u and v with respect to bruhat order will be important later on
    // we will handle it here, we say u <= z <= v
    map<vector<int>, PermtData> z_map;
    if(bruhat_data.second.empty()){
        bruhat_data = bruhat_graph_between_permt(u, v);
        z_map = bruhat_data.second;
    }
    // if a graph is already provided we shall make use of it, instead of calculating everything again
    else{
        z_map = bruhat_subgraph_between_permt(u, v, bruhat_data.first, bruhat_data.second[u].index, bruhat_data.second[v].index);
    }
    
    pair<int, int> s_i = {i+1, i+2}; // +1 is added, because -1 was subtracted from i above
    Polynomial result, poly_temp, poly_temp2; vector<int> temp_vec, temp_vec2;
    temp_vec = permt_multp_right(u, s_i); temp_vec2 = permt_multp_right(v, s_i);
    // as the name suggest, this is a dummy variable, it is not used in the definition of the polynomial
    dummy = k_l_database_check({temp_vec, temp_vec2});
    
    if(dummy.first){ // if we already have the k-l polynomial in the database, we directly use it here
        poly_temp = polynom_multiply({{{1-c, 1}}}, dummy.second->second); 
        result = polynom_add(result, poly_temp);
    }
    else{ // otherwise more calculation is needed
        poly_temp = polynom_k_l(temp_vec, temp_vec2);
        
        // Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
        temp_database[{temp_vec, temp_vec2}] = poly_temp;
        
        // adding q^(1-c) * P(u*s_i , v*s_i)
        poly_temp = polynom_multiply({{{1-c, 1}}}, poly_temp);
        result = polynom_add(result, poly_temp);
    }
    
    temp_vec = permt_multp_right(v, s_i);
    dummy = k_l_database_check({u, temp_vec});

    // Here, we apply a very similar procedure to the one above
    if(dummy.first){
        poly_temp = polynom_multiply({{{c, 1}}}, dummy.second->second); 
        result = polynom_add(result, poly_temp);
    }
    else{
        poly_temp = polynom_k_l(u, temp_vec);
        
        // Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
        temp_database[{u, temp_vec}] = poly_temp;
    
        // adding q^c * P(u, v*s_i)    
        poly_temp = polynom_multiply({{{c, 1}}}, poly_temp);
        result = polynom_add(result, poly_temp);
    }

    // This is where it gets really spicy, for any u < z < v with respect to bruhat order, if z(i) > z(i+1)
    // according to the definition, one needs to subtract μ(z, v*s_i) * q^[(l_v - l_z)/2] * P(u,z)
    // this operation should be done for any permutation z, satisfying the conditions above

    for(auto zitr = z_map.begin() ; zitr != z_map.end(); zitr++){
        vector<int> z = zitr->first; poly_temp.coefficients.clear();
        if(z[i] > z[i+1]){
            int z_len = zitr->second.length;
            poly_temp = polynom_greek_mu(z, permt_multp_right(v, s_i));
            poly_temp = polynom_multiply(poly_temp, {{{(v_len - z_len)/2, 1}}});
            // This checks if poly_temp is zero polynomial, in that case further calculation
            // is unnecessary, at the end we would just subtract 0, so we may omit it
            if(polynom_add(poly_temp, poly_temp).coefficients == poly_temp.coefficients) continue;
            
            // 'dummy' variable is also used above, it does the same thing here
            dummy = k_l_database_check({u,z});

            if(dummy.first){
                poly_temp = polynom_multiply(poly_temp, dummy.second->second);
                result = polynom_subtract(result, poly_temp);
            }
            else{
                poly_temp2 = polynom_k_l(u, z);
            
            //  Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
                temp_database[{u, z}] = poly_temp2;
                
                poly_temp = polynom_multiply(poly_temp, poly_temp2);
                result = polynom_subtract(result, poly_temp);
            } 
        }
    }
    // addding the result to the database for later use
    temp_database[{u, v}] = result;
    return result;
}

// This corresponds to the μ(u,v) function in the definition
Polynomial polynom_greek_mu(vector<int> u, vector<int> v){
    if(!bruhat_compare(u, v)) return {{{0,0}}}; // this corresponds to just zero
    
    int len_u = permt_inversion_amount(u), len_v = permt_inversion_amount(v);
    // if the difference between their length is odd, we may directly return 0, check theory later
    if(len_v - len_u % 2 == 0) return {{{0,0}}};
    
    Polynomial k_l_poly;
    auto dummy = k_l_database_check({u, v});
    // if the wanted polynomial is already in the database, no need to calculate it
    if(dummy.first) k_l_poly = dummy.second->second;
    // otherwise we calculate it
    else{
        k_l_poly = polynom_k_l(u, v);
        //Obtained polynomial will not be inside the database, so we shall add it to temp_database for later use
        temp_database[{u, v}] = k_l_poly;
    }
    // This corresponds to q^[(len_v - len_u - 1) / 2] * P(u, v)
    auto wanted_coefficient = k_l_poly.coefficients.find((len_v - len_u- 1) / 2.0);

    if(wanted_coefficient != k_l_poly.coefficients.end()) return {{{0, wanted_coefficient->second}}};
    else return {{{0,0}}}; 
}

