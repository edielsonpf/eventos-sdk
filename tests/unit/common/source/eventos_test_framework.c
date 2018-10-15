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
#include <stdio.h>
#include <string.h>

/* EventOS includes. */
#include "EventOS.h"
#include "event.h"

/* Test framework include */
#include "eventos_test_framework.h"

/* Unity include */
#include "unity.h"

/* Support libraries includes. */
#include "eventos_log.h"

#define TEST_RESULT_BUFFER_CAPACITY    1024

/*-----------------------------------------------------------*/

/* The buffer to store test result. The content will be printed if an eol character
 * is received */
static char pcTestResultBuffer[ TEST_RESULT_BUFFER_CAPACITY ];
static int16_t xBufferSize = 0;

/*-----------------------------------------------------------*/

void TEST_CacheResult( char cResult )
{
    if( TEST_RESULT_BUFFER_CAPACITY - xBufferSize == 2 )
    {
        cResult = '\n';
    }

    pcTestResultBuffer[ xBufferSize++ ] = cResult;

    if( ( '\n' == cResult ) )
    {
        TEST_SubmitResultBuffer();
    }
}
/*-----------------------------------------------------------*/

void TEST_SubmitResultBuffer()
{
    if( 0 != xBufferSize )
    {
        TEST_SubmitResult( pcTestResultBuffer );
        memset( pcTestResultBuffer, 0, TEST_RESULT_BUFFER_CAPACITY );
        xBufferSize = 0;
    }
}
/*-----------------------------------------------------------*/

void TEST_NotifyTestStart()
{
    /* Wait for test script to open serial port before starting tests on the
     * board.*/
    TEST_SubmitResult( "---------STARTING TESTS---------\n" );
}
/*-----------------------------------------------------------*/

void TEST_NotifyTestFinished()
{
    TEST_SubmitResult( "-------ALL TESTS FINISHED-------\n" );
}
/*-----------------------------------------------------------*/

void TEST_SubmitResult( const char * pcResult )
{
    /* We want to print test result no matter configPRINTF is defined or not */
    Log_print( LOG_FACILITY_USER_LEVEL_MESSAGES,
    		   LOG_SEVERITY_INFORMATIONAL,
			   pcResult );
}
