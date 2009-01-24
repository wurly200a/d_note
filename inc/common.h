#ifndef COMMON_H

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500 /* for Mouse Wheel Support */

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <process.h>
#include "resource.h"

#define WND_WIDTH  500
#define WND_HEIGHT 300

#define WND_TSK_HEIGHT_OFFSET 200

#define COM_PORT_NUM_MAX 8
#define COM_PORT_DEFAULT 1

typedef enum
{
    APP_MSG_THREAD_EXIT = 1
} APP_MSG_WPARAM;

/****** chBEGIN_THREAD_EX Macro (from CmnHdr.h by Jeffrey Richter) ******/

/* chDIMOF Macro  */
/* This macro evaluates to the number of elements in an array. */
#define chDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGIN_THREAD_EX(psa, cbStack, pfnStartAddr, \
   pvParam, fdwCreate, pdwThreadId)                   \
      ((HANDLE)_beginthreadex(                        \
         (void *)        (psa),                       \
         (unsigned)      (cbStack),                   \
         (PTHREAD_START) (pfnStartAddr),              \
         (void *)        (pvParam),                   \
         (unsigned)      (fdwCreate),                 \
         (unsigned *)    (pdwThreadId)))

/* Quick MessageBox Macro */

__inline void chMB(PCSTR s) {
   char szTMP[128];
   GetModuleFileNameA(NULL, szTMP, chDIMOF(szTMP));
   MessageBoxA(GetActiveWindow(), s, szTMP, MB_OK);
}

#define COMMON_H
#endif /* COMMON_H */
