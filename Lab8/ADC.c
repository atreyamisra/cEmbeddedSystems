// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: Vincent Nguyen
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
int delay;
SYSCTL_RCGCGPIO_R |= 0x10;				// Activate colck for Port E
while((SYSCTL_PRGPIO_R&0x10) == 0){};
GPIO_PORTE_DIR_R &= ~(0x04);			// Make PE2 input
GPIO_PORTE_AFSEL_R |= 0x04;				// Enable alternate function on PE2
GPIO_PORTE_DEN_R &= ~(0x04);			// Disable I/O on PE2
GPIO_PORTE_AMSEL_R |= 0x04;				// Enable analog function on PE2
SYSCTL_RCGCADC_R |= 0x01; 				// Activate ADC0
delay = SYSCTL_RCGCADC_R;					// Extra time to stabilize
delay = SYSCTL_RCGCADC_R;					// Extra time to stabilize
delay = SYSCTL_RCGCADC_R;					// Extra time to stabilize
delay = SYSCTL_RCGCADC_R;					// Extra time to stabilize
ADC0_PC_R = 0x01;									// Configure for 125K, we will sample slower than this
ADC0_SSPRI_R = 0x0123;						// Sequencer 3 is highest priority
ADC0_ACTSS_R &= ~(0x0008);				// Disable sample sequencer 3 (write 0 to bit 3)
ADC0_EMUX_R	&= ~(0xF000);					// Sequence 3 is software trigger (write a 0000 to bits 15-12)
ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0) + 1; 	// Clear SS3 Field
																									// set channel Ain1 (PE2)
ADC0_SSCTL3_R = 0x0006;						// Clear TS0 bit (bit 3) because we are not measuring temperature
																	// Set IE0 bit (bit 2) because we want the RIS bit to be set when sample is complete
																	// Set END0 bit (bit 1) because this is the last and only sample in the sequence
																	// Clear D0 bit (bit 0) because we don't want to use differential mode
ADC0_IM_R	&= ~(0x0008);						// Disable SS3 interrupts, with software start we don't want ADC interrupts so we clear bit 3
ADC0_ACTSS_R |= 0x0008;						// Enable sample sequencer 3		
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){
uint32_t data;
  ADC0_PSSI_R = 0x0008;						// Initiate SS3, ADC is started using the software trigger
	while((ADC0_RIS_R&0x08)==0){};	// Waits for the ADC to complete
	data = ADC0_SSFIFO3_R&0xFFF;		// Read the 12-bit digital sample out of Sequencer 3
	ADC0_ISC_R = 0x0008;						// Acknowledge completion, The RIS bit is cleared by writing to the ISC register
 	return data;
	}		



