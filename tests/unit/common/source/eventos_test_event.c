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
static unsigned long TEST_event_key = 0;
static unsigned long TEST_event_id = 0x01;
static void* TEST_event_pvHandler = NULL;

/*
 * @brief Test group definition.
 */
TEST_GROUP( Full_EVENTOS_EVENT );

TEST_SETUP( Full_EVENTOS_EVENT )
{
	TEST_event_pvHandler = &TEST_event_id; /*Used to identify the owner of event */
}

TEST_TEAR_DOWN( Full_EVENTOS_EVENT )
{
}

TEST_GROUP_RUNNER( Full_EVENTOS_EVENT )
{
	/* Run publish before initialize structure. */
	RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishEventBeforeRegister )

    /* Run register tests. */
    RUN_TEST_CASE( Full_EVENTOS_EVENT, RegisterEventWithNullString );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, RegisterEventTwice );

    /* Run publish tests. */
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidEvent );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidPriority );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidOwner );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidPayload );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishValidEvent );
}


TEST( Full_EVENTOS_EVENT, RegisterEventWithNullString )
{
    uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "\0";
    uint32_t xResult = pdFAIL;

    /* Register with invalid event name should fail. */
    xResult = xEvent_register(TEST_event_pvHandler,			/* Handler of event owner. Only event owner can publish
										   	   	       	   	   a registered event.*/
					(portCHAR*) szEventName,  				/* Text name for the event. Used to generate event key*/
					(unsigned portBASE_TYPE*)&TEST_event_key);	/* Pointer to store the generated event key */


    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */

}

TEST( Full_EVENTOS_EVENT, RegisterEventTwice )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "TEST_EVENT_NAME";
	uint32_t xResult = pdFAIL;
	uint32_t ulEventKey = 0;

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_event_pvHandler,						/* Handler of event owner. Only event owner can publish
													   	   	   	   	   	   a registered event.*/
							  (portCHAR*) szEventName,  				/* Text name for the event. Used to generate event key*/
							  (unsigned portBASE_TYPE*)&TEST_event_key);	/* Pointer to store the generated event key */


	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );
	/* End test. */

	/* Second register with same valid name should fail. */
	xResult = xEvent_register(TEST_event_pvHandler,
							  (portCHAR*) szEventName,
							  (unsigned portBASE_TYPE*)&ulEventKey);


	TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
	/* End test. */
}


TEST( Full_EVENTOS_EVENT, PublishEventBeforeRegister )
{
    uint32_t xResult = pdFAIL;

    /* Publish before initialize (before a first register) should fail. */
    xResult = xEvent_publish(TEST_event_pvHandler,
    						 TEST_event_key,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */

}

TEST( Full_EVENTOS_EVENT, PublishInvalidEvent )
{
    uint32_t xResult = pdFAIL;
    uint32_t ulEventKey = TEST_event_key + 1; /*invalid event key*/

    /* Publish with invalid event key should fail. */
    xResult = xEvent_publish(TEST_event_pvHandler,
							 ulEventKey,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishInvalidPriority )
{
    uint32_t xResult = pdFAIL;
    uint32_t ulEventKey = TEST_event_key; /*Valid event key*/
    uint32_t ulEventPriority = EVENT_PRIORITY_LAST + 10; /*invalid priority */

    /* Publish with invalid priority should fail. */
    xResult = xEvent_publish(TEST_event_pvHandler,
							 ulEventKey,
							 ulEventPriority,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishInvalidOwner )
{
    uint32_t xResult = pdFAIL;
    uint32_t ulEventKey = TEST_event_key; /*Valid event key*/
    void* pvHandler = TEST_event_pvHandler + 1 ; /*Invalid event owner*/


    /* Publish with invalid event owner should fail. */
	xResult = xEvent_publish(NULL,
							 ulEventKey,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

	TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
	/* End test. */

    /* Publish with different event owner should fail. */
    xResult = xEvent_publish(pvHandler,
							 ulEventKey,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishInvalidPayload )
{
    uint32_t xResult = pdFAIL;
    uint32_t ulEventKey = TEST_event_key; /*Valid event key*/

    /* Publish with invalid payload should fail. */
    xResult = xEvent_publish(TEST_event_pvHandler,
							 ulEventKey,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 sizeof(xResult)); /*invalid size, which means that
							 	 	 	 	 	 there is a valid payload*/

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishValidEvent )
{
    uint32_t xResult = pdFAIL;
    uint32_t ulEventKey = TEST_event_key; /*valid event key*/

    /* Publish with valid owner, key, priority and payload should succeed. */
    xResult = xEvent_publish(TEST_event_pvHandler,
							 ulEventKey,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );
    /* End test. */
}
