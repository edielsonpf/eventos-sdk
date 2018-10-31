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

#ifndef EVENTOSCONFIG_H_
#define EVENTOSCONFIG_H_


/* Ensure stdint is only used by the compiler, and not the assembler. */

#if defined( __ICCARM__ ) || defined( __ARMCC_VERSION ) || defined( __GNUC__ )

    #include <stdint.h>

    extern uint32_t SystemCoreClock;

#endif

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 *----------------------------------------------------------*/

#define configUSE_SLEEP_MODE				0


/* The maximun number of times one event stays in the same priority queue
 * (avoid starvation). */
#define configEVENTOS_LIFE_TIME				( 3 )

/* The maximun name lenght for an event. */
#define configMAX_EVENT_NAME_LEN			( 25 )

/* The maximun number of events can be stored in the kernel. */
#define configMAX_NUM_EVENTS				( 256 )

#endif /* EVENTOSCONFIG_H_ */
