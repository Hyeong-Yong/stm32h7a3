/*
 * ap.c
 *
 *  Created on: Jun 3, 2023
 *      Author: hwang
 */
#include "ap.h"


#define ADC_BUF_SIZE 20

volatile uint8_t adcFlag = RESET;
volatile uint32_t i;
uint32_t adcBuf[ADC_BUF_SIZE];

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_adc1;



int _write(int file, char *ptr, int len){
	HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, 100);
	return len;
}

void apInit(){
    uartOpen(_DEF_UART1, 115200);
    uartOpen(_DEF_UART2, 115200);

    cliOpen(_DEF_UART2, 115200);
}


void apMain(){
	uint32_t pre_time;
	pre_time = millis();



	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK){
			Error_Handler();
		}

	if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK){
		Error_Handler();
	}

	 if (HAL_ADC_Start(&hadc2) != HAL_OK)
	  {
	    /* Start Error */
	    Error_Handler();
	  }



	if(  HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcBuf, ADC_BUF_SIZE) != HAL_OK) {
		Error_Handler();
	}

	LL_ADC_SetMultiDMATransfer(ADC12_COMMON, LL_ADC_MULTI_REG_DMA_RES_32_10B); // One shot DMA mode of dual ADC
	__HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT); // avoid DMA half transfer interrupt trigger

	printf("\n\rApplication started.\n\r");

	while(1)
	{
//		if (millis() - pre_time>=500){
//	      pre_time = millis();
//          ledToggle(_DEF_LED1);
//		}


		if (adcFlag){
			printf("----------\r");
			for(i=0; i<ADC_BUF_SIZE;i++){
				printf("%u\n", (uint16_t)(adcBuf[i]&0x0000FFFF));
				printf("%u\n", (uint16_t)(adcBuf[i]>>16));

			}

			delay(1000);

			adcFlag= RESET;

			HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcBuf, ADC_BUF_SIZE);
			LL_ADC_SetMultiDMATransfer(ADC12_COMMON, LL_ADC_MULTI_REG_DMA_RES_32_10B);
			__HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT);

		}

		//cliMain();
	}

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle){
	adcFlag = SET;
}
