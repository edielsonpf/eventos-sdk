/*
 * main.c
 *
 *  Created on: 05/09/2014
 *      Author: edielson
 */
#include <stdlib.h>
#include <stdio.h>

#include "EventOS.h"
#include "event.h"


#include "Application.h"
#include "Log.h"

int main()
{
	/* Check inside Application how events
	 * are registered and managed*/
	Application_initialize();

	vEvent_startScheduler();
	/*Should never reach here*/
	return 0;
}
