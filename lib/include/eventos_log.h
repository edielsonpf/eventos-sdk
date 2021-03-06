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

#ifndef LOG_H_
#define LOG_H_


#include <stdint.h>

/*********************************************************
    public constants.
*********************************************************/

/* Define log output by console or ethernet */
#define LOG_CONSOLE
//#define LOG_DISABLE

/*********************************************************
    public types.
*********************************************************/
/*
 * As described in RFC5424
 * Facility table
 */
typedef enum enu_LogFacility
{
	LOG_FACILITY_KERNEL_MESSAGE	= 0,
	LOG_FACILITY_USER_LEVEL_MESSAGES,
	LOG_FACILITY_MAIL_SYSTEM,
	LOG_FACILITY_SYSTEM_DAEMONS,
	LOG_FACILITY_SECURITY_MESSAGES,
	LOG_FACILITY_INTERNAL_SYSLOG,
	LOG_FACILITY_LINE_PRINTER,
	LOG_FACILITY_NETWORK_NEWS,
	LOG_FACILITY_UUCP,
	LOG_FACILITY_CLOCK_DAEMON,
	LOG_FACILITY_AUTHORIZATION_MESSAGES,
	LOG_FACILITY_FTP_DAEMON,
	LOG_FACILITY_NTP,
	LOG_FACILITY_LOG_AUDIT,
	LOG_FACILITY_LOG_ALERT,
	LOG_FACILITY_CLOCK_2,
	LOG_FACILITY_LOCAL0,
	LOG_FACILITY_LOCAL1,
	LOG_FACILITY_LOCAL2,
	LOG_FACILITY_LOCAL3	,
	LOG_FACILITY_LOCAL4	,
	LOG_FACILITY_LOCAL5	,
	LOG_FACILITY_LOCAL6	,
	LOG_FACILITY_LOCAL7	,
}tenu_LogFacility;


/*
 * As described in RFC5424
 * Severity table
 */

typedef enum enu_LogSeverity
{
	LOG_SEVERITY_EMERGENCY = 0,
	LOG_SEVERITY_ALERT,
	LOG_SEVERITY_CRITICAL,
	LOG_SEVERITY_ERROR,
	LOG_SEVERITY_WARNING,
	LOG_SEVERITY_NOTICE,
	LOG_SEVERITY_INFORMATIONAL,
	LOG_SEVERITY_DEBUG,
}tenu_LogSeverity;


/*********************************************************
    Public operations.
*********************************************************/
void		Log_init(void);
void        Log_print(tenu_LogFacility enuFacility, tenu_LogSeverity enuSeverity, const char* cString, ...);


#endif /* LOG_H_ */
