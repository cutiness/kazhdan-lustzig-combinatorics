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

#ifndef PERMUTATION_BASICS
#define PERMUTATION_BASICS

#include <iostream>   // simple io
#include <vector>    // our primary data structure
#include <algorithm> // self explanatory
#include <map>     // dictionary like objects for cpp
#include <utility> // for pairs
#include <string>
#endif // !PERMUTATION_BASICS

// type definitions
struct PermtData
{
    int length;
    int index; //specifies the position, to use with other data structures, such as graphs
};

/*  This global variable is used to determine which S_n group is used.
 *  A simple, but fundemental piece of data that many function depend on for proper
 *  file naming (for databases) or other calculations. Please initialize it beforehand. */
extern int current_sn_group;

/*  A list of global variables that define the group structure, permutations inside the group
 *  and also their corresponding length (inversion amount) data to any function that needs it. */

/* This is just a list of permutations with nothing else, but they can be accessed directly
 * if the index value of a permutation is known. */
extern std::vector<std::vector<int>> all_p;

/* This is just a list of the lengths (inversion amounts) of permutations, layed out in the same order
 * with all_p . If the index of value permutation is known, that index will coresspond to its length here.*/
extern std::vector<int> all_p_len;

/* Stands for all_permutations , initialize it with 'permt_with_extra_data' 
 * This will contain necessary data for many other functions doing operations on
 * permutations, please initialize this beforehand. 
 * This is a combination of all_p and all_p_len, convenient to use when the index of the
 * permutation at hand is not known. all_p_data stores (key,value) pairs. */
extern std::map<std::vector<int>, PermtData> all_p_data;

// function declaration

int take_power10(int n);

int factorial(int n);

void print1d(FILE* stream, std::vector<int> vec1d);

void print2d(std::vector<std::vector<int>> vec2d, int n);

std::vector<std::vector<int>> permt_all_sn(int n);

std::vector<std::pair<int, int>> transp_all_sn(int n);

int permt_inversion_amount(std::vector<int> permt);

std::pair<std::vector<int>, int> merge_permt(std::vector<int> permt, int temp_permt[], int left, int mid, int right);

std::pair<std::vector<int>, int> divide_permt(std::vector<int> permt, int temp_permt[], int left, int right);

std::vector<int> permt_inverse(std::vector<int> permt);

std::vector<int> permt_lengths(std::vector<std::vector<int>> permt_list);

std::map<std::vector<int>, PermtData> permt_with_extra_data(std::vector<std::vector<int>> permt_list, std::vector<int> permt_lengths);

std::vector<std::pair<std::vector<int>, int>> permt_sorted_by_length(std::map<std::vector<int>, int> permt_length_map);

bool sorted_by_length_helper(std::pair<std::vector<int>, int> permt1, std::pair<std::vector<int>, int> permt2);

std::vector<int> permt_multp_right(std::vector<int> permt, std::pair<int, int> transposition);

std::vector<int> permt_multp_left(std::vector<int> permt, std::pair<int, int> transposition);

std::vector<std::pair<int, int>> transp_1length_diff(std::vector<int> permt);

std::vector<int> permt_right_descent(std::vector<int> permt);

int permt_first_right_descent(std::vector<int> permt);

std::vector<int> permt_prompt(void);

std::string f_name_prompt(void);
