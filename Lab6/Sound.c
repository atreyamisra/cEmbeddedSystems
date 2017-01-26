// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: Vincent Nguyen
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 4-5pm     TA: Jenny
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "tm4c123gh6pm.h"
#include "SysTickInts.h"

void DisableInterrupts(void);	//Disable interrupts
void EnableInterrupts(void);	//Enable interrupts
long StartCritical(void);			//Previous I bit, dissable interrupts
void EndCritical(long sr);		//Restore I bit to previous value
void WaitForInterrupt(void);	//low power mode

const uint32_t SineWave[32] = {
	32,38,44,49,53,58,61,62,63,62,61,58,54,49,44,38,32,26,20,15,10,6,3,2,1,2,3,6,10,15,20,26};

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Called once, with sound initially off
// Input: interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
// Output: none
void Sound_Init(uint32_t period){unsigned long volatile sr;
	
	period = 5000;
	unsigned volatile long delay3;
	
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;		//activate clock for port F
	delay3 = SYSCTL_RCGC2_R;
	sr = StartCritical();
	GPIO_PORTF_DIR_R |= 0x04;								//make PF2 output
	GPIO_PORTF_DEN_R |= 0x04;								//enable digital I/O on PE0
	NVIC_ST_CTRL_R = 0;         						// disable SysTick during setup
	NVIC_ST_RELOAD_R = period-1;						// reload value
	NVIC_ST_CURRENT_R = 0;      						// any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; 
																					// priority 2                            
	NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN;
																					// enable SysTick with core clock and interrupts
	
	EndCritical(sr);
	
}


// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
//         input of zero disable sound output
// Output: none
void Sound_Play(uint32_t period){
			NVIC_ST_RELOAD_R = period-1;				//reload value
			EnableInterrupts();
}


	uint32_t countz = 0;
	
	void SysTick_Handler(void){
			GPIO_PORTF_DATA_R ^= 0x04;	
			countz= countz+1;
			countz = countz%32;
			DAC_Out(SineWave[countz]);
	}
