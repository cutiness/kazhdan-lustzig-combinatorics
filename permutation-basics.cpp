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

#include "permutation-basics.h"

using namespace std;

/* GLOBAL VARIABLES --------------- */

int current_sn_group;

vector<vector<int>> all_p;

vector<int> all_p_len;

map<vector<int>, PermtData> all_p_data;

/*--------------------------------- */


int take_power10(int n){
    int result = 1;
    if(n == 0) return result;
    else{
        for(int i = 0; i < n; i++) result *= 10;
    }
    return result;
}

int factorial(int n){
    int result = 1;
    for(int i = 2; i <= n; i++) result = result * i;
    return result;
}

// print the elements of the given permutation to the file stream, with a whitespace between elements
void print1d(FILE* stream, vector<int> vec1d){
    if(!vec1d.empty()){
        for(int i = 0; i < vec1d.size() - 1; i++){
            fprintf(stream, "%d ", vec1d[i]);
        }
        fprintf(stream,"%d",vec1d[vec1d.size()-1]);
    }
}


void print2d(vector<vector<int>> vec2d, int n){
  for(int i = 0; i < vec2d.size(); i++){
    for(int j = 0; j < vec2d[i].size(); j++){
      printf("%d", vec2d[i][j]);
    }
    if((i + 1) % n == 0) printf("\n");
    else printf(" ");
  }
  if(vec2d.size() % n != 0) printf("\n");
}

//for a given n, this function returns the vector containing all permutations in S_n
//that is the symmetric group of order n! , in the line notation
vector<vector<int>> permt_all_sn(int n){
    vector<vector<int>> result;
    //initializing a first vector, which will be our first element
    vector<int> temp;
    for(int i = 1; i <= n; i++) temp.push_back(i);
    result.push_back(temp); // this is the first element

    //as long as new permutations are available, this loop will continue
    while(next_permutation(temp.begin(), temp.end())) result.push_back(temp);
    return result;
}

//return any combination of transpositions as pairs, inside of a list
//identity transpositions are not taken into account **
vector<pair<int, int>> transp_all_sn(int n){
    if(n == 1){pair<int, int> result(1,1); return {result};}
    vector<pair<int, int>> result;
    for(int i = n; i >= 2; i--){
        for(int j = i - 1; j >= 1; j--){
            pair<int, int> temp(i, j); result.push_back(temp);
        }
    }
    return result;
}

//for a given permutation, this function will return the amount of inversions
int permt_inversion_amount(vector<int> permt){
    // the set of inversions for a permutation w = w1 w2 w3 is {(wi,wj) | i < j, wi > wj}
    int temp_permt[permt.size()];
    return divide_permt(permt, temp_permt, 0, permt.size() - 1).second;
}

pair<vector<int>, int> divide_permt(vector<int> permt, int temp_permt[], int left, int right){
    int permt_inversions = 0, mid = 0;

    //until there is nothing to divide further, this block will run
    if(right > left){
        mid = (left + right) / 2; //this will be automatically rounded

        // total inversions is calculated as follows
        // left_inversions + right_inversions + merge_inversions (those that are found while merging)
        auto pair_p = divide_permt(permt, temp_permt, left, mid); //mid point will be the right end of the left block
        permt = pair_p.first;
        permt_inversions += pair_p.second;

        pair_p = divide_permt(permt, temp_permt, mid+1, right); //this is the right block, similar as above
        permt = pair_p.first;
        permt_inversions += pair_p.second;

        //now we can merge two block that was created above
        pair_p = merge_permt(permt, temp_permt, left, mid+1, right);
        permt = pair_p.first;
        permt_inversions += pair_p.second;
    }
    //we are done here
    pair<vector<int>, int> result; result.first = permt; result.second = permt_inversions;
    return result;
}

pair<vector<int>, int> merge_permt(vector<int> permt, int temp_permt[], int left, int mid, int right){
    int left_i_permt = left, left_i_temp = left, mid_i = mid, permt_inversions = 0;
    while((left_i_permt <= mid - 1) && mid_i <= right){
        //in this case any element bigger or equal to permt[left_i_permt] will create an inversions
        //as the left and right blocks are sorted in the ascending order, there will be (mid - left_i_permt) inversions
        if(permt[left_i_permt] > permt[mid_i]){
            temp_permt[left_i_temp] = permt[mid_i];
            left_i_temp++; mid_i++;
            permt_inversions += mid - left_i_permt;
        }
        else{
            temp_permt[left_i_temp] = permt[left_i_permt];
            left_i_temp++; left_i_permt++;
        }//in this case there are no inversions, we just continue to merge two vectors normally
    }

    //if anything is left on the left block add it to temp_permt
    while(left_i_permt <= mid - 1){
        temp_permt[left_i_temp] = permt[left_i_permt];
        left_i_temp++; left_i_permt++;
    }

    //similar as above, if anything is left on the right block, add it to temp_permt
    while(mid_i <= right){
        temp_permt[left_i_temp] = permt[mid_i];
        left_i_temp++; mid_i++;
    }

    //now, left and right block is merged and it is stored inside temp_permt, we shall push the data to the original permt
    for(left_i_permt = left; left_i_permt <= right; left_i_permt++)
        permt[left_i_permt] = temp_permt[left_i_permt];

    pair<vector<int>, int> result; result.first = permt; result.second = permt_inversions;
    return result;
}

// return inverse of a given permutation, in line notation
vector<int> permt_inverse(vector<int> permt){
    vector<int> result = permt; result.resize(permt.size());
    for(int i = 0; i < permt.size(); i++){
        result[permt[i] - 1] = i + 1;
    }
    return result;
}

// returns a list of lengths of the provided permt_list, in the same order
vector<int> permt_lengths(vector<vector<int>> permt_list){
    vector<int>result; result.resize(permt_list.size());
    for(int i = 0; i < permt_list.size(); i++){
        result[i] = permt_inversion_amount(permt_list[i]);
    }
    return result;
}

// This function combines the data obtained by 'permt_all_sn' and 'permt_lengths', and it
// returns a map containing (key, value) pairs.
map<vector<int>, PermtData> permt_with_extra_data(vector<vector<int>> permt_list, vector<int> permt_lengths){
    map<vector<int>, PermtData> result;
    for(int i = 0; i < permt_list.size(); i++){
        PermtData temp = {permt_lengths[i], i};
        result[permt_list[i]] = temp;
    }
    return result;
}

// Returns a list of all permutation, but ordered by their length using the
// permt_with_lengths function
vector<pair<vector<int>, int>> permt_sorted_by_length(map<vector<int>, int> permt_length_map){
    vector<pair<vector<int>, int>> result;
    for(auto itr = permt_length_map.begin(); itr != permt_length_map.end(); itr++){
        result.push_back({itr->first, itr->second});
    }
    sort(result.begin(), result.end(), sorted_by_length_helper);
    return result;
}


// Return true if l(w) < l(v) , false otherwise.
// If permutations have equal length then lexiographic ordering is made
// Written to be used with permt_sorted_by_length function
bool sorted_by_length_helper(pair<vector<int>, int> permt1, pair<vector<int>, int> permt2){
    if(permt1.second < permt2.second) return true;
    else if(permt1.second > permt2.second) return false;
    else return permt1.first < permt2.first; //if length is the same, just do lexiographic ordering
}



//this function multipilies the permutation with a given transposition, from right
vector<int> permt_multp_right(vector<int> permt, pair<int, int> transposition){
    //this operation simply swaps the elements in the corresponding location
    // IT IS ASSUMED THAT VALUES IN THE TRANSPOSITION IS INSIDE 'PERMT' , use with caution **
    int temp = permt[transposition.first - 1];
    permt[transposition.first - 1] = permt[transposition.second - 1];
    permt[transposition.second - 1] = temp;
    return permt;
}

//this function multipilies the permutation with a given transposition, from left
vector<int> permt_multp_left(vector<int> permt, pair<int, int> transposition){
    //this operation directly swaps (i, j) in the permutation, as the the operation is from left
    // IT IS ASSUMED THAT VALUES IN THE TRANSPOSITION IS INSIDE 'PERMT' , use with caution **
    auto ptr_first = find(permt.begin(), permt.end(), transposition.first);
    auto ptr_second = find(permt.begin(), permt.end(), transposition.second);
    int temp = *ptr_first;

    *ptr_first = *ptr_second;
    *ptr_second = temp;

    return permt;
}

vector<pair<int, int>> transp_1length_diff(vector<int> permt){
    vector<pair<int, int>> result;
   for(int i = 0; i < permt.size() - 1; i++){
        int j = i + 1;
        for( ; j < permt.size(); j++){
            if(permt[j] > permt[i]){
                result.push_back({i+1, j+1});
                break;
            }
        }
        if(j == permt.size() - 1) continue;
        else{
            int least_biggest = permt[j]; j++;
            for( ; j < permt.size() ; j++){
                if(permt[j] < least_biggest && permt[j] > permt[i]) {
                    result.push_back({i+1, j+1});
                    least_biggest = permt[j];
                }
            }
        }
    }
    return result;
}

// Elements i such what w(i) > w(i + 1) is returned as a vector
vector<int> permt_right_descent(vector<int> permt){
    vector<int> result;
    for(int i = 0; i < permt.size() - 1; i++){
        if(permt[i] > permt[i + 1]) result.push_back(i + 1); // Element's real value is used, instead
    }                                                        // of the index value
    return result;
}

// First i such what w(i) > w(i + 1) is returned
int permt_first_right_descent(vector<int> permt){
    vector<int> result;
    for(int i = 0; i < permt.size() - 1; i++){
        if(permt[i] > permt[i + 1]) return i+1; // Element's real value is used, instead
    }                                           // of the index value
    // if there is no 'i' satisfying this, then return -1
    return -1;
}

vector<int> permt_prompt(void){
  char temp_char = -1;
  string unit_element = "";
  vector<int> temp_vec;
  printf("\n%s\n%s\n%s", "  Enter a permutation below in line notation, use ane whitespace between any element.",
                         "  For example:  1 2 3 4 5 6 7 8 9 10 11 12 would be the identity permutation in S_12",
                         "  Input: ");
  temp_char = getc(stdin);
  while(temp_char != '\n'){
    if(temp_char == ' ' && unit_element.size() == 0){
        cout << "  You either used a traling/ending whitespace or there are multiple spaces between elements!\n";
        exit(0);
    }
    else if(temp_char == ' '){ // if we have a whitespace while unit_element being nonempty
                               // then we add the necessary integer
        int temp_int = 0;
        for(int i = 0; i < unit_element.size(); i++) temp_int += (take_power10(unit_element.size() -1-i)*(unit_element[i]-48));
        temp_vec.push_back(temp_int);
        unit_element.clear(); temp_char = getc(stdin); continue;
    }
    //this part checks if the input is a number
    if(temp_char < '0' || temp_char > '9'){
      cout << "  The input should contain only numbers!\n";
      exit(0);
    }
    else{
        unit_element.push_back(temp_char);
        temp_char = getc(stdin);
    }
  }
  // addition of the last element will not be triggered as a trailing whitespace in not expected
  // Therefore, we add it here
  if(unit_element.size() != 0) temp_vec.push_back(stoi(unit_element));
  return temp_vec;
}

// Return the file_name provided by the user in string form
string f_name_prompt(void){
  char temp_char = -1;
  string file_name = "";
  printf("\n%s\n%s\n%s", "  Please enter the file name you wish to take input form, with the file extention: ",
                         "  Keep in mind that your operating system may not accept any character as a file name, be cautious.",
                         "  File name: ");
  temp_char = getc(stdin);
  for(int i = 0; temp_char != '\n'; i++){
    file_name.push_back(temp_char);
    temp_char = getc(stdin);
  }
  return file_name;
}
