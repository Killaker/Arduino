/*
 *
 * SUIPlatform.h -- SerialUI system platform-specifics.
 * Copyright (C) 2013-2017 Pat Deegan, psychogenic.com. All rights reserved.
 *
 *
 * http://www.flyingcarsandstuff.com/projects/SerialUI
 *
 *
 * This program library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See file LICENSE.txt for further informations on licensing
 * terms.
 *
 *
 * ************************* OVERVIEW *************************
 *
 * Here we'll store all the platform-specific includes and structures.
 *
 */

#ifndef SERIALUI_PLATFORM_MAIN_H_
#define SERIALUI_PLATFORM_MAIN_H_

#include "SUIConfig.h"
#include "SUIExtIncludes.h"


#ifdef SUI_INCLUDE_DEBUG

#warning "Debug is ENABLED--eats allota space/mem..."

#ifndef SUI_PLATFORM_HARDWARESERIAL_DEFAULT
#define SUI_PLATFORM_HARDWARESERIAL_DEFAULT	Serial
#endif

#define SUI_OUTPUT_DEBUG_DEFAULTSTREAM(...)		SUI_PLATFORM_HARDWARESERIAL_DEFAULT.print(__VA_ARGS__)
#define SUI_OUTPUTLN_DEBUG_DEFAULTSTREAM(...)		SUI_PLATFORM_HARDWARESERIAL_DEFAULT.println(__VA_ARGS__)
#else
#define SUI_OUTPUT_DEBUG_DEFAULTSTREAM(...)
#define SUI_OUTPUTLN_DEBUG_DEFAULTSTREAM(...)

#endif

#define SUI_PLATFORM_DELAYMS(n)			delay(n)


#ifdef SOVA_PROGMEM_PTR

#define PRINT_FLASHSTR(...)		print(__VA_ARGS__)
#define PRINTLN_FLASHSTR(...)	println(__VA_ARGS__)
#define STRLEN_FLASHSTR(...)	strlen_P(reinterpret_cast<const char *>(__VA_ARGS__))
#define STRCAT_FLASHSTR(a, b)	strcat_P(reinterpret_cast<char *>(a), reinterpret_cast<const char *>(b))
#define STRCMP_FLASHSTR(a, b)	strcmp_P(reinterpret_cast<const char *>(a), reinterpret_cast<const char *>(b))
#define STRNCMP_FLASHSTR(a, b, len)	strncmp_P(reinterpret_cast<const char *>(a), reinterpret_cast<const char *>(b), (len))
#define STRNCMP_FLASHSTR(a, b, len)	strncmp_P(reinterpret_cast<const char *>(a), reinterpret_cast<const char *>(b), (len))

#else
/* No SOVA_PROGMEM_PTR avail */
#define PRINT_FLASHSTR(...)		print(__VA_ARGS__)
#define PRINTLN_FLASHSTR(...)	println(__VA_ARGS__)
#define STRLEN_FLASHSTR(...)	strlen(reinterpret_cast<const char *>(__VA_ARGS__))
#define STRCAT_FLASHSTR(a, b)	strcat(reinterpret_cast<char *>(a), reinterpret_cast<const char *>(b))
#define STRCMP_FLASHSTR(a, b)	strcmp(reinterpret_cast<const char *>(a), reinterpret_cast<const char *>(b))
#define STRNCMP_FLASHSTR(a, b, len)	strncmp(reinterpret_cast<const char *>(a), reinterpret_cast<const char *>(b), (len))
#define STRNCMP_FLASHSTR(a, b, len)	strncmp(reinterpret_cast<const char *>(a), reinterpret_cast<const char *>(b), (len))

#endif

#define SUI_FLASHSTRING SOVA_FLASHSTRING_PTR

#endif /* SERIALUI_PLATFORM_MAIN_H_ */
