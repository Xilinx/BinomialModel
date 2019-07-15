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

void ocl_check_status(cl_int err, string error_msg);

int loadFile2Memory(const char *filename, char **result);

int select_platform (cl_platform_id  *Platform_IDs, cl_platform_id *Target_Platform_ID, const char *Target_Platform_Name);
int select_device   (cl_device_id    *Device_IDs,   cl_device_id   *Target_Device_ID, cl_platform_id Target_Platform_ID, const char *Target_Device_Name);

int create_context(cl_context *Context, cl_device_id Target_Device_ID);
int create_command_queue(cl_context *Context, cl_command_queue *Command_Queue, cl_command_queue_properties Command_Queue_Properties, cl_device_id Target_Device_ID);

int build_program(cl_program *Program, const char *XCLBIN_File_Name, cl_device_id Target_Device_ID, cl_context Context);
int create_kernel(cl_program Program, cl_kernel *Kernel, const char *Kernel_Name);

