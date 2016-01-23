/*
**********************************************************************************************************
     ___   _____    _____   _   _   _   __   _   _____  
    /   | |  _  \  |  _  \ | | | | | | |  \ | | /  _  \ 
   / /| | | |_| |  | | | | | | | | | | |   \| | | | | | 
  / / | | |  _  /  | | | | | | | | | | | |\   | | | | | 
 / /  | | | | \ \  | |_| | | |_| | | | | | \  | | |_| | 
/_/   |_| |_|  \_\ |_____/ \_____/ |_| |_|  \_| \_____/ 

**********************************************************************************************************
*/
#include "avr/io.h"
#include "avr/interrupt.h"
#include "os_cpu_i.h"
#include "ucos_ii.h"
#include <arduino.h>

//extern void ARDUINO_TIMER0_ISR();   //use timer0 as system click and let the arduino delay function remain

/********************************************************************************************************
*                            DISABLE/ENABLE INTERRUPTS USING OS_CRITICAL_METHOD #3
*
* Description : These functions are used to disable and enable interrupts using OS_CRITICAL_METHOD #3.
*
*               OS_CPU_SR  OSCPUSaveSR (void)
*                     Get current value of SREG
*                     Disable interrupts
*                     Return original value of SREG
*
*               void  OSCPURestoreSR (OS_CPU_SR cpu_sr)
*                     Set SREG to cpu_sr
*                     Return
*********************************************************************************************************/
#if OS_CRITICAL_METHOD == 3    
OS_CPU_SR	OS_CPU_SR_Save(void)
{
	__asm__ __volatile__(
	"in	r24,__SREG__	\n\t"
	"cli	\n\t"
//	"ret	\n\t"
	::);

//	OS_CPU_SR tem = SREG;
//	cli();
//	return tem;
}

void	OS_CPU_SR_Restore(OS_CPU_SR  cpu_sr)
{
	__asm__ __volatile__(
	"out	__SREG__,r24	\n\t"
//	"ret	\n\t"
	::);
//	SREG = cpu_sr ;
	
}
#endif

#if OS_CRITICAL_METHOD == 111
OS_CPU_SR	OS_CPU_SR_Save(void)
{
	__asm__ __volatile__ ( "in		__tmp_reg__, __SREG__" :: );
	__asm__ __volatile__ ( "cli" :: );
}

void	OS_CPU_SR_Restore(OS_CPU_SR  cpu_sr)
{
	__asm__ __volatile__ ( "out		__SREG__, __tmp_reg__" :: );
}
#endif
/*$PAGE*/
/********************************************************************************************************
;                               START HIGHEST PRIORITY TASK READY-TO-RUN
;
; Description : This function is called by OSStart() to start the highest priority task that was created
;               by your application before calling OSStart().
;
; Note(s)     : 1) The (data)stack frame is assumed to look as follows:
;                                                +======+
;                  OSTCBHighRdy->OSTCBStkPtr --> |RAMPZ |
;                                                |RAMPY |
;                                                |RAMPX |
;                                                |RAMPD |
;                                                |EIND  |
;                                                |R31   |
;                                                |R30   |
;                                                |R27   |
;                                                |.     |
;                                                |SREG  |
;                                                |R0    |
;                                                |PCH   |-|
;                                                |PCM   | |--> PC address (3 bytes)
;                                                |PCL   |-|                         (High memory)
;                                                +======+
;                  where the stack pointer points to the task start address.
;
;
;               2) OSStartHighRdy() MUST:
;                      a) Call OSTaskSwHook() then,
;                      b) Set OSRunning to TRUE,
;                      c) Switch to the highest priority task.
;********************************************************************************************************/

void	OSStartHighRdy(void)
{
	__asm__ __volatile__(
	"call	OSTaskSwHook	\n\t"		// Invoke user defined context switch hook

	"lds	r16,OSRunning	\n\t"		// Indicate that we are multitasking
	"inc	r16				\n\t"
	"sts	OSRunning,r16	\n\t"

	"lds	r26,OSTCBHighRdy	\n\t"	// Let X point to TCB of highest priority task
	"lds	r27,OSTCBHighRdy+1	\n\t"
	
	POP_SP()				// Load stack  pointer
	POP_ALL()

	//	"reti	\n\t"				// Start task
	:: );
}

/*$PAGE*/
/********************************************************************************************************
;                                       TASK LEVEL CONTEXT SWITCH
;
; Description : This function is called when a task makes a higher priority task ready-to-run.
;
; Note(s)     : (1) (A) Upon entry,
;                       OSTCBCur     points to the OS_TCB of the task to suspend
;                       OSTCBHighRdy points to the OS_TCB of the task to resume
;
;                   (B) The stack frame of the task to suspend looks as follows:
;
;                                            SP+0 --> LSB of task code address
;                                              +2     MSB of task code address            (High memory)
;
;                   (C) The saved context of the task to resume looks as follows:
;
;                                                      +======+
;                        OSTCBHighRdy->OSTCBStkPtr --> |SPL   | ->stack pointer           (Low memory)
;                                                      |SPH   |
;                                                      |RAMPZ |
;                                                      |RAMPY |
;                                                      |RAMPX |
;                                                      |RAMPD |
;                                                      |EIND  |
;                                                      |R31   |
;                                                      |R30   |
;                                                      |R27   |
;                                                      |.     |
;                                                      |SREG  |
;                                                      |R0    |
;                                                      |PCH   |-|
;                                                      |PCM   | |--> PC address (3 bytes)
;                                                      |PCL   |-|                          (High memory)
;                                                      +======+
;                 (2) The OSCtxSW() MUST:
;                     - Save all register in the current task task.
;                     - Make OSTCBCur->OSTCBStkPtr = SP.
;                     - Call user defined task swith hook.
;                     - OSPrioCur                  = OSPrioHighRdy
;                     - OSTCBCur                   = OSTCBHihgRdy
;                     - SP                         = OSTCBHighRdy->OSTCBStrkPtr
;                     - Pop all the register from the new stack
;********************************************************************************************************/

void	OSCtxSw(void)
{
	__asm__ __volatile__(
	PUSH_ALL()                                                     // Save current task's context
	PUSH_SP()
	"in      r26,  __SP_L__              \n\t"                    // X = SP
	"in      r27,  __SP_H__              \n\t"                    
	
	"lds     r28,OSTCBCur               \n\t"                    // Y = OSTCBCur->OSTCBStkPtr
	"lds     r29,OSTCBCur+1             \n\t"                    //
	"st      y+,r26                     \n\t"                    // Y = SP
	"st      y+,r27                     \n\t"                    //
	
	"call    OSTaskSwHook               \n\t"                    // Call user defined task switch hook
	
	"lds     r16,OSPrioHighRdy          \n\t"                    // OSPrioCur = OSPrioHighRdy
	"sts     OSPrioCur,r16				\n\t"					 

	"lds     r26,OSTCBHighRdy           \n\t"                    // Let X point to TCB of highest priority task
	"lds     r27,OSTCBHighRdy+1         \n\t"                    // ready to run
	"sts     OSTCBCur,r26               \n\t"                    // OSTCBCur = OSTCBHighRdy
	"sts     OSTCBCur+1,r27				\n\t"

	POP_SP()                                                     // SP = MEM[X];
	POP_ALL()
	:: );
}

/*********************************************************************************************************
;                                INTERRUPT LEVEL CONTEXT SWITCH
;
; Description : This function is called by OSIntExit() to perform a context switch to a task that has
;               been made ready-to-run by an ISR.
;
; Note(s)     : 1) Upon entry,
;                  OSTCBCur     points to the OS_TCB of the task to suspend
;                  OSTCBHighRdy points to the OS_TCB of the task to resume
;
;               2) The stack frame of the task to suspend looks as follows:
;
;                  OSTCBCur->OSTCBStkPtr ------> SPL of (return) stack pointer           (Low memory)
;                                                SPH of (return) stack pointer
;                                                Flags to load in status register
;                                                RAMPZ
;                                                R31
;                                                R30
;                                                R27
;                                                .
;                                                .
;                                                R0
;                                                PCH
;                                                PCL                                     (High memory)
;
;               3) The saved context of the task to resume looks as follows:
;
;                  OSTCBHighRdy->OSTCBStkPtr --> SPL of (return) stack pointer           (Low memory)
;                                                SPH of (return) stack pointer
;                                                Flags to load in status register
;                                                RAMPZ
;                                                R31
;                                                R30
;                                                R27
;                                                .
;                                                .
;                                                R0
;                                                PCH
;                                                PCL                                     (High memory)
;*********************************************************************************************************/
void	OSIntCtxSw(void)
{
	__asm__ __volatile__(
	"call    OSTaskSwHook             \n\t"                   // Call user defined task switch hook

	"lds     r16,OSPrioHighRdy        \n\t"                   // OSPrioCur = OSPrioHighRdy
	"sts     OSPrioCur,r16            \n\t"                   //

	"lds     r26,OSTCBHighRdy         \n\t"                   // X = OSTCBHighRdy->OSTCBStkPtr
	"lds     r27,OSTCBHighRdy+1       \n\t"                   //
	"sts     OSTCBCur,r26             \n\t"                   // OSTCBCur = OSTCBHighRdy
	"sts     OSTCBCur+1,r27           \n\t"                   //

	POP_SP()												  // SP = MEM[X];
	POP_ALL()                                                 // Restore all registers
	:: );
}


/*
*********************************************************************************************************
*                                       SYSTEM TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*********************************************************************************************************
*/



void  SYSTEM_TickInit(void)
{
//	TCCR0A        = 0x03;                               /* Set TIMER0 prescaler to CTC Mode, CLK/1024  */
//	TCCR0B        = 0x05;                               /* Set CLK/1024 Prescale                       */
//	TCNT0         =    0;                               /* Start TCNT at 0 for a new cycle             */
//	OCR0A         = CPU_CLK_FREQ / OS_TICKS_PER_SEC / 1024 - 1;
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);
	
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
	sbi(TIMSK0, TOIE0);
	//TIFR0        |= 0x02;                               /* Clear  TIMER0 compare Interrupt Flag        */
//	TIMSK0       |= 0x01;                               /* Enable TIMER0 compare Interrupt             */
}

/********************************************************************************************************
*                                           SYSTEM TICK ISR
*
* Description : This function is the ISR used to notify uC/OS-II that a system tick has occurred.
*
* In order to the corresponding
* interrupt vector to be set to point to TIMER0_OVF_vect, one has to also #include <avr/io.h> in this file.
* Note that no error message or warning will be given, if the include is not present, the interupt just
* won't get vectored.
*
;********************************************************************************************************
;                                           SYSTEM TICK ISR
;
; Description : This function is the Tick ISR.
;
;               The following C-like pseudo-code describe the operation being performed in the code below.
;
;               Save all registers on the current task's stack:
;                      Use the PUSH_ALL macro
;                      Get the SREG, set Bit #7 and save onto the task's stack using -Y addressing
;                      Use the PUSH_SP macro to save the task's hardware stack pointer onto the current task's stack
;               OSIntNesting++;
;               if (OSIntNesting == 1) {
	;                  OSTCBCur->OSTCBStkPtr = SP
;               }
;               Clear the interrupt;                  Not needed for the timer we used.
;               OSTimeTick();                         Notify uC/OS-II that a tick has occured
;               OSIntExit();                          Notify uC/OS-II about end of ISR
;               Restore all registers that were save on the current task's stack:
;                      Use the POP_SP macro to restore the task's hardware stack pointer
;                      Use the POP_SREG macro to restore the SREG register
;                      Use the POP_ALL macro to restore the remaining registers
;               Return from interrupt
;********************************************************************************************************

**********************************************************************************************************/
extern int soso;

ISR(TIMER0_OVF_vect, ISR_NAKED)
//void TIMER0_OVF_vect(void) __attribute__ ((signal,__INTR_ATTRS,naked));
//void TIMER0_OVF_vect(void)
{
	TCNT0         =    100; //reload TCNT0
							// Reload timer to overflow at a rate of OS_TICKS_PER_SEC at a prescaler of 1024 and CPU_CLOCK_HZ AVR clock 256 - (CPU_CLK_FREQ / OS_TICKS_PER_SEC / 1024)
	__asm__ __volatile__(
			PUSH_ALL()                                                 //Save all registers and status register
			"LDS     R16,OSIntNesting          \n\t"                 // Notify uC/OS-II of ISR
			"INC     R16                       \n\t"                 //CALL    OSIntEnter
			"STS     OSIntNesting,R16          \n\t"                 //
//			"CALL    OSIntEnter 			   \n\t"				 //
			"CPI     R16,1                     \n\t"                 // if (OSIntNesting == 1) {
			"BRNE    BSP_TickISR_1			   \n\t"				 //

			PUSH_SP()				                                 //  ; X = SP
			"LDS     R28,OSTCBCur              \n\t"                 // OSTCBCur->OSTCBStkPtr = X
			"LDS     R29,OSTCBCur+1            \n\t"                 //

			"ST      Y+,R26					   \n\t"				 //
			"ST      Y+,R27                    \n\t"                 // }

			"BSP_TickISR_1:					   \n\t"				 //
			"CALL    OSTimeTick				   \n\t"                 // Handle the tick ISR

			"CALL    OSIntExit                 \n\t"                 // Notify uC/OS-II about end of ISR

			POP_ALL()                                                 //Restore all registers
			
			"RETI							   \n\t"
	:: );
}

