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

#define DebugInputAvailable() SEGGER_RTT_HasKey()
#define DebugInputRead(buffer, bufferSize) SEGGER_RTT_Read(0, buffer, bufferSize)

#else

#define DebugInit()
#define DebugInfo(...)
#define DebugWarn(...)
#define DebugErr(...)
#define DebugInputAvailable() 0
#define DebugInputRead(buffer, bufferSize)

#endif

#endif
