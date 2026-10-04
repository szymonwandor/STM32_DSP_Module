#ifndef FILTER_LP_FO_H
#define	FILTER_LP_FO_H

#define TWO_PI 6.28318530718f

typedef struct {


	// filter output
	float out;

	// sampling frequency
	float fs_Hz;

	// filter coefficients
	float coeff[2];

} LowPass_FirstOrder;


void LowPass_FirstOrder_Init(LowPass_FirstOrder *filt, float fc_Hz, float fs_Hz);

// for modifying cutoff with potentiometers
void LowPass_FirstOrder_SetCutoff(LowPass_FirstOrder *filt, float fc_Hz);

float LowPass_FirstOrder_Update(LowPass_FirstOrder *filt, float inp);



#endif
