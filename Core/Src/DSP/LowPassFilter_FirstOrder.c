/*
 * LowPassFilter_FirstOrder.c
 *
 *  Created on: Oct 2, 2026
 *      Author: szwandor
 */


#include "DSP/LowPassFilter_FirstOrder.h"

void LowPass_FirstOrder_Init(LowPass_FirstOrder *filt, float fc_Hz, float fs_Hz){

	filt ->fs_Hz = fs_Hz;


	LowPass_FirstOrder_SetCutoff(filt, fc_Hz);

	// Reset output
	filt ->out = 0.0f;

}

void LowPass_FirstOrder_SetCutoff(LowPass_FirstOrder *filt, float fc_Hz){


	if(fc_Hz > (0.5f * filt ->fs_Hz) ){
		fc_Hz = filt -> fs_Hz * 0.5f;
	}

	else if (fc_Hz < 0.0f){
		fc_Hz = 0.0f;
	}


	// compute and store filter coefficient
	float alpha  = TWO_PI * fc_Hz/filt->fs_Hz;			/* alpha = 2*pi * fc/fs */

	filt -> coeff[0] = alpha / (1.0f + alpha);	 /* alpha / (1 + alpha) */
	filt -> coeff[1] = 1.0f  / (1.0f + alpha); 	 /* 1 / (1 + alpha) */

}


float LowPass_FirstOrder_Update(LowPass_FirstOrder *filt, float inp){


	//Perform IIR filter update to compute newest output sample
	// Vout[n] = alpha / (1 + alpha) * Vin[n] + 1 / (1 + alpha) * Vout[n-1]
	filt -> out = (filt->coeff[0] * inp) + (filt->coeff[1] * filt->out);


	// clamp output between -1 and 1;
	if (filt->out < -1.0f ){

		filt->out = -1.0f;
	}
	else if(filt->out > 1.0f){
		filt->out = -1.0f;
	}

	return filt -> out;

}


