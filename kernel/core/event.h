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

#ifndef EVENT_H_
#define EVENT_H_

#include "portable.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/

#define evnKERNEL_VERSION_NUMBER "V1.0.1"

/**
 * event. h
 *
 * Type by which event handlers are referenced.  For example, a call to Event_subscribe
 * returns (via a pointer parameter) an pEventHandle variable that can then
 * be used as a parameter to vEvent_unsubscribe to unsubscribe the handler.
 *
 */
typedef void* pvEventHandle;

typedef enum enu_Events
{
	/*Kernel events*/
	EVENT_KERNEL_SLEEP = 0,
	EVENT_KERNEL_WAKEUP,

	/*
	 * !!! I M P O R T A N T !!!
	 * Kernel events can be only published by the kernel. If you are not
	 * the EventOS class, just use it for subscription!
	 */

	/*System events*/
	EVENT_SYS_TICK = 10,
	EVENT_SYS_USB,
	EVENT_SYS_ETHERNET,
	EVENT_SYS_ERROR,
	EVENT_SYS_WARNING,
	EVENT_SYS_SYSTICK,
	EVENT_SYS_UART,

	/*Application events*/
	EVENT_APP_LIGHT,
	EVENT_APP_TEMPERATURE,

	EVENT_TYPE_UNKNOWN,
	/*Must be the last one*/
	EVENT_TYPE_LAST
}tenuEventType;


typedef enum enu_Priorities
{
	EVENT_PRIORITY_HIGH = 0,
	EVENT_PRIORITY_MEDIUM,
	EVENT_PRIORITY_LOW,

	/*Must be the last one*/
	EVENT_PRIORITY_LAST
}tenuEventPriority;

/* Defines the prototype to which event handler functions must conform. */
typedef void (*pdEVENT_HANDLER_FUNCTION)( unsigned portBASE_TYPE, void*, void*, unsigned portBASE_TYPE );

/*********************************************************
    Public Operations
*********************************************************/

void 					vEvent_startScheduler( void );

signed portBASE_TYPE 	xEvent_register (void* pvEventOwner,
										 portCHAR* pszEventName,
										 unsigned portBASE_TYPE* ulEventKey);

signed portBASE_TYPE 	xEvent_subscribe (pdEVENT_HANDLER_FUNCTION pFunction,
										  unsigned portBASE_TYPE ulEventKey,
										  void* pvSubscriber);

signed portBASE_TYPE 	xEvent_publish (void* pvPublisher,
										unsigned portBASE_TYPE ulEventKey,
										unsigned portBASE_TYPE ulPriority,
										void* pvPayload,
										unsigned portBASE_TYPE ulPayloadSize);

signed portBASE_TYPE  	xEvent_getVersion(portCHAR* szKernelVersion);

/*-----------------------------------------------------------
 * SCHEDULER INTERNALS AVAILABLE FOR PORTING PURPOSES
 * documented in EventOS.h
 *----------------------------------------------------------*/
void 					vEvent_processEvents (void);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_H_ */
