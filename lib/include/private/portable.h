/*
    EventOS V1.2.0 - Copyright (C) 2014 Edielson Prevato Frigieri.

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

/*-----------------------------------------------------------
 * Portable layer API.  Each function must be defined for each port.
 *----------------------------------------------------------*/

#ifndef PORTABLE_H
#define PORTABLE_H

#include "portmacro.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Map to the memory management routines required for the port.
 */
void *pvPortMalloc( size_t xSize );
void vPortFree( void *pv );

/*
 * Setup the hardware ready for the scheduler to take control.  Must setup the system to sleep mode
 * and the control must be reached by a PendSV interruption.
 */
portBASE_TYPE 	xPortStartScheduler( void );

#ifdef __cplusplus
}
#endif

#endif /* PORTABLE_H */

