/*
 * UartEvent.c
 *
 *  Created on: 26/09/2014
 *      Author: edielson
 */
#include "EventOS.h"
#include "UartEvent.h"
#include "Log.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "core_cm3.h"
#include "stdio.h"
#include "leds.h"
#include "acc.h"


#define UART_DEV LPC_UART3
#define UART_INTERRUPT      UART3_IRQn


/*********************************************************
    private constants.
*********************************************************/

/*********************************************************
    private types.
*********************************************************/



/*********************************************************
    private attributes.
*********************************************************/

/*********************************************************
    private operations.
*********************************************************/

void UartEvent_init(void);
void UartEvent_receiveNewEvent(Handle hHandle, ttag_Event* ptagEvent);
void UartEvent_pubNewEvent(void);

/*********************************************************
    Operations implementation
*********************************************************/

void UartEvent_new(void)
{
	UartEvent_init();
	acc_init();
	EventOS_subscribe(EVENTOS_EVENT_UART,NULL, UartEvent_receiveNewEvent);

	led2_init();
	led2_on();
}

void UartEvent_delete(void)
{

}

void UartEvent_init(void)
{
	PINSEL_CFG_Type PinCfg;
	UART_CFG_Type uartCfg;

	/* Initialize UART3 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	uartCfg.Baud_rate = 115200;
	uartCfg.Databits = UART_DATABIT_8;
	uartCfg.Parity = UART_PARITY_NONE;
	uartCfg.Stopbits = UART_STOPBIT_1;

	UART_Init(UART_DEV, &uartCfg);

	UART_TxCmd(UART_DEV, ENABLE);

	UART_IntConfig(UART_DEV, UART_INTCFG_RBR, ENABLE);
	NVIC_EnableIRQ(UART_INTERRUPT);
}

void UartEvent_receiveNewEvent(Handle hHandle, ttag_Event* ptagEvent)
{
	int8_t x;
	int8_t y;
	int8_t z;

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Receiving new event from EventOS (UART)");
	acc_init();
	acc_read(&x, &y, &z);
	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Accelerometer coordinates: x: %d y: %d z: %d", x, y, z);
	UART_SendString(UART_DEV,(uint8_t*)"Hello World!\r\n");
}

void UartEvent_pubNewEvent(void)
{
	uint8_t data;
	void* pEventSlot;

	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Handling new UART interrupt");
	Log_print(LOG_FACILITY_USER_LEVEL_MESSAGES,LOG_SEVERITY_INFORMATIONAL,"[app] Publishing new event: PACKET.");
	UART_Receive(UART_DEV,(uint8_t*) &data,1, NONE_BLOCKING);
	pEventSlot = EventOS_newEvent(EVENTOS_EVENT_UART, EVENTOS_PRIORITY_HIGH, NULL, 0);

	EventOS_publish(pEventSlot);
}

void xPortUart3Handler(void)
{
	UartEvent_pubNewEvent();
}
