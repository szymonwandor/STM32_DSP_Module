/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "i2s.h"
#include "usb_device.h"
#include "gpio.h"
#include "cs43l22_custom.h"
#include "DSP/LowPassFilter_FirstOrder.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BUFFER_SIZE 128
#define SAMPLE_RATE_HZ 48000.0f

#define INT16_TO_FLOAT (1.0f / 32768.0f)
#define FLOAT_TO_INT16 (32768.0f)


#define DEBUG_BUFFER_SIZE 1000	/* Debug */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int16_t dacBuffer[BUFFER_SIZE];
int16_t adcBuffer[BUFFER_SIZE];



static volatile int16_t *inBufPtr;
static volatile int16_t *outBufPtr = &dacBuffer[0];

volatile uint8_t dataReadyFlag;
volatile float traceOutput = 0.0f;

LowPass_FirstOrder lpFilt;



// debug
float debugBuffer[DEBUG_BUFFER_SIZE];
volatile int debugIndex = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_I2S_TxHalfCpltCallback (I2S_HandleTypeDef * hi2s){

	inBufPtr = &adcBuffer[0];
	outBufPtr = &dacBuffer[0];

	dataReadyFlag = 1;
}

void HAL_I2S_TxCpltCallback (I2S_HandleTypeDef * hi2s){

	inBufPtr = &adcBuffer[BUFFER_SIZE/2];
	outBufPtr = &dacBuffer[BUFFER_SIZE/2];

	dataReadyFlag = 1;
}




void processData(void) {


    float leftOut, rightOut;
    float leftIn, rightIn;


    static int phase = 0;
    for (int i = 0; i < (BUFFER_SIZE / 2); i += 2) {

        // Sampling and converting to float
        leftIn = inBufPtr[i] * INT16_TO_FLOAT;
        rightIn = inBufPtr[i + 1] * INT16_TO_FLOAT;

        // Phase correction
        //if (leftIn > 1.0f) leftIn -= 2.0f;
        //if (rightIn > 1.0f) rightIn -= 2.0f;


        // test wave generator

        float testTone;

        if(phase < 60){
        	testTone = 0.1f;

        } else{
        	testTone = -0.1f;
        }

        phase++;


        if (phase >= 120){
        	phase = 0;
        }


        traceOutput = LowPass_FirstOrder_Update(&lpFilt, testTone);

        // debug buffer
        if (debugIndex < DEBUG_BUFFER_SIZE) {
            debugBuffer[debugIndex] = traceOutput;
            debugIndex++;
        }

        leftOut = traceOutput;
        rightOut = traceOutput;


        // convert back to in16_t and send to DAC
        outBufPtr[i]     = (int16_t)(leftOut * FLOAT_TO_INT16);
        outBufPtr[i + 1] = (int16_t)(rightOut * FLOAT_TO_INT16);
    }

    dataReadyFlag = 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S3_Init();
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* Initialize Codec */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_Delay(10);
  CS43L22_Init();


  HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)dacBuffer, BUFFER_SIZE);

  /* Initialize Filters */
  LowPass_FirstOrder_Init(&lpFilt, 10000.0f, SAMPLE_RATE_HZ);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (dataReadyFlag == 1) {
	            processData();
	        }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
