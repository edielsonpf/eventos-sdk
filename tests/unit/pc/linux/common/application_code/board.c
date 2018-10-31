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

#include <stdio.h>

#include "event.h"
#include "board.h"

/*********************************************************
    private definition.
*********************************************************/



/*********************************************************
    private operations
*********************************************************/

static void Board_initSysTick(void);


/*********************************************************
    public implementations
*********************************************************/

void Board_initialize(void)
{
	Board_initSysTick();
}

/*********************************************************
    private implementations
*********************************************************/

static void Board_initSysTick(void)
{
	// Setup Systick Timer to interrupt at 1 msec intervals
	if (SysTick_Config(SystemCoreClock/1000))
	{
	    while (1);  // Capture error
	}
}
