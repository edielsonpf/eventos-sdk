
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
	portBASE_TYPE			ulEventPriority;							/*< The priority of the event where 0 is the lowest priority. */
	portBASE_TYPE			ulEventType;
	signed char				pcEventName[configMAX_EVENT_NAME_LEN];	/*< Descriptive name given to the event when created.  Facilitates debugging only. */

	xList*					pxSubscriberList;					/*< Pointer to the list of event handlers>*/
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
	portBASE_TYPE			ulEventType;
	pdEVENT_HANDLER_FUNCTION pdEventHandlerFunction;

	xListNode				xSubscriberListNode;						/*< List node used to place the event in the list. */
}evtSCB;

/* Lists for events and event handlers. --------------------*/
__PRIVATE_ xList pxEventsLists[ EVENT_PRIORITY_LAST ];	/*< Prioritized events. */
__PRIVATE_ xList pxSubscriberLists[ EVENT_TYPE_LAST ];	/*< Event handler separated by event types. */


/*********************************************************
    private attributes.
*********************************************************/
__PRIVATE_ evtECB* volatile pxCurrentECB = NULL;

/*********************************************************
    private variables.
*********************************************************/
__PRIVATE_ volatile unsigned portBASE_TYPE ulCurrentNumberOfEvents 	= ( unsigned portBASE_TYPE ) 0;
__PRIVATE_ volatile unsigned portBASE_TYPE ulCurrentNumberOfSubscribers 	= ( unsigned portBASE_TYPE ) 0;
__PRIVATE_ volatile unsigned portBASE_TYPE ulProcessStamp = ( unsigned portBASE_TYPE ) 0;


/*********************************************************
    private operations.
*********************************************************/

/*
 * Place the subscriber represented by pxSCB into the appropriate event queue for.
 * It is inserted at the head of the list.
 */
#define prvEvent_addSubscriberToList( pxSCB ) vList_insertHead( ( xList* ) &( pxSubscriberLists[ pxSCB->ulEventType ] ), &( pxSCB->xSubscriberListNode ) );	\

/*
 * Place the event represented by pxECB into the appropriate event priority queue.
 * It is inserted at the right position in the list, considering the deadline set on ulNodeValue.
 */
#define prvEvent_addEventToList( pxECB ) vList_insert( ( xList* ) &( pxEventsLists[ pxECB->ulEventPriority ] ), &( pxECB->xEventListNode ) );	\


__PRIVATE_ void prvEvent_deleteECB( evtECB* pxECB );
__PRIVATE_ void prvEvent_initializeEventLists( void );
__PRIVATE_ void prvEvent_initializeSubscriberLists( void );
__PRIVATE_ void prvEvent_initializeSCBVariables( evtSCB* pxSCB, pdEVENT_HANDLER_FUNCTION pFunction, portBASE_TYPE	ulEventType, void* pvSubscriber);
__PRIVATE_ void prvEvent_initializeECBVariables( evtECB* pxECB, unsigned portBASE_TYPE ulEventType, unsigned portBASE_TYPE ulEventPriority);
__PRIVATE_ void prvEvent_incrementProcessStamp( void );
__PRIVATE_ portTickType prxEvent_getProcessStamp( void );
__PRIVATE_ void prvEvent_updateLifeTime (void);
__PRIVATE_ void prvEvent_getNextEvent(evtECB* pxECB);
__PRIVATE_ void prvEvent_increaseEventPriority(evtECB* pxECB);
__PRIVATE_ void prvEvent_decreaseEventPriority(evtECB* pxECB);
__PRIVATE_ portBASE_TYPE prxEvent_getEventPriority(evtECB* pxECB);
__PRIVATE_ void prvEvent_terminateEvent( evtECB* pxECB );
__PRIVATE_ void prvEvent_deleteECB( evtECB* pxECB );

/*********************************************************
    Operations implementation
*********************************************************/


__PRIVATE_ portTickType prxEvent_getProcessStamp( void )
{
	return ulProcessStamp;
}

__PRIVATE_ void prvEvent_initializeEventLists( void )
{
	unsigned portBASE_TYPE ulPriority;

	for( ulPriority = 0; ulPriority < EVENT_PRIORITY_LAST; ulPriority++ )
	{
		vList_initialize( ( xList* ) &( pxEventsLists[ ulPriority ] ) );
	}
}

__PRIVATE_ void prvEvent_initializeSubscriberLists( void )
{
	unsigned portBASE_TYPE ulEventType;

	for( ulEventType = 0; ulEventType < EVENT_TYPE_LAST; ulEventType++ )
	{
		vList_initialize( ( xList* ) &( pxSubscriberLists[ ulEventType ] ) );
	}
}

__PRIVATE_ void prvEvent_initializeSCBVariables( evtSCB* pxSCB, pdEVENT_HANDLER_FUNCTION pFunction, portBASE_TYPE	ulEventType, void* pvSubscriber)
{
	/* This is used as an array index so must ensure it's not too large.  First
	remove the privilege bit if one is present. */
	if( ulEventType >= EVENT_TYPE_LAST )
	{
		ulEventType = EVENT_TYPE_LAST - 1;
	}
	pxSCB->ulEventType = ulEventType;
	pxSCB->pdEventHandlerFunction = pFunction;
	pxSCB->pvHandler = pvSubscriber;

	vList_initialiseNode( &( pxSCB->xSubscriberListNode ) );

	/* Set the pxSCB as a link back from the xListNode.  This is so we can get
	back to	the containing SCB from a generic node in a list. */
	listSET_LIST_NODE_OWNER( &( pxSCB->xSubscriberListNode ), pxSCB );
}

__PRIVATE_ void prvEvent_initializeECBVariables( evtECB* pxECB, unsigned portBASE_TYPE ulEventType, unsigned portBASE_TYPE ulEventPriority)
{
	/* This is used as an array index so must ensure it's not too large.  First
	remove the privilege bit if one is present. */
	if( ulEventType >= EVENT_TYPE_LAST )
	{
		ulEventType = EVENT_TYPE_LAST - 1;
	}

	/* This is used as an array index so must ensure it's not too large.  First
	remove the privilege bit if one is present. */
	if( ulEventPriority >= EVENT_PRIORITY_LAST )
	{
		ulEventType = EVENT_PRIORITY_LAST - 1;
	}

	pxECB->ulEventType = ulEventType;
	pxECB->ulEventPriority = ulEventPriority;

	/*Easier to access and run over the subscribers list*/
	pxECB->pxSubscriberList = (& pxSubscriberLists[ ulEventType ]);

	vList_initialiseNode( &( pxECB->xEventListNode ) );

	/* Set the pxECB as a link back from the xListNode.  This is so we can get
	back to	the containing SCB from a generic node in a list. */
	listSET_LIST_NODE_OWNER( &( pxECB->xEventListNode ), pxECB );
	/* Event lists are always in priority order. */
	listSET_LIST_NODE_VALUE( &( pxECB->xEventListNode ), ( portBASE_TYPE ) prxEvent_getProcessStamp());

}

__PRIVATE_ void prvEvent_incrementProcessStamp( void )
{
	ulProcessStamp++;
}


/**
	Event start scheduler. This is the method that starts the scheduler process.

    @param void
    @return void
    @author edielson
    @date   05/09/2014
*/
void vEvent_startScheduler( void )
{
	/* Start RTC */
	portSTART_RTC();

	/* Initialize the event list before scheduler start */
	prvEvent_initializeEventLists();

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
    	   ulEventType: type of event to be notified
    	   pvSubscriber: event handler object
    @return void
    @author Edielson
    @date   15/09/2017
*/
signed portBASE_TYPE xEvent_subscribe (pdEVENT_HANDLER_FUNCTION pFunction, portBASE_TYPE ulEventType, void* pvSubscriber)
{
	if(ulEventType >= (portBASE_TYPE)EVENT_TYPE_LAST) return pdFALSE;
	if(!pFunction) return pdFALSE;

	signed portBASE_TYPE xReturn = pdFALSE;

	evtSCB* pxNewSubscriber = (evtSCB*)pvPortMalloc(sizeof(evtSCB));
	if(pxNewSubscriber)
	{
		/*Initializing variables*/
		prvEvent_initializeSCBVariables(pxNewSubscriber,pFunction,ulEventType,pvSubscriber);

		portDISABLE_INTERRUPTS();
		{
			/*Inserting new subscriber in the respective event list*/
			ulCurrentNumberOfSubscribers++;

			if(ulCurrentNumberOfSubscribers == ( unsigned portBASE_TYPE ) 1)
			{
				/* This is the first subscriber to be created so do the preliminary
				required initialization. */
				prvEvent_initializeSubscriberLists();
			}
			prvEvent_addSubscriberToList( pxNewSubscriber );

			xReturn = pdPASS;
		}
		portENABLE_INTERRUPTS();
	}
	//EventOS_printLog((portCHAR*)"[subscribe] Event: %d", eEvent);
	return xReturn;
}
/*
	EventOS publish. Publishing events in a particular queue according to its priority

    @param - void* pxEventSlot
    @return void
    @author samuel/amanda
    @date   22/09/2014
*/

signed portBASE_TYPE xEvent_publish (portBASE_TYPE ulEventType, portBASE_TYPE ulPriority, void* pvPayload, portBASE_TYPE ulPayloadSize)
{
	if (ulEventType >= EVENT_TYPE_LAST) return pdFALSE;
	if (ulPriority >= EVENT_PRIORITY_LAST) return pdFALSE;

	portBASE_TYPE xStatus = pdFALSE;

	evtECB* pxNewEvent = (evtECB*)pvPortMalloc(sizeof(evtECB));
	if(pxNewEvent)
	{
		prvEvent_initializeECBVariables(pxNewEvent,ulEventType, ulPriority);

		portDISABLE_INTERRUPTS();
		{
			ulCurrentNumberOfEvents++;
			prvEvent_addEventToList( pxNewEvent );
			xStatus = pdPASS;
			//EventOS_printLog((portCHAR*)"[publish] Event: %d / Priority: %d", pxNew->pxEvent->xHeader.eEvent, pxNew->pxEvent->xHeader.ePriority);
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

	prvEvent_getNextEvent(pxCurrentECB);
	while (pxCurrentECB)
	{
		//EventOS_printLog((portCHAR*)"[process] Event: %d / Priority: %d", pEvent->xHeader.eEvent,pEvent->xHeader.ePriority);

		/*take the first subscriber from the sub list related to the event*/
		listGET_OWNER_OF_NEXT_NODE( pxSCB, ( xList* ) &( pxCurrentECB->pxSubscriberList ))
		while(pxSCB)
		{
			if((pxSCB->ulEventType == pxCurrentECB->ulEventType)&&( pxSCB->pdEventHandlerFunction))
			{
				pxSCB->pdEventHandlerFunction(pxSCB->ulEventType,pxSCB->pvHandler,pxCurrentECB->pvPayload,pxCurrentECB->ulPayloadSize); //call event related function
			}
			/*take the next subscriber from the sub list related to the event*/
			listGET_OWNER_OF_NEXT_NODE( pxSCB, ( xList* ) &( pxCurrentECB->pxSubscriberList ))
		}
		/*discard event after processed by all subscribers*/
		prvEvent_terminateEvent(pxCurrentECB);
		prvEvent_updateLifeTime();
		prvEvent_getNextEvent(pxCurrentECB);
	}
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
	portBASE_TYPE ulPriority = EVENT_PRIORITY_HIGH-1;
	evtECB* pxECB = NULL;

	prvEvent_incrementProcessStamp();

	while(ulPriority < EVENT_PRIORITY_LAST)
	{

		if(listIS_EMPTY( (xList*) &( pxEventsLists[ ulPriority ] ) ) ) ulPriority++;
		else
		{
			pxECB = listGET_OWNER_OF_HEAD_ENTRY((xList*)&( pxEventsLists[ ulPriority ] ) );

			if(((portBASE_TYPE)(prxEvent_getProcessStamp() - listGET_LIST_ITEM_VALUE(pxECB->xEventListNode))) >= EVENTOS_LIFE_TIME)
			{
				//EventOS_printLog((portCHAR*)"[update] Event: %d / Priority: %d to Priority: %d", pxEventListAux->pxEvent->xHeader.eEvent,pxEventListAux->pxEvent->xHeader.ePriority, (pxEventListAux->pxEvent->xHeader.ePriority - 1));

				portDISABLE_INTERRUPTS();
				{
					/* Remove from lower priority list */
					vList_remove(pxECB->xEventListNode);

					/* Update ctrl variables */
					prvEvent_increaseEventPriority(pxECB);
					listSET_LIST_NODE_VALUE(pxECB->xEventListNode,prxEvent_getProcessStamp());

					/* Insert from higher priority list */
					vList_insert((xList*)&( pxEventsLists[prxEvent_getEventPriority(pxECB)] ));
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
__PRIVATE_ void prvEvent_getNextEvent(evtECB* pxECB)
{
	portBASE_TYPE xPriority = EVENT_PRIORITY_HIGH;
	pxECB = NULL;

	/*check the event list with highest priority that is not empty*/
	while ((xPriority < EVENTOS_PRIORITY_LAST) && (pxECB == NULL))
	{
		pxECB = listGET_OWNER_OF_HEAD_ENTRY(( xList* ) &( pxEventsLists[ xPriority ] ));
		xPriority++;
	}
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

/*
	EventOS dequeue event. This is the method that dequeue the events in the publish queue.

    @param tx_EventNode* pxEventList
    @return void
    @author Amanda/Samuel
    @date   20/10/2014

*/
__PRIVATE_ void prvEvent_decreaseEventPriority(evtECB* pxECB)
{
	if(!pxECB) return;
	pxECB->ulEventPriority++;
	if(pxECB->ulEventPriority >= EVENT_PRIORITY_LAST)
	{
		pxECB->ulEventPriority = (portBASE_TYPE) EVENT_PRIORITY_LAST-1;
	}
}

/*
	EventOS dequeue event. This is the method that dequeue the events in the publish queue.

    @param tx_EventNode* pxEventList
    @return void
    @author Amanda/Samuel
    @date   20/10/2014

*/
__PRIVATE_ portBASE_TYPE prxEvent_getEventPriority(evtECB* pxECB)
{
	if(!pxECB) return EVENT_PRIORITY_LAST;
	return (portBASE_TYPE)pxECB->ulEventPriority;
}

__PRIVATE_ void prvEvent_terminateEvent( evtECB* pxECB )
{
	portDISABLE_INTERRUPTS();
	{
		vList_remove( &( pxECB->xEventListNode ) );
		--ulCurrentNumberOfEvents;
	}
	portENABLE_INTERRUPTS();
	prvEvent_deleteECB( pxECB );
}

__PRIVATE_ void prvEvent_deleteECB( evtECB* pxECB )
{
	/* Free up the memory allocated by the scheduler for the task.  It is up to
	the task to free any memory allocated at the application level. */
	vPortFree( pxECB->pvPayload );
	vPortFree( pxTCB );
}


/**
	Get event scheduler version

    @param void
    @return pointer to the version string
    @author edielsonpf
    @date   19/09/2017
*/
portCHAR*  pxEvent_getVersion(void)
{
	return (portCHAR*) evnKERNEL_VERSION_NUMBER;
}
