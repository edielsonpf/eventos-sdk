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


#ifndef PORTMACRO_H
#define PORTMACRO_H

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

/** @brief Time structure definitions for easy manipulate the data */
typedef struct {
	portULONG SEC; 		/*!< Seconds Register */
	portULONG MIN; 		/*!< Minutes Register */
	portULONG HOUR; 	/*!< Hours Register */
	portULONG DOM;		/*!< Day of Month Register */
	portULONG DOW; 		/*!< Day of Week Register */
	portULONG DOY; 		/*!< Day of Year Register */
	portULONG MONTH; 	/*!< Months Register */
	portULONG YEAR; 	/*!< Years Register */
} portRTC_TIME_Type;

typedef enum enu_Events
{
	/*Kernel events*/
	EVENTOS_EVENT_SLEEP,
	EVENTOS_EVENT_WAKEUP,

	/*
	 * !!! I M P O R T A N T !!!
	 * Kernel events can be only published by the kernel. If you are not
	 * the EventOS class, just use it for subscription!
	 */

	/*Application events*/
	EVENTOS_EVENT_TICK=10,
	EVENTOS_EVENT_USB,
	EVENTOS_EVENT_PACKET,
	EVENTOS_EVENT_ETHERNET,
	EVENTOS_EVENT_ERROR,
	EVENTOS_EVENT_WARNING,
	EVENTOS_EVENT_TEMPERATURE,
	EVENTOS_EVENT_SYSTICK,
	EVENTOS_EVENT_UART,
	EVENTOS_EVENT_LIGHT,

	/*Must be the last one*/
	EVENTOS_EVENT_LAST
}port_EVENT_List;


typedef enum enu_Priorities
{
	EVENTOS_PRIORITY_HIGH,
	EVENTOS_PRIORITY_MEDIUM,
	EVENTOS_PRIORITY_LOW,

	/*Must be the last one*/
	EVENTOS_PRIORITY_LAST
}port_PRIORITY_List;

/* Standard header, every event must start with this header. */
typedef struct tag_Header
{
	port_EVENT_List 		eEvent;
	port_PRIORITY_List		ePriority;

	/*
	 * !!! I M P O R T A N T !!!
	 * This variable is the queue control in the EventOS. If you are not
	 * the EventOS class, don't touch it! This is PRIVETE.
	 */
} port_EVENT_Header;

typedef struct tag_Event
{
	port_EVENT_Header	tagHeader;
	void*				pvPayload;
	portULONG			ulPayloadSize;
} port_EVENT_Type;


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

/* Defines the prototype to which event handler functions must conform. */
typedef void (*pdEventHandlerFunction)(void* hHandle, port_EVENT_Type* ptagEvent);


#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

