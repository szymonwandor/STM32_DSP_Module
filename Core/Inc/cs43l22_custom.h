/*
 * cs43l22_custom.h
 *
 *  Created on: Sep 28, 2026
 *      Author: szwandor
 */

#ifndef CS43L22_CUSTOM_H
#define CS43L22_CUSTOM_H

#include "stm32f4xx_hal.h"

void CS43L22_Init(void);
void CS43L22_WriteReg(uint8_t reg, uint8_t value);

#endif /* CS43L22_CUSTOM_H */
