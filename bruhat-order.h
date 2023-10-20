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

//=======================================================================
// Copyright 2001 University of Notre Dame.
// Copyright 2003 Jeremy Siek
// Authors: Lie-Quan Lee, Jeremy Siek, and Douglas Gregor
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#ifndef BRUHAT_ORDER
#define BRUHAT_ORDER
/*--------------------------------*/
#ifndef PERMUTATION_BASICS
#include "permutation-basics.h"
#endif //!PERMUTATION_BASICS
/*--------------------------------*/
#ifndef BRUHAT_MATRIX
#include "bruhat-matrix.h"
#endif //!BRUHAT_MATRIX
/*--------------------------------*/

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>
#include <set>

#endif // !BRUHAT_ORDER
#define BRUHAT_ORDER

// type definitions
struct PermtVertex
{
    std::vector<int> permt; // store the permutation itself inside vertex
    int length; // the length of the above permutation
    int index;  // indicates the position of the permt, inside the graph
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, PermtVertex> bruhat_graph;

// function definitions

std::pair<bruhat_graph, std::map<std::vector<int>, PermtData>> bruhat_graph_all_sn(int n);

bool bruhat_compare_with_graph(std::vector<int> permt1, std::vector<int> permt2, std::pair<bruhat_graph, std::map<std::vector<int>, PermtData>> p);

bool bruhat_compare_helper(bruhat_graph g, std::vector<int> permt1, std::vector<int> permt2, int permt1_index, int permt2_index);

bool bruhat_compare(std::vector<int> permt1, std::vector<int> permt2, int p_len1 = -1, int p_len2 = -1);

std::pair<bruhat_graph, std::map<std::vector<int>, PermtData>>bruhat_graph_between_permt(std::vector<int> permt1, std::vector<int> permt2, PermtData permt1_data = {-1,-1}, PermtData permt2_data = {-1,-1}, bool use_b_matrix = false);

std::pair<bruhat_graph, std::map<std::vector<int>, PermtData>> between_permt_helper(std::vector<int> permt1, std::vector<int> permt2, PermtData permt1_data = {-1,-1}, PermtData permt2_data = {-1,-1}, bruhat_graph g = {}, std::map<std::vector<int>, PermtData> road_map = {}, bool use_b_matrix = false);

std::map<std::vector<int>, PermtData> bruhat_subgraph_between_permt(std::vector<int> permt1, std::vector<int> permt2, bruhat_graph g, int permt1_index, int permt2_index);

std::map<std::vector<int>, PermtData> bruhat_subgraph_helper(std::vector<int> permt1, std::vector<int> permt2, bruhat_graph g, int permt1_index, int permt2_index, std::map<std::vector<int>, PermtData> result_map);
