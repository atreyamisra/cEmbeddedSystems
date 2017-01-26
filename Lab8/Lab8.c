// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: Vincent Nguyen
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 3/6/2015 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); 			// Disable interrupts
void EnableInterrupts(void);  			// Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
int ADCMail;
int ADCStatus;
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x20;					// activate clock for port F
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_AFSEL_R |= 0x00;					// Enable alternate function on PE2
	GPIO_PORTF_AMSEL_R &= 0x00;					// Enable analog function on PE2
	GPIO_PORTF_DIR_R |= 0x06;						// make PF2 output
	GPIO_PORTF_DEN_R |= 0x06;						// enable digital I/O on PE0
	
	
	

}
uint32_t Data;        								// 12-bit ADC
uint32_t Position;    								// 32-bit fixed-point 0.001 cm
int main1(void){      								// single step this program and look at Data
  TExaS_Init();       								// Bus clock is 80 MHz 
  ADC_Init();         								// turn on ADC, set channel to 1
  while(1){                
    Data = ADC_In();  								// sample 12-bit channel 1
  }
}

int main2(void){
  TExaS_Init();       								// Bus clock is 80 MHz 
  ADC_Init();         								// turn on ADC, set channel to 1
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  while(1){           								// use scope to measure execution time for ADC_In and LCD_OutDec           
    PF2 = 0x04;       								// Profile ADC
    Data = ADC_In();  								// sample 12-bit channel 1
    PF2 = 0x00;       								// end of ADC Profile
    ST7735_SetCursor(0,0);
    PF1 = 0x02;       								// Profile LCD
    LCD_OutDec(Data); 
    ST7735_OutString("    ");  				// these spaces are used to coverup characters from last output
    PF1 = 0;          								// end of LCD Profile
  }
}

uint32_t Convert(uint32_t input){
  if( input < 2.6 ){									// Tried to reduce the shakiness
	return 0;
	}
	
	return ((3*input + 694) / 5);				// Conversion equation
}

int main3(void){ 
  TExaS_Init();         							// Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         								// turn on ADC, set channel to 1
  while(1){  
    PF2 ^= 0x04;      								// Heartbeat
    Data = ADC_In();  								// sample 12-bit channel 1
    PF3 = 0x08;       								// Profile Convert
    Position = Convert(Data); 
    PF3 = 0;          								// end of Convert Profile
    PF1 = 0x02;       								// Profile LCD
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position);
    PF1 = 0;          								// end of LCD Profile
  }
}   

void DisableInterrupts(void); 				// Disable interrupts
void EnableInterrupts(void);  				// Enable interrupts
long StartCritical (void);    				// previous I bit, disable interrupts
void EndCritical(long sr);    				// restore I bit to previous value
void WaitForInterrupt(void);  				// low power mode

// **************SysTick_Init*********************

void SysTick_Init(){long sr;
  sr = StartCritical();
  NVIC_ST_CTRL_R = 0;         					// disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000-1;					// reload value
  NVIC_ST_CURRENT_R = 0;      					// any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
																				// enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN;
  EndCritical(sr);
}
void SysTick_Handler(void){
			GPIO_PORTF_DATA_R ^= 0x04;				// Toggle PF2
			GPIO_PORTF_DATA_R ^= 0x04;				// Toggle PF2
			Data = ADC_In();									// Sample 12-bit ADC
			ADCMail = Data;										// Save ADC sample into mailbox
			ADCStatus = 0x01;									// Signifies new data is available
			GPIO_PORTF_DATA_R ^= 0x04;				// Toggle PF2
	}





int main(void){
TExaS_Init();         									// Bus clock is 80 MHz 
ST7735_InitR(INITR_REDTAB); 
PortF_Init();
ADC_Init();         										// turn on ADC, set channel to 1
SysTick_Init();													// calls systick
	int i;	
  while(1){ 
		if(ADCStatus == 0x01){							// When status is true
		i = ADCMail;												// Read ADC from mailbox
		ADCStatus = 0x00; 									// Clear mailbox flaf to signify it's now empty
		i = Convert(i);											// convert into fixed-point
		ST7735_SetCursor(0,0);
		LCD_OutDec(ADCMail);
		ST7735_OutString("    ");
		LCD_OutFix(i);											// Output fixed-point number
		ST7735_OutString(" cm ");
		ST7735_SetCursor(0,2);
		ST7735_OutString("Follow me on Twitter");
		ST7735_SetCursor(0,3);
		ST7735_OutString(" @JefferiesPassMe");
    	
  }
}	
}

				
