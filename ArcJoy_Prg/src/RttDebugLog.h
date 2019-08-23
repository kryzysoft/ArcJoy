#ifndef RTT_DEBUG_LOG_H
#define RTT_DEBUG_LOG_H

#include "SEGGER_RTT.h"

#ifdef DEBUG_RTT

#define DebugInit() SEGGER_RTT_printf(0, "%s%s",RTT_CTRL_CLEAR,RTT_CTRL_BG_BLACK)

#define DebugInfo(...) SEGGER_RTT_printf(0, RTT_CTRL_TEXT_WHITE);\
                       SEGGER_RTT_printf(0,__VA_ARGS__);\
                       SEGGER_RTT_printf(0,"\r\n");
#define DebugWarn(...) SEGGER_RTT_printf(0, RTT_CTRL_TEXT_YELLOW);\
                       SEGGER_RTT_printf(0,__VA_ARGS__);\
                       SEGGER_RTT_printf(0,"\r\n");
#define DebugErr(...)  SEGGER_RTT_printf(0, RTT_CTRL_TEXT_RED);\
                       SEGGER_RTT_printf(0,__VA_ARGS__);\
                       SEGGER_RTT_printf(0,"\r\n");

#else

#define DebugInit()
#define DebugInfo(message)
#define DebugWarn(message)
#define DebugError(message)

#endif

#endif
