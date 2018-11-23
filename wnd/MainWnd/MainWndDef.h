#ifndef MAINWND_DEF_H

typedef enum
{
    MAINWND_ON_CREATE          , /* WM_CREATE                     */
    MAINWND_ON_SIZE            , /* WM_SIZE                       */
    MAINWND_ON_MOVE            , /* WM_MOVE                       */
    MAINWND_ON_WINDOWPOSCHANGED, /* WM_WINDOWPOSCHANGED           */
    MAINWND_ON_CLOSE           , /* WM_CLOSE                      */
    MAINWND_ON_DESTROY         , /* WM_DESTROY                    */
    MAINWND_ON_COMMAND         , /* WM_COMMAND                    */
    MAINWND_ON_MOUSEWHEEL      , /* WM_MOUSEWHEEL                 */
    MAINWND_ON_MOUSEHWHEEL     , /* WM_MOUSEHWHEEL                */
    MAINWND_ON_SETFOCUS        , /* WM_SETFOCUS                   */
    MAINWND_ON_KILLFOCUS       , /* WM_KILLFOCUS                  */
    MAINWND_ON_DROPFILES       , /* WM_DROPFILES                  */
    MAINWND_ON_INITMENUPOPUP   , /* WM_INITMENUPOPUP              */
    MAINWND_ON_FINDMSGSTRING   , /* FINDMSGSTRINGÇÃìoò^ÉÅÉbÉZÅ[ÉW */
    MAINWND_ON_DEFAULT         , /* default                       */
    MAINWND_MAX
} MAINWND_INDEX;

typedef struct
{
    HINSTANCE hInstance;
    PTSTR     szAppName;
    BOOL execute;
    BOOL bAppExit;
    int  xWindowPos;
    int  yWindowPos;
    int  xPos;
    int  yPos;
    int  cxWindow;
    int  cyWindow;
    int  cxClient;
    int  cyClient;
    int  cxChar;
    int  cyChar;
    int  cxCaps;
    HWND hWndIo;
    HFONT hFontIo;
    HACCEL hAccel;
    BOOL bNeedSave;
    UINT messageFindReplace;
    HWND hDlgModeless;
    HWND hWndDebug;
    BOOL bFrUp;
    BOOL bFrMatchCase;
    TCHAR* searchDataPtr;
    DWORD searchDataSize;
    TCHAR* replaceDataPtr;
    DWORD replaceDataSize;
} S_MAINWND_DATA;

#define MAINWND_DEF_H
#endif /* MAINWND_DEF_H */
