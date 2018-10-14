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

#ifndef _EVENTOS_APPVERSION32_H_
#define _EVENTOS_APPVERSION32_H_

#include <stdint.h>

/* Application version structure. */
#pragma pack(push,1)
typedef struct {
	union {
#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || (__little_endian__ == 1) || WIN32 || (__BYTE_ORDER == __LITTLE_ENDIAN)
		struct version {
			uint16_t    usBuild;
			uint8_t     ucMinor;
			uint8_t     ucMajor;
		} x;
#elif (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || (__big_endian__ == 1) || (__BYTE_ORDER == __BIG_ENDIAN)
		struct version {
			uint8_t     ucMajor;
			uint8_t     ucMinor;
			uint16_t    usBuild;
		} x;
#else
#error "Unable to determine byte order!"
#endif
		uint32_t ulVersion32;
		int32_t  lVersion32;
	} u;
} AppVersion32_t;
#pragma pack(pop)

#endif
