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

/* Application specific configuration options. */
#include "EventOSConfig.h"
/* Definitions specific to the port being used. */
#include "portable.h"

#define EVENTOS_LIFE_TIME		3

/*********************************************************
    Public Operations
*********************************************************/

void				EventOS_createInstance(void);
void 				EventOS_deleteInstance(void);
void 				EventOS_startScheduler(void);
void 				EventOS_subscribe (portEVENT_EVENT_List eEvent, void* hHandle, pdEventHandlerFunction pFunction);
void				EventOS_publish (portEVENT_EVENT_Type* ptagEvent);
portEVENT_EVENT_Type* 	EventOS_newEvent(portINTEGER iEvent, portINTEGER iPriority, void* pvData, portUINTEGER uiDataSize);
void    			EventOS_setDateTime(portRTC_TIME_Type* ptagDateTime);
void    			EventOS_getDateTime(portRTC_TIME_Type* ptagDateTime);
portCHAR*			EventOS_getVersion(void);

/*-----------------------------------------------------------
 * SCHEDULER INTERNALS AVAILABLE FOR PORTING PURPOSES
 * documented in EventOS.h
 *----------------------------------------------------------*/
void 			EventOS_processEvents (void);

#endif /* EVENTOS_H_ */
