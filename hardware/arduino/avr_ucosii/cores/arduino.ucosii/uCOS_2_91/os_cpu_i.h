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

#ifndef _OS_CPU_A_H_
#define _OS_CPU_A_H_

#include "avr/io.h"

/*
 * Macros for declaring a "naked" ISR: registers are not saved and restored and a ret command is missing.
 *
 */


//__SREG__ 状态寄存器 SREG（0X3F）
//__SP_H__ 堆栈指针高字节（0X3E）
//__SP_L__ 堆栈指针低字节 （0X3D）
//__tmp_reg__ R0
//__zero_reg__ R1,对 C 代码来说其值永远为 0


/* 
 * Macro to save all the general purpose registers, the save the stack pointer
 * into the TCB.  
 * 
 * The first thing we do is save the flags then disable interrupts.  This is to 
 * guard our stack against having a context switch interrupt after we have already 
 * pushed the registers onto the stack - causing the 32 registers to be on the 
 * stack twice. 
 * 
 * r1 is set to zero as the compiler expects it to be thus, however some
 * of the math routines make use of R1. 
 * 
 * The interrupts will have been disabled during the call to PUSH_ALL()
 * so we need not worry about reading/writing to the stack pointer. 
 
 ;*
 ;*********************************************************************************************************
 ;*                               PUSH ALL THE REGISTER INTO THE STACK
 ;*
 ;* Description : Save all the register into the stack.
 ;*                           +-------+
 ;*             [RO..RAMPZ] ->|  RO   |
 ;*                           |  SREG |
 ;*                           |  R1   |
 ;*                           |  R2   |
 ;*                           |   .   |
 ;*                           |   .   |
 ;*                           | RAMPZ |
 ;*                           +-------+
 ;*********************************************************************************************************
 ;*
 
 */

#define PUSH_ALL()	\
					"push	r0						\n\t"	\
					"in		r0, __SREG__			\n\t"	\
					"cli							\n\t"	\
					"push	r0						\n\t"	\
					"push	r1						\n\t"	\
					"clr	r1						\n\t"	\
					"push	r2						\n\t"	\
					"push	r3						\n\t"	\
					"push	r4						\n\t"	\
					"push	r5						\n\t"	\
					"push	r6						\n\t"	\
					"push	r7						\n\t"	\
					"push	r8						\n\t"	\
					"push	r9						\n\t"	\
					"push	r10						\n\t"	\
					"push	r11						\n\t"	\
					"push	r12						\n\t"	\
					"push	r13						\n\t"	\
					"push	r14						\n\t"	\
					"push	r15						\n\t"	\
					"push	r16						\n\t"	\
					"push	r17						\n\t"	\
					"push	r18						\n\t"	\
					"push	r19						\n\t"	\
					"push	r20						\n\t"	\
					"push	r21						\n\t"	\
					"push	r22						\n\t"	\
					"push	r23						\n\t"	\
					"push	r24						\n\t"	\
					"push	r25						\n\t"	\
					"push	r26						\n\t"	\
					"push	r27						\n\t"	\
					"push	r28						\n\t"	\
					"push	r29						\n\t"	\
					"push	r30						\n\t"	\
					"push	r31						\n\t"	\
					"in     r16, 0x3c               \n\t"	\
					"push   r16						\n\t"	\
					"in     r16, 0x3b				\n\t"	\
					"push   r16						\n\t"	


/* 
 * Opposite to POP_ALL().  Interrupts will have been disabled during
 * the context save so we can write to the stack pointer. 
 ;*
 ;*********************************************************************************************************
 ;*                               POP ALL THE REGISTER OUT THE STACK
 ;*
 ;* Description : Restore all the register form the stack
 ;*                +-------+
 ;*                |  RO   | ---> [RO..RAMPZ]
 ;*                |  SREG |
 ;*                |  R1   |
 ;*                |  R2   |
 ;*                |   .   |
 ;*                |   .   |
 ;*                | RAMPZ |
 ;*                +-------+
 ;*********************************************************************************************************
 ;*
 */


#define POP_ALL()	\
					"pop    r16                     \n\t"	\
					"out    0x3b, r16				\n\t"	\
					"pop    r16						\n\t"	\
					"out    0x3c, r16				\n\t"	\
					"pop	r31						\n\t"	\
					"pop	r30						\n\t"	\
					"pop	r29						\n\t"	\
					"pop	r28						\n\t"	\
					"pop	r27						\n\t"	\
					"pop	r26						\n\t"	\
					"pop	r25						\n\t"	\
					"pop	r24						\n\t"	\
					"pop	r23						\n\t"	\
					"pop	r22						\n\t"	\
					"pop	r21						\n\t"	\
					"pop	r20						\n\t"	\
					"pop	r19						\n\t"	\
					"pop	r18						\n\t"	\
					"pop	r17						\n\t"	\
					"pop	r16						\n\t"	\
					"pop	r15						\n\t"	\
					"pop	r14						\n\t"	\
					"pop	r13						\n\t"	\
					"pop	r12						\n\t"	\
					"pop	r11						\n\t"	\
					"pop	r10						\n\t"	\
					"pop	r9						\n\t"	\
					"pop	r8						\n\t"	\
					"pop	r7						\n\t"	\
					"pop	r6						\n\t"	\
					"pop	r5						\n\t"	\
					"pop	r4						\n\t"	\
					"pop	r3						\n\t"	\
					"pop	r2						\n\t"	\
					"pop	r1						\n\t"	\
					"pop	r0						\n\t"	\
					"out	__SREG__, r0			\n\t"	\
					"pop	r0						\n\t"

/*----;*
;*********************************************************************************************************
;*                               PUSH THE STACK POINTER INTO THE STACK
;*
;* Description : Save the stack pointer
;*
;* Note(s)     : R28 and R29 are equal to the current stak pointer [R28-R29] = SP
;*               after the macro exit.
;*
;*********************************************************************************************************
;*
-------*/


		

#define	PUSH_SP() \
        "in     r26,  __SP_L__          \n\t"	\
        "in     r27,  __SP_H__			\n\t"

/*
		"in		r0, __SP_L__			\n\t"	\
		"st		x+, r0					\n\t"	\
		"in		r0, __SP_H__			\n\t"	\
		"st		x+, r0					\n\t"
*/

/*
;*
;*********************************************************************************************************
;*                               RESTORE THE STACK POINTER
;*
;* Description : Restore the Stack Pointer
;*
;* Note(s)     : X register points to the memory location where the SP is contained.
;*
;*********************************************************************************************************
;*
*/

#define POP_SP() \
		"ld		r28, x+					\n\t"	\
		"out	__SP_L__, r28			\n\t"	\
		"ld		r29, x+					\n\t"	\
		"out	__SP_H__, r29			\n\t"	


#endif