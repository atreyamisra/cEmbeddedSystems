// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: Vincent Nguyen
// Date Created: 1/24/2015 
// Last Modified: 3/6/2015 
// Section 4-5pm     TA: Jenny
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "SysTickInts.h"
#include "dac.h"

#define NoteA 2841		//880 Hz
#define NoteG 3189		//784 Hz
#define NoteF 3579		//698.5 Hz
#define EF1 	2009		//1244.5
#define BF 		2681		//932.3
#define G 		3189		//784
#define DF1 	2255		//1108.7
#define F 		3579		//698.5
#define C0 		4778		//523.3
#define AF0 	6020		//415.3
#define F0 		7159		//349.2

uint32_t song [30]={EF1,BF,G,EF1,BF,G,EF1,BF,DF1,BF,F,DF1,BF,F,DF1,BF,DF1,BF,F,DF1,BF,F,C0,AF0,F0,C0,AF0,F0,C0,AF0};
uint32_t tempo [30]={25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25};	
int s;
int counter;	
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Piano_Init();
  Sound_Init(0);
	DAC_Init();
  // other initialization
  EnableInterrupts();
	
	uint32_t pitch;
	uint32_t heartbeat;
	uint32_t sec;
	uint32_t time;
	uint32_t note;
	
  while(1){
			int Input = Piano_In();
				if (Input==1) {
					pitch = NoteA;
				}
				else if (Input==2) {
					pitch = NoteG;
				}
				else if (Input==4) {
					pitch = NoteF;
				}
				else if (Input==8) {
						 for(s=0;s<30;s++){
								sec = 1000000000/(note*100);
							  pitch = (sec/12.5);
						 for(counter=0; counter<300;counter++){
								pitch = song[s];
							 
						 }
					 }
				 }		 
				else {
					pitch = 0;
				}
		Sound_Play(pitch);			//jump to subroutine Sound_Play with value (pitch)
	
		for (heartbeat=0; heartbeat < 100000; ++heartbeat) {
			GPIO_PORTF_DATA_R ^= ~(0x04);
			}
  }      
	
	
}	

	




