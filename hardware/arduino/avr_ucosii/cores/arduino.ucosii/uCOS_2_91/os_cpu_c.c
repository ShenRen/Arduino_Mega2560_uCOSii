/*
*********************************************************************************************************
*                                              uC/OS-II
*                                        The Real-Time Kernel
* 
*                                       ATmega128 Specific code for V2.8x
*
* File     : OS_CPU_C.C
* Rebuilt : wookongbun
* Data    :27.10.10
*********************************************************************************************************
*/

#define   OS_CPU_GLOBALS
#include "ucos_ii.h"

/*
*********************************************************************************************************
*                                        LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

#if OS_VERSION >= 281 && OS_TMR_EN > 0
static  INT16U  OSTmrCtr;
#endif

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSInitHookBegin (void)
{
#if OS_VERSION >= 281 && OS_TMR_EN > 0
    OSTmrCtr = 0;
#endif
}
#endif

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSInitHookEnd (void)
{
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 
void  OSTaskCreateHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}
#endif


/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 
void  OSTaskDelHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}
#endif

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do  
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251
void  OSTaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your 
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0 
void  OSTaskStatHook (void)
{
}
#endif

/*$PAGE*/
/*
**********************************************************************************************************
*                                       INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created. This function is highly processor specific.
*
* Arguments  : task          is a pointer to the task code
*
*              p_arg         is a pointer to a user supplied data area that will be passed to the task
*                            when the task first executes.
*
*              ptos          is a pointer to the top of stack. It is assumed that 'ptos' points to the
*                            highest valid address on the stack.
*
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
*                            (see uCOS_II.H for OS_TASK_OPT_???).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : Interrupts are enabled when your task starts executing. You can change this by setting the
*              SREG to 0x00 instead. In this case, interrupts would be disabled upon task startup. The
*              application code would be responsible for enabling interrupts at the beginning of the task
*              code. You will need to modify OSTaskIdle() and OSTaskStat() so that they enable interrupts.
*              Failure to do this will make your system crash!
*
*              The AVR return stack is placed OS_TASK_HARD_STK_SIZE bytes before the bottom of the task's
*              stack.
*
*              (1) IMPORTANT: The ICC compiler handles function pointers by actually passing the pointer
*                             to a location in Flash that actually contains the pointer to the function.
*
*              (2) IMPORTANT: You MUST set OSTaskStkSize and OSTaskStkSizeHard BEFORE calling OSInit()
*                             to initialize the Idle and statistic tasks stacks.
**********************************************************************************************************
*/

OS_STK  *OSTaskStkInit (void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt)
{
    INT32U  tmp;

	(void)opt;                              /* 'opt' is not used, prevent warning                       */
	tmp      = (INT32U)task;
	/* Put task start address on top of "hardware stack"        */
	*ptos--  = (INT8U)(tmp & 0xFF);    		/* Save PC return address                                   */
	tmp    >>= 8;
	*ptos--  = (INT8U)(tmp & 0xFF);
	tmp    >>= 8;
	*ptos--  = (INT8U)(tmp & 0xFF);

	*ptos--  = (INT8U)0x00;            		/* R0    = 0x00                                             */
	*ptos--  = (INT8U)0x80;            		/* SREG  = Interrupts enabled                               */

	*ptos--  = (INT8U)0x00;            		/* R1    = 0x00                                             */
	*ptos--  = (INT8U)0x02;            		/* R2    = 0x02                                             */
	*ptos--  = (INT8U)0x03;            		/* R3    = 0x03                                             */
	*ptos--  = (INT8U)0x04;            		/* R4    = 0x04                                             */
	*ptos--  = (INT8U)0x05;            		/* R5    = 0x05                                             */
	*ptos--  = (INT8U)0x06;            		/* R6    = 0x06                                             */
	*ptos--  = (INT8U)0x07;            		/* R7    = 0x07                                             */
	*ptos--  = (INT8U)0x08;            		/* R8    = 0x08                                             */
	*ptos--  = (INT8U)0x09;            		/* R9    = 0x09                                             */
	*ptos--  = (INT8U)0x10;            		/* R10   = 0x10                                             */
	*ptos--  = (INT8U)0x11;           		/* R11   = 0x11                                             */
	*ptos--  = (INT8U)0x12;            		/* R12   = 0x12                                             */
	*ptos--  = (INT8U)0x13;            		/* R13   = 0x13                                             */
	*ptos--  = (INT8U)0x14;            		/* R14   = 0x14                                             */
	*ptos--  = (INT8U)0x15;            		/* R15   = 0x15                                             */
	*ptos--  = (INT8U)0x16;            		/* R16   = 0x16                                             */
	*ptos--  = (INT8U)0x17;            		/* R17   = 0x17                                             */
	*ptos--  = (INT8U)0x18;            		/* R18   = 0x18                                             */
	*ptos--  = (INT8U)0x19;            		/* R19   = 0x19                                             */
	*ptos--  = (INT8U)0x20;            		/* R20   = 0x20                                             */
	*ptos--  = (INT8U)0x21;            		/* R21   = 0x21                                             */
	*ptos--  = (INT8U)0x22;            		/* R22   = 0x22                                             */
	*ptos--  = (INT8U)0x23;            		/* R23   = 0x23                                             */
	tmp      = (INT16U)pdata;
	*ptos--  = (INT8U)tmp;             		/* 'pdata' passed in R24:R25                                */
	*ptos--  = (INT8U)(tmp >> 8);
	*ptos--  = (INT8U)0x26;            		/* R26 X = 0x26                                             */
	*ptos--  = (INT8U)0x27;            		/* R27   = 0x27                                             */
	*ptos--  = (INT8U)0x28;            		/* R28 Y = 0x28                                             */
	*ptos--  = (INT8U)0x29;            		/* R29   = 0x29                                             */
	*ptos--  = (INT8U)0x30;            		/* R30 Z = 0x30                                             */
	*ptos--  = (INT8U)0x31;            		/* R31   = 0x31                                             */
	*ptos--  = (INT8U)0x00;            		/* EIND  = 0x00                                             */
	*ptos--  = (INT8U)0x00;            		/* RAMPZ = 0x00                                             */

	return ((OS_STK *)ptos);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the 
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/
#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
void  OSTaskSwHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                           OS_TCBInit() HOOK
*
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSTCBInitHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                                           /* Prevent Compiler warning                 */
}
#endif


/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
#if (OS_CPU_HOOKS_EN > 0) && (OS_TIME_TICK_HOOK_EN > 0)
void  OSTimeTickHook (void)
{
  #if OS_VERSION >= 281 && OS_TMR_EN > 0
    OSTmrCtr++;
    if (OSTmrCtr >= (OS_TICKS_PER_SEC / OS_TMR_CFG_TICKS_PER_SEC)) {
        OSTmrCtr = 0;
        OSTmrSignal();
    }
  #endif
}
#endif
