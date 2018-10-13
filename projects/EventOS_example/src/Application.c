
#include <cr_section_macros.h>
#include <NXP/crp.h>
#include <stdio.h>

#include "event.h"

#include "Application.h"
#include "Log.h"

#include "leds.h"
#include "light.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"


/*********************************************************
    private definition.
*********************************************************/

unsigned long App_ulSystickKey = 0;
unsigned long App_ulLightKey = 0;
unsigned long App_ulLedKey = 0;
unsigned long App_ID = 0x10;
unsigned long App_ulTicks = 0; // counter for 1ms Applications
void* App_pvHandler = NULL;

/*********************************************************
    private operations
*********************************************************/


static void Application_initLed(void);

static void Application_initSysTick(void);

static void Application_initLight(void);

static void Application_receiveLightEvent(unsigned portBASE_TYPE EventKey,
										  void* pvHandler,
										  void* pvPayload,
										  unsigned portBASE_TYPE ulPayloadSize);

static void Application_receiveSystickEvent(unsigned portBASE_TYPE EventKey,
											void* pvHandler,
											void* pvPayload,
											unsigned portBASE_TYPE ulPayloadSize);

static void Application_receiveLedEvent(unsigned portBASE_TYPE EventKey,
										void* pvHandler,
										void* pvPayload,
										unsigned portBASE_TYPE ulPayloadSize);

/*********************************************************
    public implementations
*********************************************************/

void Application_initialize(void)
{

	App_pvHandler = &App_ID; /*Used just to identify the owner of the events in this application*/

	Application_initSysTick();
	Application_initLight();
	Application_initLed();

	/*Example in how to register a new event to the system.*/
	xEvent_register(App_pvHandler,		/* Handler of event owner. Only event owner can publish
										   a registered event.*/
					"EVENT_SYSTICK",    /* Text name for the event. Used to generate event key*/
					&App_ulSystickKey); /* Pointer to store the generated event key */

	xEvent_subscribe(Application_receiveSystickEvent, /* Pointer to the function that handles the event*/
				     App_ulSystickKey, 	/* Event key on which you want to subscribe*/
					 App_pvHandler); 	/* Subscriber handler.*/

	xEvent_register(App_pvHandler,
					"EVENT_LIGHT_READ",
					&App_ulLightKey);

	xEvent_subscribe(Application_receiveLightEvent,
					 App_ulLightKey,
					 App_pvHandler);

	xEvent_register(App_pvHandler,
					"EVENT_LET_TOGLE",
					&App_ulLedKey);

	xEvent_subscribe(Application_receiveLedEvent,
					 App_ulLedKey,
					 App_pvHandler);
}

/*********************************************************
    private implementations
*********************************************************/


static void Application_initLight(void)
{
	PINSEL_CFG_Type PinCfg;

	/* Initialize I2C2 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000);

	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);

	light_enable();
}

static void Application_initSysTick(void)
{

	// Setup Systick Timer to interrupt at 1 msec intervals
	if (SysTick_Config(SystemCoreClock/1000))
	{
	    while (1);  // Capture error
	}
}

static void Application_initLed(void)
{
	led2_init();
	led2_on();
}


static void Application_receiveLightEvent(unsigned portBASE_TYPE EventKey,
		  	  	  	  	  	  	  	  	  void* pvHandler,
										  void* pvPayload,
										  unsigned portBASE_TYPE ulPayloadSize)
{
	if(pvPayload == NULL) return;
	if(EventKey != App_ulLightKey) return; /*security check*/
	if(pvHandler == NULL) return;
	if(pvHandler != App_pvHandler) return;

	if(ulPayloadSize > 0)
	{
		unsigned long ulLight = *(unsigned long*) pvPayload;

		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
				LOG_SEVERITY_INFORMATIONAL,
				"[app] Receiving new event from EventOS (Light)");

		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
				  LOG_SEVERITY_INFORMATIONAL,
				  "[app] Light: %ul",
				  ulLight);
	}
}

static void Application_receiveSystickEvent(unsigned portBASE_TYPE EventKey,
											void* pvHandler,
											void* pvPayload,
											unsigned portBASE_TYPE ulPayloadSize)
{
	if(pvPayload == NULL) return;
	if(EventKey != App_ulSystickKey) return; /*security check*/
	if(pvHandler == NULL) return;
	if(pvHandler != App_pvHandler) return;

	if(ulPayloadSize > 0)
	{
		unsigned long ulTicks = *(unsigned long*) (pvPayload);
		unsigned long ulLight = 0;

		if(ulTicks % 100 == 0)
		{
			Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
					LOG_SEVERITY_INFORMATIONAL,
					"[app] Ticks %ul. Publishing new event: Led", ulTicks);

			/*Publish the new light value for respective subscribers*/
			xEvent_publish(App_pvHandler,
						   App_ulLedKey,
						   EVENT_PRIORITY_MEDIUM,
						   NULL,
						   0);
		}
		if(ulTicks % 500 == 0)
		{
			Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
					LOG_SEVERITY_INFORMATIONAL,
					"[app] Ticks %ul. Publishing new event: Light", ulTicks);

			/*Read a new light value from sensor*/
			ulLight = light_read();

			/*Publish the new light value for respective subscribers*/
			xEvent_publish(App_pvHandler,
						   App_ulLightKey,
						   EVENT_PRIORITY_LOW,
						   &ulLight,
						   sizeof(unsigned long));
		}
	}
}

static void Application_receiveLedEvent(unsigned portBASE_TYPE EventKey,
										void* pvHandler, /*not used*/
										void* pvPayload, /*not used*/
										unsigned portBASE_TYPE ulPayloadSize /*not used*/)
{
	if(EventKey != App_ulLedKey) return;
	if(pvHandler == NULL) return;
	if(pvHandler != App_pvHandler) return;

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
			  LOG_SEVERITY_INFORMATIONAL,
			  "[app] Receiving new event from EventOS (Led)");

	led2_invert();
}


void SysTick_Handler(void)
{
	App_ulTicks++;

	/* On every systick interrupt, a new event
	 * is generated with high priority and with
	 * the actual tick count*/
	xEvent_publish( App_pvHandler, 			/*Publisher handler. Indicates the event publisher.*/
					App_ulSystickKey,		/* Event key to be published.*/
					EVENT_PRIORITY_HIGH,	/* Event priority. Here will be published with high priority*/
					&App_ulTicks,			/* Content to be published */
					sizeof(App_ulTicks));	/* Content size */
}
