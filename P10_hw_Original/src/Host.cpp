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

/*******************************************************************************
** HOST Code
*******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

#include <CL/cl.h>
#include <CL/cl_ext.h>

#include "help_functions.h"
#include "host_functions.h"
#include "kernel.h"

#define ALL_MESSAGES

void K_americanPut_sw_model(t_in_data* host_IN_DATA, float* sw_RES, int NB_OF_TESTS, int Nb_Of_Threads);

// ********************************************************************************** //
// DEBUG Settings
// ********************************************************************************** //
// #define DEBUG_PRINT_SW_HW_RESULTS
#define DEBUG_NB_OF_RESULTS_TO_PRINT 5

// ********************************************************************************** //
// ---------------------------------------------------------------------------------- //
//                          M A I N    F U N C T I O N                                //
// ---------------------------------------------------------------------------------- //
// ********************************************************************************** //


int main(int argc, char* argv[])
{
	cout << endl;

	// ============================================================================
	// Step: Check Command Line Arguments
	// ============================================================================
	//    o) argv[1] Device Name
	//    o) argv[2] XCLBIN_file
	//    o) argv[3] SW_HW_Mode
    //    o) argv[4] Test_Config_File Name (FULL Version)
    //    o) argv[5] Test_Config_File Name (HW Emu Version)
    //    o) argv[6] SW_HW_Config_File Name
	// ============================================================================
	#ifdef ALL_MESSAGES
	cout << "HOST-Info: ============================================================= " << endl;
	cout << "HOST-Info: Step: Check Command Line Arguments                      " << endl;
	cout << "HOST-Info: ============================================================= " << endl;
	#endif

	if (argc != 7)
	{
		cout << "HOST-Error: Incorrect command line syntax " << endl;
		cout << "HOST-Info:  Usage: " << argv[0] << " <Device> <XCLBIN_File> <SW_HW_Mode> <Test_Config_File_FULL> <Test_Config_File_HW_Emu> <SW_HW_Config_File>" << endl << endl;
		return EXIT_FAILURE;
	} 


	const char*  Target_Platform_Vendor       = "Xilinx";
	const char*  Target_Device_Name           = argv[1];
	const char*  xclbinFilename               = argv[2];
	const string SW_HW_Mode                   = argv[3];
	const char*  Test_Config_File_FULL        = argv[4];
	const char*  Test_Config_File_HW_Emu      = argv[5];
	const char*  SW_HW_Config_File_Name       = argv[6];
	const char*  Print_Custom_Profiling       = "no";

	const char *Test_Config_File_Name;
	Test_Config_File_Name = Test_Config_File_FULL;

	char *emulation_mode = getenv("XCL_EMULATION_MODE");
	if ((emulation_mode != NULL) && (strcmp(emulation_mode,"hw_emu")==0))
		Test_Config_File_Name = Test_Config_File_HW_Emu;

	cout << "HOST-Info: Device                  : " << Target_Device_Name     << endl;
	cout << "HOST-Info: SW_HW_Mode              : " << SW_HW_Mode             << endl;
	cout << "HOST-Info: Test_Config_File_Name   : " << Test_Config_File_Name  << endl;
	cout << "HOST-Info: SW_HW_Config_File_Name  : " << SW_HW_Config_File_Name << endl;

    // ---------------------------------------------------------
    // Check SW_HW_Mode value
    // ---------------------------------------------------------
	if ((SW_HW_Mode!="sw") && (SW_HW_Mode!="hw")) {
		cout << endl << "HOST-Error: SW_HW_Mode option does not support the following value: " << SW_HW_Mode << endl;
		cout <<         "            Supported values are: sw, hw" << endl << endl;
		return EXIT_FAILURE;
	}


    // ---------------------------------------------------------
    // Initialize some fields in Test_Config and then
    // Read Test and SW_HW configuration files
    // ---------------------------------------------------------
    vector<test_config_t>  Test_Config;
    sw_hw_config_t         SW_HW_Config;

    SW_HW_Config.MAX_NB_OF_TESTS = CONST_MAX_NB_OF_TESTS;
    SW_HW_Config.MAX_TREE_HEIGHT = CONST_MAX_TREE_HEIGHT;

    read_sw_hw_config_file(SW_HW_Config_File_Name, &SW_HW_Config);
    print_sw_hw_config_info(SW_HW_Config);

    read_test_config_file (Test_Config_File_Name,  &Test_Config);
    print_test_config_info(&Test_Config);

    // -----------------------------------------------------------
    // Check command line options
    // Calculate ROUNDED_NB_OF_TESTS
    // ------------------------------------------------------------
    int DEFINED_NB_OF_TESTS;
    int ROUNDED_NB_OF_TESTS;
    process_configurations(SW_HW_Mode, &SW_HW_Config, &Test_Config, &DEFINED_NB_OF_TESTS, &ROUNDED_NB_OF_TESTS);


	// =========================================================================
	// Step: Prepare Data in Host Memory
	// =========================================================================
	cout << endl;
	cout << "HOST-Info: ============================================================= " << endl;
	cout << "HOST-Info: Step: Prepare Data to Run Application                         " << endl;
	cout << "HOST-Info: ============================================================= " << endl;

	t_in_data*  host_IN_DATA;
	float*      sw_RES;             // For Results from SW model
	float*      hw_RES;             // For Results from HW Kernel

	// ---------------------------------------------------------------------------------
	// Allocate Memory for host_IN_DATA and initialize it (t_in_data)
	// ---------------------------------------------------------------------------------
	host_IN_DATA = allocate_host_mem<t_in_data>(ROUNDED_NB_OF_TESTS,"host_IN_DATA",true);
    generate_test_vectors(host_IN_DATA, Test_Config, ROUNDED_NB_OF_TESTS);

	// ---------------------------------------------------------------------------------
	// Allocate Memory for sw_RES and hw_RES to store SW and HW results
	// ---------------------------------------------------------------------------------
	sw_RES = allocate_host_mem<float>(ROUNDED_NB_OF_TESTS,"sw_RES",true);
	hw_RES = allocate_host_mem<float>(ROUNDED_NB_OF_TESTS,"hw_RES",true);


	// ============================================================================
	// ============================================================================
	// Step: Run SW Model End Exit
	// ============================================================================
	// ============================================================================

	if (SW_HW_Mode == "sw") {
		cout << endl;
		cout << "HOST-Info: ============================================================= " << endl;
		cout << "HOST-Info: Step: Run SW Model                                         " << endl;
		cout << "HOST-Info: ============================================================= " << endl;

		double tstart, tstop;
		struct timeval t;

		gettimeofday(&t, NULL);
		tstart = 1.0e-6*t.tv_usec + t.tv_sec;

		K_americanPut_sw_model(host_IN_DATA, sw_RES, ROUNDED_NB_OF_TESTS, SW_HW_Config.NB_OF_THREADS);

		gettimeofday(&t, NULL);
		tstop = 1.0e-6*t.tv_usec + t.tv_sec;

		cout << "HOST_Info: SW Model Execution"                                                       << endl;
		cout << "HOST_Info:     # Threads    = " <<  SW_HW_Config.NB_OF_THREADS                       << endl;
		cout << "HOST_Info:     Runtime (ms) = " << fixed << setprecision(1) << (tstop-tstart)*1000.0 << endl << endl;

		// ============================================================================
		// Step: Store results in a file
		// ============================================================================
	    string HW_Out_File_Name = "SW_Res.txt";
	    cout << "HOST-Info: Results stored in the " + HW_Out_File_Name + " file ..." << endl;
	    store_results(SW_HW_Mode, HW_Out_File_Name, host_IN_DATA, hw_RES, &Test_Config);

		cout << endl << "HOST-Info: Application Completed" << endl << endl;
		return EXIT_SUCCESS;
	}

	// ============================================================================
	// ============================================================================
	// Step: Run HW Implementation
	// ============================================================================
	// ============================================================================
	cout << endl;
	cout << "HOST-Info: ============================================================= " << endl;
	cout << "HOST-Info: Step: Generate Reference Data                                 " << endl;
	cout << "HOST-Info: ============================================================= " << endl;

	K_americanPut_sw_model(host_IN_DATA, sw_RES, ROUNDED_NB_OF_TESTS, 1);

	// ============================================================================
	// Step: Detect Target Platform and Target Device in a system.
	//       Create Context and Command Queue.
	// ============================================================================
	cout << endl;
	#ifdef ALL_MESSAGES
	cout << "HOST-Info: ============================================================= " << endl;
	cout << "HOST-Info: Step: Detect Target Platform and Target Device                " << endl;
	cout << "HOST-Info:       Create Context and Command Queue                        " << endl;
	cout << "HOST-Info: ============================================================= " << endl;
	#endif

	cl_int          errCode;

	cl_platform_id      *Platform_IDs, Target_Platform_ID;
	cl_device_id        *Device_IDs,   Target_Device_ID;
	cl_context          Context;
	cl_command_queue    Command_Queue;

	#define Command_Queue_Properties CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE

	Platform_IDs = NULL; Device_IDs = NULL;
	if ( select_platform(Platform_IDs,&Target_Platform_ID, Target_Platform_Vendor) != 1)                  return EXIT_FAILURE;
	if ( select_device(Device_IDs,&Target_Device_ID, Target_Platform_ID, Target_Device_Name) != 1)        return EXIT_FAILURE;
	if ( create_context(&Context, Target_Device_ID) != 1)                                                 return EXIT_FAILURE;
	if ( create_command_queue(&Context, &Command_Queue, Command_Queue_Properties, Target_Device_ID) != 1) return EXIT_FAILURE;

	// ============================================================================
	// Step: Create Program and Kernels and Associated Buffers
	//       for All kernels implemented on on Alveo
	// ============================================================================
	#ifdef ALL_MESSAGES
	cout << endl;
	cout << "HOST-Info: ============================================================= " << endl;
	cout << "HOST-Info: Step: Create Program, Kernels and Associated Buffers          " << endl;
	cout << "HOST-Info: ============================================================= " << endl;
	#endif

	// -------------------------------------------------------------
	// Step: Create Program
	// -------------------------------------------------------------
	cl_program Program;

	if ( build_program(&Program, xclbinFilename, Target_Device_ID, Context) != 1) return EXIT_FAILURE;

	// -------------------------------------------------------------
	// Step: Create Array to store Kernels info
	//       Note: each kernel may have multiple CUs
	// -------------------------------------------------------------
	typedef struct {
			string           name;                  // {"K_americanPut_0", "K_americanPut_1", ... };
			cl_kernel        kernel;

			int              Nb_Of_Test_Vectors;    // Defines the number of test vectors and results, buffers will store
			                                        // This value is setup manually in the code, depending on the Host Code implementation strategy

			t_in_data*       host_IBuf;             // In Buffer in Host Mem associated with a kernel

			cl_mem           GlobMem_IBuf;          // In Buffer in Global Mem associated with a kernel
			cl_mem_ext_ptr_t GlobMem_IBuf_EXT;
			cl_mem           GlobMem_OBuf;          // OUT Buffer in Global Mem associated with a kernel
			cl_mem_ext_ptr_t GlobMem_OBuf_EXT;

			float*           host_OBuf;             // OUT Buffer in Host Mem associated with a kernel
	} t_kernel;

	// ....................................................................
	// Create Kernel related objects for EACH kernel implemented on Alveo
	//   o) Allocate memory to store kernel information
	//   o) Generate Kernel Name and Kernel object
	//   o) Allocate In/Out Host   Memory buffers
	//   o) Allocate In/Out Global Memory buffers
	// ....................................................................
	t_kernel *HW_Kernels = new t_kernel[(SW_HW_Config).NB_OF_KERNELS];

	for (int i=0; i<(SW_HW_Config).NB_OF_KERNELS; i++) {

		// Create Kernel Name
		//............................................................
		HW_Kernels[i].name= "K_americanPut_" + to_string(i);

		// Create Kernel Object
		//............................................................
		if ( create_kernel(Program, &(HW_Kernels[i].kernel), HW_Kernels[i].name.c_str()) != 1)
		    return EXIT_FAILURE;

		// Define number of test vectors/results buffers will store
		//............................................................
		HW_Kernels[i].Nb_Of_Test_Vectors = 1;   // This value is specific for the implementation strategy

		// Allocate In/Out Host buffers
		//............................................................
		HW_Kernels[i].host_IBuf = allocate_host_mem<t_in_data>(HW_Kernels[i].Nb_Of_Test_Vectors,HW_Kernels[i].name+".host_IBuf",true);
		HW_Kernels[i].host_OBuf = allocate_host_mem<float>(HW_Kernels[i].Nb_Of_Test_Vectors,HW_Kernels[i].name+".host_OBuf",true);
	}

	for (int i=0; i<(SW_HW_Config).NB_OF_KERNELS; i++) {

		// GlobMem_IBuf
		// .....................
		cout << "HOST-Info: Allocating Global Memory for " + HW_Kernels[i].name + ".GlobMem_IBuf ..." << endl;
		HW_Kernels[i].GlobMem_IBuf = clCreateBuffer(Context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, HW_Kernels[i].Nb_Of_Test_Vectors * sizeof(t_in_data),  HW_Kernels[i].host_IBuf, &errCode);
		ocl_check_status(errCode,"Failed to allocate Global Memory for " + HW_Kernels[i].name + ".GlobMem_IBuf");

		errCode = clEnqueueMigrateMemObjects(Command_Queue, 1, &(HW_Kernels[i].GlobMem_IBuf), CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, 0, NULL, NULL);
		ocl_check_status(errCode,"Failed to Migrate " + HW_Kernels[i].name + ".GlobMem_IBuf from Host Memory");

		// GlobMem_OBuf
		// .....................
		cout << "HOST-Info: Allocating Global Memory for " + HW_Kernels[i].name + ".GlobMem_OBuf ..." << endl;
		HW_Kernels[i].GlobMem_OBuf = clCreateBuffer(Context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, HW_Kernels[i].Nb_Of_Test_Vectors * sizeof(float), HW_Kernels[i].host_OBuf, &errCode);
		ocl_check_status(errCode,"Failed to allocate Global Memory for " + HW_Kernels[i].name+".GlobMem_OBuf");

		errCode = clEnqueueMigrateMemObjects(Command_Queue, 1, &(HW_Kernels[i].GlobMem_OBuf), CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, 0, NULL, NULL);
		ocl_check_status(errCode,"Failed to Migrate " + HW_Kernels[i].name + ".GlobMem_OBuf from Host Memory");
	}

	// ============================================================================
	// Step: Run Application
	//       This part of the Host is very implementation specific
	// ============================================================================
	cout << endl;
	cout << "HOST-Info: ============================================================= " << endl;
	cout << "HOST-Info: Step: Run Application                                         " << endl;
	cout << "HOST-Info: ============================================================= " << endl;

	// ---------------------------------------------------------
	// Allocate memory to store Memory R/W and Kernel Exe events
	// ---------------------------------------------------------
	int NB_OF_MEM_RD_EVENTS, NB_OF_MEM_WR_EVENTS, NB_OF_EXE_EVENTS;

	NB_OF_MEM_RD_EVENTS = ROUNDED_NB_OF_TESTS;
	NB_OF_MEM_WR_EVENTS = ROUNDED_NB_OF_TESTS;
	NB_OF_EXE_EVENTS    = ROUNDED_NB_OF_TESTS;

	cl_event *Mem_rd_event = new cl_event[NB_OF_MEM_RD_EVENTS];
	cl_event *Mem_wr_event = new cl_event[NB_OF_MEM_WR_EVENTS];
	cl_event *K_exe_event  = new cl_event[NB_OF_EXE_EVENTS];

	// ------------------------
	// Setup Kernel Arguments
	// ------------------------
	for (int i=0; i<(SW_HW_Config).NB_OF_KERNELS; i++) {
		int arg_indx = 0;
		errCode = CL_SUCCESS;
		errCode |= clSetKernelArg(HW_Kernels[i].kernel,  arg_indx++, sizeof(cl_mem),    &(HW_Kernels[i].GlobMem_IBuf));
		errCode |= clSetKernelArg(HW_Kernels[i].kernel,  arg_indx++, sizeof(cl_mem),    &(HW_Kernels[i].GlobMem_OBuf));

		ocl_check_status(errCode,"Unable to setup Kernel Arguments");
	}

	// ------------------------------------------------------------------------------------------------
	// Run Test Vectors
	// ------------------------------------------------------------------------------------------------
	cout << endl << "HOST_Info: Waiting for application to be completed ..." << endl << endl;

	int k_index = 0;
	for (int i=0; i<ROUNDED_NB_OF_TESTS; i++) {

		// .................................................................
		// Copy 1 test vector: host_IN_DATA[i] -> host_IBuf -> GlobMem_IBuf
		// .................................................................
		HW_Kernels[k_index].host_IBuf[0] = host_IN_DATA[i];
	    errCode = clEnqueueMigrateMemObjects(Command_Queue, 1, &(HW_Kernels[k_index].GlobMem_IBuf), 0,
                                               0, NULL, &Mem_wr_event[i]);
	    ocl_check_status(errCode,"Failed to write: " + HW_Kernels[k_index].name + ".Host_IBuf -> " + HW_Kernels[k_index].name + ".GlobMem_IBuf");

		// .................................................................
		// Submit Kernel for execution
		// .................................................................
		size_t globalSize[1]; globalSize[0] = 1;
		size_t localSize[1];  localSize[0]  = 1;

		errCode = clEnqueueNDRangeKernel(Command_Queue, HW_Kernels[k_index].kernel, 1, NULL, globalSize, localSize,
				                                1, &Mem_wr_event[i], &K_exe_event[i]);

	    ocl_check_status(errCode,"Failed to submit kernel for execution: " + HW_Kernels[k_index].name);


		// .................................................................
		// Copy 1 result: GlobMem_OBuf -> host_OBuf -> hw_RES[i]
		// .................................................................
		errCode = clEnqueueMigrateMemObjects(Command_Queue, 1, &(HW_Kernels[k_index].GlobMem_OBuf), CL_MIGRATE_MEM_OBJECT_HOST,
                                                1, &K_exe_event[i], &Mem_rd_event[i]);
	    ocl_check_status(errCode,"Failed to write: " + HW_Kernels[k_index].name + ".GlobMem_OBuf -> " + HW_Kernels[k_index].name + ".Host_OBuf");

		clWaitForEvents(1,&Mem_rd_event[i]);
		hw_RES[i] = HW_Kernels[k_index].host_OBuf[0];
	}

	#ifdef DEBUG_PRINT_SW_HW_RESULTS
		for (int i=0; i<Test_Config.NB_OF_TESTS; i++) {
			cout << "HOST_DEBUG: hw_RES[" << i << "] = " << setprecision(10) << hw_RES[i] << " (Strike Price = " << host_IN_DATA[i].K << ") "<< endl;
			if (i==DEBUG_NB_OF_RESULTS_TO_PRINT) break;
		 }
		cout << endl;
	#endif


	// ============================================================================
	// Step: Check Results
	//       IMPORTANT: We compare only DEFINED_NB_OF_TESTS
	// ============================================================================
	int Nb_Of_Errors = compare_results(sw_RES, hw_RES, DEFINED_NB_OF_TESTS, 5);

	if (Nb_Of_Errors == 0) {
		cout << "HOST_Info: Test Passed" << endl;
	} else {
		cout << "HOST_Info: Test Failed (#Errors=" << Nb_Of_Errors << ")" << endl << endl;
		return EXIT_FAILURE;
	}

	// ============================================================================
	// Step: Store results in a file
	// ============================================================================
    string HW_Out_File_Name = "HW_Res.txt";
    cout << "HOST-Info: Results stored in the " + HW_Out_File_Name + " file ..." << endl << endl;
    store_results(SW_HW_Mode, HW_Out_File_Name, host_IN_DATA, hw_RES, &Test_Config);

	// ============================================================================
	// Step: Custom Profiling
	// ============================================================================
    if (((emulation_mode != NULL) && strcmp(Print_Custom_Profiling,"yes") == 0) || (emulation_mode == NULL)) {

		cout << "HOST-Info: ============================================================= " << endl;
		cout << "HOST-Info: Step: Profiling                                        " << endl;
		cout << "HOST-Info: ============================================================= " << endl;
		int Nb_Of_Kernels = (SW_HW_Config).NB_OF_KERNELS;
		int Nb_Of_Memory_Tranfers = NB_OF_MEM_RD_EVENTS + NB_OF_MEM_WR_EVENTS;

		string TMP_List_OF_Kernel_Names[(SW_HW_Config).NB_OF_KERNELS];
		for (int i=0; i<(SW_HW_Config).NB_OF_KERNELS; i++) TMP_List_OF_Kernel_Names[i] = HW_Kernels[i].name;

		cl_event *Mem_op_event = new cl_event[Nb_Of_Memory_Tranfers];
		for (int i=0; i<NB_OF_MEM_RD_EVENTS; i++) 	Mem_op_event[i]                     = Mem_rd_event[i];
		for (int i=0; i<NB_OF_MEM_WR_EVENTS; i++) 	Mem_op_event[NB_OF_MEM_RD_EVENTS+i] = Mem_wr_event[i];

		double Kernels_EXE_Time = run_custom_profiling (Nb_Of_Kernels,Nb_Of_Memory_Tranfers,K_exe_event,Mem_op_event,TMP_List_OF_Kernel_Names);
		cout << "HOST-Info:     NUMBER_OF_KERNELS      :  " << right << setw(10) << (SW_HW_Config).NB_OF_KERNELS << endl;
		cout << "HOST-Info:     NB_OF_TESTS            :  " << right << setw(10) << DEFINED_NB_OF_TESTS << endl;
		cout << "HOST-Info:     HW Execution Time (ms) :  " << right << setw(10) << fixed << setprecision(1) << Kernels_EXE_Time << endl;
		cout << "HOST-Info: " << string(62, '-') << endl;
    }

	// ============================================================================
	// Step: Release Allocated Resources
	// ============================================================================
	clReleaseDevice(Target_Device_ID);

	for (int event_index=0; event_index<NB_OF_MEM_RD_EVENTS; event_index++)
		clReleaseEvent(Mem_rd_event[event_index]);

	for (int event_index=0; event_index<NB_OF_MEM_WR_EVENTS; event_index++)
		clReleaseEvent(Mem_wr_event[event_index]);

	for (int event_index=0; event_index<NB_OF_EXE_EVENTS; event_index++)
		clReleaseEvent(K_exe_event[event_index]);

	for (int i=0; i<(SW_HW_Config).NB_OF_KERNELS; i++) {
		clReleaseMemObject(HW_Kernels[i].GlobMem_IBuf);
		clReleaseMemObject(HW_Kernels[i].GlobMem_OBuf);
	}

	for (int i=0; i<(SW_HW_Config).NB_OF_KERNELS; i++) {
		clReleaseKernel(HW_Kernels[i].kernel);
	}

	clReleaseProgram(Program);
	clReleaseCommandQueue(Command_Queue);
	clReleaseContext(Context);


	cout << endl << "HOST-Info: Application Completed" << endl << endl;
	return EXIT_SUCCESS;

}

