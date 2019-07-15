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

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstring>
#include <regex>
#include <typeinfo>

using namespace std;

#include "help_functions.h"

// ==================================================
// Read Test Config File
//   At the end we check if we have at least one
//   test configuration. If not, then Error and exit
// ==================================================
void  read_test_config_file(const char* Test_Config_File_Name, vector<test_config_t> *Test_Config) {
    fstream Config_File;
    string	in_line;
    int     line_nb = 0;
    int     nb_of_read_values;
    int     Nb_Of_Values_To_Read_Per_Line = 10;
    test_config_t Current_Test_Confguration;

    Config_File.open(Test_Config_File_Name,ios::in);
	if (! Config_File.is_open()) {
	    cout << endl << "HOST-Error: Failed to open the " <<  Test_Config_File_Name << " for read" << endl << endl;
	    exit(1);
	}

	while (getline (Config_File,in_line)) {
		line_nb ++;

		// -----------------------------------
		// If comment exists, then remove it
		// -----------------------------------
		int comment_index = in_line.find("#");
		if (comment_index != -1) {
			in_line = in_line.substr(0,comment_index);
		}

		// ---------------------------------------------------------------
		// If a string contains only space characters then read next line
		// ---------------------------------------------------------------
		int start_str = in_line.find_first_not_of(" \t");
		if (start_str == -1) continue;

		// --------------------------------------------------------
		// Remove all leading, trailing and extra spaces
		// --------------------------------------------------------
		in_line = regex_replace(in_line, regex("^ +| +$|( ) +"), "$1");

		// --------------------------------------------------------
		// Read All values from the line
		// --------------------------------------------------------
		stringstream in_str_stream;
		in_str_stream << in_line;

		Current_Test_Confguration.File_Name = Test_Config_File_Name;
		Current_Test_Confguration.Line_Nb   = line_nb;

		nb_of_read_values = 0;
		while (!in_str_stream.eof()) {
			nb_of_read_values ++;
			switch (nb_of_read_values) {
				case  1: in_str_stream >> Current_Test_Confguration.Company_Name; break;
				case  2: in_str_stream >> Current_Test_Confguration.T;            break;
				case  3: in_str_stream >> Current_Test_Confguration.S;            break;
				case  4: in_str_stream >> Current_Test_Confguration.K;            break;
				case  5: in_str_stream >> Current_Test_Confguration.r;            break;
				case  6: in_str_stream >> Current_Test_Confguration.sigma;        break;
				case  7: in_str_stream >> Current_Test_Confguration.q;            break;
				case  8: in_str_stream >> Current_Test_Confguration.n;            break;
				case  9: in_str_stream >> Current_Test_Confguration.K_Step;       break;
				case 10: in_str_stream >> Current_Test_Confguration.NB_OF_TESTS;  break;
				default: break;
			}
			if (nb_of_read_values == Nb_Of_Values_To_Read_Per_Line) break;
		}

		if (nb_of_read_values != Nb_Of_Values_To_Read_Per_Line) {
			cout << endl << "HOST-Error: The " <<  Test_Config_File_Name << " file is incomplete." << endl;
			cout <<         "            Line " << line_nb << " contains " << nb_of_read_values << " values instead of " << Nb_Of_Values_To_Read_Per_Line << endl;
			exit(1);
		}

		(*Test_Config).push_back(Current_Test_Confguration);
	}

	Config_File.close();

	if ((*Test_Config).size() == 0) {
		cout << endl << "HOST-Error: No Test configurations were found in the " <<  Test_Config_File_Name << " file." << endl;
		exit(1);
	}
}


// ==================================================
// Print Test Config File Info
// ==================================================
void print_test_config_info(vector<test_config_t>  *Test_Config) {
	vector<string> column_names = {"Company","T","S","K","r","sigma","q","n","K_Step","NB_OF_TESTS"};

    cout << endl;
    cout << "HOST-Info: "; for (unsigned i=0; i<column_names.size()*12; i++) cout << "~";                                cout << endl;
	cout << "HOST-Info: Test Vector Configurations: #" << (*Test_Config).size() << endl;

	// -------------------
	// Print Table Header
	// -------------------
	cout << "HOST-Info: "; for (unsigned i=0; i<column_names.size()*12; i++) cout << "-";                                cout << endl;
	cout << "HOST-Info: "; for (unsigned i=0; i<column_names.size();    i++) cout << setw(12) << column_names[i]+ " | "; cout << endl;
	cout << "HOST-Info: "; for (unsigned i=0; i<column_names.size()*12; i++) cout << "-";                                cout << endl;

	// --------------------------
	// Print Test Configurations
	// --------------------------
	for (unsigned i=0; i<(*Test_Config).size(); i++) {
		cout << "HOST-Info: ";
		cout << fixed;
		cout <<   setw(9) << (*Test_Config)[i].Company_Name;
		cout <<  setw(12) << (*Test_Config)[i].T;
		cout <<  setw(12) << setprecision(3) << (*Test_Config)[i].S;
		cout <<  setw(12) << setprecision(3) << (*Test_Config)[i].K;
		cout <<  setw(12) << setprecision(3) << (*Test_Config)[i].r;
		cout <<  setw(12) << setprecision(3) << (*Test_Config)[i].sigma;
		cout <<  setw(12) << setprecision(3) << (*Test_Config)[i].q;
		cout <<  setw(12) << (*Test_Config)[i].n;
		cout <<  setw(12) << setprecision(3) << (*Test_Config)[i].K_Step;
		cout <<  setw(12) << (*Test_Config)[i].NB_OF_TESTS;

		cout << endl;
	}

	// -------------------
	// Print Final Line
	// -------------------
    cout << "HOST-Info: "; for (unsigned i=0; i<column_names.size()*12; i++) cout << "~";                                cout << endl;

    cout << endl;

}


// ==================================================
// Read SW_HW_Config File
//   We check that we read all values.
//   f not, then Error and exit
// ==================================================

void  read_sw_hw_config_file(const char* SW_HW_Config_File_Name, sw_hw_config_t* SW_HW_Config) {
    fstream Config_File;
    string	in_line;
    int     line_nb = 0;
    int     nb_of_read_values = 0;
    int     Nb_Of_Values_To_Read_Per_Line = 4;

    Config_File.open(SW_HW_Config_File_Name,ios::in);
	if (! Config_File.is_open()) {
	    cout << endl << "HOST-Error: Failed to open the " <<  SW_HW_Config_File_Name << " for read" << endl << endl;
	    exit(1);
	}

	while (getline (Config_File,in_line)) {
		line_nb ++;

		// -----------------------------------
		// If comment exists, then remove it
		// -----------------------------------
		int comment_index = in_line.find("#");
		if (comment_index != -1) {
			in_line = in_line.substr(0,comment_index);
		}

		// ---------------------------------------------------------------
		// If a string contains only space characters then read next line
		// ---------------------------------------------------------------
		int start_str = in_line.find_first_not_of(" \t");
		if (start_str == -1) continue;

		// --------------------------------------------------------
		// Remove all leading, trailing and extra spaces
		// --------------------------------------------------------
		in_line = regex_replace(in_line, regex("^ +| +$|( ) +"), "$1");

		// --------------------------------------------------------
		// Read All values from the line
		// --------------------------------------------------------
		stringstream in_str_stream;
		in_str_stream << in_line;

		SW_HW_Config->File_Name = SW_HW_Config_File_Name;
		SW_HW_Config->Line_Nb   = line_nb;

		while (!in_str_stream.eof()) {
			nb_of_read_values ++;
			switch (nb_of_read_values) {
				case  1: in_str_stream >> SW_HW_Config->NB_OF_THREADS;                   break;
				case  2: in_str_stream >> SW_HW_Config->NB_OF_KERNELS;                   break;
				case  3: in_str_stream >> SW_HW_Config->NB_OF_CUs_PER_KERNEL;            break;
				case  4: in_str_stream >> SW_HW_Config->NB_OF_PARALLEL_FUNCTIONS_PER_CU; break;
				default: break;
			}
			if (nb_of_read_values == Nb_Of_Values_To_Read_Per_Line) break;
		}

		break;
	}

	// --------------------------------------------------------
	// Check All values were specified
	// --------------------------------------------------------
	if (nb_of_read_values != Nb_Of_Values_To_Read_Per_Line) {
		cout << endl << "HOST-Error: The " <<  SW_HW_Config_File_Name << " file is incomplete." << endl;
		cout <<         "            Line " << line_nb << " contains " << nb_of_read_values << " values instead of " << Nb_Of_Values_To_Read_Per_Line << endl;
		exit(1);
	}

	Config_File.close();
}


// ==================================================
// Print SW_HW_Config File Info
// ==================================================
void print_sw_hw_config_info(sw_hw_config_t SW_HW_Config) {

    cout << endl;
	cout << "HOST-Info: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"                   << endl;
	cout << "HOST-Info: Software Resources"                                                                 << endl;
	cout << "HOST-Info: ------------------"                                                                 << endl;
    cout << "HOST-Info: NB_OF_THREADS                   = " << SW_HW_Config.NB_OF_THREADS                   << endl;

	cout << "HOST-Info: "                                                                                   << endl;
    cout << "HOST-Info: Hardware Resources"                                                                 << endl;
	cout << "HOST-Info: ------------------"                                                                 << endl;
	cout << "HOST-Info: NB_OF_KERNELS                   = " << SW_HW_Config.NB_OF_KERNELS                   << endl;
	cout << "HOST-Info: NB_OF_CUs_PER_KERNEL            = " << SW_HW_Config.NB_OF_CUs_PER_KERNEL            << endl;
	cout << "HOST-Info: NB_OF_PARALLEL_FUNCTIONS_PER_CU = " << SW_HW_Config.NB_OF_PARALLEL_FUNCTIONS_PER_CU << endl;
	cout << "HOST-Info: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"                   << endl;
    cout << endl;

}

// ==================================================
// SW Version
// Check Command Line Option Values
// IMPORTANT:
//    o) Only SW Related options are checked
//    o) We also check that specified 'n' and 'NB_OF_TESTS'
//       do not exceed the limits we implement on HW
// Calculate ROUNDED_NB_OF_TESTS
// ==================================================
void process_configurations(string sw_hw, sw_hw_config_t* SW_HW_Config, vector<test_config_t>* Test_Config, int *DEFINED_NB_OF_TESTS, int *ROUNDED_NB_OF_TESTS) {

	// --------------------------------------------------------
	// Check SW_HW_Config
	// --------------------------------------------------------
	if (sw_hw == "sw") {

		if (SW_HW_Config->NB_OF_THREADS <= 0) {
			cout << endl << "HOST-Error: " <<  (*SW_HW_Config).File_Name << " (line " << (*SW_HW_Config).Line_Nb << "):  Incorrect value NB_OF_THREADS=" << SW_HW_Config->NB_OF_THREADS << endl;
			cout <<         "            The value should be >0" << endl;
			exit(1);
		}

	} else { // (sw_hw == "hw")

		if (SW_HW_Config->NB_OF_KERNELS <= 0) {
			cout << endl << "HOST-Error: " <<  (*SW_HW_Config).File_Name << " (line " << (*SW_HW_Config).Line_Nb << "):  Incorrect value NB_OF_KERNELS=" << SW_HW_Config->NB_OF_KERNELS << endl;
			cout <<         "            The value should be >0" << endl;
			exit(1);
		}

		if (SW_HW_Config->NB_OF_CUs_PER_KERNEL <= 0) {
			cout << endl << "HOST-Error: " <<  (*SW_HW_Config).File_Name << " (line " << (*SW_HW_Config).Line_Nb << "):  Incorrect value NB_OF_CUs_PER_KERNEL=" << SW_HW_Config->NB_OF_CUs_PER_KERNEL << endl;
			cout <<         "            The value should be >0" << endl;
			exit(1);
		}

		if (SW_HW_Config->NB_OF_PARALLEL_FUNCTIONS_PER_CU <= 0) {
			cout << endl << "HOST-Error: " <<  (*SW_HW_Config).File_Name << " (line " << (*SW_HW_Config).Line_Nb << "):  Incorrect value NB_OF_PARALLEL_FUNCTIONS_PER_CU=" << SW_HW_Config->NB_OF_PARALLEL_FUNCTIONS_PER_CU << endl;
			cout <<         "            The value should be >0" << endl;
			exit(1);
		}
	}

	// --------------------------------------------------------
	// Check Test_Config
	// --------------------------------------------------------
	(*DEFINED_NB_OF_TESTS) = 0;
	for (unsigned i=0; i<(*Test_Config).size(); i++) {

		// .........
		// Check n
		// .........
		if (((*Test_Config)[i].n <= 0) || ((*Test_Config)[i].n > (*SW_HW_Config).MAX_TREE_HEIGHT)) {
			cout << endl << "HOST-Error: " <<  (*Test_Config)[i].File_Name << " (line " << (*Test_Config)[i].Line_Nb << "):  Incorrect value n=" << (*Test_Config)[i].n << endl;
			cout <<         "            The value should be in the following range: [1...MAX_TREE_HEIGHT(" << (*SW_HW_Config).MAX_TREE_HEIGHT << ")]" << endl;
			exit(1);
		}

		// ..................
		// Check NB_OF_TESTS
		// ..................
		if (((*Test_Config)[i].NB_OF_TESTS <= 0) || ((*Test_Config)[i].NB_OF_TESTS > (*SW_HW_Config).MAX_NB_OF_TESTS)) {
			cout << endl << "HOST-Error: " <<  (*Test_Config)[i].File_Name << " (line " << (*Test_Config)[i].Line_Nb << "):  Incorrect value NB_OF_TESTS=" << (*Test_Config)[i].NB_OF_TESTS << endl;
			cout <<         "            The value should be in the following range: [1...MAX_NB_OF_TESTS(" << (*SW_HW_Config).MAX_NB_OF_TESTS << ")]" << endl;
			exit(1);
		}
		(*DEFINED_NB_OF_TESTS) += (*Test_Config)[i].NB_OF_TESTS;

	}

	// --------------------------------------------------------
	// Check DEFINED_NB_OF_TESTS <= MAX_NB_OF_TESTS
	// --------------------------------------------------------
	if ((*DEFINED_NB_OF_TESTS) > (*SW_HW_Config).MAX_NB_OF_TESTS) {
		cout << endl << "HOST-Error: Total number of tests (" << (*DEFINED_NB_OF_TESTS) << ") specified in the " << (*Test_Config)[0].File_Name << " file exceeds MAX_NB_OF_TESTS(" << (*SW_HW_Config).MAX_NB_OF_TESTS << ")" << endl;
	}

	// -----------------------------------------------------------------------------------------
	// In our solution we equally distribute calculation across specified
	//    o) Number of Threads (SW) or
	//    o) implemented kernels, CUs, Number of parallel functions pur CU
	// Therefore we need to calculate a final number of tests, stored in ROUNDED_NB_OF_TESTS
	// -----------------------------------------------------------------------------------------
	int BASE;

	if (sw_hw == "sw") {
		BASE = SW_HW_Config->NB_OF_THREADS;
	} else {
		BASE = (*SW_HW_Config).NB_OF_KERNELS * (*SW_HW_Config).NB_OF_CUs_PER_KERNEL * (*SW_HW_Config).NB_OF_PARALLEL_FUNCTIONS_PER_CU;
	}

	if (((*DEFINED_NB_OF_TESTS) % BASE) != 0)
		(*ROUNDED_NB_OF_TESTS) = (*DEFINED_NB_OF_TESTS) + (BASE - ((*DEFINED_NB_OF_TESTS) % BASE));
	else
		(*ROUNDED_NB_OF_TESTS) = (*DEFINED_NB_OF_TESTS);

	if ((*ROUNDED_NB_OF_TESTS) > (*SW_HW_Config).MAX_NB_OF_TESTS) {
		cout << endl << "HOST-Error: Rounded number of tests " << *ROUNDED_NB_OF_TESTS << " exceeds MAX_NB_OF_TESTS(" << (*SW_HW_Config).MAX_NB_OF_TESTS << ")" << endl;
		exit(1);
	}

}




// ==============================================
// Generate Test Vectors
// ==============================================
void generate_test_vectors(t_in_data* host_IN_DATA, vector<test_config_t> Test_Config, int ROUNDED_NB_OF_TESTS) {
	int indx = 0;

	cout << "HOST-Info: Generating Test Vectors in host_IN_DATA ... " << endl;

	for (unsigned i=0; i<Test_Config.size(); i++ ) {
		for (int k=0; k<Test_Config[i].NB_OF_TESTS; k++) {
			host_IN_DATA[indx].T         = Test_Config[i].T;
			host_IN_DATA[indx].S         = Test_Config[i].S;
			host_IN_DATA[indx].K         = Test_Config[i].K + Test_Config[i].K_Step*k;
			host_IN_DATA[indx].r         = Test_Config[i].r;
			host_IN_DATA[indx].sigma     = Test_Config[i].sigma;
			host_IN_DATA[indx].q         = Test_Config[i].q;
			host_IN_DATA[indx].n         = Test_Config[i].n;
			host_IN_DATA[indx].dummy_val = 0.0f;
			indx ++;
		}
	}

	// ----------------------------------------------
	// Generate additional dummy tests to run kernels
	// ----------------------------------------------
	for (int i=indx; i<ROUNDED_NB_OF_TESTS; i++) {
		host_IN_DATA[i].T         = 1;
		host_IN_DATA[i].S         = 1;
		host_IN_DATA[i].K         = 1;
		host_IN_DATA[i].r         = 1;
		host_IN_DATA[i].sigma     = 1;
		host_IN_DATA[i].q         = 1;
		host_IN_DATA[i].n         = 1; // We specify min Tree height , because the results will be ignored
	}

}


// ============================================================================
// Custom Profiling
// ============================================================================
#define HIDE_PROFILE_MSG 1
double run_custom_profiling (int Nb_Of_Kernels, int Nb_Of_Memory_Tranfers, cl_event* K_exe_event, cl_event* Mem_op_event,string* list_of_kernel_names) {
	typedef struct {
		string    action_type; // kernel, "memory (H->G)", "memory (G->H)"
		string    name;
		cl_event  event;
		cl_ulong  profiling_command_start;
		cl_ulong  profiling_command_end;
		double    duration;
	} profile_t;

	cl_int              errCode;

	// ---------------------------------
	// Profiling
	// ---------------------------------
	profile_t *PROFILE;

	PROFILE = new profile_t[Nb_Of_Kernels + Nb_Of_Memory_Tranfers];

	for (int i=0; i<Nb_Of_Kernels; i++) {
		PROFILE[i].action_type="kernel"; PROFILE[i].name=list_of_kernel_names[i]; PROFILE[i].event = K_exe_event[i];
	}

	for (int i=0; i<Nb_Of_Memory_Tranfers; i++) {
		PROFILE[Nb_Of_Kernels+i].action_type="mem (H<->G)";
		PROFILE[Nb_Of_Kernels+i].name="Transfer_" + to_string(i+1);
		PROFILE[Nb_Of_Kernels+i].event = Mem_op_event[i];
	}

	// -------------------------------------------------------------------------------------
	// Get events Start and End times and calculate duration for
	//   o) each Kernel and
	//   o) Memory (Global <-> Host) transfer.
	// Event Profile Info:
	//   o) CL_PROFILING_COMMAND_QUEUED
	//   o) CL_PROFILING_COMMAND_SUBMIT
	//   o) CL_PROFILING_COMMAND_START
	//   o) CL_PROFILING_COMMAND_END
	// -------------------------------------------------------------------------------------
	size_t nb_of_returned_bytes;

	for (int i=0; i<Nb_Of_Kernels + Nb_Of_Memory_Tranfers; i++) {
		errCode = clGetEventProfilingInfo(PROFILE[i].event, CL_PROFILING_COMMAND_START,
										  sizeof(cl_ulong), &PROFILE[i].profiling_command_start, &nb_of_returned_bytes);
		if (errCode != CL_SUCCESS) {
			cout << endl << "HOST-Error: Failed to get profiling info for " << PROFILE[i].name << " " << PROFILE[i].action_type << endl << endl;
			exit(0);
		}

		errCode = clGetEventProfilingInfo(PROFILE[i].event, CL_PROFILING_COMMAND_END,
										  sizeof(cl_ulong), &PROFILE[i].profiling_command_end, &nb_of_returned_bytes);
		if (errCode != CL_SUCCESS) {
			cout << endl << "HOST-Error: Failed to get profiling info for " << PROFILE[i].name << " " << PROFILE[i].action_type << endl << endl;
			exit(0);
		}

		PROFILE[i].duration = (double)(PROFILE[i].profiling_command_end - PROFILE[i].profiling_command_start) * 1.0e-6;
	}

	// -------------------------------------------------------------------------------------
	// Calculate Duration of
	//   o) All kernels execution time
	//   o) Application execution time (Kernels + Memory transfer)
	// -------------------------------------------------------------------------------------
	struct {
		int      Kernels_Start_Time_Index=0;
		int      Kernels_End_Time_Index=0;
		cl_ulong Kernels_Start_Time=0;
		cl_ulong Kernels_End_Time=0;

		int      Application_Start_Time_Index=0;
		int      Application_End_Time_Index=0;
		cl_ulong Application_Start_Time=0;
		cl_ulong Application_End_Time=0;
	} PROFILE_STAT;


	for (int i=0; i<Nb_Of_Kernels + Nb_Of_Memory_Tranfers; i++) {

		// Calculate Application statistics
		// .................................
		if ((PROFILE_STAT.Application_Start_Time == 0) || (PROFILE_STAT.Application_Start_Time > PROFILE[i].profiling_command_start)) {
			PROFILE_STAT.Application_Start_Time       = PROFILE[i].profiling_command_start;
			PROFILE_STAT.Application_Start_Time_Index = i;
		}

		if (PROFILE_STAT.Application_End_Time < PROFILE[i].profiling_command_end) {
			PROFILE_STAT.Application_End_Time       = PROFILE[i].profiling_command_end;
			PROFILE_STAT.Application_End_Time_Index = i;
		}

		// Calculate Kernel statistics
		// .................................
		if (PROFILE[i].action_type == "kernel") {
			if ((PROFILE_STAT.Kernels_Start_Time == 0) || (PROFILE_STAT.Kernels_Start_Time > PROFILE[i].profiling_command_start)) {
				PROFILE_STAT.Kernels_Start_Time       = PROFILE[i].profiling_command_start;
				PROFILE_STAT.Kernels_Start_Time_Index = i;
			}

			if (PROFILE_STAT.Kernels_End_Time < PROFILE[i].profiling_command_end) {
				PROFILE_STAT.Kernels_End_Time       = PROFILE[i].profiling_command_end;
				PROFILE_STAT.Kernels_End_Time_Index = i;
			}
		}
	}

	// ------------------------------
	// Print Profiling Data
	// ------------------------------
//	int Column_Widths[5] = {15, 16, 15, 15, 15}, Separation_Line_Width = 0;
	int Column_Widths[5] = {20, 16, 15, 15, 15}, Separation_Line_Width = 0;

	// Print Table Header
	// ....................
	if (HIDE_PROFILE_MSG != 1) {

		for (int i=0; i<5; i++)  Separation_Line_Width += Column_Widths[i];
		Separation_Line_Width += 3;
		cout << "HOST-Info: " << string(Separation_Line_Width, '-') << endl;

		cout << "HOST-Info: "          << left << setw(Column_Widths[0]-1) << "Name"
							  << " | " << left << setw(Column_Widths[1]-3) << "type"
							  << " | " << left << setw(Column_Widths[2]-3) << "start"
							  << " | " << left << setw(Column_Widths[2]-3) << "end"
							  << " | " << left << setw(Column_Widths[2]-3) << "Duration(ms)"
							  << endl;

		cout << "HOST-Info: " << string(Separation_Line_Width, '-') << endl;


		// Print Individual info for each Kernel and Memory transfer
		// ..........................................................
		for (int i=0; i<Nb_Of_Kernels + Nb_Of_Memory_Tranfers; i++) {
			cout << "HOST-Info: "          << left  << setw(Column_Widths[0]-1) << PROFILE[i].name
								  << " | " << left  << setw(Column_Widths[1]-3) << PROFILE[i].action_type
								  << " | " << right << setw(Column_Widths[2]-3) << PROFILE[i].profiling_command_start
								  << " | " << right << setw(Column_Widths[2]-3) << PROFILE[i].profiling_command_end
								  << " | " << right << setw(Column_Widths[2]-3) << fixed << setprecision(3) << PROFILE[i].duration
								  << endl;
		}
		cout << "HOST-Info: " << string(Separation_Line_Width, '-') << endl;

		// Print Duration of Kernels and Application execution
		// ..........................................................
		cout << "HOST-Info:     Kernels Execution Time (ms) :  "
				<< (double) (PROFILE_STAT.Kernels_End_Time - PROFILE_STAT.Kernels_Start_Time) * 0.000001 //1.0e-6
				<< "  (" << PROFILE[PROFILE_STAT.Kernels_End_Time_Index].name << "\'end - " << PROFILE[PROFILE_STAT.Kernels_Start_Time_Index].name << "\'begin)"
				<< endl;

		cout << "HOST-Info: Application Execution Time (ms) :  "
				<< (double) (PROFILE_STAT.Application_End_Time - PROFILE_STAT.Application_Start_Time) * 0.000001 //1.0e-6
				<< "  (" << PROFILE[PROFILE_STAT.Application_End_Time_Index].name << "\'end - " << PROFILE[PROFILE_STAT.Application_Start_Time_Index].name << "\'begin)"
				<< endl;

		cout << "HOST-Info: " << string(Separation_Line_Width, '-') << endl << endl;
	}

//	double Kernels_EXE_Time;
//	Kernels_EXE_Time = (double) (PROFILE_STAT.Kernels_End_Time - PROFILE_STAT.Kernels_Start_Time) * 0.000001;
//	return(Kernels_EXE_Time);

	double Application_EXE_Time;
	Application_EXE_Time = (double) (PROFILE_STAT.Application_End_Time - PROFILE_STAT.Application_Start_Time) * 0.000001;
	return(Application_EXE_Time);

}

// ============================================================================
// Compare SW and HW Results
// ============================================================================
int compare_results(float* sw_RES, float* hw_RES, int Nb_of_Results, int Nb_Of_Errors_To_Reports) {
	int Nb_Of_Errors = 0;

    for (int i=0; i<Nb_of_Results; i++) {
        if (cmp_floats(sw_RES[i],hw_RES[i]) == 0) {
            Nb_Of_Errors ++;
            if (Nb_Of_Errors <= Nb_Of_Errors_To_Reports) {
                cout << "HOST_ERROR: SW and HW Results do not match: test_nb=" << i << ":  SW (" << setprecision(10) << sw_RES[i] << ")  HW (" << hw_RES[i] << ")" << endl;
            }
        }
    }

    return(Nb_Of_Errors);
}


// ============================================================================
// Compare 2 floating point results
//  =1 - results considered as equal
// ============================================================================
#define CMP_ERROR 0.001f

int cmp_floats(float val1, float val2) {
	float cmp_res = abs((val1 - val2)/max(val1,val2));
	// cout << endl << setprecision (10) << cmp_res << endl;
    if (cmp_res <= CMP_ERROR) return (1);
    else                      return (0);
}


// ============================================================================
// Sore results in a File
// ============================================================================
void store_results(string SW_HW_Mode, string Out_File_Name, t_in_data* host_IN_DATA, float* hw_RES, vector<test_config_t>* Test_Config) {
	fstream out_file;
	vector<string> column_names = {"T","S","K","r","sigma","q","n","BOPM_Result"};
	string Report_Type;
	unsigned nb_of_columns = column_names.size();

	if (SW_HW_Mode == "sw") Report_Type = "SW Model results";
	else Report_Type = "HW results";

	out_file.open(Out_File_Name,ios::out);
    if (!out_file.is_open()) {
    	cout << "HOST_ERROR: Unable to open a file for write: " << Out_File_Name << endl << endl;
    	exit(1);
    }

    out_file << "===================================================" << endl;
    out_file << " Binomial Options Pricing Model: " + Report_Type << endl;
    out_file << "===================================================" << endl;
    out_file << endl;

    int global_indx = 0;
    for (unsigned i=0; i<(*Test_Config).size(); i++) {
    	out_file << fixed << setprecision(3);

    	for(unsigned k=0; k<nb_of_columns*12; k++)
    		out_file << "-";

    	out_file << endl;
    	out_file << "Company: " << (*Test_Config)[i].Company_Name;
    	out_file << " (K_Step=" << (*Test_Config)[i].K_Step;
    	out_file << " #tests="<< (*Test_Config)[i].NB_OF_TESTS << ")" << endl;

    	// -------------------
    	// Print Table Header
    	// -------------------
    	for(unsigned k=0; k<nb_of_columns*12; k++)
    		out_file << "-";
    	out_file << endl;
    	for(unsigned k=0; k<nb_of_columns;    k++)
    		out_file << setw(12) << column_names[k]+ " | ";
    	out_file << endl;
    	for(unsigned k=0; k<nb_of_columns*12; k++)
    		out_file << "-";
    	out_file << endl;

    	// -------------------------------
    	// Print Test Vectors and Results
    	// -------------------------------
    	for (int k=0; k<(*Test_Config)[i].NB_OF_TESTS; k++) {
    		out_file << fixed;
    		out_file <<   setw(9)                    << (*Test_Config)[i].T;
    		out_file <<  setw(12) << setprecision(3) << (*Test_Config)[i].S;
    		out_file <<  setw(12) << setprecision(3) << host_IN_DATA[global_indx].K;
    		out_file <<  setw(12) << setprecision(3) << (*Test_Config)[i].r;
    		out_file <<  setw(12) << setprecision(3) << (*Test_Config)[i].sigma;
    		out_file <<  setw(12) << setprecision(3) << (*Test_Config)[i].q;
    		out_file <<  setw(12)                    << (*Test_Config)[i].n;
    		out_file <<  setw(14) << setprecision(5) << hw_RES[global_indx];
    		out_file << endl;
    		global_indx++;
    	}
    	out_file << endl;
    }
    out_file.close();
}
