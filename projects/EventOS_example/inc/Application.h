/*
 * SystickEvent.h
 *
 *  Created on: 01/10/2014
 *      Author: Samuel
 */

#ifndef SYSTICKEVENT_H_
#define SYSTICKEVENT_H_

#include "EventOS.h"

void SystickEvent_new(void);
void SystickEvent_delete(void);
portULONG getMsTicks(void);

#endif /* SYSTICKEVENT_H_ */
