/*
     EventOS V1.1.1 - Copyright (C) 2014 Edielson Prevato Frigieri.

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


#ifndef PORT_H
#define PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure EventOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

#include "lpc17xx_rtc.h"

/* Type definitions. */
#define portCHAR		char
#define portUCHAR		unsigned portCHAR
#define portINTEGER		short int
#define portUINTEGER	unsigned portINTEGER
#define portFLOAT		float
#define portDOUBLE		double
#define portUDOUBLE		unsigned portDOUBLE
#define portLONG		long
#define portULONG		unsigned portLONG
#define portSHORT		short
#define portSTACK_TYPE	unsigned portLONG
#define portBASE_TYPE	long
#define portVOID		void
typedef RTC_TIME_Type    portRtcType;

#define __HEADER_						volatile
#define __PRIVATE_						static

/* Scheduler utilities. */
extern void		vPortGenerateEvent( void );
extern void		vPortEnterSleepMode( void );
extern void     vPortStartRtc(void);
extern void     vPortGetDateTime(portRtcType* ptagRtc);
extern void     vPortSetDateTime(portRtcType* ptagRtc);
extern void     vPortPrintLog(char* pcMessage);
extern void     vPortDisableInterrupts(void);
extern void     vPortEnableInterrupts(void);

#define portGENERATE_EVENT()					vPortGenerateEvent()
#define portSLEEP_MODE()						vPortEnterSleepMode()
#define portSTART_RTC()							vPortStartRtc()
#define portGET_DATE_TIME(pDateTime)			vPortGetDateTime(pDateTime)
#define portSET_DATE_TIME(pDateTime)			vPortSetDateTime(pDateTime)
#define portPRINT_LOG_CONSOLE(pMessage)         vPortPrintLog(pMessage)
#define portDISABLE_INTERRUPTS()				vPortDissableInterrupts()
#define portENABLE_INTERRUPTS()					vPortEnableInterrupts()
/*-----------------------------------------------------------*/

/* Critical section management. */

#ifdef __cplusplus
}
#endif

#endif /* PORT_H */

