/*
 * main.c
 *
 *  Created on: 05/09/2014
 *      Author: edielson
 */
#include "EventOS.h"
#include <stdlib.h>
#include <stdio.h>


#include "UartEvent.h"
#include "SystickEvent.h"
#include "Log.h"

int main()
{
	EventOS_createInstance();

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES, LOG_SEVERITY_INFORMATIONAL, (char*)EventOS_getVersion());

	SystickEvent_new();
	UartEvent_new();

	EventOS_startScheduler();
	/*Should never reach here*/
	return 0;
}
