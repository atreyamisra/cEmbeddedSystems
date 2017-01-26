; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
	PRESERVE8
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

psum 	EQU 	0		; Binding 32-bit unsigned number
n		EQU		4		; Binding 32-bit unsigned number	 	
  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
		PUSH {R0,R4,R5,R7,LR}			; Save registers
		SUB SP, #4						; Allocation (We know the highest number will be 4 bits)
		MOV R5, SP						; R5 holds SP
		
		MOV R4, #0
		MOV R2, #10						; R2 equals 10
		ADD R7, R0, #0
pusher	UDIV R1, R7, R2					; R1=(input/10)
		MUL R3, R1, R2		
		SUB R3, R7, R3					; modulo input/10
		PUSH {R3}						; Push last number onto stack to print first
		ADD R4, R4, #1 					; keeps the count of how many pushes
		STR R4, [R5,#psum]  			; Access
	    ADD R7, R1, #0
		CMP R7, #0						; checks to see if no more numbers to push
		BNE pusher
		
check	POP {R0}						; pops all numbers out in correct order
		LDR R4, [R5,#psum]  			; Access, updates SP by taking into account pops
		SUB R4, R4, #1 
		STR R4, [R5,#psum]				

		ADD R0, R0, #0x30				; allows ASCII decimals
		BL ST7735_OutChar
		
		CMP R4, #0						; checks if push/pop count is zero
		BNE check
		ADD SP, #4						; deallocation
		POP {R0,R4,R5,R7,PC}			; restore registers
		
		BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix

		PUSH {R0-R7,LR}					; Save registers
		SUB SP, #8						; Allocation (Highest number will be 5 bits)
		MOV R5, SP						; R5 holds SP
		
		MOV R6, #10000					; for star case
		CMP R0, R6
		BHS stars
		
		MOV R4, #4						
		STR R4, [R5,#n]					; Access, sets up local variable aka count
		MOV R2, #10						; R2 equals 10
		ADD R7, R0, #0					; R7 holds input 
		

pushOF	UDIV R1, R7, R2					; R1=(input/10)
		MUL R3, R1, R2		
		SUB R3, R7, R3					; modulo input/10
		PUSH {R3}
  		LDR R4, [R5,#n]  				; Access, keeps push count
		SUB R4, R4, #1
		STR R4, [R5,#n]
		ADD R7, R1, #0					; makes the rest of the spots 0s if there are no more numbers to push		
		CMP R4, #0						; checks if push/pop count is zero
		BNE pushOF

		POP {R0}						; prints last number out first
		ADD R0, R0, #0x30				;
		BL ST7735_OutChar				;
		MOV R0, #0x2E					; prints out decimal
		BL ST7735_OutChar				;
		POP {R0}			
		ADD R0, R0, #0x30
		BL ST7735_OutChar
		POP {R0}
		ADD R0, R0, #0x30
		BL ST7735_OutChar
		POP {R0}
		ADD R0, R0, #0x30
		BL ST7735_OutChar
		B ending
		
stars	MOV R0, #0x2A					; prints out star
		BL ST7735_OutChar				;
		MOV R0, #0x2E					; prints out decimal
		BL ST7735_OutChar				;
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		
ending
		ADD SP, #8						; deallocation
		POP {R0-R7,PC} 					; restore registers
 
		BX LR
		ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
