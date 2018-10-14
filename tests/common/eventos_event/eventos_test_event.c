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

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* EventOS includes. */
#include "EventOS.h"
#include "event.h"

/* Test includes. */
#include "unity_fixture.h"
#include "unity.h"

/**
 * @brief Configuration for this test group.
 */
unsigned long TEST_event_UnityKey = 0;
unsigned long TEST_event_ID = 0x01;
void* TEST_event_pvHandler = NULL;

/*
 * @brief Test group definition.
 */
TEST_GROUP( Full_EVENTOS_EVENT );

TEST_SETUP( Full_EVENTOS_EVENT )
{
	TEST_event_pvHandler = &TEST_event_ID; /*Used just to identify the owner of
											the events in  the test framework*/
}

TEST_TEAR_DOWN( Full_EVENTOS_EVENT )
{
}

TEST_GROUP_RUNNER( Full_EVENTOS_EVENT )
{
    /* Run a parser test. */
    RUN_TEST_CASE( Full_EVENTOS_EVENT, RegisterEventWithNullString );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, RegisterEventTwice );

}

TEST( Full_EVENTOS_EVENT, RegisterEventWithNullString )
{
    uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "\0";
    uint32_t xResult = pdFAIL;
    uint32_t ulEventKey = 0;

    /* Register with invalid event name should fail. */
    xResult = xEvent_register(TEST_event_pvHandler,			/* Handler of event owner. Only event owner can publish
										   	   	       	   	   a registered event.*/
					(portCHAR*) szEventName,  				/* Text name for the event. Used to generate event key*/
					(unsigned portBASE_TYPE*)&ulEventKey);	/* Pointer to store the generated event key */


    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */

}

TEST( Full_EVENTOS_EVENT, RegisterEventTwice )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "EVENT_TEST_NAME";
	uint32_t xResult = pdFAIL;
	uint32_t ulEventKey = 0;

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_event_pvHandler,						/* Handler of event owner. Only event owner can publish
													   	   	   	   	   	   a registered event.*/
							  (portCHAR*) szEventName,  				/* Text name for the event. Used to generate event key*/
							  (unsigned portBASE_TYPE*)&ulEventKey);	/* Pointer to store the generated event key */


	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );
	/* End test. */

	/* Second register with same valid name should fail. */
	xResult = xEvent_register(TEST_event_pvHandler,						/* Handler of event owner. Only event owner can publish
																		   a registered event.*/
							  (portCHAR*) szEventName,  				/* Text name for the event. Used to generate event key*/
							  (unsigned portBASE_TYPE*)&ulEventKey);	/* Pointer to store the generated event key */


	TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
	/* End test. */
}
