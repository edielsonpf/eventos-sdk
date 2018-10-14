
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "eventos_log.h"


#ifdef LOG_CONSOLE

#else

#endif

/*********************************************************
    private constants.
*********************************************************/

#define LOG_PACKET_SIZE							300   /*  */

#ifdef LOG_CONSOLE
	#define LOG_IP_REFERENCE					0xC0A80101  /* 192.168.1.1 */
#endif

/*********************************************************
    private types.
*********************************************************/


/*********************************************************
    private variable.
*********************************************************/

/*********************************************************
    private operations.
*********************************************************/
unsigned long Log_getDateTime(char* pcDateTimeFormat, unsigned long ulCurrentSize);
unsigned long Log_getHostIp(char* pcIpFormat, unsigned long ulCurrentSize);


/**
	Get date time in application, and return string in syslog format

    @param pointer to save string
    @param Current string size
    @return string size
    @author gabriels
    @date   15/10/2014
*/
unsigned long Log_getDateTime(char* pcDateTimeFormat, unsigned long ulCurrentSize)
{
	unsigned char ucSecond = 0;
	unsigned char ucMinute = 0;
	unsigned char ucHour = 12;
	unsigned char ucDay = 23;
	unsigned char ucMonth = 10;
	unsigned char ucYear = 14;

	unsigned long ulSize;

	/* Insert rtc get time */


	ulSize = snprintf((char*)pcDateTimeFormat, (LOG_PACKET_SIZE - ulCurrentSize),"%2.2ld-%2.2ld-%2.2ld %2.2ld:%2.2ld:%2.2ld ",
			ucYear, ucMonth, ucDay, ucHour, ucMinute, ucSecond);

	return ulSize;

}
/**
	Get host ip in application, and return string ip in syslog format

    @param pointer to save string
    @param Current string size
    @return string size
    @author gabriels
    @date   15/10/2014
*/
unsigned long Log_getHostIp(char* pcIpFormat, unsigned long ulCurrentSize)
{
	unsigned long ulSize;
	unsigned char ucIp[4] = {192, 168, 1,1};

	/* Insert get host ip */

	ulSize = snprintf((char*)pcIpFormat, (LOG_PACKET_SIZE - ulCurrentSize),"%d.%d.%d.%d ",
				ucIp[0], ucIp[1], ucIp[2], ucIp[3]);

	return ulSize;
}


/*********************************************************
    Operations implementation
*********************************************************/
/**
	Initialize log app

    @param void
    @return void
    @author gabriels
    @date   15/10/2014
*/
void Log_init(void)
{
	#ifdef LOG_CONSOLE

	#else

	#endif
}

/**
	Print log in console or ethernet

    @param Log Facility
    @param Log Severity
    @param message
    @return void
    @author gabriels
    @date   15/10/2014
*/
void  Log_print(tenu_LogFacility enuFacility, tenu_LogSeverity enuSeverity, const char* cString, ...)
{
	unsigned long  ulMsgSize;
	char    cMessage[LOG_PACKET_SIZE];

	#ifdef LOG_DISABLE

	#else
		va_list arguments;
		va_start( arguments, cString );

		ulMsgSize = snprintf((char*)cMessage, LOG_PACKET_SIZE,"<%d> ", ((enuFacility*8) + enuSeverity));
		ulMsgSize += Log_getDateTime((char*)&cMessage[ulMsgSize], ulMsgSize);
		ulMsgSize += Log_getHostIp((char*)&cMessage[ulMsgSize], ulMsgSize);
		ulMsgSize += vsnprintf((char*)&cMessage[ulMsgSize], (LOG_PACKET_SIZE-ulMsgSize), (char*)cString, arguments);

		#ifdef LOG_CONSOLE
			snprintf((char*)&cMessage[ulMsgSize], (LOG_PACKET_SIZE - ulMsgSize),(char*)"\n");
			printf((char*)cMessage);
		#else

		#endif
	#endif
}

