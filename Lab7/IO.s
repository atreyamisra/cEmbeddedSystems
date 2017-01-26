; IO.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120/TM4C123

; EE319K lab 7 device driver for the switch and LED.
; You are allowed to use any switch and any LED,
; although the Lab suggests the SW1 switch PF4 and Red LED PF1

; As part of Lab 7, students need to implement these three functions

; negative logic SW2 connected to PF0 on the Launchpad
; red LED connected to PF1 on the Launchpad
; blue LED connected to PF2 on the Launchpad
; green LED connected to PF3 on the Launchpad
; negative logic SW1 connected to PF4 on the Launchpad

        EXPORT   IO_Init
        EXPORT   IO_Touch
        EXPORT   IO_HeartBeat

GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_PORTF_AMSEL_R EQU 0x40025528
GPIO_PORTF_PCTL_R  EQU 0x4002552C
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
PF0       EQU 0x40025004
PF1       EQU 0x40025008
PF2       EQU 0x40025010
PF3       EQU 0x40025020
PF4       EQU 0x40025040
LEDS      EQU 0x40025038
RED       EQU 0x02
BLUE      EQU 0x04
GREEN     EQU 0x08
SWITCHES  EQU 0x40025044
SW1       EQU 0x10                 ; on the left side of the Launchpad board
SW2       EQU 0x01                 ; on the right side of the Launchpad board
SYSCTL_RCGCGPIO_R  EQU 0x400FE608
    
        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB



;------------IO_Init------------
; Initialize GPIO Port for a switch and an LED
; Input: none
; Output: none
; This is a public function
; Invariables: This function must not permanently modify registers R4 to R11
IO_Init
    LDR R1,=SYSCTL_RCGCGPIO_R		; R1 points to SYSCTL_RCGCGPIO_R
	LDR R0, [R1]					; read SYSCTL_RCGCGPIO_R into R0
	ORR R0, R0, #0x20				; turn on clock Bin: 00100000
	STR R0, [R1]	  				; write back to SYSCTL_RCGCGPIO_R
	NOP								; wait for clock to stabilize
	NOP
	LDR R1,=GPIO_PORTF_DIR_R		; R1 points to GPIO_PORTF_DIR_R
	LDR R0, [R1]
	ORR R0, R0, #0x0E				; setting PF3-1 to 1 for output
	BIC R0, #0x10					; set PF4 to 0 for input
	STR R0, [R1]
	LDR R1,=GPIO_PORTF_AFSEL_R		; Deselects alt function of pins
	LDR R0, [R1]
	BIC R0, R0, #0x1E				; Clear out bits 4-1
	STR R0, [R1]
	LDR R1,=GPIO_PORTF_DEN_R		; Enables port pins
	LDR R0, [R1]
	ORR R0, R0, #0x1E				; Set bits 4-1 to 1 (enable them)
	STR R0, [R1] 
	LDR R1,=GPIO_PORTF_PUR_R		; Enable negative logic
	LDR R0, [R1]
	ORR R0, R0, #0x10				; Set bit 4 to 1
	STR R0, [R1]
    
    BX  LR
;* * * * * * * * End of IO_Init * * * * * * * *

;------------IO_HeartBeat------------
; Toggle the output state of the LED.
; Input: none
; Output: none
; This is a public function
; Invariables: This function must not permanently modify registers R4 to R11
IO_HeartBeat
	LDR R1,=GPIO_PORTF_DATA_R		; R1 points to GPIO_PORTF_DATA_R
	LDR R0, [R1]					
	EOR R0, R0, #0x04				; Toggle (NOT) bit 2
	
	STR R0, [R1]
        
    BX  LR                          ; return
;* * * * * * * * End of IO_HeartBeat * * * * * * * *

;------------IO_Touch------------
; First: wait for the release of the switch
; and then: wait for the touch of the switch
; Input: none
; Input: none
; Output: none
; This is a public function
; Invariables: This function must not permanently modify registers R4 to R11
IO_Touch
rel		LDR R1,=GPIO_PORTF_DATA_R	; R1 points to GPIO_PORTF_DATA_R
		LDR R0, [R1]					 
		AND R0, R0, #0x10			; check if 0 to branch back
		MOV R1, #0x10
		CMP R0, R1
		BNE rel
		
tou		LDR R1,=GPIO_PORTF_DATA_R	; R1 points to GPIO_PORTF_DATA_R
		LDR R0, [R1]					 
		AND R0, R0, #0x10			; check if 1 to branch back
		MOV R1, #0x10
		CMP R0, R1
		BEQ tou
	
		BX  LR                      ; return
;* * * * * * * * End of IO_Touch * * * * * * * *

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file