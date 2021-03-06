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

#include <stdlib.h>
#include <stdio.h>

#include "EventOS.h"
#include "event.h"

#include "application.h"

int main()
{
	/* Check inside Application how events
	 * are registered and managed*/
	Application_initialize();

	vEvent_startScheduler();
	/*Should never reach here*/
	return 0;
}
