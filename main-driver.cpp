/*
The GPLv3 License (GPLv3)

Copyright (c) 2022 cutiness

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
    This file only contains the driver code for the program.
    For a more in depth view, please look at "functions.cpp"
*/

#include "permutation-basics.h"
#include "bruhat-order.h"
#include "polynomials.h"

using namespace std;

template<typename T>
void print_permt_data(FILE* ifp, T m){
    for(auto itr = m.begin(); itr != m.end(); itr++){
        print1d(ifp, itr->first); fprintf(ifp, " - %d\n", itr->second);
    }
}

// true means 't'erminal , false mean 'f'ile
pair<char*, bool> t_f_prompt(const char* file_extention = ".txt"){
    printf("\n%s", "  Do you wish to output the result to [t]erminal, or a [f]ile? [t-f] : ");
          char output_place = -1, terminating_char = -1;
          output_place = getc(stdin); terminating_char = getc(stdin);
          if(terminating_char != '\n' || (output_place != 'T' && output_place != 't' && output_place != 'f' && output_place != 'F')){
            printf("  Please only use 't' or 'f'.\n");
            exit(0);
          }

          if(output_place == 't' || output_place == 'T') return {NULL, true};

          else{
            printf("\n%s%s%s\n%s\n%s\n\n%s\n%s",
                                         "  REMARK: The program will create a ",
                                         file_extention,
                                         " file in the same directory",
                                         "  that is being executed, any file with the same name will be overwritten.",
                                         "  Use with caution. The name can be maximum 50 characters.",
                                         "  Allowed characters other than english letters are ' - ' and ' _ '",
                                         "  Please specify a file name, only eng. letters, no file extensions, no spaces: ");
            char *file_name = new char[56];
            for(int i = 0; i < 51; i++){
              char temp_char = getc(stdin);
              if(temp_char >= 65 && temp_char <= 90) file_name[i] = temp_char;
              else if(temp_char >= 97 && temp_char <= 122) file_name[i] = temp_char;
              else if(temp_char >= 48 && temp_char <= 57) file_name[i] = temp_char;
              else if(temp_char == 45 || temp_char == 95) file_name[i] = temp_char; //adds '-' and '_'
              else if(temp_char == '\n'){
                //adds the file extention
                file_name[i] = file_extention[0]; file_name[i+1] = file_extention[1];
                file_name[i+2] = file_extention[2]; file_name[i+3] = file_extention[3];
                break;
              }
              else{
                printf("%s%c%s\n", "  The character: ", temp_char, " is not allowed in a file name.\n");
                delete[] file_name;
                exit(0);
              }
              if(i == 50){
                printf("  The name you entered is longer than 50 characters.\n");
                delete[] file_name;
                exit(0);
              }
            }
            return {file_name, false};
          }
}

int input_prompt(void){
  char temp_char = -1;
  vector<int> temp_vec;
  printf("\n%s\n%s", "  Please enter a number below (ex. '5' corresponds to S_5) ",
                       "  'n': ");
  temp_char = getc(stdin);
  while(temp_char != '\n'){
    int temp = temp_char - 48;
    //this part checks if the input is a number
    if(temp < 0 || temp > 9){
      cout << "  The input should contain only numbers!\n";
      exit(0);
    }
    else{
      temp_vec.push_back(temp_char - 48);
      temp_char = getc(stdin);
    }
  }
  int result = 0;
  for(int i = 0; i < temp_vec.size(); i++) result += (take_power10(temp_vec.size() -1-i)* temp_vec[i]);
  return result;
}

int main(void){

  bool continue_program = true;

  while(continue_program){
    char user_choice = -1; char helper_char = -1;
    printf("\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n%s",
           "  Please choose one of the options below:",
           "  1-) All permutations in S_n with their lengths, ordered lexiographically",
           "  2-) Entire bruhat order graph for S_n",
           "  3-) Bruhat order graph between two permutations in S_n",
           "  4-) Kazhdan-Lustzig polynomial for two permutations (using bruhat_matrix)",
           "  5-) Same as the option (4), but multi threaded (uses 7 cores) [BETA, CAN BE BUGGY]",
           "  6-) Kazhdan-Lustzig polynomial for two permutations (using graphs) [DEPRECATED, DO NOT USE]",
           "  Enter a number[1-6] : ");

    user_choice = getc(stdin);
    helper_char = getc(stdin);
    if(user_choice - 48 < 1 || user_choice - 48 > 6 || helper_char != '\n'){
        cout << "  The input should contain only numbers! [1-5]\n";
        exit(0);
    }

    else if(user_choice == '1'){
        current_sn_group = input_prompt();
        auto all_p = permt_all_sn(current_sn_group);
        auto all_p_len = permt_lengths(all_p);
        pair<char*, bool> x = t_f_prompt();
        if(x.second){
            for(int i = 0; i < factorial(current_sn_group); i++){
                print1d(stdout, all_p[i]); printf(" - %d\n", all_p_len[i]);
            }
        }
        else{
            FILE* ifp = fopen(x.first, "w");
             for(int i = 0; i < factorial(current_sn_group); i++){
                print1d(ifp, all_p[i]); fprintf(ifp, " - %d\n", all_p_len[i]);
             }
            printf("\n%s%s\n", "  The result has been successfully written to the file: ", x.first);
            delete[] x.first;
            fclose(ifp);
        }
    }

    else if(user_choice == '2'){
        int user_n_input = input_prompt();
        auto lulvec = bruhat_graph_all_sn(user_n_input);
        pair<char*, bool> x = t_f_prompt(".dot");
        vector<string> permt_names;
        // this part just creates the string version of our permutation, so that we can name our vertex
        for(auto itr = lulvec.second.begin(); itr != lulvec.second.end(); itr++){
            string temp_permt_name;
            for(auto itr2 = itr->first.begin(); itr2 != itr->first.end(); itr2++){
                temp_permt_name.push_back(*itr2 + 48); // 48 is added to obtain the ASCII value
            }
            permt_names.push_back(temp_permt_name);
        }

        if(x.second){
            boost::write_graphviz(cout, lulvec.first, boost::make_label_writer(&permt_names[0]));
        }
        else{
            ofstream ifp(x.first); // output file stream
            boost::write_graphviz(ifp, lulvec.first, boost::make_label_writer(&permt_names[0]));

            printf("\n%s%s\n", "  The result has been successfully written to the file: ", x.first);
            delete[] x.first;
            ifp.close();
        }
    }

    else if(user_choice == '3'){
        vector<int> permt1 = permt_prompt(), permt2 = permt_prompt();
        pair<char*, bool> x = t_f_prompt(".dot");

        auto lulvec = bruhat_graph_between_permt(permt1, permt2);
        vector<string> permt_names;
        boost::graph_traits<bruhat_graph>::vertex_iterator gitr, gitr_end;
        auto g = lulvec.first;

        // this part just creates the string version of our permutation, so that we can name our vertex
        for(boost::tie(gitr, gitr_end) = boost::vertices(g); gitr != gitr_end; gitr++){
            string temp_permt_name; vector<int> z = g[*gitr].permt;
            for(auto vitr = z.begin(); vitr != z.end(); vitr++){
                temp_permt_name.push_back(*vitr + 48); // 48 is added to obtain the ASCII value
            }
            permt_names.push_back(temp_permt_name);
        }

        if(x.second){
            printf("\n");
            boost::write_graphviz(cout, lulvec.first, boost::make_label_writer(&permt_names[0]));
        }
        else{
            ofstream ifp(x.first); // output file stream
            boost::write_graphviz(ifp, lulvec.first, boost::make_label_writer(&permt_names[0]));

            printf("\n%s%s\n", "  The result has been successfully written to the file: ", x.first);
            delete[] x.first;
            ifp.close();
        }
    }

    else if(user_choice == '4'){
        printf("%s\n%s\n%s\n%s\n%s\n",
                "  In order to be more efficient with resource usage, the program creates a database,",
                "  containing previously calculated Kazhdan-Lustzig polynomials. By default it is named",
                "  'KL-database.txt' , it should reside in the same directory that you are running this program.",
                "  If you wish to change that name, you should edit the source code directly.",
                "  Corresponding variable is defined on 'polynomials.cpp' file, check that out for more info.");
        vector<int> permt1 = permt_prompt(), permt2 = permt_prompt();

        // Letting the user know that the input is problematic
        if(permt1.size() != permt2.size()){
            printf("\n  Permutations you entered do not belong to the same group!\n"); exit(0);
        }

        // Reading the database file, for that particular S_n group that permutations belong to
        current_sn_group = permt1.size();
        all_p = permt_all_sn(current_sn_group);
        all_p_len = permt_lengths(all_p);
        all_p_data = permt_with_extra_data(all_p, all_p_len);

        int f_n = factorial(current_sn_group);
        /*  Allocating space inside b_matrix */
        b_matrix = new int*[f_n];
        for(int i = 0; i < f_n; i++){
            b_matrix[i] = new int[f_n];
        }

        printf("  Initiating K-L polynomial database ...\n");
        k_l_database_initiate();
        printf("  Initiating Bruhat matrix ...\n");
        bruhat_matrix_initiate();

        Polynomial result; auto dummy = k_l_database_check({permt1, permt2});
        if(dummy.first) result = dummy.second;
        else result = polynom_k_l(permt1, permt2);

        // in case new information is obtained
        k_l_database_append();

        printf("\n K-L polynomial: ");
        polynom_display(stdout, result); printf("\n");
    }

    else if(user_choice == '5'){
        printf("%s\n%s\n%s\n%s\n%s\n",
                "  In order to be more efficient with resource usage, the program creates a database,",
                "  containing previously calculated Kazhdan-Lustzig polynomials. By default it is named",
                "  'KL-database.txt' , it should reside in the same directory that you are running this program.",
                "  If you wish to change that name, you should edit the source code directly.",
                "  Corresponding variable is defined on 'polynomials.cpp' file, check that out for more info.");
        vector<int> permt1 = permt_prompt(), permt2 = permt_prompt();

        // Letting the user know that the input is problematic
        if(permt1.size() != permt2.size()){
            printf("\n  Permutations you entered do not belong to the same group!\n"); exit(0);
        }

        // Reading the database file, for that particular S_n group that permutations belong to
        current_sn_group = permt1.size();
        all_p = permt_all_sn(current_sn_group);
        all_p_len = permt_lengths(all_p);
        all_p_data = permt_with_extra_data(all_p, all_p_len);

        int f_n = factorial(current_sn_group);
        /*  Allocating space inside b_matrix */
        b_matrix = new int*[f_n];
        for(int i = 0; i < f_n; i++){
            b_matrix[i] = new int[f_n];
        }

        printf("  Initiating K-L polynomial database ...\n");
        k_l_database_initiate();
        printf("  Initiating Bruhat matrix ...\n");
        
        ostringstream s; s << "bruhat-matrix" << current_sn_group << ".txt";
        FILE* ifp = fopen(s.str().c_str(), "r");
        if(ifp == NULL){
            printf("%s%s",
                    "  No previous bruhat matrix data is found, generating for the entire group...\n",
                    "  This might take some time, stand still...\n");
            bruhat_matrix_all_sn_multi_threaded(current_sn_group);
            bruhat_matrix_write();
        }

        Polynomial result; auto dummy = k_l_database_check({permt1, permt2});
        if(dummy.first) result = dummy.second;
        else result = polynom_k_l(permt1, permt2);

        // in case new information is obtained
        k_l_database_append();

        printf("\n K-L polynomial: ");
        polynom_display(stdout, result); printf("\n");
    }

    else if(user_choice == '6'){
        printf("%s\n%s\n%s\n%s\n%s\n",
                "  In order to be more efficient with resource usage, the program creates a database,",
                "  containing previously calculated Kazhdan-Lustzig polynomials. By default it is named",
                "  'KL-database.txt' , it should reside in the same directory that you are running this program.",
                "  If you wish to change that name, you should edit the source code directly.",
                "  Corresponding variable is defined on 'polynomials.cpp' file, check that out for more info.");
        vector<int> permt1 = permt_prompt(), permt2 = permt_prompt();

        // Letting the user know that the input is problematic
        if(permt1.size() != permt2.size()){
            printf("\n  Permutations you entered do not belong to the same group!\n"); exit(0);
        }

        // Reading the database file, for that particular S_n group that permutations belong to
        current_sn_group = permt1.size();
        int f_n = factorial(current_sn_group);
        all_p = permt_all_sn(current_sn_group);

        // For a standlane operation calculating every length is a bit of a waste, for this reason
        // '-1' is used as a placeholder, while index value is still utilized
        for(int i = 0; i < f_n; i++){
            all_p_data[all_p[i]] = {permt_inversion_amount(all_p[i]),i};
        }

        printf("  Initiating K-L polynomial database ...\n");
        k_l_database_initiate();

        Polynomial result; auto dummy = k_l_database_check({permt1, permt2});
        if(dummy.first) result = dummy.second;
        else result = polynom_k_l_standalone(permt1, permt2, {-1,-1}, {-1,-1}, false);

        // Because this method is deprecated only show the output
        // we do not store it for later use
        //k_l_database_append();

        printf("\n K-L polynomial: ");
        polynom_display(stdout, result); printf("\n");
    }

    printf("\n  Do you wish to go back to the main [m]enu or [q]uit ? [m-q] : ");
    user_choice = getc(stdin); helper_char = getc(stdin);
    if(user_choice != 'm' && user_choice != 'M') continue_program = false;
  }
}
