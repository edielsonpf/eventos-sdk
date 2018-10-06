
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
unsigned long App_Handler = 0x10;
unsigned long App_ulTicks = 0; // counter for 1ms Applications

/*********************************************************
    private operations
*********************************************************/


static void Application_initLed(void);

static void Application_initSysTick(void);

static void Application_initLight(void);

static void Application_receiveLightEvent(portBASE_TYPE EventType,
										  void* pvHandler,
										  void* pvPayload,
										  portBASE_TYPE xPayloadSize);

static void Application_receiveSystickEvent(portBASE_TYPE EventType,
											void* pvHandler,
											void* pvPayload,
											portBASE_TYPE XPayloadSize);

static void Application_receiveLedEvent(portBASE_TYPE EventType,
										void* pvHandler,
										void* pvPayload,
										portBASE_TYPE XPayloadSize);

/*********************************************************
    public implementations
*********************************************************/

void Application_initialize(void)
{
	Application_initSysTick();
	Application_initLight();
	Application_initLed();

	/*Example in how to register a new event to the system.*/

	xEvent_register(&App_Handler,		/* Handler of event owner. Only event owner can publish
										   a respective event.*/
					"EVENT_SYSTICK",    /* Text name for the event. Used to generate event key*/
					&App_ulSystickKey); /* Pointer to store the generated event key */

	xEvent_subscribe(Application_receiveSystickEvent, /* Pointer to the function that handles the event*/
				     App_ulSystickKey, 	/* Event key on which you want to subscribe*/
					 &App_Handler); 	/* Subscriber handler.*/

	xEvent_register(&App_Handler,
					"EVENT_LIGHT_READ",
					&App_ulLightKey);

	xEvent_subscribe(Application_receiveLightEvent,
					 App_ulLightKey,
					 &App_Handler);

	xEvent_register(&App_Handler,
					"EVENT_LET_FLIP",
					&App_ulLedKey);

	xEvent_subscribe(Application_receiveLedEvent,
					 App_ulLedKey,
					 &App_Handler);
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


static void Application_receiveLightEvent(portBASE_TYPE EventType, void* pvHandler, void* pvPayload, portBASE_TYPE XPayloadSize)
{
	if(pvPayload == NULL) return;

	unsigned long ulLight = *(unsigned long*) pvPayload;

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
			LOG_SEVERITY_INFORMATIONAL,
			"[app] Receiving new event from EventOS (Light)");

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
			  LOG_SEVERITY_INFORMATIONAL,
			  "[app] Light: %ul)",
			  ulLight);
}

static void Application_receiveSystickEvent(portBASE_TYPE EventType, void* pvHandler, void* pvPayload, portBASE_TYPE XPayloadSize)
{
	if(pvPayload == NULL) return;

	unsigned long ulTicks = *(unsigned long*) (pvPayload);
	unsigned long ulLight = 0;

	if(ulTicks % 1000 == 0)
	{
		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
				LOG_SEVERITY_INFORMATIONAL,
				"[app] Publishing new event: Led");

		/*Publish the new light value for respective subscribers*/
		xEvent_publish(&App_Handler,
				App_ulLedKey,
				EVENT_PRIORITY_MEDIUM,
				NULL,
				0);
	}
	if(ulTicks % 5000 == 0)
	{
		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
				LOG_SEVERITY_INFORMATIONAL,
				"[app] Publishing new event: Light");

		/*Read a new light value from sensor*/
		ulLight = light_read();

		/*Publish the new light value for respective subscribers*/
		xEvent_publish(&App_Handler,
				App_ulLightKey,
				EVENT_PRIORITY_LOW,
				&ulLight,
				sizeof(unsigned long));
	}
}

static void Application_receiveLedEvent(portBASE_TYPE EventType, void* pvHandler, void* pvPayload, portBASE_TYPE XPayloadSize)
{
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
	xEvent_publish(&App_Handler, 			/*Publisher handler. Indicates the event publisher.*/
					App_ulSystickKey,		/* Event key to be published.*/
					EVENT_PRIORITY_HIGH,	/* Event priority. Here will be published with high priority*/
					&App_ulTicks,			/* Content to be published */
					sizeof(App_ulTicks));	/* Content size */
}
