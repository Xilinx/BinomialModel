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

#include "kernel.h"
#include "math.h"

// ============================================================================================================ //
// ------------------------------------------------------------------------------------------------------------ //
//                                              HW Implementation
// ------------------------------------------------------------------------------------------------------------ //
// ============================================================================================================ //

float hw_calc_p0_1 (t_in_data in_d) {
    #pragma HLS INLINE off
	#pragma HLS DATA_PACK variable=in_d

    float p[CONST_MAX_TREE_HEIGHT];

    int T; float S; float K; float r; float sigma; float q; int n;
    float deltaT, up, p0, p1, exercise;

    // -------------------------------
    // in_d -> individual variables
    // -------------------------------
    T = in_d.T; S = in_d.S; K = in_d.K; r = in_d.r; sigma = in_d.sigma; q = in_d.q; n = in_d.n;

    // -------------------------------
    // Start Calculation
    // -------------------------------
    deltaT = (float) T / n;
    up = expf(sigma * sqrtf(deltaT));

    p0 = (up*expf(-q * deltaT) - expf(-r * deltaT)) / (powf(up,2) - 1); // up^2
    p1 = expf(-r * deltaT) - p0;

    // -------------------------------
    // initial values at time T
    // -------------------------------
    loop_init: for (int i = 0; i < n; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=100 max=100 avg=100
		#pragma HLS UNROLL factor=2

        p[i] = K - S * powf(up,(2*i - n)); // up^(2*i - n)
        if (p[i] < 0) p[i] = 0;
    }

    // -------------------------------
    // move to earlier times
    // -------------------------------
    loop_j: for (int j = n-1; j > 0; j--) {
        #pragma HLS LOOP_TRIPCOUNT min=100 max=100 avg=100

        loop_i: for (int i = 0; i < j; i++) {
            #pragma HLS LOOP_TRIPCOUNT min=100 max=100 avg=100
			#pragma HLS UNROLL factor=2

            p[i] = p0 * p[i+1] + p1 * p[i];        // binomial value
            exercise = K - S * powf(up,(2*i - j));  // exercise value // up^(2*i - j)
            if (p[i] < exercise) p[i] = exercise;
        }
    }

    return(p[0]);
}


// ================================================================================ //

extern "C" {
void K_americanPut_1(t_in_data* IN_Data, float* Res,
                     int Nb_of_Tests, int Start_Index ) {

    // ---------------------------------------------------------------------------- //
	#pragma HLS INTERFACE s_axilite port=IN_Data        bundle=control
	#pragma HLS INTERFACE s_axilite port=Res            bundle=control
	#pragma HLS INTERFACE s_axilite port=Nb_of_Tests    bundle=control
	#pragma HLS INTERFACE s_axilite port=Start_Index    bundle=control
	#pragma HLS INTERFACE s_axilite port=return         bundle=control

	#pragma HLS INTERFACE m_axi port=IN_Data            offset=slave bundle=gmem_0
	#pragma HLS INTERFACE m_axi port=Res                offset=slave bundle=gmem_1

	#pragma HLS DATA_PACK variable=IN_Data
	// ---------------------------------------------------------------------------- //

    t_in_data  tmp_IN_Data[CONST_MAX_NB_OF_TESTS];
    #pragma HLS DATA_PACK variable=tmp_IN_Data
    #pragma HLS ARRAY_PARTITION variable=tmp_IN_Data cyclic factor=2 dim=1

    float      tmp_Res[CONST_MAX_NB_OF_TESTS];
    #pragma HLS ARRAY_PARTITION variable=tmp_Res     cyclic factor=2 dim=1

    // -------------------------------------
    // Transfer data: Global Memory -> BRAM
    // -------------------------------------
    read_in_data_loop: for (int i = 0; i < Nb_of_Tests; i++)
        #pragma HLS LOOP_TRIPCOUNT min=100 max=100 avg=100
        tmp_IN_Data[i] = IN_Data[Start_Index+i];

    // -------------------------------------
    // Calculate
    // -------------------------------------
    calcualte_i: for (int i = 0; i < Nb_of_Tests/4; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=25 max=25 avg=25

        calcualte_sub_i: for (int sub_i = 0; sub_i < 4; sub_i++) {
            #pragma HLS UNROLL
            tmp_Res[ i*4 + sub_i ] = hw_calc_p0_1(tmp_IN_Data[ i*4 + sub_i ]);
        }
    }

    // -------------------------------------
    // Transfer data: BRAM -> Global Memory
    // -------------------------------------
    write_out_data_loop: for (int i = 0; i < Nb_of_Tests; i++)
        #pragma HLS LOOP_TRIPCOUNT min=100 max=100 avg=100
        Res[Start_Index+i] = tmp_Res[i];

}
}





