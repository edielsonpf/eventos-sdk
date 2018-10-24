/*
    EventOS V1.0.1
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

/**
 * @file main.c
 * @brief Implements the main function.
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* EventOS include. */
#include "EventOS.h"
#include "event.h"

/* Test runner includes. */
#include "eventos_test_runner.h"

/* Unity includes. */
#include "unity.h"

/* Support includes */
#include "board.h"

/*********************************************************
    private definition.
*********************************************************/
static unsigned long TEST_main_key = 0;
static unsigned long TEST_main_id = 0x02;
static void* TEST_main_pvHandler = NULL;
static unsigned long TEST_main_sys_initialized = 0;

/*********************************************************
    private prototypes.
*********************************************************/
static void vTest_StartTestEvent( unsigned portBASE_TYPE EventKey,
		  	  	  	  	  	   	  void* pvHandler,
								  void* pvPayload,
								  unsigned portBASE_TYPE ulPayloadSize);


/*********************************************************
    private prototypes.
*********************************************************/

/**
 * @brief Performs board and logging initializations, then starts the OS.
 *
 * Functions that require the OS to be running
 * are handled in vTest_StartTestEvent().
 *
 * @sa vTest_StartTestEvent()
 *
 * @return This function should not return.
 */
int main()
{
	TEST_main_pvHandler = &TEST_main_id;

	Board_initialize();

	xEvent_register(TEST_main_pvHandler,		/* Handler of event owner.*/
					"EVENT_START_TESTS",    	/* Text name for the event. Used to generate event key*/
					&TEST_main_key); 			/* Pointer to store the generated event key */

	xEvent_subscribe(vTest_StartTestEvent, 		/* Pointer to the function that handles the event*/
					 TEST_main_key, 			/* Event key on which you want to subscribe*/
					 TEST_main_pvHandler);		/* Subscriber handler.*/

	vEvent_startScheduler();

	/*Should never reach here*/
	return 0;
}


/**
 * @brief Execute the tests.
 *
 *
 * @return This function returns  void.
 */
static void vTest_StartTestEvent( unsigned portBASE_TYPE EventKey,
								  void* pvHandler,
								  void* pvPayload,
								  unsigned portBASE_TYPE ulPayloadSize)
{
	if(EventKey != TEST_main_key) return; /*security check*/
	if(pvHandler == NULL) return;
	if(pvHandler != TEST_main_pvHandler) return;

	/* Execute the tests*/
	TEST_RUNNER_RunTests();
}


/**
 * @brief Handles systick interrupts. Used to publish an event to start the tests.
 *
 * This function is used to guarantee that the OS is running and
 * has been waked up by an interruption.
 *
 * @return This function returns void.
 */
void xPortSysTickHandler(void)
{
	if(TEST_main_sys_initialized == 0)
	{
		/*We are starting the tests once*/
		TEST_main_sys_initialized = 1;

		xEvent_publish(TEST_main_pvHandler, 		/* Pointer to the publisher */
					   TEST_main_key, 				/* Event key to be published */
					   EVENT_PRIORITY_HIGH,			/* Event priority */
					   NULL,						/* No payload*/
					   0); 							/* Payload size = 0 */
	}
}

