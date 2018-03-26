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
	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES, LOG_SEVERITY_INFORMATIONAL, (char*)pxEvent_getVersion());

	Application_new();

	vEvent_startScheduler();
	/*Should never reach here*/
	return 0;
}
