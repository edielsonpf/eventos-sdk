/*
    EventOS V1.0 - Copyright (C) 2014 Edielson Prevato Frigieri.

       This file is part of the EventOS distribution.

    EventTOS is free software; you can redistribute it and/or modify it under
    the terms of the MIT License (MIT).
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

 */

#ifndef ICOMMON_H_
#define ICOMMON_H_

#include "portable.h"

/***********************************************************************
 * Global typedefs
 **********************************************************************/

/* Function handle return  */
typedef portVOID*	Handle;

/* SMA type for character type */
typedef portCHAR Int8;

/* SMA type for 8 bit unsigned value */
typedef portUCHAR uInt8;

/* SMA type for 16 bit unsigned value */
typedef	portUINTEGER uInt16;

/* SMA type for 16 bit signed value */
typedef	portINTEGER Int16;

/* SMA type for 32 bit unsigned value */
typedef	unsigned long int uInt32;

/* SMA type for 32 bit signed value */
typedef	portLONG Int32;

/* 32 bit boolean type */
typedef Int32 Bool32;
#define True32	(Bool32)1
#define False32	(Bool32)0

/* 16 bit boolean type */
typedef Int16 Bool16;
#define True16	(Bool16)1
#define False16	(Bool16)0

/* 8 bit boolean type */
typedef Int8 Bool8;
#define True8	(Bool8)1
#define False8	(Bool8)0

typedef Int8 Status8;
#define Free8		(Status8)0
#define NotFree8	(Status8)1

/* SMA type for character type */
typedef portCHAR* pInt8;

/* SMA type for 8 bit unsigned value */
typedef portUCHAR* puInt8;

/* SMA type for 16 bit unsigned value */
typedef	portUINTEGER* puInt16;

/* SMA type for 16 bit signed value */
typedef	portINTEGER* pInt16;

/* SMA type for 32 bit unsigned value */
typedef	portULONG* puInt32;

/* SMA type for 32 bit signed value */
typedef	portLONG* pInt32;

/* 32 bit boolean type */
typedef pInt32 pBool32;

/* 16 bit boolean type */
typedef pInt16 pBool16;

/* 8 bit boolean type */
typedef pInt8 pBool8;


#ifdef PROCESSOR_BIG_ENDIAN
#	define ENDIAN16(n) 					(n)
#	define ENDIAN16_2X8TO16(n1, n2) 	(uInt16)(((uInt16)(n1)<<8) | (uInt16)(n2))
#  	define ENDIAN16_HIGHT8BIT(n)		(uInt8)(n>>8)
#  	define ENDIAN16_LOW8BIT(n)			(uInt8)(n&0xFF)

#	define ENDIAN32(n) 					(n)
#	define ENDIAN32_4X8TO32(n1, n2, n3, n4) 	(uInt32)((((uInt32)n1)<<24) | (((uInt32)n2)<<16) | (((uInt32)n3)<<8) | ((uInt32)n4))
#	define ENDIAN32_2X16TO32(n1, n2) 	(uInt32)((((uInt32)n1)<<16) | ((uInt32)n2))
#  	define ENDIAN32_HIGHT16BIT(n)		(uInt16)(n>>16)
#  	define ENDIAN32_LOW16BIT(n)			(uInt16)(n&0xFFFF)
#	define ENDIAN32_BYTE0(n)		 	(uInt8)(((uInt32)n)>>24)
#	define ENDIAN32_BYTE1(n)		 	(uInt8)((((uInt32)n)&0X00FF0000)>>16)
#	define ENDIAN32_BYTE2(n)		 	(uInt8)((((uInt32)n)&0X0000FF00)>>8)
#	define ENDIAN32_BYTE3(n)		 	(uInt8)(((uInt32)n)&0x000000FF)
#else /* PROCESSOR_LITTLE_ENDIAN */
#	define ENDIAN16(n) 					(uInt16)((((uInt16)(n)) << 8) | (((uInt16)(n)) >> 8))
#	define ENDIAN16_2X8TO16(n1, n2) 	(uInt16)((((uInt16)n2)<<8) | ((uInt16)n1))
#  	define ENDIAN16_HIGHT8BIT(n)		(uInt8)(n&0xFF)
#  	define ENDIAN16_LOW8BIT(n)			(uInt8)(n>>8)

#	define ENDIAN32(n) 					(uInt32)(((((uInt32)n)) << 24) | ((((uInt32)n)&0x0000FF00) << 8) | ((((uInt32)n)&0x00FF0000) >> 8) | ((((uInt32)n)) >> 24))
#	define ENDIAN32_4X8TO32(n1, n2, n3, n4) 	(uInt32)((((uInt32)n4)<<24) | (((uInt32)n3)<<16) | (((uInt32)n2)<<8) | ((uInt32)n1))
#	define ENDIAN32_2X16TO32(n1, n2) 	(uInt32)((((uInt32)n2)<<16) | ((uInt32)n1))
#  	define ENDIAN32_HIGHT16BIT(n)		(uInt16)(n&0xFFFF)
#  	define ENDIAN32_LOW16BIT(n)			(uInt16)(n>>16)
#	define ENDIAN32_BYTE0(n)		 	(uInt8)(((uInt32)n)&0x000000FF)
#	define ENDIAN32_BYTE1(n)		 	(uInt8)((((uInt32)n)&0X0000FF00)>>8)
#	define ENDIAN32_BYTE2(n)		 	(uInt8)((((uInt32)n)&0X00FF0000)>>16)
#	define ENDIAN32_BYTE3(n)		 	(uInt8)(((uInt32)n)>>24)
#endif /* PROCESSOR_LITTLE_ENDIAN */

#	define CONVERT32_4X8TO32(n1, n2, n3, n4) 	(uInt32)((((uInt32)n1)<<24) | (((uInt32)n2)<<16) | (((uInt32)n3)<<8) | ((uInt32)n4))
#	define CONVERT32_BYTE0(n)		 	(uInt8)(((uInt32)n)>>24)
#	define CONVERT32_BYTE1(n)		 	(uInt8)((((uInt32)n)&0X00FF0000)>>16)
#	define CONVERT32_BYTE2(n)		 	(uInt8)((((uInt32)n)&0X0000FF00)>>8)
#	define CONVERT32_BYTE3(n)		 	(uInt8)(((uInt32)n)&0x000000FF)

#define __HEADER_						volatile
#define __PRIVATE_						static
#define __MSG_PAR						volatile

#define _IN_
#define _OUT_
#define _IO_

/*
 * Test and set bits.
 */
#define testBit(Var, Bit) 				((Var&Bit)==Bit)
#define setBit(Var, Bit) 				Var = (Var|(0x01<<Bit))
#define clearBit(Var, Bit) 				Var = (Var&(!(0x01<<Bit)))

#endif /* ICOMMON_H_ */
