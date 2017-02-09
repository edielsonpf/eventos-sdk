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

/** @brief Time structure definitions for easy manipulate the data */
typedef struct {
	uint32_t SEC; 		/*!< Seconds Register */
	uint32_t MIN; 		/*!< Minutes Register */
	uint32_t HOUR; 		/*!< Hours Register */
	uint32_t DOM;		/*!< Day of Month Register */
	uint32_t DOW; 		/*!< Day of Week Register */
	uint32_t DOY; 		/*!< Day of Year Register */
	uint32_t MONTH; 	/*!< Months Register */
	uint32_t YEAR; 		/*!< Years Register */
} portRTC_TIME_Type;

#define __HEADER_						volatile
#define __PRIVATE_						static

/* Scheduler utilities. */
extern void		vPortGenerateEvent( void );
extern void		vPortEnterSleepMode( void );
extern void     vPortStartRtc(void);
extern void     vPortGetDateTime(portRTC_TIME_Type* ptagDateTime);
extern void     vPortSetDateTime(portRTC_TIME_Type* ptagDateTime);
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

