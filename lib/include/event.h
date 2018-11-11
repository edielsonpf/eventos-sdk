/*
    EventOS V1.2.0 - Copyright (C) 2014 Edielson Prevato Frigieri.

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

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/

#define evtKERNEL_VERSION_NUMBER "V2.0.0"

#define evtKERNEL_VERSION_MAJOR 2

#define evtKERNEL_VERSION_MINOR 0

#define evtKERNEL_VERSION_BUILD 0

/**
 * event.h
 *
 * Type by which event handlers are referenced.  For example, a call to Event_register
 * returns (via a pointer parameter) a EventHandle_t variable that can then be used
 * as a parameter to vEvent_unregister to unregister the event or vEvent_publish to
 * publish a new event.
 *
 */
typedef void* EventHandle_t;


typedef enum enu_Priorities
{
	EVENT_PRIORITY_HIGH = 0,
	EVENT_PRIORITY_MEDIUM,
	EVENT_PRIORITY_LOW,

	/*Must be the last one*/
	EVENT_PRIORITY_LAST
}tenuEventPriority;

/* Defines the prototype to which event handler functions must conform. */
typedef void (*EventHandlerFunction_t)( EventHandle_t, void*, void*, uint32_t );

/*********************************************************
    Public Operations
*********************************************************/

void 			vEvent_startScheduler( void );

BaseType_t 		xEvent_register (void* pvEventOwner,
								 const char* pszEventName,
								 EventHandle_t* pxRegisteredEvent);

BaseType_t 		xEvent_subscribe (void* pvEventSubscriber,
								  EventHandle_t xEventToSubscribe,
								  EventHandlerFunction_t pFunctionCode);

BaseType_t 		xEvent_publish (void* pvEventPublisher,
								EventHandle_t xEventToPublish,
								UBaseType_t ulPriority,
								void* pvPayload,
								uint32_t ulPayloadSize );


/*-----------------------------------------------------------
 * SCHEDULER INTERNALS AVAILABLE FOR PORTING PURPOSES
 * documented in EventOS.h
 *----------------------------------------------------------*/
void 					vEvent_processEvents (void);
void 					vEvent_idleTask (void);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_H_ */
