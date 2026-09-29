/*
 * cs43l22_custom.c
 *
 *  Created on: Sep 28, 2026
 *      Author: szwandor
 */

#include "cs43l22_custom.h"

#define CS43L22_ADDRESS 0x94

extern I2C_HandleTypeDef hi2c1;

void CS43L22_WriteReg(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    HAL_I2C_Master_Transmit(&hi2c1, CS43L22_ADDRESS, data, 2, HAL_MAX_DELAY);
}

void CS43L22_Init(void) {
    // Reset on PD4
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_Delay(10);

    // initialization settings 4.11 from datasheet
    CS43L22_WriteReg(0x00, 0x99);
    CS43L22_WriteReg(0x47, 0x80);
    CS43L22_WriteReg(0x32, 0x80);
    CS43L22_WriteReg(0x32, 0x00);
    CS43L22_WriteReg(0x00, 0x00);

    // configuration of registers
    CS43L22_WriteReg(0x02, 0x01); // Power Ctl 1: power down for the time of config
    CS43L22_WriteReg(0x04, 0xAF); // Power Ctl 2: turn on headphones, turn off speakers
    CS43L22_WriteReg(0x05, 0x81); // Clocking Ctl: MCLK
    CS43L22_WriteReg(0x06, 0x04); // Interface Ctl 1: Format I2S Philips, 16-bit

    // Volume (0x00 is 0dB)
    CS43L22_WriteReg(0x20, 0x00);
    CS43L22_WriteReg(0x21, 0x00);

    // Power Up
    CS43L22_WriteReg(0x02, 0x9E);
}
