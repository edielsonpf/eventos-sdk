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


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure EventOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portINTEGER		short int
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	unsigned portLONG
#define portBASE_TYPE	long

typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

#define pdTRUE		( 1 )
#define pdFALSE		( 0 )
#define pdPASS		pdTRUE
#define pdFAIL		pdFALSE


#define __HEADER_						volatile
#define __PRIVATE_						static

/* Scheduler utilities. */
extern void		vPortGenerateEvent( void );
extern void		vPortEnterSleepMode( void );
extern void     vPortPrintLog(char* pcMessage);
extern void     vPortDisableInterrupts(void);
extern void     vPortEnableInterrupts(void);

#define portGENERATE_EVENT()					vPortGenerateEvent()
#define portSLEEP_MODE()						vPortEnterSleepMode()
#define portPRINT_LOG_CONSOLE(pMessage)         vPortPrintLog(pMessage)
#define portDISABLE_INTERRUPTS()				vPortDisableInterrupts()
#define portENABLE_INTERRUPTS()					vPortEnableInterrupts()
/*-----------------------------------------------------------*/

/*
 * Map to the memory management routines required for the port.
 */
#define pvPortMalloc( xSize ) malloc( xSize );
#define vPortFree( pvMemory ) free( pvMemory );

///* Critical section management. */


#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

