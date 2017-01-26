// ***** 0. Documentation Section *****
// TrafficLight.c for Lab 5
// Runs on LM4F120/TM4C123
// Implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// Feb 24, 2015

// east/west red light connected to PB2
// east/west yellow light connected to PB1
// east/west green light connected to PB0
// north/south facing red light connected to PB5
// north/south facing yellow light connected to PB4
// north/south facing green light connected to PB3
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

struct State {
	unsigned long Out;
	unsigned long Out2;
	unsigned long Time;
	unsigned long Next[8];};
typedef const struct State STyp;
#define RedAll    0
#define GreenW1   1
#define YellowW1 	2
#define GreenW2  	3
#define YellowW2 	4
#define GreenP  	5
#define RedP1  		6
#define Off1  		7
#define RedP2  		8
#define Off2   		9
#define RedP3 		10
#define Off3 			11
#define GreenS1 	12 
#define YellowS1 	13
#define GreenS2 	14
#define YellowS2	15
#define YellowS3	16
#define RedS		 	17
#define YellowW3  18
#define RedW      19
#define RedS2     20
#define GreenS3   21
#define YellowS4  22
STyp FSM[23]={
	{0x24, 0x02, 200,{RedAll,GreenW1,GreenS1,GreenW1,GreenP,GreenW2,GreenS2,GreenW2}},							//State: 	RedAll 0
	{0x21, 0x02, 200,{YellowW1,GreenW1,YellowW1,YellowW3,YellowW2,YellowW2,YellowW2,YellowW2}}, 		//				GreenW1 1
  {0x22, 0x02, 100,{RedAll,RedAll,RedAll,RedAll,GreenP,GreenP,GreenP,GreenP}},               			//				YellowW1 2
  {0x21, 0x02, 200,{YellowW1,YellowW1,YellowW1,YellowW1,YellowW1,YellowW1,YellowW1,YellowW1}}, 		//				GreenW2 3
  {0x22, 0x02, 100,{GreenP,GreenP,GreenP,GreenP,GreenP,GreenP,GreenP,GreenP}},										//				YellowW2 4
  {0x24, 0x08, 200,{RedP1,RedP1,RedP1,RedP1,RedP1,RedP1,RedP1,RedP1}},													  //				GreenP 5
  {0x24, 0x02, 50,{Off1,Off1,Off1,Off1,Off1,Off1,Off1,Off1}},																			//				RedP1 6
  {0x24, 0x00, 50,{RedP2,RedP2,RedP2,RedP2,RedP2,RedP2,RedP2,RedP2}},															//				Off1 7
  {0x24, 0x02, 50,{Off2,Off2,Off2,Off2,Off2,Off2,Off2,Off2}},																			//				RedP2 8
	{0x24, 0x00, 50,{RedP3,RedP3,RedP3,RedP3,RedP3,RedP3,RedP3,RedP3}},															//				Off2 9
	{0x24, 0x02, 50,{Off3,Off3,Off3,Off3,Off3,Off3,Off3,Off3}},																			//				RedP3 10
	{0x24, 0x00, 50,{RedAll,RedAll,RedS2,RedS2,RedAll,RedAll,RedS2,RedS2}},													//				Off3 11
	{0x0C, 0x02, 200,{YellowS1,YellowS3,GreenS1,YellowS3,YellowS2,YellowS2,YellowS2,YellowS2}},			//				GreenS1 12
	{0x14, 0x02, 100,{RedAll,RedS,RedAll,RedS,GreenP,GreenP,GreenP,GreenP}},												//				YellowS1 13
	{0x0C, 0x02, 200,{YellowS2,YellowS2,YellowS2,YellowS2,YellowS2,YellowS2,YellowS2,YellowS2}},		//				GreenS2 14
	{0x14, 0x02, 100,{GreenP,GreenP,GreenP,GreenP,GreenP,GreenP,GreenP,GreenP}},										//				YellowS2 15
	{0x14, 0x02, 100,{RedS,RedS,RedS,RedS,RedS,RedS,RedS,RedS}},																		//				YellowS3 16
	{0x24, 0x02, 200,{GreenW1,GreenW1,GreenW1,GreenW1,GreenW1,GreenW1,GreenW1,GreenW1}},						//				RedS 17
	{0x22, 0x02, 100,{RedW,RedW,RedW,RedW,RedW,RedW,RedW,RedW}},																		//				YellowW3 18
	{0x24, 0x02, 200,{GreenS1,GreenS1,GreenS1,GreenS1,GreenS1,GreenS1,GreenS1,GreenS1}},						//				RedW 19
	{0x24, 0x02, 200,{GreenS3,GreenS3,GreenS3,GreenS3,GreenS3,GreenS3,GreenS3,GreenS3}},						//				RedS2 20
	{0x0C, 0x02, 200,{YellowS4,YellowS4,YellowS4,YellowS4,YellowS4,YellowS4,YellowS4,YellowS4}},		//				GreenS3 21
	{0x14, 0x02, 100,{RedAll,RedAll,RedAll,RedAll,RedAll,RedAll,RedAll,RedAll}}};										//				YellowS4 22
 
 // ***** 3. Subroutines Section *****
unsigned long S; 																// index to the current state
unsigned long Input;	
int main(void){ volatile unsigned long delay;
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); 	// activate grader and set system clock to 80 MHz 
	SysTick_Init();
	SYSCTL_RCGC2_R |= 0x32;												// B E F
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_AMSEL_R &= ~(0x07); 								// disable analog function on PE0-2
	GPIO_PORTE_DIR_R &= ~(0x07);   								// inputs on PE0-2
	GPIO_PORTE_AFSEL_R &= ~(0x07);								// regular function on PE0-2
	GPIO_PORTE_DEN_R |= 0x07;											// enable digital on PE0-2
	
	GPIO_PORTB_AMSEL_R &= ~(0x3F);								// disable analog funtion on PB0-5
	GPIO_PORTB_DIR_R |= 0x3F;    									// outputs on PB0-5
	GPIO_PORTB_AFSEL_R &= ~(0x3F); 								// regular function on PB0-5
	GPIO_PORTB_DEN_R |= 0x3F;  	  								// enable digital on PB0-5
	
	GPIO_PORTF_AMSEL_R &= ~(0x0A);								// disable analog funtion on PF1 and PF3
	GPIO_PORTF_DIR_R |= 0x0A;    									// outputs on PF1 and PF3
	GPIO_PORTF_AFSEL_R &= ~(0x0A); 								// regular function on PF1 and PF3
	GPIO_PORTF_DEN_R |= 0x0A;  	  								// enable digital on PF1 and PF3
  S = RedAll;
  EnableInterrupts();
  while(1){
		((GPIO_PORTB_DATA_R) = FSM[S].Out);					// set lights												
		((GPIO_PORTF_DATA_R) = FSM[S].Out2);				// set lights
		SysTick_Wait10ms(FSM[S].Time);
		Input = GPIO_PORTE_DATA_R; 									// read sensors (inputs)
		S = FSM[S].Next[Input];
  }
}
