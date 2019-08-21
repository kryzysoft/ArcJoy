#ifndef RTT_DEBUG_LOG_H
#define RTT_DEBUG_LOG_H

#include "SEGGER_RTT.h"

#define DebugInfo(message) SEGGER_RTT_printf(0, "%s%s%s", RTT_CTRL_TEXT_WHITE, message, "\r\n");
#define DebugWarn(message) SEGGER_RTT_printf(0, "%s%s%s", RTT_CTRL_TEXT_WHITE, message, "\r\n");
#define DebugError(message) SEGGER_RTT_printf(0, "%s%s%s", RTT_CTRL_TEXT_WHITE, message, "\r\n");

#endif
