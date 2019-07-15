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

#ifndef __KERNEL_H__
#define __KERNEL_H__

#define CONST_MAX_TREE_HEIGHT 1024
#define CONST_MAX_NB_OF_TESTS 1024

typedef struct {
	int T; float S; float K; float r; float sigma; float q; int n;
	float dummy_val;
} t_in_data;

#endif
