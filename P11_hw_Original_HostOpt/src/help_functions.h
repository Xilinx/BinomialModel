/*****************************************************************************

 Copyright (c) 2019, Xilinx, Inc.
 
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
 
      http://www.apache.org/licenses/LICENSE-2.0
 
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

******************************************************************************/

#ifndef __HELP_FUNCTIONS_H__
#define __HELP_FUNCTIONS_H__

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include <CL/cl.h>
#include "kernel.h"

using namespace std;


typedef struct {
    // ------------------------------------------------
    // Binomial settings
    // ------------------------------------------------
    string Company_Name;                      // set in a Test config file
    int    T;                                 // set in a Test config file
    float  S, K, r, sigma, q;                 // set in a Test config file
    int    n;                                 // set in a Test config file

    // ------------------------------------------------
    // Test Vectors configuration
    // ------------------------------------------------
    float K_Step;                             // set in a Test config file
    int   NB_OF_TESTS;                        // set in a Test config file

    // ------------------------------------------------
    // Debug Information
    // ------------------------------------------------
    string File_Name;                         // Config file name
    int    Line_Nb;                           // Line Nb in Config file

} test_config_t;


typedef struct {
    // ------------------------------------------------
    // SW Resources to be used
    // ------------------------------------------------
    int   NB_OF_THREADS;                      // set in a SW_HW_config file

    // ------------------------------------------------
    // Hardware Resources: Available & to be used
    // ------------------------------------------------
    int   NB_OF_KERNELS;                      // set in a SW_HW_config file
    int   NB_OF_CUs_PER_KERNEL;               // set in a SW_HW_config file
    int   NB_OF_PARALLEL_FUNCTIONS_PER_CU;    // set in a SW_HW_config file
    int   MAX_TREE_HEIGHT;                    // Set during Host Code execution. Max height of a Binomial tree supported by a Kernel (defined during kernel implementation)
    int   MAX_NB_OF_TESTS;                    // Set during Host Code execution. Max number of test vectors supported by the application

    // ------------------------------------------------
    // Debug Information
    // ------------------------------------------------
    string File_Name;                         // Config file name
    int    Line_Nb;                           // Line Nb in Config file
} sw_hw_config_t;



void read_sw_hw_config_file (const char* SW_HW_Config_File_Name, sw_hw_config_t* SW_HW_Config);
void print_sw_hw_config_info(sw_hw_config_t SW_HW_Config);

void read_test_config_file (const char* Test_Config_File_Name,  vector<test_config_t>  *Test_Config);
void print_test_config_info(vector<test_config_t>  *Test_Config);

void process_configurations(string sw_hw, sw_hw_config_t* SW_HW_Config, vector<test_config_t>* Test_Config, int *DEFINED_NB_OF_TESTS, int *ROUNDED_NB_OF_TESTS);
void generate_test_vectors(t_in_data* host_IN_DATA, vector<test_config_t> Test_Config, int ROUNDED_NB_OF_TESTS);

// =======================================================
// Helper Function: Allocate HOST Memory aligned to 4096
// =======================================================
template <typename T>
T* allocate_host_mem(size_t number_of_elements, string mem_name, bool print_message) {
	void *ptr=nullptr;

	if (print_message == true)
		cout << "HOST-Info: Allocating Host Memory for " << mem_name << " ... " << endl;

	if (posix_memalign(&ptr,4096,number_of_elements*sizeof(T))) {
		cout << endl << "HOST-Error: Out of Memory during memory allocation for " << mem_name << " array" << endl << endl;
		exit(0);
	}
	return reinterpret_cast<T*>(ptr);
}

double run_custom_profiling (int Nb_Of_Kernels, int Nb_Of_Memory_Tranfers, cl_event* K_exe_event, cl_event* Mem_op_event,string* list_of_kernel_names);

int compare_results(float* sw_Res, float* hw_Res, int Nb_of_Results, int Nb_Of_Errors_To_Reports);
int cmp_floats(float val1, float val2);

void store_results(string SW_HW_Mode, string Out_File_Name, t_in_data* host_IN_DATA, float* hw_res, vector<test_config_t>* Test_Config);
#endif
