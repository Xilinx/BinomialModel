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

#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include "kernel.h"
#include "help_functions.h"
#include "cmath"


// ============================================================================================================ //
// ------------------------------------------------------------------------------------------------------------ //
//                                                SW MODEL
// ------------------------------------------------------------------------------------------------------------ //
// ============================================================================================================ //

float sw_calc_p0(int T, float S, float K, float r, float sigma, float q, int n) {
	//    T... expiration time
	//    S... stock price
	//    K... strike price
	//    q... dividend yield
	//    n... height of the binomial tree

	float deltaT, up, p0, p1, exercise;
	float p[CONST_MAX_TREE_HEIGHT];

	deltaT = (float) T / n;
	up = expf(sigma * sqrtf(deltaT));

	p0 = (up*expf(-q * deltaT) - expf(-r * deltaT)) / (powf(up,2) - 1); // up^2
	p1 = expf(-r * deltaT) - p0;

	// initial values at time T
	for (int i = 0; i < n; i++) {
		p[i] = K - S * powf(up,(2*i - n)); // up^(2*i - n)
		if (p[i] < 0) p[i] = 0;
	}

	// move to earlier times
	for (int j = n-1; j > 0; j--) {
		for (int i = 0; i < j; i++) {
			p[i] = p0 * p[i+1] + p1 * p[i];   // binomial value
			exercise = K - S * powf(up,(2*i - j));  // exercise value // up^(2*i - j)
			if (p[i] < exercise) p[i] = exercise;
		}
	}

	return (p[0]);
}


// ============================================================================================================ //
// ------------------------------------------------------------------------------------------------------------ //
//                               SW MODEL - Multi-threading Implementation
// ------------------------------------------------------------------------------------------------------------ //
// ============================================================================================================ //
void K_americanPut_sw_model_task(t_in_data* host_IN_DATA, float* sw_RES, int Nb_Of_Tests, int Start_Index) {

	for (int i = 0; i<Nb_Of_Tests; i++) {
		int indx = Start_Index + i;
		sw_RES[indx] = sw_calc_p0 (host_IN_DATA[indx].T, host_IN_DATA[indx].S, host_IN_DATA[indx].K, host_IN_DATA[indx].r, host_IN_DATA[indx].sigma, host_IN_DATA[indx].q, host_IN_DATA[indx].n);
	}

}

void K_americanPut_sw_model(t_in_data* host_IN_DATA, float* sw_RES, int NB_OF_TESTS, int Nb_Of_Threads) {

	int Nb_of_Test_Vectors_per_Task = NB_OF_TESTS/Nb_Of_Threads;
	thread* t = new thread[Nb_Of_Threads];

	for (int i=0; i<Nb_Of_Threads; i++) {
		t[i] = thread(K_americanPut_sw_model_task, host_IN_DATA, sw_RES, Nb_of_Test_Vectors_per_Task, i*Nb_of_Test_Vectors_per_Task);
	}

	for (int i=0; i<Nb_Of_Threads; i++) {
		t[i].join();
	}

	delete[] t;

}
