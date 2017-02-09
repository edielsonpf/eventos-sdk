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

#ifndef EVENTOS_H_
#define EVENTOS_H_

#include "ICommon.h"
/* Application specific configuration options. */
#include "EventOSConfig.h"
/* Definitions specific to the port being used. */
#include "portmacro.h"

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
}tenu_Events;


typedef enum enu_Priorities
{
	EVENTOS_PRIORITY_HIGH,
	EVENTOS_PRIORITY_MEDIUM,
	EVENTOS_PRIORITY_LOW,

	/*Must be the last one*/
	EVENTOS_PRIORITY_LAST
}tenu_Priorities;

/* Standard header, every event must start with this header. */
typedef struct tag_Header
{
	tenu_Events 			eEvent;
	tenu_Priorities			ePriority;

	/*
	 * !!! I M P O R T A N T !!!
	 * This variable is the queue control in the EventOS. If you are not
	 * the EventOS class, don't touch it! This is PRIVETE.
	 */
} ttag_Header;

typedef struct tag_Event
{
	ttag_Header 		tagHeader;
	void*				pvPayload;
} ttag_Event;

typedef portRtcType  ttag_DateTime;

typedef void (*tcbf_EventHandlerFunction)(Handle hHandle, ttag_Event* ptagEvent);

#define EVENTOS_LIFE_TIME		3

/*********************************************************
    Public Operations
*********************************************************/

void			EventOS_createInstance(void);
void 			EventOS_deleteInstance(void);
void 			EventOS_startScheduler(void);
void 			EventOS_subscribe (tenu_Events eEvent, Handle hHandle, tcbf_EventHandlerFunction pFunction);
void			EventOS_publish (ttag_Event* ptagEvent);
ttag_Event* 	EventOS_newEvent(Int16 iEvent, Int16 iPriority, void* pvData, uInt16 uiDataSize);
void    		EventOS_setDateTime(ttag_DateTime* ptagDateTime);
void    		EventOS_getDateTime(ttag_DateTime* ptagDateTime);
pInt8  			EventOS_getVersion(void);

/*-----------------------------------------------------------
 * SCHEDULER INTERNALS AVAILABLE FOR PORTING PURPOSES
 * documented in EventOS.h
 *----------------------------------------------------------*/
void 			EventOS_processEvents (void);

#endif /* EVENTOS_H_ */
