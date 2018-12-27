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

void TestEventHandlerFunction(EventHandle_t pxEventHandle,
							  void* pvEventHandler,
							  void* pvBuffer,
							  uint32_t ulBufferSize);

/*
 * @brief Test group definition.
 */
TEST_GROUP( Full_EVENTOS_EVENT );

TEST_SETUP( Full_EVENTOS_EVENT )
{

}

TEST_TEAR_DOWN( Full_EVENTOS_EVENT )
{
}

TEST_GROUP_RUNNER( Full_EVENTOS_EVENT )
{
	/* Run publish before initialize structure. */
	RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishEventBeforeRegister )

    /* Run register tests. */
	RUN_TEST_CASE( Full_EVENTOS_EVENT, RegisterEventWithNullEventHandle );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, RegisterEventWithNullString );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, RegisterEventTwice );

    /* Run publish tests. */
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidEvent );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidPriority );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidOwner );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishInvalidContent );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, PublishValidEvent );

    /* Run subscribe tests. */
    RUN_TEST_CASE( Full_EVENTOS_EVENT, SubscribeEventBeforeRegister );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, SubscribeInvalidEvent );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, SubscribeInvalidFunction );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, SubscribeInvalidOwner );
    RUN_TEST_CASE( Full_EVENTOS_EVENT, SubscribeValidEvent );
}


TEST( Full_EVENTOS_EVENT, RegisterEventWithNullEventHandle )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "TEST_EVENT_NAME";
    uint32_t xResult = pdFAIL;
    unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

    /* Register with NULL event handle should pass. */
    xResult = xEvent_register(TEST_Handler,				/* Handler of event owner. Only event owner can publish a registered event.*/
							 (const char*) szEventName, /* Text name for the event. Used to generate event key*/
							  NULL);					/* Pointer to store the generated event handle */

    TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

    /* End test. */

}

TEST( Full_EVENTOS_EVENT, RegisterEventWithNullString )
{
    uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "\0";
    uint32_t xResult = pdFAIL;
    EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* Register with invalid event name should fail. */
    xResult = xEvent_register(TEST_Handler,					/* Handler of event owner. Only event owner can publish a registered event.*/
					(const char*) szEventName,  			/* Text name for the event. Used to generate event key*/
					(EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event handle */

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */

}

TEST( Full_EVENTOS_EVENT, RegisterEventTwice )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "RegisterEventTwice";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

	/* Second register with same valid name should fail. */
	xResult = xEvent_register(TEST_Handler,
							  (const char*) szEventName,
							  (EventHandle_t*)&TEST_event_handle);

	TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
	/* End test. */
}


TEST( Full_EVENTOS_EVENT, PublishEventBeforeRegister )
{
    uint32_t xResult = pdFAIL;
    EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

    /* Publish before initialize (before a first register) should fail. */
    xResult = xEvent_publish(TEST_Handler,
    						 TEST_event_handle,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */

}

TEST( Full_EVENTOS_EVENT, PublishInvalidEvent )
{
    uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "PublishInvalidEvent";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */


	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

	/*invalid event handle*/
	TEST_event_handle = &xResult;

	/* Publish with invalid event handle should fail. */
    xResult = xEvent_publish(TEST_Handler,
							 TEST_event_handle,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishInvalidPriority )
{

    uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "PublishInvalidPriority";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */
	uint32_t ulEventPriority = EVENT_PRIORITY_LAST + 10; /*invalid priority */


	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

    /* Publish with invalid priority should fail. */
    xResult = xEvent_publish(TEST_Handler,
							 TEST_event_handle,
							 ulEventPriority,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishInvalidOwner )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "PublishInvalidOwner";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */


	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

	void* pvHandler = TEST_Handler + 1 ; /*Invalid event owner*/

    /* Publish with invalid event owner should fail. */
	xResult = xEvent_publish(NULL,
							 TEST_event_handle,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

	TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );

    /* Publish with different event owner should fail. */
    xResult = xEvent_publish(pvHandler,
							 TEST_event_handle,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishInvalidContent )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "PublishInvalidContent";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

    /* Publish with invalid content should fail. */
    xResult = xEvent_publish(TEST_Handler,
							 TEST_event_handle,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 sizeof(xResult)); /*invalid size, which means that
							 	 	 	 	 	 there is a valid content*/
    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, PublishValidEvent )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "PublishValidEvent";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

    /* Publish with valid owner, key, priority and payload should succeed. */
    xResult = xEvent_publish(TEST_Handler,
							 TEST_event_handle,
							 EVENT_PRIORITY_HIGH,
							 NULL,
							 0);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, SubscribeEventBeforeRegister )
{
    uint32_t xResult = pdFAIL;
    EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

    /* Subscribe before register should fail. */
    xResult = xEvent_subscribe(TEST_Handler,
							   TEST_event_handle,
							   TestEventHandlerFunction);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */

}

TEST( Full_EVENTOS_EVENT, SubscribeInvalidEvent )
{
    uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "SubscribeInvalidEvent";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

	/*invalid event handle*/
	TEST_event_handle = &xResult;

	/* Publish with invalid event handle should fail. */
    xResult = xEvent_subscribe(TEST_Handler,
							   TEST_event_handle,
							   TestEventHandlerFunction);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, SubscribeInvalidFunction )
{

    uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "SubscribeInvalidFunction";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

	/* Publish with invalid priority should fail. */
	xResult = xEvent_subscribe(TEST_Handler,
							   TEST_event_handle,
							   NULL);

    TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, SubscribeInvalidOwner )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "SubscribeInvalidOwner";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

	/* Subscribe with invalid owner should fail. */
	xResult = xEvent_subscribe(NULL,
							   TEST_event_handle,
							   TestEventHandlerFunction);

	TEST_ASSERT_EQUAL_UINT32( xResult, pdFAIL );
    /* End test. */
}

TEST( Full_EVENTOS_EVENT, SubscribeValidEvent )
{
	uint8_t szEventName[configMAX_EVENT_NAME_LEN] = "SubscribeEventSuccess";
	uint32_t xResult = pdFAIL;

	EventHandle_t TEST_event_handle = NULL;
	unsigned long TEST_event_id = 0x01;
	void* TEST_Handler = &TEST_event_id; /*Used to identify the owner of event */

	/* First register with valid name should succeed. */
	xResult = xEvent_register(TEST_Handler,							/* Handler of event owner. Only event owner can publish a registered event.*/
							  (portCHAR*) szEventName,  			/* Text name for the event. Used to generate event key*/
							  (EventHandle_t*)&TEST_event_handle);	/* Pointer to store the generated event key */

	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );

	/* Publish with valid owner, key, priority and payload should succeed. */
	xResult = xEvent_subscribe(TEST_Handler,
							   TEST_event_handle,
							   TestEventHandlerFunction);
	TEST_ASSERT_EQUAL_UINT32( xResult, pdPASS );
    /* End test. */
}



void TestEventHandlerFunction(EventHandle_t pxEventHandle,
							  void* pvEventHandler,
							  void* pvBuffer,
							  uint32_t ulBufferSize)
{
	/*Do nothing*/

}
