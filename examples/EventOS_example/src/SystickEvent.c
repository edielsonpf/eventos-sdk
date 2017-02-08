
#include <cr_section_macros.h>
#include <NXP/crp.h>
#include <stdio.h>

#include "LPC17xx.h"
#include "EventOS.h"
#include "SystickEvent.h"
#include "Log.h"
#include "TestFunctions.h"
#include "leds.h"
#include "light.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"


/*********************************************************
    private operations.
*********************************************************/

void Systick_Handler(void);
void SystickEvent_new(void);
void SystickEvent_delete(void);
void SystickEvent_init();
void SystickEvent_receiveNewEvent(Handle hHandle, ttag_Event* ptagEvent);
void SystickEvent_receiveLight(Handle hHandle, ttag_Event* ptagEvent);
uint32_t getMsTicks(void);

volatile uint32_t msTicks; // counter for 1ms SysTicks

void SysTick_Handler(void)
{

	msTicks++;

	void* pEventSlot;

	//Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"Handling new SYSTICK interrupt\n");

	if(msTicks % 1000 == 0)
	{
		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Publishing new event: TICK");
		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_TICK, EVENTOS_PRIORITY_LOW, NULL, 0);
		EventOS_publish(pEventSlot);

		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_ETHERNET, EVENTOS_PRIORITY_MEDIUM, NULL, 0);
		EventOS_publish(pEventSlot);

		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_TEMPERATURE, EVENTOS_PRIORITY_MEDIUM, NULL, 0);
		EventOS_publish(pEventSlot);

		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_UART, EVENTOS_PRIORITY_MEDIUM, NULL, 0);
		EventOS_publish(pEventSlot);

		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_USB, EVENTOS_PRIORITY_MEDIUM, NULL, 0);
		EventOS_publish(pEventSlot);

		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_TEMPERATURE, EVENTOS_PRIORITY_MEDIUM, NULL, 0);
		EventOS_publish(pEventSlot);
	}
	if(msTicks % 5000 == 0)
	{
		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Publishing new event: ETHERNET");
		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_ETHERNET, EVENTOS_PRIORITY_MEDIUM, NULL, 0);
		EventOS_publish(pEventSlot);
	}

	if(msTicks % 10000 == 0)
	{
		Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Publishing new event: SYSTICK");
		pEventSlot = EventOS_newEvent(EVENTOS_EVENT_SYSTICK, EVENTOS_PRIORITY_HIGH, NULL, 0);
	    EventOS_publish(pEventSlot);

	    msTicks = 0;

	    led2_invert();
	}
}



static void i2c_init(void)
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
}


void SystickEvent_new(void)
{
	SystickEvent_init();
	i2c_init();
	//light_init();
	light_enable();

	EventOS_subscribe(EVENTOS_EVENT_TICK, NULL, SystickEvent_receiveNewEvent);
	EventOS_subscribe(EVENTOS_EVENT_SYSTICK, NULL, SystickEvent_receiveNewEvent);
	EventOS_subscribe(EVENTOS_EVENT_ETHERNET, NULL, SystickEvent_receiveNewEvent);
	EventOS_subscribe(EVENTOS_EVENT_LIGHT, NULL, SystickEvent_receiveLight);

	led2_init();
	led2_on();
}


void SystickEvent_init()
{

	// Setup SysTick Timer to interrupt at 1 msec intervals
	if (SysTick_Config(SystemCoreClock/1000))
	{
	    while (1);  // Capture error
	}
	return;
}

void SystickEvent_delete(void)
{

}

void SystickEvent_receiveLight(Handle hHandle, ttag_Event* ptagEvent)
{
	int32_t* iLight = (int32_t*)ptagEvent->pvPayload;

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Light: %d", *iLight);
}

void SystickEvent_receiveNewEvent(Handle hHandle, ttag_Event* ptagEvent)
{
	int32_t iLight;
	ttag_Event* pEvent = NULL;

	switch (ptagEvent->tagHeader.eEvent) {
		case EVENTOS_EVENT_ETHERNET:
			Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Receiving new event from EventOS (Ethernet)");
			break;
		case EVENTOS_EVENT_SYSTICK:
			Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Receiving new event from EventOS (Systick)");
			iLight = light_read();
			pEvent = EventOS_newEvent(EVENTOS_EVENT_LIGHT, EVENTOS_PRIORITY_HIGH,(pInt8)&iLight, sizeof(int32_t));
			EventOS_publish(pEvent);
			break;
		case EVENTOS_EVENT_TICK:
			Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Receiving new event from EventOS (Tick)");
			break;
		default:
			Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_ERROR,"[app] Event not expected");
			break;
	}
}

uint32_t getMsTicks(void)
{
	return msTicks;
}
