/*
    EventOS V1.0 - Copyright (C) 2014 Edielson Prevato Frigieri.

       This file is part of the EventOS distribution.

    EventTOS is free software; you can redistribute it and/or modify it under
    the terms of the MIT License (MIT).
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.


*/

/*-----------------------------------------------------------
 * Implementation of functions defined in port.h for the ARM CM3 port.
 *----------------------------------------------------------*/
#include <stdio.h>

/* Scheduler includes. */
#include "EventOS.h"
#include "event.h"

#include "lpc17xx_rtc.h"

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSPRI2			( ( volatile unsigned long *) 0xe000ed20 )
#define portNVIC_INT_CTRL			( ( volatile unsigned long *) 0xe000ed04 )
#define portNVIC_PENDSVSET			0x10000000
#define portNVIC_PENDSV_PRI			( ( ( unsigned long ) configKERNEL_INTERRUPT_PRIORITY ) << 16 )

/*
 * Exception handlers.
 */


/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
	/* Make PendSV the same priority as the kernel. */
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;

	/* Put the system in sleep mode. */
	vPortEnterSleepMode();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

/**
	Configure

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortEnterSleepMode( void )
{
#ifdef NDEBUG
	/* Enter in sleep on exit mode. */
	SCB->SCR = 0x2;
	LPC_SC->PCON = 0x00;
	__WFI();
#else
	while(1);
#endif
}

/**
	Start RTC

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortStartRtc(void)
{
	/* RTC init module*/
	RTC_Init(LPC_RTC);
	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);
}

/**
	Get rtc date and time

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortGetDateTime(portRTC_TIME_Type* ptagDateTime)
{
	RTC_TIME_Type ptagRtc;

	RTC_GetFullTime (LPC_RTC, &ptagRtc);

	ptagDateTime->DOM = ptagRtc.DOM;
	ptagDateTime->DOW = ptagRtc.DOW;
	ptagDateTime->DOY = ptagRtc.DOY;
	ptagDateTime->HOUR = ptagRtc.HOUR;
	ptagDateTime->MIN = ptagRtc.MIN;
	ptagDateTime->MONTH = ptagRtc.MONTH;
	ptagDateTime->SEC = ptagRtc.SEC;
	ptagDateTime->YEAR = ptagRtc.YEAR;
}
/**
	Set rtc date and time

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortSetDateTime(portRTC_TIME_Type* ptagDateTime)
{
	RTC_TIME_Type ptagRtc;

	ptagRtc.DOM = ptagDateTime->DOM;
	ptagRtc.DOW = ptagDateTime->DOW;
	ptagRtc.DOY = ptagDateTime->DOY;
	ptagRtc.HOUR = ptagDateTime->HOUR;
	ptagRtc.MIN = ptagDateTime->MIN;
	ptagRtc.MONTH = ptagDateTime->MONTH;
	ptagRtc.SEC = ptagDateTime->SEC;
	ptagRtc.YEAR = ptagDateTime->YEAR;

	RTC_SetFullTime (LPC_RTC, &ptagRtc);
}

/**
	Print log in console

    @param pointer log message
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortPrintLog(char* pcMessage)
{
	#ifdef NDEBUG
		/*  */
	#else
		printf(pcMessage);
	#endif
}

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void vPortGenerateEvent( void )
{
	/*Generate an interrupt for treating a published event*/
	*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
}

/**
	Masks all interrupts for execution of a critical block

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortDisableInterrupts(void)
{
	__disable_irq();
}

/**
	Unmasks all interrupts.

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortEnableInterrupts(void)
{
	__enable_irq();
}

void xPortPendSVHandler(void)
{
	vEvent_processEvents();
}
