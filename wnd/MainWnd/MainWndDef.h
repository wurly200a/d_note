#ifndef MAINWND_DEF_H

typedef enum
{
    MAINWND_ON_CREATE    , /* WM_CREATE        */
    MAINWND_ON_PAINT     , /* WM_PAINT         */
    MAINWND_ON_SIZE      , /* WM_SIZE          */
    MAINWND_ON_CLOSE     , /* WM_CLOSE         */
    MAINWND_ON_DESTROY   , /* WM_DESTROY       */
    MAINWND_ON_COMMAND   , /* WM_COMMAND       */
    MAINWND_ON_KEYUP     , /* WM_KEYUP         */
    MAINWND_ON_KEYDOWN   , /* WM_KEYDOWN       */
    MAINWND_ON_CHAR      , /* WM_CHAR          */
    MAINWND_ON_HSCROLL   , /* WM_HSCROLL       */
    MAINWND_ON_VSCROLL   , /* WM_VSCROLL       */
    MAINWND_ON_MOUSEWHEEL, /* WM_MOUSEWHEEL    */
    MAINWND_ON_APP       , /* WM_APP           */
    MAINWND_ON_DEFAULT   , /* default          */
    MAINWND_MAX
} MAINWND_INDEX;

typedef struct
{
    BOOL execute;
    BOOL bAppExit;
    int  cxClient;
    int  cyClient;
    int  cxChar;
    int  cyChar;
    int  cxCaps;
    HMENU hMenu;
    HWND hWndIo;
} S_MAINWND_DATA;

#define MAINWND_DEF_H
#endif /* MAINWND_DEF_H */
