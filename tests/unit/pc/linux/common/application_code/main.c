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

/*********************************************************
    private definition.
*********************************************************/


/*********************************************************
    private prototypes.
*********************************************************/


/*********************************************************
    private prototypes.
*********************************************************/

/**
 * @brief Performs board and logging initializations,
 * then starts the OS.
 *
 * @return This function should return.
 */
int main()
{
	vEvent_startScheduler();

	/*Should never reach here*/
	return 0;
}

void vApplicationIdleHook(void)
{
	/* Execute the tests*/
	TEST_RUNNER_RunTests();

	/* The ApplicationIdleHook function should never terminate */
	while(1);
}
