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

/*-----------------------------------------------------------
 * Implementation of functions defined in port.h for the Linux port.
 *----------------------------------------------------------*/
#include <stdio.h>

/* Scheduler includes. */
#include "EventOS.h"
#include "event.h"


/* Port specif includes*/
#include<wait.h> 
#include<signal.h> 

/*
 * The signal id used for simulating interruptions.
 */
#define	portUSER_SIGNAL	SIGUSR1

/* Simulated interrupts using signals.  This is the process pid which is
 * used togeneate asimulatd interruption. */
static volatile uint32_t ulProcessPid = 0UL;

/* Kill calls passing portUSER_SIGNAL will generate an simulated interrupt
 * which will be handled by this function. We a reusing it to simulate
 * PendSV interruption calls on ARM */
static void prvInterruptionHandler(int SignalId); 


/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
	/* Extra code to configure simulated interruptions. */	
	ulProcessPid = getpid();
    
    	signal(portUSER_SIGNAL, prvInterruptionHandler); 	

#if (configUSE_SLEEP_MODE == 1)
	/* Put the system in sleep mode. */
	vPortEnterSleepMode();
#else

	/* If not using power sleep mode, we can call an user
	 * function to handle an idle task or keep the code
	 * executing */
	vEvent_idleTask();

	/* We should guarantee that the code will
	 * never terminate if running in a microcontroller
	 * and not running in power sleep mode or handled by
	 * vEvent_idleTask */
	//while(1); // Not applicable for Linux simulation since the unit test must be finished.
#endif
	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

/**
	Configure

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortEnterSleepMode( void )
{
	/* Enter in sleep on exit mode. */
	/* Do not implement */
}

/*-----------------------------------------------------------*/

void vPortGenerateEvent( void )
{
	/*Generate an interrupt for treating a published event*/
	kill(ulProcessPid, portUSER_SIGNAL); 	
}

/**
	Masks all interrupts for execution of a critical block

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortDisableInterrupts(void)
{
	/* That resets the signal handler back to whatever 
         * the default behavior was for that signal 
         */
       signal(portUSER_SIGNAL, SIG_DFL);
}

/**
	Unmasks all interrupts.

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void vPortEnableInterrupts(void)
{
	signal(portUSER_SIGNAL, prvInterruptionHandler);
}


/**
	Handles PendSV interrupts and call function to process even queue.

    @param void
    @return void
    @author gabriels
    @date   25/09/2014
*/
void xPortPendSVHandler(void)
{
	vEvent_processEvents();
}



static void prvInterruptionHandler(int SignalId) 
{ 
    if( SignalId == portUSER_SIGNAL )
    {
	xPortPendSVHandler();
    } 
} 
