/*
 * hw.c
 *
 *  Created on: Jun 3, 2023
 *      Author: hwang
 */


#include "hw.h"

void hwInit(void){
	bspInit();
	cliInit();
	cdcInit();
	uartInit();
	ledInit();
	usbInit();
	adcInit();
}
