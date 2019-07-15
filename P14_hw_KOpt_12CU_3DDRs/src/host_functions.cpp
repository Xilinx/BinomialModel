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
#include <iomanip>
#include <cstring>

using namespace std;

#include <CL/cl.h>

// ========================================================
// This function checks the value of the OpenCL API status
// If the status is not CL_SUCCESS, then the function
//  - generates and error
//  - exits from the program
// ========================================================
void ocl_check_status(cl_int err, string error_msg) {
	if (err != CL_SUCCESS) {
		cout << "Host-Error: " << error_msg << endl << endl;
		exit(0);
	}
}

// =========================================
// Helper Function: Loads program to memory
// =========================================
int loadFile2Memory(const char *filename, char **result) {

    int size = 0;

    std::ifstream stream(filename, std::ifstream::binary);
    if (!stream) {
        return -1;
    }

    stream.seekg(0, stream.end);
    size = stream.tellg();
    stream.seekg(0, stream.beg);

    *result = new char[size + 1];
    stream.read(*result, size);
    if (!stream) {
        return -2;
    }
    stream.close();
    (*result)[size] = 0;
    return size;
}


// ===========================================================================
// Helper Function: Detects specific platform based on the CL_PLATFORM_NAME
//
// Returns:
//  1 - if platform detected, 
// ===========================================================================
int select_platform(cl_platform_id  *Platform_IDs, cl_platform_id *Target_Platform_ID, const char *Target_Platform_Name) {

  cl_uint         ui;

  cl_uint             Nb_Of_Platforms;
  bool                Platform_Detected;
  char                *platform_info;

  cl_int              errCode;
  size_t              size;

  // ------------------------------------------------------------------------------------
  // Get All PLATFORMS, then search for Target_Platform_Name (CL_PLATFORM_NAME)
  // ------------------------------------------------------------------------------------
	
  // Get the number of platforms
  // ..................................................
  errCode = clGetPlatformIDs(0, NULL, &Nb_Of_Platforms);
  if (errCode != CL_SUCCESS || Nb_Of_Platforms <= 0) {
      cout << endl << "HOST-Error: Failed to get the number of available platforms" << endl << endl;
      exit (EXIT_FAILURE);
  }
  // cout << "HOST-Info: Number of detected platforms: " << Nb_Of_Platforms << endl;

  // Allocate memory to store platforms
  // ..................................................
  Platform_IDs = new cl_platform_id[Nb_Of_Platforms];
  if (!Platform_IDs) {
      cout << endl << "HOST-Error: Out of Memory during memory allocation for Platform_IDs" << endl << endl;
      exit (EXIT_FAILURE);
  }

  // Get and store all PLATFORMS
  // ..................................................
  errCode = clGetPlatformIDs(Nb_Of_Platforms, Platform_IDs, NULL);
  if (errCode != CL_SUCCESS) {
      cout << endl << "HOST-Error: Failed to get the available platforms" << endl << endl;
      exit (EXIT_FAILURE);
  }
 
  // Search for a Platform using: CL_PLATFORM_NAME = Target_Platform_Name
  // ..............................................................................
  Platform_Detected = false;
  for (ui = 0; ui < Nb_Of_Platforms; ui++) {

      errCode = clGetPlatformInfo(Platform_IDs[ui], CL_PLATFORM_NAME, 0, NULL, &size);
      if (errCode != CL_SUCCESS) {
          cout << endl << "HOST-Error: Failed to get the size of the Platform parameter " << "CL_PLATFORM_NAME" << " value " << endl << endl;
          exit (EXIT_FAILURE);
      }

      platform_info = new char[size];
      if (!platform_info) {
          cout << endl << "HOST-Error: Out of Memory during memory allocation for Platform Parameter " << "CL_PLATFORM_NAME" << endl << endl;
          exit (EXIT_FAILURE);
      }

      errCode = clGetPlatformInfo(Platform_IDs[ui], CL_PLATFORM_NAME, size, platform_info , NULL);
      if (errCode != CL_SUCCESS) {
          cout << endl << "HOST-Error: Failed to get the " << "CL_PLATFORM_NAME" << " platform info" << endl << endl;
          exit (EXIT_FAILURE);
      }

     // Check if the current platform matches Target_Platform_Name
     // .............................................................
     if (strcmp(platform_info, Target_Platform_Name) == 0) {
          Platform_Detected        = true;
          *Target_Platform_ID      = Platform_IDs[ui];
          break;
      }
  }

  if (Platform_Detected == false) {
      cout << endl << "HOST-Error: Failed to detect the " << Target_Platform_Name << " platform" << endl << endl;
      exit (EXIT_FAILURE);
  }

  cout << "HOST-Info: Selected platform:  " << Target_Platform_Name << endl;
  return 1;

}

// ===========================================================================
// Helper Function: Detects specific device based on the CL_DEVICE_NAME 
//                  in the provided platform Target_Platform_ID
//
// Returns:
//  1 - if platform detected, 
// ===========================================================================

int select_device(cl_device_id *Device_IDs, cl_device_id *Target_Device_ID, cl_platform_id Target_Platform_ID, const char *Target_Device_Name) {

  cl_uint         ui;

  cl_uint             Nb_Of_Devices;
  bool                Device_Detected;
  char                *device_info;

  cl_int              errCode;
  size_t              size;

  // ------------------------------------------------------------------------------------
  // Get All Devices for selected platform Target_Platform_ID
  // then search for the device (CL_DEVICE_NAME = Target_Device_Name)
  // ------------------------------------------------------------------------------------

  // Get the Number of Devices
  // ............................................................................
  errCode = clGetDeviceIDs(Target_Platform_ID, CL_DEVICE_TYPE_ALL, 0, NULL, &Nb_Of_Devices);
  if (errCode != CL_SUCCESS) {
      cout << endl << "HOST-Error:Failed to get the number of available Devices" << endl << endl;
      exit (EXIT_FAILURE);
  }
  // cout << "HOST-Info: Number of available devices:  " << Nb_Of_Devices << endl;

  Device_IDs = new cl_device_id[Nb_Of_Devices];
  if (!Device_IDs) {
      cout << endl << "HOST-Error: Out of Memory during memory allocation for Device_IDs" << endl << endl;
      exit (EXIT_FAILURE);
  }

  errCode = clGetDeviceIDs(Target_Platform_ID, CL_DEVICE_TYPE_ALL, Nb_Of_Devices, Device_IDs, NULL);
  if (errCode != CL_SUCCESS) {
      cout << endl << "HOST-Error: Failed to get available Devices" << endl << endl;
      exit (EXIT_FAILURE);
  }

  Device_Detected = false;
  for (ui = 0; ui < Nb_Of_Devices; ui++) {
      errCode = clGetDeviceInfo(Device_IDs[ui], CL_DEVICE_NAME, 0, NULL, &size);
      if (errCode != CL_SUCCESS) {
          cout << endl << "HOST-Error: Failed to get the size of the Device parameter value " << "CL_DEVICE_NAME" << endl << endl;
          exit (EXIT_FAILURE);
      }
				
      device_info = new char[size];
      if (!device_info) {
          cout << endl << "HOST-Error: Out of Memory during memory allocation for Device parameter " << "CL_DEVICE_NAME" << " value " << endl << endl;
          exit (EXIT_FAILURE);
      }
				
      errCode = clGetDeviceInfo(Device_IDs[ui], CL_DEVICE_NAME, size, device_info, NULL);
      if (errCode != CL_SUCCESS) {
          cout << endl << "HOST-Error: Failed to get the " << "CL_DEVICE_NAME" << " device info" << endl << endl;
          exit (EXIT_FAILURE);
      }

      // Check if the current device matches Target_Device_Name
      // ............................................................................
      if (strcmp(device_info, Target_Device_Name) == 0) {
          Device_Detected        = true;
          *Target_Device_ID       = Device_IDs[ui];
          break;
      }
  }

  if (Device_Detected == false) {
      cout << endl << "HOST-Error: Failed to detect the " << Target_Device_Name << " device" << endl << endl;
      exit (EXIT_FAILURE);
  }

  cout << "HOST-Info: Selected device:    " << Target_Device_Name << endl;
  return 1;
}

// ===========================================================================
// Helper Function: Creates Context and Command Queue
//
// Returns:
//  1 - if context and command queues created 
// ===========================================================================
int create_context(cl_context *Context, cl_device_id Target_Device_ID) {
  cl_int          errCode;

  cout << "HOST-Info: Creating Context ... ";
  *Context = clCreateContext(0, 1, &Target_Device_ID, NULL, NULL, &errCode);
  if (errCode != CL_SUCCESS) {
      cout << endl << "HOST-Error: Failed to create a Context" << endl << endl;
      exit (EXIT_FAILURE);
  }
  cout << "Done" << endl;

  return 1;
}


int create_command_queue(cl_context *Context, cl_command_queue *Command_Queue, cl_command_queue_properties Command_Queue_Properties, cl_device_id Target_Device_ID) {
  cl_int          errCode;

  cout << "HOST-Info: Creating Command_Queue ... ";
  *Command_Queue = clCreateCommandQueue(*Context, Target_Device_ID, Command_Queue_Properties, &errCode);

  if (errCode != CL_SUCCESS) {
      cout << endl << "HOST-Error: Failed to create a Command_Queue" << endl << endl;
      exit (EXIT_FAILURE);
  }
  cout << "Done" << endl;

  return 1;
}

// ===========================================================================
// Helper Function: Create a Build a program from a Binary file
//
// Returns:
//  1 - if program is created
// ===========================================================================

int build_program(cl_program *Program, const char *XCLBIN_File_Name, cl_device_id Target_Device_ID, cl_context Context) {
  cl_int          errCode;

  // ------------------------------------------------------------------
  // Load Binary File from a disk to Memory
  // ------------------------------------------------------------------
  unsigned char *xclbin_Memory;
  size_t Program_Length_in_Bytes;

  cout << "HOST-Info: Reading " << XCLBIN_File_Name << " file ... " << endl;
  Program_Length_in_Bytes = loadFile2Memory(XCLBIN_File_Name, (char **) &xclbin_Memory);
  if (Program_Length_in_Bytes < 0) {
      cout << endl << "HOST-Error: Failed to Read " << XCLBIN_File_Name << " file" << endl << endl;
      exit (EXIT_FAILURE);
  }

  // ------------------------------------------------------------
  // Create a program using a Binary File
  // ------------------------------------------------------------
  cl_int  Binary_Status;

  cout << "HOST-Info: Creating Program from " << XCLBIN_File_Name << " ... " << endl;
  *Program = clCreateProgramWithBinary(Context, 1, &Target_Device_ID, &Program_Length_in_Bytes, (const unsigned char **) &xclbin_Memory, &Binary_Status, &errCode);
  if (errCode != CL_SUCCESS) {
      cout << endl << "HOST-Error: Failed to create a Program" << endl << endl;
      exit (EXIT_FAILURE);
  }


  // -------------------------------------------------------------
  // Build (compiles and links) a program executable from binary
  // -------------------------------------------------------------
  cout << "HOST-Info: Building Program from " << XCLBIN_File_Name << " ... "  << endl;
  errCode = clBuildProgram(*Program, 1, &Target_Device_ID, NULL, NULL, NULL);
  if (errCode != CL_SUCCESS) {
      cout << endl << "HOST-Error: Failed to build a Program" << endl << endl;
      exit (EXIT_FAILURE);
  }
  return 1;
}

// ===========================================================================
// Helper Function: Create a kernel from a program
//
// Returns:
//  1 - if context and command queues created
// ===========================================================================

int create_kernel(cl_program Program, cl_kernel *Kernel, const char *Kernel_Name) {
  cl_int          errCode;

  cout << "HOST-Info: Creating Kernel: " <<  Kernel_Name << " ... " << endl;
  *Kernel = clCreateKernel(Program, Kernel_Name, &errCode);
  if (errCode != CL_SUCCESS) {
      cout << endl << endl << "HOST-Error: Failed to create kernel " << Kernel_Name << endl << endl;
      exit (EXIT_FAILURE);
  }
  return 1;
}

