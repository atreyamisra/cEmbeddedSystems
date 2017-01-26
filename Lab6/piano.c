// Piano.c
// This software configures the off-board piano keys
// Runs on LM4F120 or TM4C123
// Program written by: Vincent Nguyen
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 5-6pm	    TA: Jenny
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "tm4c123gh6pm.h"

// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
void Piano_Init(void){unsigned long volatile delay;
	
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;	;	//activate clock for port E
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R	&= ~0x0F;							//make PE3-0 input	
	GPIO_PORTE_DEN_R	|= 0x0F;							//enable data for PE3-0 
	
}


// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2



uint32_t Piano_In(void){
  
	uint32_t Input;
	
 	Input = (GPIO_PORTE_DATA_R & 0x0F);  		//Read Port E Input, bits PE3-0
	
  return Input; 													// replace this line with actual code
}
