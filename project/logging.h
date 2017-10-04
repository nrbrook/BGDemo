/*****************************************************************************
 * @file logging.h
 * @brief Logging print definitions
 * @author Nick Brook
 * @version 1.00
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2017 Nick Brook, https://www.nickbrook.me</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Nick Brook has no
 * obligation to support this Software. Nick Brook is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Nick Brook will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdio.h>

#define USES_COLOURS 1
#define OUTPUT_LOCATION 1
#define ADD_LINE_BREAKS 1

#define LogError(fmt, args...)
#define LogWarn(fmt, args...)
#define LogInfo(fmt, args...)

#if defined(LOG_LEVEL) && LOG_LEVEL > 0
	#define LOG_PRINT(prefix, suffix, fmt, args...) fprintf(stderr, prefix fmt suffix, ##args)

	#if ADD_LINE_BREAKS
		#define LOG_PRINT_LINE(prefix, suffix, fmt, args...) LOG_PRINT(prefix, suffix "\r\n", fmt, ##args)
	#else
		#define LOG_PRINT_LINE(prefix, suffix, fmt, args...) LOG_PRINT(prefix, suffix, fmt, ##args)
	#endif

	#if OUTPUT_LOCATION
		#define LOG_PRINT_LOCATION(prefix, suffix, fmt, args...) LOG_PRINT_LINE(prefix "%s:%s():%d: ", suffix, fmt, __FILE__, __func__, __LINE__, ##args)
	#else
		#define LOG_PRINT_LOCATION(prefix, suffix, fmt, args...) LOG_PRINT_LINE(prefix, suffix, fmt, ##args)
	#endif

	#if USES_COLOURS
		#define LOG_PRINT_COLOUR(colour, prefix, suffix, fmt, args...) LOG_PRINT_LOCATION("\033[0;" colour "m" prefix, suffix "\033[0m", fmt, ##args)
	#else
		#define LOG_PRINT_COLOUR(colour, prefix, suffix, fmt, args...) LOG_PRINT_LOCATION(prefix, suffix, fmt, ##args)
	#endif

	#undef LogError
	#define LogError(fmt, args...) LOG_PRINT_COLOUR("31", "ERROR: ", "", fmt, ##args)

	#if LOG_LEVEL > 1
		#undef LogWarn
		#define LogWarn(fmt, args...) LOG_PRINT_COLOUR("33", "Warn: ", "", fmt, ##args)

		#if LOG_LEVEL > 2
			#undef LogInfo
			#define LogInfo(fmt, args...) LOG_PRINT_LOCATION("Info: ", "", fmt, ##args)
		#endif
	#endif
#endif /* LOGGING_H_ */

#endif
