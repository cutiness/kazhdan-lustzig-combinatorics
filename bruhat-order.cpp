/*
The GPLv3 License (GPLv3)

Copyright (c) 2023 cutiness

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * The code written in this file utilizes Boost Graph Library, see the info about
 * them below.
*/

//=======================================================================
// Copyright 2001 University of Notre Dame.
// Copyright 2003 Jeremy Siek
// Authors: Lie-Quan Lee, Jeremy Siek, and Douglas Gregor
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include "bruhat-order.h"

using namespace std;

/*
 This functions returns a graph of bruhat order map for the entire symmetric group S_n
 Vertexes are elements, and edges represent the bruhat order between elements
 Keep in mind that additional data is also returned as a pair, the graph is the first element
 The second one includes all the length, index data to do searches or alterations on the graph
*/
pair<bruhat_graph, map<vector<int>, PermtData>> bruhat_graph_all_sn(int n){

    auto permts_data = all_p_data;

    bruhat_graph bruhat_g;

    //this part adds vertices to the graph
    for(auto itr = permts_data.begin(); itr != permts_data.end(); itr++){
        PermtVertex temp = {itr->first, itr->second.length, itr->second.index};
        boost::add_vertex(temp, bruhat_g);
    }

    // Here, we access the vertices with an iterator, and start to calculate bruhat relations
    boost::graph_traits<bruhat_graph>::vertex_iterator vitr, vitr_end;
    for(boost::tie(vitr, vitr_end) = vertices(bruhat_g); vitr != vitr_end; vitr++){
        //this part calculates which pairs can be used in order to increase the length by 1
        auto transp_necessary = transp_1length_diff(bruhat_g[*vitr].permt);

        //if there are possible pairs, we may continue
        if(!transp_necessary.empty()){
            for(auto tempitr = transp_necessary.begin(); tempitr != transp_necessary.end(); tempitr++){
                vector<int> tempvec = bruhat_g[*vitr].permt;
                //Applying the transposition, from the right side
                tempvec = permt_multp_right(tempvec, *tempitr);

                //Searching for the index value of the obtained permutation in our database
                //this happens in logarithmic time
                auto target_index = permts_data.find(tempvec)->second.index;
                auto target_vertex = boost::vertex(target_index, bruhat_g);

                //now we may create the required edge between two vertices
                boost::add_edge(*vitr, target_vertex, bruhat_g);
            }
        }
    }
    return {bruhat_g, permts_data};
}

/*
 Compare two permutations with respect to bruhat order, this function is designed to be used
 for a large number of permutations, over and over. For that reason, it is assumed that the programmer
 has the entire bruhat_graph for the necessary group already, the data is read from there, rather than
 being calculated on the fly. The order of permt1 and permt2 is IMPORTANT.
*/
bool bruhat_compare_with_graph(vector<int> permt1, vector<int> permt2, pair<bruhat_graph, map<vector<int>, PermtData>> p){
    // 'p' stand for the pair that is provided as an input, this is basically the outcome
    // of the function bruhat_graph_all_sn() defined above
    int permt1_len = p.second[permt1].length, permt2_len = p.second[permt2].length;
    int max_len = ((permt2.size() * (permt2.size() - 1)) / 2);
    // identity element will be smaller than anything, provided that permt2 is not also identity
    if(p.second[permt1].length == 0 && p.second[permt2].length == 0) return false;
    else if(p.second[permt1].length == 0) return true;

    // reverse identity will be bigger than any element, with respect to bruhat order
    // the maximum length a permutation can have in S_n is , n * (n - 1) / 2 , which is only possible
    // when the permutation is reverse identity
    else if(p.second[permt2].length == max_len && p.second[permt1].length != max_len) return true;
    else if(p.second[permt2].length == max_len) return false; // this is the case that both of them is reverse identity
    else if(permt1_len >= permt2_len) return false;
    else return bruhat_compare_helper(p.first, permt1, permt2, p.second[permt1].index, p.second[permt2].index);
}

/*
 This function is written to work with bruhat_compare_with_graph function
 it does not have much meaning on its own
 index values represent the places of the permutations inside the bruhat_graph
*/
bool bruhat_compare_helper(bruhat_graph g, vector<int> permt1, vector<int> permt2, int permt1_index, int permt2_index){
    bool result = false, desired_level = false;
    auto permt1_vertex = boost::vertex(permt1_index, g), permt2_vertex = boost::vertex(permt2_index, g);
    boost::graph_traits<bruhat_graph>::adjacency_iterator aditr, aditr_end;
    boost::tie(aditr, aditr_end) = boost::adjacent_vertices(permt1_vertex, g);

    // if we have reached the length we are looking for, we check all the
    // connected elements to see if we have a match
    if(g[*aditr].length == g[permt2_vertex].length){
        desired_level = true;
        for(; aditr != aditr_end; aditr++){
            if(g[*aditr].permt == permt2) result = true;
        }
    }
    /*
     after the last if block, if result is still false, there are 2 cases
     either we are on the desired length level and no elements has matched to permt2
     or we are on a level below the desired length
     only in the latter case we continue to investigate further, using adjacent vertices
    */
    if(!desired_level){
        while(result == false && aditr != aditr_end){
            result = bruhat_compare_helper(g, g[*aditr].permt, permt2, g[*aditr].index, permt2_index);
            aditr++;
        }
    }
    return result;
}
/*
 This function, unlike the version above, does not use an entire graph to check whether or not
 two given permutations is comparable in terms of bruhat order ,the idea is the following:
 Define w[i, j] = {a in (1,2,... i) such that w(a) > j} and we say w < l in terms of bruhat order
 iff w[i, j] <= l[i, j] for any i,j in (1,2,... n) where w,l is elements of S_n
 The definition is taken directly from "Combinatorics of Coxeter Groups" textbook

 It is assumed that the user will provide two permutations from the same S_n group
*/
bool bruhat_compare(vector<int> permt1, vector<int> permt2, int p_len1, int p_len2){
    int max_len = ((permt2.size() * (permt2.size() - 1)) / 2);
    if(p_len1 == -1) p_len1 = permt_inversion_amount(permt1);
    if(p_len2 == -1) p_len2 = permt_inversion_amount(permt2);
    if(permt1 == permt2) return false; // an element is not bruhat comparable to itself
    else if(permt2[0] < permt1[0]) return false; // in this case choose i = 1 , it will fail
    else if(p_len2 == max_len || p_len1 == 0) return true; // if either permt1 is identity or permt2 is reverse identity
    else if(p_len1 == max_len || p_len2 == 0) return false; // if either permt1 is reverse identity or permt2 is identity
    else{
        multiset<int> checked_permt1, checked_permt2;
        for(int i = 0; i < permt2.size(); i++){
            checked_permt1.insert(permt1[i]); checked_permt2.insert(permt2[i]);
            if(permt2[i] >= permt1[i]) continue; // no termination could happen in this case
            else{
                auto p1_itr = checked_permt1.begin(), p2_itr = checked_permt2.begin();
                for( ; p1_itr != checked_permt1.end(); p1_itr++){
                    if(*p1_itr > *p2_itr) return false; // in this case choose j = *p1_itr and 'i' accordingly, it will fail
                    p2_itr++;
                }
            }
        }
    }
    // if the above loop did not return anything, then we return true
    return true;
}

/*
 Return the bruhat graph where 'w' is an element of the returned graph iff
 permt1 <= w <= permt2 with respect to bruhat ordering
 You may consider this as a subgraph of the entire bruhat graph for a group
 It is assumed that the permt1 < permt2 with respect to bruhat order, otherwise this is pointless

 !! IMPORTANT REMARKS !!
 The function returns a graph containing all elements in the group that is considered to be
 bigger than permt1 and smaller than permt2 with respect to bruhat order.
 Together with this data a map structure containing necessary index data to reach elements easier
 on the graph is also returned. These two types of data that is mentioned here is given to the
 user as a pair.

  !! The function accepts a boolean at the end to decide whether or not to use global variable b_matrix
  If said yes, it should be initialized for the ENTIRE group first, which may not be ideal for every application
  By default, the function will act as a STANDALONE function, and it will only use "bruhat_compare" for its own needs
  This has the side effect of being slower, as it will try to calculate pre-calculated things over and over again
  Use 'use_b_matrix = true' as a parameter for large applications.

 The bruhat_graph_all_sn function defined above also uses the same data structures
*/
pair<bruhat_graph, map<vector<int>, PermtData>>bruhat_graph_between_permt(vector<int> permt1, vector<int> permt2, PermtData permt1_data, PermtData permt2_data, bool use_b_matrix){

    if(permt1_data.length == -1 && all_p_data[permt1].length != -1) permt1_data = all_p_data[permt1];
    else if(permt1_data.length == -1) permt1_data = {permt_inversion_amount(permt1), all_p_data[permt1].index};
    if(permt2_data.length == -1 && all_p_data[permt2].length != -1) permt2_data = all_p_data[permt2];
    else if(permt1_data.length == -1) permt1_data = {permt_inversion_amount(permt2), all_p_data[permt2].index};
    int current_length = permt1_data.length, desired_length = permt2_data.length;


    bruhat_graph g;
    // Trivial cases are handled here
    if(current_length == desired_length - 1){
        boost::add_vertex({permt1, current_length, 0}, g);
        boost::add_vertex({permt2, desired_length, 1}, g);
        map<vector<int>, PermtData> temp_map;
        temp_map[permt1] = {current_length, 0}; temp_map[permt2] = {desired_length, 1};
        return {g, temp_map};
    }
    else if(current_length >= desired_length){
        // we simply return empty in this case
        return {g, {}};
    }
    else{
        // In the non-trivial case we call the helper function
        PermtVertex temp_vertex = {permt1, current_length, 0};
        auto permt1_vertex = boost::add_vertex(temp_vertex, g);
        map<vector<int>, PermtData> road_map;
        road_map[permt1] = {current_length, 0};
        if(use_b_matrix) return between_permt_helper(permt1, permt2, permt1_data, permt2_data, g, road_map, true);
        else             return between_permt_helper(permt1, permt2, permt1_data, permt2_data, g, road_map, false);
    }
}

// This function is written to be used with bruhat_graph_between_permt function
// It does not have much meaning on its own
pair<bruhat_graph, map<vector<int>, PermtData>> between_permt_helper(vector<int> permt1, vector<int> permt2, PermtData permt1_data, PermtData permt2_data, bruhat_graph g, map<vector<int>, PermtData> road_map, bool use_b_matrix){

    if(permt1_data.length == -1 && all_p_data[permt1].length != -1) permt1_data = all_p_data[permt1];
    else if(permt1_data.length == -1) permt1_data = {permt_inversion_amount(permt1), all_p_data[permt1].index};
    if(permt1_data.length == -1 && all_p_data[permt1].length != -1) permt1_data = all_p_data[permt1];
    else if(permt1_data.length == -1) permt1_data = {permt_inversion_amount(permt1), all_p_data[permt1].index};
    int current_length = permt1_data.length, desired_length = permt2_data.length;

    int total_vertices = boost::num_vertices(g);
    auto permt1_vertex = boost::vertex(permt1_data.index, g);

    if(current_length < desired_length -1){
        auto transp_necessary = transp_1length_diff(permt1);
        // this will only store adjacent vertices that is -comparable- to permt2
        vector<vector<int>> adjacent_vertices;
        for(auto titr = transp_necessary.begin(); titr != transp_necessary.end(); titr++){
            auto temp_vec = permt_multp_right(permt1, *titr);
            PermtData temp_vec_data;
            if(all_p_data[temp_vec].length != -1) temp_vec_data = all_p_data[temp_vec];
            else temp_vec_data = {permt_inversion_amount(temp_vec), all_p_data[temp_vec].index};
            //if the obtained temp_vec is comparable to the target permutation
            //we investigate further, otherwise we simply do not add it
            if(use_b_matrix && b_matrix[temp_vec_data.index][permt2_data.index] == 0) continue;
            else{
                if(!bruhat_compare(temp_vec, permt2, temp_vec_data.length, permt2_data.length)) continue;
            }

            // There is a chance temp_vec was already added before, so we check whether or not
            // it is in road_map
            auto temp_vec_ptr = road_map.find(temp_vec);
            if(temp_vec_ptr == road_map.end()){
                total_vertices = boost::num_vertices(g);
                PermtVertex temp_vertex = {temp_vec, current_length+1, total_vertices};
                auto temp_vertex_g = boost::add_vertex(temp_vertex, g);

                // We may construct the edge right away because we know temp_vec is comparable to permt2
                boost::add_edge(permt1_vertex, temp_vertex_g, g);

                // Update necessary data
                road_map[temp_vec] = {current_length+1, total_vertices};
                adjacent_vertices.push_back(temp_vec);
            }
            else{ // if temp_vec was already added as a vertex, we just create the edge
                auto temp_vertex_g = boost::vertex(temp_vec_ptr->second.index, g);
                boost::add_edge(permt1_vertex, temp_vertex_g, g);
            }
        }

        // Now, we will run the function again on the obtained adjacent vertices
        for(auto titr = adjacent_vertices.begin(); titr != adjacent_vertices.end(); titr++){
            PermtData titr_data;
            if(all_p_data[*titr].length != -1) titr_data = all_p_data[*titr];
            else titr_data = {permt_inversion_amount(*titr), all_p_data[*titr].index};
            auto temp_data = between_permt_helper(*titr, permt2, titr_data, permt2_data, g, road_map);
            g = temp_data.first; road_map = temp_data.second;
        }
        // when changes are made to the graph, we return it back
        // together with the needed data as a pair
        return {g, road_map};
    }
    else{
        if(use_b_matrix && b_matrix[permt1_data.index][permt2_data.index] == 0) return{g, road_map};
        else{
            if(!bruhat_compare(permt1, permt2, permt1_data.length, permt2_data.length)) return{g, road_map};
        }
        /*
         When the function call reaches here, vertices below the desired_length is already
         constructed, and we know permt1 is comparable to permt2, because we check this above
         before reaching here. Therefore, we only add the final vertex permt2 and necessary edges.

         If the last vertex is not permt2 we may add it
        */
        auto permt2_ptr = road_map.find(permt2);
        boost::graph_traits<bruhat_graph>::vertex_descriptor permt2_vertex;
        if(permt2_ptr == road_map.end()){
            PermtVertex temp_vertex = {permt2, desired_length,total_vertices};
            permt2_vertex = boost::add_vertex(temp_vertex, g);
            road_map[permt2] = {desired_length, total_vertices};
        }
        else{
            permt2_vertex = boost::vertex(permt2_ptr->second.index, g);
        }
        // Adding the necessary edge
        boost::add_edge(permt1_vertex, permt2_vertex, g);
        // when changes are made to the graph, we return it back
        // together with the needed data as a pair
        return {g, road_map};
    }
}

/*
 If one already has the graph between two permutations u and v , then it is pointless to
 construct everything again for permutations that are between u , and v
 IMPORTANT : It is assumed that the user will provide a permutation 'z' as input such that
 u <= z <= v with respect to bruhat order, where u and v are the max and min permutations of the given bruhat_graph
 OTHERWISE, this function will return WRONG values, use with caution.
 Keep in mind that this function only returns a list of permutations in between together with necessary data,
 it does not construct a graph, it is assumed that the programmer already constructed a graph
*/
map<vector<int>, PermtData> bruhat_subgraph_between_permt(vector<int> permt1, vector<int> permt2, bruhat_graph g, int permt1_index, int permt2_index){
    map<vector<int>, PermtData> result = {};
    auto permt1_vertex = boost::vertex(permt1_index, g);
    auto permt2_vertex = boost::vertex(permt2_index, g);
    int current_length = g[permt1_vertex].length, desired_length = g[permt2_vertex].length;
    // adding permt1 itself to the result map, and also permt2
    result[permt1] = {current_length, permt1_index};
    result[permt2] = {desired_length, permt2_index};

    //trivial case
    if(permt1 == permt2) return result;

    // on a non-trivial case
    result = bruhat_subgraph_helper(permt1, permt2, g, permt1_index, permt2_index, result);
    return result;
}

// This functioned is programmed to work together with bruhat_subgraph_between_permt function defined above
// It does not have much meaning on its own
map<vector<int>, PermtData> bruhat_subgraph_helper(vector<int> permt1, vector<int> permt2, bruhat_graph g, int permt1_index, int permt2_index, map<vector<int>, PermtData> result_map){
    map<vector<int>, PermtData> result = result_map;
    auto permt1_vertex = boost::vertex(permt1_index, g);
    auto permt2_vertex = boost::vertex(permt2_index, g);
    int current_length = g[permt1_vertex].length, desired_length = g[permt2_vertex].length;

    // in this case we look at the adjacent vertices
    if(current_length < desired_length - 1){
        vector<pair<vector<int>, int>> adjacent_vertices = {};
        boost::graph_traits<bruhat_graph>::adjacency_iterator aditr, aditr_end;
        boost::tie(aditr, aditr_end) = boost::adjacent_vertices(permt1_vertex, g);
        for( ; aditr != aditr_end; aditr++){
            auto z = g[*aditr];
            // if z is comparable to permt2, it should be in the result map
            if(bruhat_compare(z.permt, permt2)){
                adjacent_vertices.push_back({z.permt, z.index});
                result[z.permt] = {z.length, z.index};
            }
        }
        // Now we are ready to check other permutations that are connected to the ones we found
        for(auto itr = adjacent_vertices.begin(); itr != adjacent_vertices.end(); itr++){
            result = bruhat_subgraph_helper(itr->first, permt2, g, itr->second, permt2_index, result);
        }
        return result;
    }
    /*
     If the current_length = desired_length - 1, then we don't have anything to do
     We know that permt1 will be comparable to the desired permutation, that is permt2
     it is already checked on the previous stages of this function
    */
    else return result;
}
