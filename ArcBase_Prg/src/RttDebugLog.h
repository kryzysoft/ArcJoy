#ifndef RTT_DEBUG_LOG_H
#define RTT_DEBUG_LOG_H

#include "SEGGER_RTT.h"

#ifdef DEBUG_RTT

#define DebugInit() SEGGER_RTT_printf(0, "%s%s",RTT_CTRL_CLEAR,RTT_CTRL_BG_BLACK)
#define DebugInfo(message) SEGGER_RTT_printf(0, "%s%s%s", RTT_CTRL_TEXT_WHITE, message, "\r\n")
#define DebugInfoInt(message, num) SEGGER_RTT_printf(0, "%s%s %d%s", RTT_CTRL_TEXT_WHITE, message, num,"\r\n")
#define DebugWarn(message) SEGGER_RTT_printf(0, "%s%s%s", RTT_CTRL_TEXT_YELLOW, message, "\r\n")
#define DebugError(message) SEGGER_RTT_printf(0, "%s%s%s", RTT_CTRL_TEXT_RED, message, "\r\n")

#else

#define DebugInit()
#define DebugInfo(message)
#define DebugWarn(message)
#define DebugError(message)

#endif

#endif
