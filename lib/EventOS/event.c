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
	xListNode				xEventListNode;						/*< List node used to place the event in the list. */
	portBASE_TYPE			ulEventPriority;					/*< The priority of the event where 0 is the lowest priority. */
	portBASE_TYPE			ulEventKey;


	xList*					pxSubscriberList;					/*< Pointer to the list of event subscribers>*/
	void*					pvPayload;
	portBASE_TYPE			ulPayloadSize;
} evtECB;

/*
 * Event handler control block.  A event handler control block (HCB) is allocated to each event subscriber,
 * and stores the parameters of the event handler.
 */
typedef struct evtSubscriber
{
	void* pvHandler;
	unsigned portBASE_TYPE			ulEventKey;
	pdEVENT_HANDLER_FUNCTION 		pdEventHandlerFunction;

	xListNode						xSubscriberListNode;						/*< List node used to place the event in the list. */
}evtSCB;


/*
 * Event handler control block.  A event handler control block (HCB) is allocated to each event subscriber,
 * and stores the parameters of the event handler.
 */
typedef struct evtEventAdmin
{
	void* 					pvEventOwner;
	unsigned portBASE_TYPE	ulEventKey;
	portCHAR				szEventName[configMAX_EVENT_NAME_LEN];
	portBASE_TYPE			bInUse;

	xList 					xSubscriberList;						/*< List of subscribers for a specific event. */
	unsigned portBASE_TYPE 	ulCurrentNumberOfSubscribers;

}evtEventAdmin_t;

/* Lists for events and event handlers. --------------------*/
__PRIVATE_ xList pxEventsLists[ EVENT_PRIORITY_LAST ];	/*< Prioritized events. */
__PRIVATE_ evtEventAdmin_t pxEventAdminList[ configMAX_NUM_EVENTS ];	/*< Event administration . */


/*********************************************************
    private attributes.
*********************************************************/
__PRIVATE_ evtECB* volatile pxCurrentECB = NULL;

/*********************************************************
    private variables.
*********************************************************/
__PRIVATE_ volatile unsigned portBASE_TYPE evt_ulCurrentNumberOfPubEvents 	= ( unsigned portBASE_TYPE ) 0;
__PRIVATE_ volatile unsigned portBASE_TYPE evt_ulCurrentNumberOfRegEvents 	= ( unsigned portBASE_TYPE ) 0;
__PRIVATE_ volatile unsigned portBASE_TYPE evt_ulProcessStamp = ( unsigned portBASE_TYPE ) 0;


/*********************************************************
    private operations.
*********************************************************/

/*
 * Check if a event key is valid in the system.
 */
#define prvEvent_checkEventKey( ulEventKey ) (pxEventAdminList[ulEventKey].bInUse ) \

/*
 * Check if a event key is valid and belongs to the respective publisher.
 */
#define prvEvent_validateEventPublisher( pvPublisher, ulEventKey ) ( (pxEventAdminList[ulEventKey].bInUse ) && \
		(pxEventAdminList[ulEventKey].pvEventOwner == pvPublisher) ) \



/*
 * Place the subscriber represented by pxSCB into the appropriate event queue for.
 * It is inserted at the head of the list.
 */
#define prvEvent_addSubscriberToList( pxSCB ) \
{\
	vList_insertHead( ( xList* ) &( pxEventAdminList[pxSCB->ulEventKey].xSubscriberList), &( pxSCB->xSubscriberListNode ) ); \
	pxEventAdminList[ulEventKey].ulCurrentNumberOfSubscribers++; \
}\

/*
 * Place the event represented by pxECB into the appropriate event priority queue.
 * It is inserted at the right position in the list, considering the deadline set on ulNodeValue.
 */
#define prvEvent_addEventToList( pxECB ) \
{ \
	vList_insert( ( xList* ) &( pxEventsLists[ pxECB->ulEventPriority ] ), &( pxECB->xEventListNode ) );	\
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
												 pdEVENT_HANDLER_FUNCTION pFunction,
												 unsigned portBASE_TYPE	ulEventKey,
												 void* pvSubscriber);
__PRIVATE_ void prvEvent_initializeECBVariables( evtECB* pxECB,
												 unsigned portBASE_TYPE ulEventKey,
												 unsigned portBASE_TYPE ulEventPriority);
__PRIVATE_ void prvEvent_incrementProcessStamp( void );
__PRIVATE_ void prvEvent_getProcessStamp( portTickType*  pulProcessStamp);
__PRIVATE_ void prvEvent_updateLifeTime (void);
__PRIVATE_ evtECB* prvEvent_getNextEvent(void);
__PRIVATE_ void prvEvent_increaseEventPriority(evtECB* pxECB);
__PRIVATE_ void prvEvent_terminateEvent( evtECB* pxECB );
__PRIVATE_ void prvEvent_deleteECB( evtECB* pxECB );
__PRIVATE_ void prvEvent_generateEventKey(void* pvEventOwner,
										  portCHAR* szEventName,
										  unsigned portBASE_TYPE* ulEventKey);

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
	Event subscribe. Function subscribes a event handler to receive notifications about as specific event

    @param pvFunction: function to be called for event notification
    	   ulEventKey: type of event to be notified
    	   pvSubscriber: event handler object
    @return void
    @author Edielson
    @date   15/09/2017
*/
signed portBASE_TYPE xEvent_register (	void* pvEventOwner,
										portCHAR* szEventName,
										unsigned portBASE_TYPE* pulEventKey)
{
	if(szEventName == NULL) return pdFAIL;
	if(pvEventOwner == NULL) return pdFAIL;
	if(strlen(szEventName) >= configMAX_EVENT_NAME_LEN) return pdFAIL;
	if(strlen(szEventName) < 2) return pdFAIL; /* event name should have length greater then 1 */

	signed portBASE_TYPE xReturn = pdTRUE;
	unsigned portBASE_TYPE ulNewKey = 0;
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
		prvEvent_generateEventKey(pvEventOwner, szEventName, &ulNewKey);

		/*
		 * We have to check if the event administration is already in use
		 * for this specific key. This is important to keep the key
		 * unique in the system.
		 * */
		if(prvEvent_checkEventKey(ulNewKey) == pdFALSE)
		{
			pxEventAdminList[ulNewKey].bInUse = pdTRUE;
			pxEventAdminList[ulNewKey].ulEventKey = ulNewKey;

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
			 * return the new event key to the user.
			 */
			*pulEventKey = ulNewKey;
		}
		else
		{
			/* For now, we consider just one event admin per key.
			 * Further version can use a linked list of event
			 * admin per key. */

			*pulEventKey = 0;
			xReturn = pdFAIL;
		}
	}
	portENABLE_INTERRUPTS();

	return xReturn;
}

/*
	Event subscribe. Function subscribes a event handler to receive notifications about as specific event

    @param pvFunction: function to be called for event notification
    	   ulEventKey: type of event to be notified
    	   pvSubscriber: event handler object
    @return void
    @author Edielson
    @date   15/09/2017
*/
signed portBASE_TYPE xEvent_subscribe (	pdEVENT_HANDLER_FUNCTION pFunction,
										unsigned portBASE_TYPE ulEventKey,
										void* pvSubscriber)
{
	if (pFunction == NULL) 		return pdFAIL;
	if (pvSubscriber == NULL) 	return pdFAIL;
	if (evt_ulCurrentNumberOfRegEvents == 0) return pdFAIL;

	signed portBASE_TYPE xReturn = pdTRUE;

	/*
	 * Check if the event key is already registered in the system
	 */
	if( prvEvent_checkEventKey(ulEventKey) == pdTRUE )
	{
		evtSCB* pxNewSubscriber = (evtSCB*)pvPortMalloc(sizeof(evtSCB));

		if(pxNewSubscriber)
		{
			/*Initializing variables*/
			prvEvent_initializeSCBVariables(pxNewSubscriber,
											pFunction,
											ulEventKey,
											pvSubscriber);

			portDISABLE_INTERRUPTS();
			{
				prvEvent_addSubscriberToList( pxNewSubscriber );

				xReturn = pdPASS;
			}
			portENABLE_INTERRUPTS();
		}
	}

	return xReturn;
}

/*
	EventOS publish. Publishing events in a particular queue according to its priority

    @param - void* pxEventSlot
    @return void
    @author samuel/amanda
    @date   22/09/2014
*/

signed portBASE_TYPE xEvent_publish (void* pvPublisher,
									 unsigned portBASE_TYPE ulEventKey,
									 unsigned portBASE_TYPE ulPriority,
									 void* pvPayload,
									 unsigned portBASE_TYPE ulPayloadSize)
{
	if (ulPriority >= EVENT_PRIORITY_LAST) return pdFAIL;
	if (evt_ulCurrentNumberOfRegEvents == 0) return pdFAIL;

	portBASE_TYPE xStatus = pdPASS;
	evtECB* pxNewEvent = NULL;

	if( prvEvent_validateEventPublisher(pvPublisher, ulEventKey) )
	{
		pxNewEvent = (evtECB*)pvPortMalloc(sizeof(evtECB));
		if(pxNewEvent)
		{
			prvEvent_initializeECBVariables(pxNewEvent,
											ulEventKey,
											ulPriority);

			/* Initialize the payload container in the event block */
			if(ulPayloadSize > 0)
			{
				/* Check if payload pointer is not NULL*/
				if(pvPayload)
				{
					pxNewEvent->pvPayload = pvPortMalloc(ulPayloadSize);
					if(pxNewEvent->pvPayload)
					{
						memcpy(pxNewEvent->pvPayload, pvPayload, ulPayloadSize);
						pxNewEvent->ulPayloadSize = ulPayloadSize;
					}
				}
				else
				{
					xStatus = pdFAIL;
					prvEvent_deleteECB(pxNewEvent);
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
			prvEvent_addEventToList( pxNewEvent );
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
		listGET_OWNER_OF_NEXT_NODE( pxSCB, ( xList* ) pxCurrentECB->pxSubscriberList );
		while(pxSCB)
		{
			if((pxSCB->ulEventKey == pxCurrentECB->ulEventKey)&&( pxSCB->pdEventHandlerFunction))
			{
				pxSCB->pdEventHandlerFunction(pxSCB->ulEventKey,pxSCB->pvHandler,pxCurrentECB->pvPayload,pxCurrentECB->ulPayloadSize); //call event related function
			}
			/*take the next subscriber from the sub list related to the event*/
			listGET_OWNER_OF_NEXT_NODE( pxSCB, ( xList* ) pxCurrentECB->pxSubscriberList );
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

__PRIVATE_ void prvEvent_getProcessStamp( portTickType*  pulProcessStamp)
{
	*pulProcessStamp = evt_ulProcessStamp;
}

__PRIVATE_ void prvEvent_initializeEventLists( void )
{
	unsigned portBASE_TYPE ulPriority;

	for( ulPriority = 0; ulPriority < EVENT_PRIORITY_LAST; ulPriority++ )
	{
		vList_initialize( ( xList* ) &( pxEventsLists[ ulPriority ] ) );
	}
}

__PRIVATE_ void prvEvent_initializeEventAdmin( void )
{
	unsigned portBASE_TYPE ulEventKey = 0;

	for( ulEventKey = 0; ulEventKey < configMAX_NUM_EVENTS; ulEventKey++ )
	{
		pxEventAdminList[ulEventKey].pvEventOwner = NULL;
		pxEventAdminList[ulEventKey].ulEventKey = ulEventKey;
		pxEventAdminList[ulEventKey].bInUse = pdFALSE;

		vList_initialize( ( xList* ) &( pxEventAdminList[ulEventKey].xSubscriberList ) );
		pxEventAdminList[ulEventKey].ulCurrentNumberOfSubscribers = 0;
	}

	prvEvent_initializeEventLists();
}

__PRIVATE_ void prvEvent_initializeSCBVariables( evtSCB* pxSCB,
												 pdEVENT_HANDLER_FUNCTION pFunction,
												 unsigned portBASE_TYPE	ulEventKey,
												 void* pvSubscriber)
{
	pxSCB->ulEventKey = ulEventKey;
	pxSCB->pdEventHandlerFunction = pFunction;
	pxSCB->pvHandler = pvSubscriber;

	vList_initialiseNode( &( pxSCB->xSubscriberListNode ) );

	/* Set the pxSCB as a link back from the xListNode.  This is so we can get
	back to	the containing SCB from a generic node in a list. */
	listSET_LIST_NODE_OWNER((xListNode*) &( pxSCB->xSubscriberListNode ), pxSCB );
}

__PRIVATE_ void prvEvent_initializeECBVariables( evtECB* pxECB,
												 unsigned portBASE_TYPE ulEventKey,
												 unsigned portBASE_TYPE ulEventPriority)
{
	unsigned portBASE_TYPE ulProcessStamp = 0;

	prvEvent_getProcessStamp(&ulProcessStamp);

	pxECB->ulEventKey = ulEventKey;
	pxECB->ulEventPriority = ulEventPriority;
	pxECB->pvPayload = NULL;
	pxECB->ulPayloadSize = 0;

	/*Easier to access and run over the subscribers list*/
	pxECB->pxSubscriberList = (& pxEventAdminList[ulEventKey].xSubscriberList);

	vList_initialiseNode( &( pxECB->xEventListNode ) );

	/* Set the pxECB as a link back from the xListNode.  This is so we can get
	back to	the containing SCB from a generic node in a list. */
	listSET_LIST_NODE_OWNER((xListNode*) &( pxECB->xEventListNode ), pxECB );

	/* Event lists are always in priority order. */
	listSET_LIST_NODE_VALUE( &( pxECB->xEventListNode ), ( portBASE_TYPE ) ulProcessStamp);

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
	portBASE_TYPE ulPriority = EVENT_PRIORITY_HIGH;
	evtECB* pxECB = NULL;
	unsigned portBASE_TYPE ulProcessStamp = 0;

	/*update the process stamp*/
	prvEvent_incrementProcessStamp();
	/*get the new process stamp value*/
	prvEvent_getProcessStamp(&ulProcessStamp);

	while(ulPriority < EVENT_PRIORITY_LAST)
	{
		if( listIS_EMPTY((xList*) &( pxEventsLists[ ulPriority ] ) ) )
		{
			ulPriority++;
		}
		else
		{
			pxECB = listGET_OWNER_OF_HEAD_ENTRY((xList*)&( pxEventsLists[ ulPriority ] ) );

			if(((portBASE_TYPE)(ulProcessStamp - listGET_LIST_ITEM_VALUE( &( pxECB->xEventListNode )))) >= configEVENTOS_LIFE_TIME)
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
			else ulPriority++;
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
	portBASE_TYPE xPriority = EVENT_PRIORITY_HIGH;
	evtECB* pxECB = NULL;

	/*check the event list with highest priority that is not empty*/
	while ((xPriority < EVENT_PRIORITY_LAST) && (pxECB == NULL))
	{
		pxECB = (evtECB*) listGET_OWNER_OF_HEAD_ENTRY(( xList* ) &( pxEventsLists[ xPriority ] ) );
		xPriority++;
	}

	return pxECB;
}

/*
	EventOS dequeue event. This is the method that dequeue the events in the publish queue.

    @param tx_EventNode* pxEventList
    @return void
    @author Amanda/Samuel
    @date   20/10/2014

*/
__PRIVATE_ void prvEvent_increaseEventPriority(evtECB* pxECB)
{
	if(!pxECB) return;
	pxECB->ulEventPriority--;
	if(pxECB->ulEventPriority < EVENT_PRIORITY_HIGH)
	{
		pxECB->ulEventPriority = (portBASE_TYPE) EVENT_PRIORITY_HIGH;
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
	if(pxECB->pvPayload)
	{
		vPortFree( pxECB->pvPayload );
	}
	vPortFree( pxECB );
}


__PRIVATE_ void prvEvent_generateEventKey(void* pvEventOwner,
										  portCHAR* szEventName,
										  unsigned portBASE_TYPE* ulEventKey)
{
	unsigned portLONG hash = 5381;
	portCHAR c;

	while ( (c = *szEventName) )
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		szEventName++;
	}

	*ulEventKey = hash % configMAX_NUM_EVENTS;
}
