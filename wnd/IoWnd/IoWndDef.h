#ifndef IOWND_DEF_H

typedef enum
{
    IOWND_ON_CREATE       , /* WM_CREATE        */
    IOWND_ON_PAINT        , /* WM_PAINT         */
    IOWND_ON_SIZE         , /* WM_SIZE          */
    IOWND_ON_CLOSE        , /* WM_CLOSE         */
    IOWND_ON_DESTROY      , /* WM_DESTROY       */
    IOWND_ON_COMMAND      , /* WM_COMMAND       */
    IOWND_ON_KEYUP        , /* WM_KEYUP         */
    IOWND_ON_KEYDOWN      , /* WM_KEYDOWN       */
    IOWND_ON_CHAR         , /* WM_CHAR          */
    IOWND_ON_HSCROLL      , /* WM_HSCROLL       */
    IOWND_ON_VSCROLL      , /* WM_VSCROLL       */
    IOWND_ON_SETFOCUS     , /* WM_SETFOCUS      */
    IOWND_ON_KILLFOCUS    , /* WM_KILLFOCUS     */
    IOWND_ON_MOUSEACTIVATE, /* WM_MOUSEACTIVATE */
    IOWND_ON_MOUSEWHEEL   , /* WM_MOUSEWHEEL    */
    IOWND_ON_MOUSEMOVE    , /* WM_MOUSEMOVE     */
    IOWND_ON_LBUTTONDOWN  , /* WM_LBUTTONDOWN   */
    IOWND_ON_MBUTTONDOWN  , /* WM_MBUTTONDOWN   */
    IOWND_ON_RBUTTONDOWN  , /* WM_RBUTTONDOWN   */
    IOWND_ON_LBUTTONUP    , /* WM_LBUTTONUP     */
    IOWND_ON_MBUTTONUP    , /* WM_MBUTTONUP     */
    IOWND_ON_RBUTTONUP    , /* WM_RBUTTONUP     */
    IOWND_ON_DEFAULT      , /* default          */
    IOWND_MAX
} IOWND_INDEX;

typedef struct
{
    int    cxChar;    /* 文字サイズ(X)             */
    int    cyChar;    /* 文字サイズ(Y)             */
    int    cxClient;  /* クライアント領域サイズ(X) */
    int    cyClient;  /* クライアント領域サイズ(Y) */
    int    xCaret;    /* カーソル位置(X)           */
    int    yCaret;    /* カーソル位置(Y)           */
    int    cxBuffer;  /* 表示可能な文字数(X)       */
    int    cyBuffer;  /* 表示可能な文字数(Y)       */
    DWORD  dwCharSet;
    int    cxCaps;    /*  */
    int    iAccumDelta;
    int    iDeltaPerLine;
    int    iHorzPos;     /* スクロールバーの横位置  */
    int    iVertPos;     /* スクロールバーの縦位置  */
    LOGFONT *logFontPtr;
} S_IOWND_DATA;

#define IOWND_DEF_H
#endif /* IOWND_DEF_H */
