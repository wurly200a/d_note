#ifndef MAINWND_DEF_H

typedef enum
{
    WNDPROC_ON_CREATE , /* WM_CREATE  */
    WNDPROC_ON_PAINT  , /* WM_PAINT   */
    WNDPROC_ON_SIZE   , /* WM_SIZE    */
    WNDPROC_ON_CLOSE  , /* WM_CLOSE   */
    WNDPROC_ON_DESTROY, /* WM_DESTROY */
    WNDPROC_ON_COMMAND, /* WM_COMMAND */
    WNDPROC_ON_KEYUP  , /* WM_KEYUP   */
    WNDPROC_ON_KEYDOWN, /* WM_KEYDOWN */
    WNDPROC_ON_CHAR   , /* WM_CHAR    */
    WNDPROC_ON_HSCROLL, /* WM_HSCROLL */
    WNDPROC_ON_VSCROLL, /* WM_VSCROLL */
    WNDPROC_ON_APP    , /* WM_APP     */
    WNDPROC_ON_DEFAULT, /* default    */
    WNDPROC_MAX
} WNDPROC_INDEX;

typedef struct
{
    BOOL execute;
    BOOL bAppExit;
    int  cxClient;
    int  cyClient;
    int  cxChar;
    int  cyChar;
    int  cxCaps;
} S_MAIN_STATUS;

#define MAINWND_DEF_H
#endif /* MAINWND_DEF_H */
