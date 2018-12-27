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

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include <stdio.h>

#include "EventOS.h"
#include "event.h"

#include "application.h"
#include "eventos_log.h"

#include "leds.h"
#include "light.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"


/*********************************************************
    private definition.
*********************************************************/

EventHandle_t App_SystickEventHandle;
EventHandle_t App_LightEventHandle;
EventHandle_t App_LedEventHandle;
unsigned long App_ID = 0x10;
unsigned long App_ulTicks = 0; // counter for 1ms Applications
void* App_Handler = NULL;

/*********************************************************
    private operations
*********************************************************/


static void Application_initLed(void);

static void Application_initSysTick(void);

static void Application_initLight(void);

static void Application_receiveLightEvent(EventHandle_t EventHandle,
										  void* pvHandler,
										  void* pvContent,
										  uint32_t ulContentSize);

static void Application_receiveSystickEvent(EventHandle_t EventHandle,
											void* pvHandler,
											void* pvContent,
											uint32_t ulContentSize);

static void Application_receiveLedEvent(EventHandle_t EventHandle,
										void* pvHandler,
										void* pvContent,
										uint32_t ulContentSize);

/*********************************************************
    public implementations
*********************************************************/

void Application_initialize(void)
{

	App_Handler = &App_ID; /*Used just to identify the owner of the events in this application*/

	Application_initSysTick();
	Application_initLight();
	Application_initLed();

	/*Example in how to register a new event to the system.*/
	xEvent_register(App_Handler,		/* Handler of event owner. Only event owner can publish
										   a registered event.*/
					"EVENT_SYSTICK",    /* Text name for the event. Used to generate event key*/
					&App_SystickEventHandle); /* Pointer to store the generated event key */

	xEvent_subscribe(App_Handler, 				/* Subscriber handler.*/
					 App_SystickEventHandle, 	/* Event key on which you want to subscribe*/
					 Application_receiveSystickEvent); /* Pointer to the function that handles the event*/


	xEvent_register(App_Handler,
					"EVENT_LIGHT_READ",
					&App_LightEventHandle);

	xEvent_subscribe(App_Handler,
					 App_LightEventHandle,
					 Application_receiveLightEvent);

	xEvent_register(App_Handler,
					"EVENT_LET_TOGLE",
					&App_LedEventHandle);

	xEvent_subscribe(App_Handler,
					 App_LedEventHandle,
					 Application_receiveLedEvent);
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


static void Application_receiveLightEvent(EventHandle_t EventHandle,
		  	  	  	  	  	  	  	  	  void* pvHandler,
										  void* pvContent,
										  uint32_t ulContentSize)
{
	if(pvContent == NULL) return;
	if(EventHandle != App_LightEventHandle) return; /*security check*/
	if(pvHandler == NULL) return;
	if(pvHandler != App_Handler) return;

	if(ulContentSize > 0)
	{
		unsigned long ulLight = *(unsigned long*) pvContent;

		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
				LOG_SEVERITY_INFORMATIONAL,
				"[app] Receiving new event from EventOS (Light)");

		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
				  LOG_SEVERITY_INFORMATIONAL,
				  "[app] Light: %ul",
				  ulLight);
	}
}

static void Application_receiveSystickEvent(EventHandle_t EventHandle,
											void* pvHandler,
											void* pvContent,
											uint32_t ulContentSize)
{
	if(pvContent == NULL) return;
	if(EventHandle != App_SystickEventHandle) return; /*security check*/
	if(pvHandler == NULL) return;
	if(pvHandler != App_Handler) return;

	if(ulContentSize > 0)
	{
		unsigned long ulTicks = *(unsigned long*) (pvContent);
		unsigned long ulLight = 0;

		if(ulTicks % 1000 == 0)
		{
			Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
					LOG_SEVERITY_INFORMATIONAL,
					"[app] Ticks %ul. Publishing new event: Led", ulTicks);

			/*Publish the new light value for respective subscribers*/
			xEvent_publish(App_Handler,
						   App_LedEventHandle,
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
			xEvent_publish(App_Handler,
						   App_LightEventHandle,
						   EVENT_PRIORITY_LOW,
						   &ulLight,
						   sizeof(unsigned long));
		}
	}
}

static void Application_receiveLedEvent(EventHandle_t EventHandle,
										void* pvHandler, /*not used*/
										void* pvContent, /*not used*/
										uint32_t ulContentSize /*not used*/)
{
	if(EventHandle != App_LedEventHandle) return;
	if(pvHandler == NULL) return;
	if(pvHandler != App_Handler) return;

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,
			  LOG_SEVERITY_INFORMATIONAL,
			  "[app] Receiving new event from EventOS (Led)");

	led2_invert();
}


void xPortSysTickHandler(void)
{
	App_ulTicks++;

	/* On every systick interrupt, a new event
	 * is generated with high priority and with
	 * the actual tick count*/
	xEvent_publish( App_Handler, 			/*Publisher handler. Indicates the event publisher.*/
					App_SystickEventHandle,		/* Event key to be published.*/
					EVENT_PRIORITY_HIGH,	/* Event priority. Here will be published with high priority*/
					&App_ulTicks,			/* Content to be published */
					sizeof(App_ulTicks));	/* Content size */
}

void vApplicationIdleHook(void)
{
	/* The ApplicationIdleHook function should never terminate */
	while(1);
}
