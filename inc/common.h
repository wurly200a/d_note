#ifndef COMMON_H

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500 /* for Mouse Wheel Support */

#include <windows.h>
#include <imm.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <process.h>

/****** chBEGIN_THREAD_EX Macro (from CmnHdr.h by Jeffrey Richter) ******/

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

#define nop()

#define COMMON_H
#endif /* COMMON_H */
