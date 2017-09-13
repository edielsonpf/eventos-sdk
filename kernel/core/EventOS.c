
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
#include "EventOSConfig.h"

/*********************************************************
    private constants.
*********************************************************/

/*********************************************************
    private types.
*********************************************************/
typedef struct tag_SubscribeNode
{
	void* hHandle;
	pdEventHandlerFunction pcbfHandlerFunction;

	struct tag_SubscribeNode* ptagNext;
	struct tag_SubscribeNode* ptagPrev;
}ttag_SubscribeNode;

typedef struct tag_EventNode
{
	port_EVENT_Type* ptagEvent;
	portULONG		ulLifeTimeCount;
	struct tag_EventNode* ptagNext;
	struct tag_EventNode* ptagPrev;
}ttag_EventNode;


typedef struct tag_EventOS
{
	portULONG				ulProcessStamp;
	ttag_EventNode* 		ptagEventList[EVENTOS_PRIORITY_LAST];
	ttag_SubscribeNode* 	ptagSubList[EVENTOS_EVENT_LAST];
}ttag_EventOS;


/*********************************************************
    private attributes.
*********************************************************/
__PRIVATE_ ttag_EventOS* EventOS_ptagEventOS = NULL;

/*********************************************************
    private operations.
*********************************************************/
__PRIVATE_ void 			EventOS_updateLifeTime (void);
__PRIVATE_ port_EVENT_Type* EventOS_getEvent(void);
__PRIVATE_ void  	 		EventOS_printLog(const portCHAR* cString, ...);

/*********************************************************
    Operations implementation
*********************************************************/

/**
	EventOS constructor. This is the constructor method, it allocates memory
	for the current object and initialize it. In case of a failure the method
	returns zero, otherwise it returns the class handle.

    @param void
    @return - True8 if succeeded
    @author edielson
    @date   02/09/2014
*/
void	EventOS_createInstance(void)
{

	if(EventOS_ptagEventOS) return;

	portUCHAR ucCounter;

	EventOS_ptagEventOS = (ttag_EventOS*)malloc(sizeof(ttag_EventOS));
	if(EventOS_ptagEventOS)
	{
		/*Those are the subscriber lists*/
		for (ucCounter = 0; ucCounter < EVENTOS_EVENT_LAST; ucCounter++)
		{
			EventOS_ptagEventOS->ptagSubList[ucCounter] = (ttag_SubscribeNode*)malloc(sizeof(ttag_SubscribeNode));
			if(EventOS_ptagEventOS->ptagSubList[ucCounter])
			{
				EventOS_ptagEventOS->ptagSubList[ucCounter]->hHandle = NULL;
				EventOS_ptagEventOS->ptagSubList[ucCounter]->pcbfHandlerFunction=NULL;
				EventOS_ptagEventOS->ptagSubList[ucCounter]->ptagNext = EventOS_ptagEventOS->ptagSubList[ucCounter];
				EventOS_ptagEventOS->ptagSubList[ucCounter]->ptagPrev = EventOS_ptagEventOS->ptagSubList[ucCounter];
			}
		}

		/*Those are the event lists, each one with a different priority*/
		for(ucCounter = 0; ucCounter < EVENTOS_PRIORITY_LAST; ucCounter++)
		{
			EventOS_ptagEventOS->ptagEventList[ucCounter] =(ttag_EventNode*)malloc(sizeof(ttag_EventNode));
			if(EventOS_ptagEventOS->ptagEventList[ucCounter])
			{
				EventOS_ptagEventOS->ptagEventList[ucCounter]->ptagEvent = NULL;
				EventOS_ptagEventOS->ptagEventList[ucCounter]->ptagNext = EventOS_ptagEventOS->ptagEventList[ucCounter];
				EventOS_ptagEventOS->ptagEventList[ucCounter]->ptagPrev = EventOS_ptagEventOS->ptagEventList[ucCounter];
			}
        }
		EventOS_ptagEventOS->ulProcessStamp = 0;
	}
	/* Start RTC */
	portSTART_RTC();
}

/**
	EventOS destructor. This is the destructor method, it deallocates the memory
	reserved for the object. If the method is executed correctly the return value
	is true.

    @param void
    @return Bool8 - True8 if succeeded or False8 if not
    @author edielson
    @date   02/09/2014
*/
void 	EventOS_deleteInstance(void)
{
	if(EventOS_ptagEventOS)
	{
		portUCHAR ucCounter;
		ttag_EventNode* 	ptagEventNodeRun=NULL;
		ttag_SubscribeNode* ptagSubNodeRun=NULL;

		/*Those are the subscriber lists*/
		for (ucCounter = 0; ucCounter < EVENTOS_EVENT_LAST; ucCounter++)
		{
			ptagSubNodeRun = EventOS_ptagEventOS->ptagSubList[ucCounter]->ptagNext;
			while(ptagSubNodeRun!=EventOS_ptagEventOS->ptagSubList[ucCounter]) //searching for node in the list
			{
				(ptagSubNodeRun->ptagPrev)->ptagNext = (ptagSubNodeRun->ptagNext);
				(ptagSubNodeRun->ptagNext)->ptagPrev = (ptagSubNodeRun->ptagPrev);
				free(ptagSubNodeRun);
				ptagSubNodeRun = EventOS_ptagEventOS->ptagSubList[ucCounter]->ptagNext;
			}
		}

		/*Those are the event lists, each one with a different priority*/
		for(ucCounter = 0; ucCounter < EVENTOS_PRIORITY_LAST; ucCounter++)
		{
			ptagEventNodeRun = EventOS_ptagEventOS->ptagEventList[ucCounter]->ptagNext;
			while(ptagEventNodeRun!=EventOS_ptagEventOS->ptagEventList[ucCounter]) //searching for node in the list
			{
				(ptagEventNodeRun->ptagPrev)->ptagNext = (ptagEventNodeRun->ptagNext);
				(ptagEventNodeRun->ptagNext)->ptagPrev = (ptagEventNodeRun->ptagPrev);
				if(ptagEventNodeRun->ptagEvent->pvPayload) free(ptagEventNodeRun->ptagEvent->pvPayload);
				free(ptagEventNodeRun);
				ptagEventNodeRun = EventOS_ptagEventOS->ptagEventList[ucCounter]->ptagNext;
			}
		}
		free(EventOS_ptagEventOS);
	}
}

/**
	EventOS start scheduler. This is the method that starts the scheduler process.

    @param void
    @return void
    @author edielson
    @date   05/09/2014
*/
void EventOS_startScheduler( void )
{
	if(!EventOS_ptagEventOS) return;

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
	EventOs subscribe. Function subscribe a determined event

    @param portINTEGER - iEvent, function* pFunction
    @return void
    @author Samuel/Amanda
    @date   22/09/2014
*/
void EventOS_subscribe (port_EVENT_List eEvent, void* hHandle, pdEventHandlerFunction pFunction)
{
	if(!EventOS_ptagEventOS) return;
	if(eEvent >= EVENTOS_EVENT_LAST) return;
	if(!pFunction) return;

	ttag_SubscribeNode* ptagNew = (ttag_SubscribeNode*)malloc(sizeof(ttag_SubscribeNode));
	if(ptagNew)
	{
		ptagNew->pcbfHandlerFunction = pFunction;
		ptagNew->hHandle = hHandle;

		/*Inserting new subscriber in the respective event list*/
		ptagNew->ptagNext = EventOS_ptagEventOS->ptagSubList[eEvent]->ptagNext;
		EventOS_ptagEventOS->ptagSubList[eEvent]->ptagNext->ptagPrev = ptagNew;
		EventOS_ptagEventOS->ptagSubList[eEvent]->ptagNext = ptagNew;
		ptagNew->ptagPrev = EventOS_ptagEventOS->ptagSubList[eEvent];
	}
	EventOS_printLog((portCHAR*)"[subscribe] Event: %d", eEvent);
}
/*
	EventOS publish. Publishing events in a particular queue according to its priority

    @param - void* ptagEventSlot
    @return void
    @author samuel/amanda
    @date   22/09/2014
*/

void EventOS_publish (port_EVENT_Type* ptagEvent)
{
	if(!ptagEvent) return;
	if(!EventOS_ptagEventOS) return;

	vPortDisableInterrupts();
	{
		ttag_EventNode* ptagNew = (ttag_EventNode*)malloc(sizeof(ttag_EventNode));
		if(ptagNew)
		{
			ptagNew->ptagEvent = ptagEvent;
			ptagNew->ulLifeTimeCount = EventOS_ptagEventOS->ulProcessStamp;
			/*Inserting new subscriber in the respective event list*/
			ptagNew->ptagNext = EventOS_ptagEventOS->ptagEventList[ptagEvent->tagHeader.ePriority]->ptagNext;
			EventOS_ptagEventOS->ptagEventList[ptagEvent->tagHeader.ePriority]->ptagNext->ptagPrev = ptagNew;
			EventOS_ptagEventOS->ptagEventList[ptagEvent->tagHeader.ePriority]->ptagNext = ptagNew;
			ptagNew->ptagPrev = EventOS_ptagEventOS->ptagEventList[ptagEvent->tagHeader.ePriority];

			EventOS_printLog((portCHAR*)"[publish] Event: %d / Priority: %d", ptagNew->ptagEvent->tagHeader.eEvent, ptagNew->ptagEvent->tagHeader.ePriority);
		}
	}
	vPortEnableInterrupts();
	portGENERATE_EVENT();

}

/*
	EventOS process event. This is the method that processes the events in the queue.

    @param void
    @return void
    @author Amanda/Samuel
    @date   02/09/2014
*/

void EventOS_processEvents (void)
{
	if(!EventOS_ptagEventOS) return;

	ttag_SubscribeNode* ptagSubList;
	port_EVENT_Type* pEvent = NULL;

	pEvent = EventOS_getEvent();
	while (pEvent)
	{

		EventOS_printLog((portCHAR*)"[process] Event: %d / Priority: %d", pEvent->tagHeader.eEvent,pEvent->tagHeader.ePriority);

		/*take the first subscriber from the sub list related to the event*/
		ptagSubList = EventOS_ptagEventOS->ptagSubList[pEvent->tagHeader.eEvent]->ptagNext;
		while(ptagSubList!=EventOS_ptagEventOS->ptagSubList[pEvent->tagHeader.eEvent])
		{
			if(ptagSubList->pcbfHandlerFunction)
			{
				ptagSubList->pcbfHandlerFunction(ptagSubList->hHandle,pEvent); //call event related function
			}
			/*take the next subscriber from the sub list related to the event*/
			ptagSubList=ptagSubList->ptagNext;
		}
		/*discard event after processed by all subscribers*/
		if(pEvent->pvPayload) free(pEvent->pvPayload);
		free(pEvent);
		EventOS_updateLifeTime();
		pEvent = EventOS_getEvent();
	}
}

/*
	EventOs update lifetime. This function check event lifetime and updates the priority

    @param void
    @return void
    @author Gabriel
    @date   05/01/2017
*/
__PRIVATE_ void EventOS_updateLifeTime (void)
{
	ttag_EventNode* ptagEventListAux = NULL;
	portUCHAR ucPriority = EVENTOS_PRIORITY_MEDIUM;

	EventOS_ptagEventOS->ulProcessStamp++;

	vPortDisableInterrupts();

	while(ucPriority < EVENTOS_PRIORITY_LAST)
	{
		ptagEventListAux = EventOS_ptagEventOS->ptagEventList[ucPriority]->ptagPrev;
		if(ptagEventListAux == EventOS_ptagEventOS->ptagEventList[ucPriority]) ucPriority++;
		else
		{
			if(((portULONG)(EventOS_ptagEventOS->ulProcessStamp - ptagEventListAux->ulLifeTimeCount)) >= EVENTOS_LIFE_TIME)
			{
				EventOS_printLog((portCHAR*)"[update] Event: %d / Priority: %d to Priority: %d", ptagEventListAux->ptagEvent->tagHeader.eEvent,ptagEventListAux->ptagEvent->tagHeader.ePriority, (ptagEventListAux->ptagEvent->tagHeader.ePriority - 1));
				/* Remove from lower priority list */
				(ptagEventListAux->ptagPrev)->ptagNext = (ptagEventListAux->ptagNext);
				(ptagEventListAux->ptagNext)->ptagPrev = (ptagEventListAux->ptagPrev);
				/* Update ctrl variables */
				ptagEventListAux->ptagEvent->tagHeader.ePriority--;
				ptagEventListAux->ulLifeTimeCount = EventOS_ptagEventOS->ulProcessStamp;
				/* Insert from higher priority list */
				ptagEventListAux->ptagNext = EventOS_ptagEventOS->ptagEventList[ucPriority - 1]->ptagNext;
				EventOS_ptagEventOS->ptagEventList[ucPriority - 1]->ptagNext->ptagPrev = ptagEventListAux;
				EventOS_ptagEventOS->ptagEventList[ucPriority - 1]->ptagNext = ptagEventListAux;
				ptagEventListAux->ptagPrev = EventOS_ptagEventOS->ptagEventList[ucPriority - 1];
			}
			else ucPriority++;
		}
	}

	vPortEnableInterrupts();
}

/*
	EventOS dequeue event. This is the method that dequeue the events in the publish queue.

    @param ttag_EventNode* ptagEventList
    @return void
    @author Amanda/Samuel
    @date   20/10/2014

*/
__PRIVATE_ port_EVENT_Type* EventOS_getEvent(void)
{
	if(!EventOS_ptagEventOS) return NULL;

	port_EVENT_Type* ptagEvent=NULL;
	ttag_EventNode* ptagEventListAux = NULL;
	portUCHAR ucPriority = EVENTOS_PRIORITY_HIGH;

	vPortDisableInterrupts();
	{
		/*check the event list with highest priority that is not empty*/
		while (ucPriority < EVENTOS_PRIORITY_LAST && ptagEventListAux == NULL)
		{
			ptagEventListAux = EventOS_ptagEventOS->ptagEventList[ucPriority]->ptagPrev;
			if(ptagEventListAux == EventOS_ptagEventOS->ptagEventList[ucPriority]) ptagEventListAux = NULL;
			ucPriority++;
		}
		if(ptagEventListAux)
		{
			(ptagEventListAux->ptagPrev)->ptagNext = (ptagEventListAux->ptagNext);
			(ptagEventListAux->ptagNext)->ptagPrev = (ptagEventListAux->ptagPrev);
			ptagEvent = ptagEventListAux->ptagEvent;
			free(ptagEventListAux);
		}
	}
	vPortEnableInterrupts();

	return ptagEvent;
}

/**
	Get message. This method returns an allocated memory store new message.
	New message must be send to the messenger using IMessenger_postMessage()
	method.

    @param void
    @return port_EVENT_Type* - New event slot handler or NULL.
    @author: edielsonpf
  	@date: 08/05/2014
*/
port_EVENT_Type* EventOS_newEvent(portINTEGER iEvent, portINTEGER iPriority, void* pvData, portUINTEGER uiDataSize)
{
	if(!EventOS_ptagEventOS) return NULL;
	if(iEvent >= EVENTOS_EVENT_LAST) return NULL;
	if(iPriority >= EVENTOS_PRIORITY_LAST) return NULL;

	port_EVENT_Type* ptagNewEvent = NULL;

	/* create event slot to transmit a dynamic payload */
	ptagNewEvent = (port_EVENT_Type*)malloc(sizeof(port_EVENT_Type));
	if(ptagNewEvent)
	{
		ptagNewEvent->tagHeader.eEvent = iEvent;
		ptagNewEvent->tagHeader.ePriority = iPriority;
		ptagNewEvent->pvPayload = NULL;
		if(uiDataSize > 0 && pvData != NULL)
		{
			ptagNewEvent->pvPayload = malloc(uiDataSize);
			if(ptagNewEvent->pvPayload) //Error
			{
				memcpy(ptagNewEvent->pvPayload,pvData,uiDataSize);
			}
			else
			{
				free(ptagNewEvent);
				ptagNewEvent = NULL;
			}
		}
	}
	EventOS_printLog((portCHAR*)"[new] Event: %d / Priority: %d", iEvent, iPriority);
	return ptagNewEvent;
}

/*
	Configure EventOS reference date time.

    @param pointer date time struct
    @return void
    @author gabriels
    @date   02/09/2014
*/
void    EventOS_setDateTime(portRTC_TIME_Type* ptagDateTime)
{
	portSET_DATE_TIME(ptagDateTime);
}
/*
	Get EventOS reference date time.

    @param pointer date time struct
    @return void
    @author gabriels
    @date   02/09/2014
*/
void    EventOS_getDateTime(portRTC_TIME_Type* ptagDateTime)
{
	portGET_DATE_TIME(ptagDateTime);
}

/**
	Print log in console or ethernet

    @param Log Facility
    @param Log Severity
    @param message
    @return void
    @author gabriels
    @date   15/10/2014
*/
void  EventOS_printLog(const portCHAR* cString, ...)
{
	#ifdef EVENTOS_DISABLE_LOG

	#else
		portRTC_TIME_Type tagDateTime;
		portULONG  ulMsgSize;
		portCHAR    cMessage[EVENTOS_LOG_PACKET_SIZE];


		va_list arguments;
		va_start( arguments, cString );
		EventOS_getDateTime(&tagDateTime);
		ulMsgSize = snprintf((char*)cMessage, EVENTOS_LOG_PACKET_SIZE,
						"%4.4ld-%2.2ld-%2.2ld %2.2ld:%2.2ld:%2.2ld [EventOS] ",
						tagDateTime.YEAR, tagDateTime.MONTH, tagDateTime.DOM,
						tagDateTime.HOUR, tagDateTime.MIN, tagDateTime.SEC);
		ulMsgSize += vsnprintf((char*)&cMessage[ulMsgSize], (EVENTOS_LOG_PACKET_SIZE-ulMsgSize), (char*)cString, arguments);
		snprintf((char*)&cMessage[ulMsgSize], (EVENTOS_LOG_PACKET_SIZE-ulMsgSize), "\n");
		portPRINT_LOG_CONSOLE((char*)cMessage);
	#endif
}

/**
	Get EventOS version

    @param void
    @return pointer to the version string
    @author edielsonpf
    @date   19/06/2015
*/
portCHAR*  EventOS_getVersion(void)
{
	return (portCHAR*) EVENTOS_VERSION;
}
