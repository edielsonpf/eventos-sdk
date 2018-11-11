/*
    EventOS V1.2.0
    Copyright (C) 2014 Edielson Prevato Frigieri. All Rights Reserved.

       This file is part of the EventOS distribution.

    EventOS is free software; you can redistribute it and/or modify it under
    the terms of the MIT License (MIT).
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "EventOS.h"
#include "event.h"
#include "list.h"
/*********************************************************
    private constants.
*********************************************************/

/*********************************************************
    private types.
*********************************************************/

/*
 * Event control block.  A event control block (ECB) is allocated to each event,
 * and stores the parameters of the event.
 */
typedef struct evtEventControlBlock
{
	xListNode_t				xEventListNode;						/*< List node used to place the event in the list. */
	UBaseType_t				uxEventPriority;					/*< The priority of the event where 0 is the lowest priority. */
	EventHandle_t			xEventHandle;

	xList_t*				pxSubscriberList;					/*< Pointer to the list of event subscribers>*/

	void*					pvEventContent;
	uint32_t				ulContentSize;
} evtECB;

/*
 * Event handler control block.  A event handler control block (HCB) is allocated to each event subscriber,
 * and stores the parameters of the event handler.
 */
typedef struct evtSubscriber
{
	void* 						pvSubscriberHandle;
	EventHandle_t				xEventHandle;
	EventHandlerFunction_t 		pvEventHandlerFunction;

	xListNode_t					xSubscriberListNode;						/*< List node used to place the event in the list. */
}evtSCB;


/*
 * Event handler control block.  A event handler control block (HCB) is allocated to each event subscriber,
 * and stores the parameters of the event handler.
 */
typedef struct evtEventAdmin
{
	void* 					pvEventOwner;
	char					szEventName[configMAX_EVENT_NAME_LEN];
	BaseType_t				bInUse;

	xList_t					xSubscriberList;						/*< List of subscribers for a specific event. */
	UBaseType_t			 	ulCurrentNumberOfSubscribers;

}evtEventAdmin_t;

/* Lists for events and event handlers. --------------------*/
__PRIVATE_ xList_t pxEventsLists[ EVENT_PRIORITY_LAST ];	/*< Prioritized events. */
__PRIVATE_ evtEventAdmin_t pxEventAdminList[ configMAX_NUM_EVENTS ];	/*< Event administration . */


/*********************************************************
    private attributes.
*********************************************************/
__PRIVATE_ evtECB* volatile pxCurrentECB = NULL;

/*********************************************************
    private variables.
*********************************************************/
__PRIVATE_ volatile UBaseType_t evt_ulCurrentNumberOfPubEvents 	= ( UBaseType_t ) 0;
__PRIVATE_ volatile UBaseType_t evt_ulCurrentNumberOfRegEvents 	= ( UBaseType_t ) 0;
__PRIVATE_ volatile TickType_t evt_ulProcessStamp = ( TickType_t ) 0;


/*********************************************************
    private operations.
*********************************************************/

/*
 * Check if a event key is valid in the system.
 */
#define prvEvent_checkEventKey( uxNewEventKey ) ( pxEventAdminList[uxNewEventKey].bInUse ) \

/*
 * Check if a event key is valid and belongs to the respective publisher.
 */
#define prvEvent_validateEventPublisher( pvPublisher, xEventToPublish ) ( ( ( (evtEventAdmin_t*) xEventToPublish )->bInUse ) && ( ( ( (evtEventAdmin_t*) xEventToPublish )->pvEventOwner ) == pvPublisher) )

/*
 * Place the subscriber represented by pxSCB into the appropriate event queue for.
 * It is inserted at the head of the list.
 */
#define prvEvent_addSubscriberToList( pxSCB, xEventToSubscribe ) \
{\
	vList_insertHead( ( xList_t* ) &( ( (evtEventAdmin_t*) xEventToSubscribe )->xSubscriberList), &( pxSCB->xSubscriberListNode ) ); \
	( (evtEventAdmin_t*) xEventToSubscribe )->ulCurrentNumberOfSubscribers++; \
}\

/*
 * Place the event represented by pxECB into the appropriate event priority queue.
 * It is inserted at the right position in the list, considering the deadline set on ulNodeValue.
 */
#define prvEvent_addEventToList( pxECB ) \
{ \
	vList_insert( ( xList_t* ) &( pxEventsLists[ pxECB->uxEventPriority ] ), &( pxECB->xEventListNode ) );	\
	evt_ulCurrentNumberOfPubEvents++; \
}\


/*
 * Place the event represented by pxECB into the appropriate event priority queue.
 * It is inserted at the right position in the list, considering the deadline set on ulNodeValue.
 */
#define prvEvent_removeEventFromList( pxECB ) \
{ \
	vList_remove( &( pxECB->xEventListNode ) );	\
	evt_ulCurrentNumberOfPubEvents--; \
}\


__PRIVATE_ void prvEvent_deleteECB( evtECB* pxECB );
__PRIVATE_ void prvEvent_initializeEventLists( void );
__PRIVATE_ void prvEvent_initializeEventAdmin( void );
__PRIVATE_ void prvEvent_initializeSCBVariables( evtSCB* pxSCB,
												 EventHandlerFunction_t pFunction,
												 EventHandle_t xEventHandle,
												 void* pvSubscriber);
__PRIVATE_ void prvEvent_initializeECBVariables( evtECB* pxECB,
												 EventHandle_t xEventHandle,
												 UBaseType_t uxEventPriority);
__PRIVATE_ void prvEvent_incrementProcessStamp( void );
__PRIVATE_ TickType_t prxEvent_getProcessStamp( void );
__PRIVATE_ void prvEvent_updateLifeTime (void);
__PRIVATE_ evtECB* prvEvent_getNextEvent(void);
__PRIVATE_ void prvEvent_increaseEventPriority(evtECB* pxECB);
__PRIVATE_ void prvEvent_terminateEvent( evtECB* pxECB );
__PRIVATE_ void prvEvent_deleteECB( evtECB* pxECB );
__PRIVATE_ BaseType_t prxEvent_generateEventKey(void* pvEventOwner,
										  	  	const char* szEventName);
__PRIVATE_ BaseType_t  prxEvent_validateEventHandle(EventHandle_t xEvent);
/*********************************************************
    Operations implementation
*********************************************************/

/**
	Event start scheduler. This is the method that starts the scheduler process.

    @param void
    @return void
    @author edielson
    @date   05/09/2014
*/
void vEvent_startScheduler( void )
{
	/* Setting up the system for sleep mode in the specific. */
	if( xPortStartScheduler() )
	{
		/* Should not reach here as if the scheduler is running the
		function will not return. */
	}
	else
	{
		/* Error */
	}
}

/*
	Event register. Function to register new events to the system.

    @param pvEventOwner: Event owner handler object
    	   pszEventName: string with event name identification
    	   pxRegisteredEvent: event handler object

    @return BaseType_t

*/
BaseType_t 		xEvent_register (void* pvEventOwner,
								 const char* szEventName,
								 EventHandle_t* pxRegisteredEvent)
{
	if(szEventName == NULL) return pdFAIL;
	if(pvEventOwner == NULL) return pdFAIL;
	if(strlen(szEventName) >= configMAX_EVENT_NAME_LEN) return pdFAIL;
	if(strlen(szEventName) < 2) return pdFAIL; /* event name should have length greater then 1 */

	signed portBASE_TYPE xReturn = pdTRUE;
	UBaseType_t ulNewKey = 0;
	portBASE_TYPE x = 0;

	portDISABLE_INTERRUPTS();
	{
		evt_ulCurrentNumberOfRegEvents++;

		if(evt_ulCurrentNumberOfRegEvents == (portBASE_TYPE) 1)
		{
			/* This is the first event to be registered so do the preliminary
			   required initialization. We will not recover if this call
			   fails, but we will report the failure.
			 */
			prvEvent_initializeEventAdmin();

		}

		/**
		 * This method generates a new event key based on
		 * the event name. For future implementation,
		 * the pvOwner can be used to make the key
		 * more specific or even create any security key.
		 */
		ulNewKey = prxEvent_generateEventKey(pvEventOwner, szEventName);

		/*
		 * We have to check if the event administration is already in use
		 * for this specific key. This is important to keep the key
		 * unique in the system.
		 * */
		if(prvEvent_checkEventKey(ulNewKey) == pdFALSE)
		{
			pxEventAdminList[ulNewKey].bInUse = pdTRUE;

			/* Store the task name in the TCB. */
			for( x = ( portBASE_TYPE ) 0; x < ( portBASE_TYPE ) configMAX_EVENT_NAME_LEN; x++ )
			{
				pxEventAdminList[ulNewKey].szEventName[x] = szEventName[x];

				/* Don't copy all configMAX_EVENT_NAME_LEN if the string is shorter than
				configMAX_EVENT_NAME_LEN characters just in case the memory after the
				string is not accessible (extremely unlikely). */

				if( szEventName[ x ] == 0x00 )
				{
					break;
				}
			}

			/* Ensure the name string is terminated in the case that the string length
			was greater or equal to configMAX_EVENT_NAME_LEN. */

			pxEventAdminList[ulNewKey].szEventName[ configMAX_EVENT_NAME_LEN - 1 ] = '\0';
			pxEventAdminList[ulNewKey].pvEventOwner = pvEventOwner;
			pxEventAdminList[ulNewKey].ulCurrentNumberOfSubscribers = 0;

			/*
			 * return the new event handle to the user.
			 */
			if(( void * ) pxRegisteredEvent != NULL)
			{
				*pxRegisteredEvent = (EventHandle_t)&(pxEventAdminList[ulNewKey]);
			}
		}
		else
		{
			/* For now, we consider just one event admin per key.
			 * Further version can use a linked list of events
			 * per key. */
			if(( void * ) pxRegisteredEvent != NULL)
			{
				*pxRegisteredEvent = NULL;
			}

			xReturn = pdFAIL;
		}
	}
	portENABLE_INTERRUPTS();

	return xReturn;
}

/*
	Event subscribe. Function subscribes a event handler to receive notifications
	about as specific event

    @param  pvSubscriber: subscriber handler object
    		xEventToSubscribe: event handle to be subscribed
    		pvFunction: function to be called for event notification

    @return BaseType_t

*/
BaseType_t 		xEvent_subscribe (void* pvEventSubscriber,
								  EventHandle_t xEventToSubscribe,
								  EventHandlerFunction_t pFunctionCode)
{
	if (pFunctionCode == NULL) 		return pdFAIL;
	if (pvEventSubscriber == NULL) 	return pdFAIL;
	if (evt_ulCurrentNumberOfRegEvents == 0) return pdFAIL;

	signed portBASE_TYPE xReturn = pdTRUE;

	/*
	 * Check if the event key is already registered in the system
	 */
	if( prxEvent_validateEventHandle(xEventToSubscribe) == pdTRUE )
	{
		evtSCB* pxNewSubscriberBlock = (evtSCB*)pvPortMalloc(sizeof(evtSCB));

		if(pxNewSubscriberBlock)
		{
			/*Initializing variables*/
			prvEvent_initializeSCBVariables(pxNewSubscriberBlock,
											pFunctionCode,
											xEventToSubscribe,
											pvEventSubscriber);

			portDISABLE_INTERRUPTS();
			{
				prvEvent_addSubscriberToList( pxNewSubscriberBlock, xEventToSubscribe );

				xReturn = pdPASS;
			}
			portENABLE_INTERRUPTS();
		}
	}

	return xReturn;
}

/*
	EventOS publish. Publishing events in a particular queue according to its priority

    @param  pvEventPublisher: publisher handler object
    		xEventToPublish: event handle to be published
    		uxPriority: priority to be published
    		pvEventContent: pointer to event content
    		ulContentSize: content size

	@return BaseType_t
*/
BaseType_t 		xEvent_publish (void* pvEventPublisher,
								EventHandle_t xEventToPublish,
								UBaseType_t uxPriority,
								void* pvEventContent,
								uint32_t ulContentSize )
{
	if (uxPriority >= EVENT_PRIORITY_LAST) return pdFAIL;
	if (evt_ulCurrentNumberOfRegEvents == 0) return pdFAIL;
	if ((void*) xEventToPublish == NULL) return pdFAIL;

	BaseType_t xStatus = pdPASS;
	evtECB* pxNewEventBlock = NULL;

	if( prvEvent_validateEventPublisher(pvEventPublisher, xEventToPublish) == pdTRUE)
	{
		pxNewEventBlock = (evtECB*)pvPortMalloc(sizeof(evtECB));
		if(pxNewEventBlock)
		{
			prvEvent_initializeECBVariables(pxNewEventBlock,
											xEventToPublish,
											uxPriority);

			/* Initialize the content container in the event block */
			if(ulContentSize > 0)
			{
				/* Check if content pointer is not NULL*/
				if(pvEventContent)
				{
					pxNewEventBlock->pvEventContent = pvPortMalloc(ulContentSize);
					if(pxNewEventBlock->pvEventContent)
					{
						memcpy(pxNewEventBlock->pvEventContent, pvEventContent, ulContentSize);
						pxNewEventBlock->ulContentSize = ulContentSize;
					}
				}
				else
				{
					xStatus = pdFAIL;
					prvEvent_deleteECB(pxNewEventBlock);
				}
			}
		}
		else
		{
			xStatus = pdFAIL;
		}
	}
	else
	{
		xStatus = pdFAIL;
	}

	if(xStatus == pdPASS)
	{
		portDISABLE_INTERRUPTS();
		{
			prvEvent_addEventToList( pxNewEventBlock );
		}
		portENABLE_INTERRUPTS();
		portGENERATE_EVENT();
	}

	return xStatus;
}

/*
	Process event. This is the method that processes the events in the queue.

    @param void
    @return void
    @author Edielson
    @date   15/09/2017
*/

void vEvent_processEvents (void)
{
	static evtSCB* pxSCB;

	pxCurrentECB = prvEvent_getNextEvent();
	while (pxCurrentECB)
	{
		/*take the first subscriber from the sub list related to the event*/
		listGET_OWNER_OF_NEXT_NODE( pxSCB, ( xList_t* ) pxCurrentECB->pxSubscriberList );
		while(pxSCB)
		{
			if((pxSCB->xEventHandle == pxCurrentECB->xEventHandle)&&( pxSCB->pvEventHandlerFunction))
			{
				pxSCB->pvEventHandlerFunction(pxSCB->xEventHandle,
											  pxSCB->pvSubscriberHandle,
											  pxCurrentECB->pvEventContent,
											  pxCurrentECB->ulContentSize); //call event related function
			}
			/*take the next subscriber from the sub list related to the event*/
			listGET_OWNER_OF_NEXT_NODE( pxSCB, ( xList_t* ) pxCurrentECB->pxSubscriberList );
		}

		/*discard event after processed by all subscribers*/
		prvEvent_terminateEvent(pxCurrentECB);
		prvEvent_updateLifeTime();
		pxCurrentECB = prvEvent_getNextEvent();
	}
}


/*
 * -----------------------------------------------------------
 * The idle task.
 * ----------------------------------------------------------
 *
 * The idle task is called if the MCU is nog configured to operate in
 * sleep mode. In that case, the user can set its background operations
 * is this function call.
 *
 *
 */
void vEvent_idleTask( void )
{
	#if ( configUSE_SLEEP_MODE == 0 )
	{
		extern void vApplicationIdleHook( void );

		/* Call the user defined function from within the idle event.  This
		allows the application designer to add background functionality, since.
		the MCU will not operate in sleep mode.

		vApplicationIdleHook() MUST CALL A BLOCKING FUNCTION. */

		vApplicationIdleHook();
	}
	#endif
}

//======================================================================

__PRIVATE_ TickType_t prxEvent_getProcessStamp( void )
{
	return evt_ulProcessStamp;
}

__PRIVATE_ void prvEvent_initializeEventLists( void )
{
	UBaseType_t uxPriority;

	for( uxPriority = 0; uxPriority < EVENT_PRIORITY_LAST; uxPriority++ )
	{
		vList_initialize( ( xList_t* ) &( pxEventsLists[ uxPriority ] ) );
	}
}

__PRIVATE_ void prvEvent_initializeEventAdmin( void )
{
	UBaseType_t ulEventKey = 0;

	for( ulEventKey = 0; ulEventKey < configMAX_NUM_EVENTS; ulEventKey++ )
	{
		pxEventAdminList[ulEventKey].pvEventOwner = NULL;
		pxEventAdminList[ulEventKey].bInUse = pdFALSE;

		vList_initialize( ( xList_t* ) &( pxEventAdminList[ulEventKey].xSubscriberList ) );
		pxEventAdminList[ulEventKey].ulCurrentNumberOfSubscribers = 0;
	}

	prvEvent_initializeEventLists();
}

__PRIVATE_ void prvEvent_initializeSCBVariables( evtSCB* pxSCB,
												 EventHandlerFunction_t pFunction,
												 EventHandle_t	xEvent,
												 void* pvSubscriber)
{
	pxSCB->xEventHandle = xEvent;
	pxSCB->pvEventHandlerFunction = pFunction;
	pxSCB->pvSubscriberHandle = pvSubscriber;

	vList_initialiseNode( &( pxSCB->xSubscriberListNode ) );

	/* Set the pxSCB as a link back from the xListNode.  This is so we can get
	back to	the containing SCB from a generic node in a list. */
	listSET_LIST_NODE_OWNER((xListNode_t*) &( pxSCB->xSubscriberListNode ), pxSCB );
}

__PRIVATE_ void prvEvent_initializeECBVariables( evtECB* pxECB,
												 EventHandle_t xEvent,
												 UBaseType_t uxEventPriority)
{
	UBaseType_t ulProcessStamp = prxEvent_getProcessStamp();

	pxECB->xEventHandle = xEvent;
	pxECB->uxEventPriority = uxEventPriority;
	pxECB->pvEventContent = NULL;
	pxECB->ulContentSize = 0;

	/*Easier to access and run over the subscribers list*/
	pxECB->pxSubscriberList = ( ( xList_t* ) &( ( (evtEventAdmin_t*) xEvent )->xSubscriberList) );

	vList_initialiseNode( &( pxECB->xEventListNode ) );

	/* Set the pxECB as a link back from the xListNode.  This is so we can get
	back to	the containing SCB from a generic node in a list. */
	listSET_LIST_NODE_OWNER((xListNode_t*) &( pxECB->xEventListNode ), pxECB );

	/* Event lists are always in priority order. */
	listSET_LIST_NODE_VALUE( &( pxECB->xEventListNode ), ( TickType_t ) ulProcessStamp);

}

__PRIVATE_ void prvEvent_incrementProcessStamp( void )
{
	evt_ulProcessStamp++;
}


/*
	Event update lifetime. This function check event lifetime and updates the priority

    @param void
    @return void
    @author Gabriel
    @date   05/01/2017
*/
__PRIVATE_ void prvEvent_updateLifeTime (void)
{
	UBaseType_t uxPriority = EVENT_PRIORITY_HIGH;
	evtECB* pxECB = NULL;

	/*update the process stamp*/
	prvEvent_incrementProcessStamp();

	TickType_t ulProcessStamp = prxEvent_getProcessStamp(); /*get the new process stamp value*/

	while(uxPriority < EVENT_PRIORITY_LAST)
	{
		if( listIS_EMPTY((xList_t*) &( pxEventsLists[ uxPriority ] ) ) )
		{
			uxPriority++;
		}
		else
		{
			pxECB = listGET_OWNER_OF_HEAD_ENTRY( (xList_t*) &( pxEventsLists[ uxPriority ] ) );

			if(((TickType_t)(ulProcessStamp - listGET_LIST_ITEM_VALUE( &( pxECB->xEventListNode )))) >= configEVENTOS_LIFE_TIME)
			{
				portDISABLE_INTERRUPTS();
				{
					/* Remove from lower priority list */
					prvEvent_removeEventFromList( pxECB );

					/* Update ctrl variables */
					prvEvent_increaseEventPriority(pxECB);

					listSET_LIST_NODE_VALUE( &( pxECB->xEventListNode), ulProcessStamp );

					/* Insert from higher priority list */
					prvEvent_addEventToList( pxECB );
				}
				portENABLE_INTERRUPTS();
			}
			else uxPriority++;
		}
	}
}

/*
	EventOS dequeue event. This is the method that dequeue the events in the publish queue.

    @param tx_EventNode* pxEventList
    @return void
    @author Amanda/Samuel
    @date   20/10/2014

*/
__PRIVATE_ evtECB* prvEvent_getNextEvent(void)
{
	UBaseType_t xPriority = EVENT_PRIORITY_HIGH;
	evtECB* pxECB = NULL;

	/*check the event list with highest priority that is not empty*/
	while ((xPriority < EVENT_PRIORITY_LAST) && (pxECB == NULL))
	{
		pxECB = (evtECB*) listGET_OWNER_OF_HEAD_ENTRY(( xList_t* ) &( pxEventsLists[ xPriority ] ) );
		xPriority++;
	}

	return pxECB;
}

/*
	EventOS dequeue event. This is the method that dequeue the events in the publish queue.

    @param tx_EventNode* pxEventList

    @return void

*/
__PRIVATE_ void prvEvent_increaseEventPriority(evtECB* pxECB)
{
	if(!pxECB) return;
	pxECB->uxEventPriority--;
	if(pxECB->uxEventPriority < EVENT_PRIORITY_HIGH)
	{
		pxECB->uxEventPriority = (BaseType_t) EVENT_PRIORITY_HIGH;
	}
}

__PRIVATE_ void prvEvent_terminateEvent( evtECB* pxECB )
{
	portDISABLE_INTERRUPTS();
	{
		vList_remove( &( pxECB->xEventListNode ) );
		--evt_ulCurrentNumberOfPubEvents;
	}
	portENABLE_INTERRUPTS();
	prvEvent_deleteECB( pxECB );
}

__PRIVATE_ void prvEvent_deleteECB( evtECB* pxECB )
{
	/* Free up the memory allocated by the scheduler for the task.  It is up to
	the task to free any memory allocated at the application level. */
	if(pxECB->pvEventContent)
	{
		vPortFree( pxECB->pvEventContent );
	}
	vPortFree( pxECB );
}


__PRIVATE_ BaseType_t prxEvent_generateEventKey(void* pvEventOwner,
										  	     const char* szEventName)
{
	unsigned portLONG hash = 5381;
	portCHAR c;

	while ( (c = *szEventName) )
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		szEventName++;
	}

	return (BaseType_t)(hash % configMAX_NUM_EVENTS);
}

__PRIVATE_ BaseType_t prxEvent_validateEventHandle( EventHandle_t xEvent )
{
	return  ( ( (evtEventAdmin_t*) xEvent )->bInUse );
}
