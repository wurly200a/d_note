/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "IoWndDef.h"

/* 外部関数定義 */
#include "WinMain.h"
#include "StsBar.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWnd.h"
LRESULT CALLBACK IoWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static IOWND_INDEX ioWndConvertMSGtoINDEX( UINT message );
static void ioWndBufferOut( int x, int y, TCHAR c );
static LRESULT ioOnCreate       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnPaint        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSize         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnClose        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDestroy      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCommand      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyUp        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyDown      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnChar         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnHscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnVscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSetFocus     ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKillFocus    ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseActivate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseWheel   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDefault      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static void setAllScrollInfo( void );
static void printCaretPos( void );

/* 内部変数定義 */
static HWND hWndIo; /* ウィンドウのハンドラ */
static S_IOWND_DATA ioWndData;

/* *INDENT-OFF* */
static LRESULT (*ioWndProcTbl[IOWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    ioOnCreate       , /* WM_CREATE        */
    ioOnPaint        , /* WM_PAINT         */
    ioOnSize         , /* WM_SIZE          */
    ioOnClose        , /* WM_CLOSE         */
    ioOnDestroy      , /* WM_DESTROY       */
    ioOnCommand      , /* WM_COMMAND       */
    ioOnKeyUp        , /* WM_KEYUP         */
    ioOnKeyDown      , /* WM_KEYDOWN       */
    ioOnChar         , /* WM_CHAR          */
    ioOnHscroll      , /* WM_HSCROLL       */
    ioOnVscroll      , /* WM_VSCROLL       */
    ioOnSetFocus     , /* WM_SETFOCUS      */
    ioOnKillFocus    , /* WM_KILLFOCUS     */
    ioOnMouseActivate, /* WM_MOUSEACTIVATE */
    ioOnMouseWheel   , /* WM_MOUSEWHEEL    */
    ioOnDefault        /* default          */
};
/* *INDENT-ON* */


/********************************************************************************
 * 内容  : IOウィンドウクラスの登録、ウィンドウの生成
 * 引数  : int nCmdShow
 * 戻り値: HWND
 ***************************************/
HWND
IoWndCreate( HWND hWnd )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();

    wc.lpfnWndProc      = (WNDPROC) IoWndProc;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszClassName    = "ioWndClass";
    wc.lpszMenuName     = NULL;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
        return (HWND)NULL;
    }

    hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW | WS_EX_ACCEPTFILES,
                                 "ioWndClass", "IO Window",
                                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 0, 0,
                                 hWnd, NULL, hInst, NULL );

    if( hWndIo != NULL )
    {
        SetFocus(hWndIo);
    }

    return hWndIo;
}

/********************************************************************************
 * 内容  : IOウィンドウのサイズ変更
 * 引数  : int x
 * 引数  : int y
 * 引数  : int cxClient
 * 引数  : int cyClient
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndSize( int x, int y, int cxClient, int cyClient )
{
    MoveWindow( hWndIo,x,y,cxClient,cyClient, TRUE);
}

/********************************************************************************
 * 内容  : IOウィンドウの破壊
 * 引数  : int nCmdShow
 * 戻り値: なし
 ***************************************/
void
IoWndDestroy( void )
{
    if( hWndIo != NULL )
    {
        DestroyWindow( hWndIo );
        hWndIo = NULL;
    }
}

/********************************************************************************
 * 内容  : 文字列出力
 * 引数  : TCHAR* strPtr
 * 引数  : int length
 * 戻り値: なし
 ***************************************/
void
IoWndPrint( TCHAR* strPtr, int length )
{
    DWORD i,j;

    if( ioWndData.bufferPtr != NULL )
    {
        free( ioWndData.bufferPtr );
        ioWndData.bufferPtr = NULL;
        ioWndData.bufferSize = 0;
        ioWndData.lineMax   = 0;
        ioWndData.columnMax = 0;
    }
    else
    {
        /* do nothing */
    }

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndData.bufferPtr = (TCHAR *) malloc( (length * sizeof(TCHAR)) );

        if( ioWndData.bufferPtr != NULL )
        {
            ioWndData.bufferSize = length;

            for( i=0,j=0; i<length; i++ )
            {
                *(ioWndData.bufferPtr+i) = *(strPtr+i);
                if( *(strPtr+i) == '\n' )
                {
                    (ioWndData.lineMax)++; /* 改行の数をカウント */
                    ioWndData.columnMax = max( ioWndData.columnMax, j );
                    j=0;
                }
                else
                {
                    j++;
                }
            }

            if( ioWndData.lineMax==0 )
            { /* 改行が無かった場合 */
                ioWndData.lineMax   = 1;
                ioWndData.columnMax = ioWndData.bufferSize;
            }
        }
        else
        {
            /* error */
        }
    }
    else
    {
        /* error */
    }
    StsBarSetText( STS_BAR_0,"%d bytes,%d lines",ioWndData.bufferSize,ioWndData.lineMax);

    ioWndData.xCaret = 0;
    ioWndData.yCaret = 0;

    setAllScrollInfo();
    SendMessage( hWndIo, WM_PAINT, 0, 0 );
    InvalidateRect( hWndIo, NULL, TRUE );
}

/********************************************************************************
 * 内容  : メインウィンドウ内で処理するメッセージを判定する
 * 引数  : MSG *msg
 * 戻り値: BOOL
 ***************************************/
BOOL
IsIoWndMessage( MSG *msg )
{
    return FALSE;
}

/********************************************************************************
 * 内容  : ウィンドウプロシージャ。WINDOWSから受け取ったメッセージの処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
LRESULT CALLBACK
IoWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return ioWndProcTbl[ioWndConvertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 内容  : ウィンドウズメッセージから対応するインデックスに変換する
 * 引数  : UINT message
 * 戻り値: MAIN_WNDPRC_INDEX
 ***************************************/
static IOWND_INDEX
ioWndConvertMSGtoINDEX( UINT message )
{
    IOWND_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE       :rtn = IOWND_ON_CREATE       ;break;
    case WM_PAINT        :rtn = IOWND_ON_PAINT        ;break;
    case WM_SIZE         :rtn = IOWND_ON_SIZE         ;break;
    case WM_CLOSE        :rtn = IOWND_ON_CLOSE        ;break;
    case WM_DESTROY      :rtn = IOWND_ON_DESTROY      ;break;
    case WM_COMMAND      :rtn = IOWND_ON_COMMAND      ;break;
    case WM_KEYUP        :rtn = IOWND_ON_KEYUP        ;break;
    case WM_KEYDOWN      :rtn = IOWND_ON_KEYDOWN      ;break;
    case WM_CHAR         :rtn = IOWND_ON_CHAR         ;break;
    case WM_HSCROLL      :rtn = IOWND_ON_HSCROLL      ;break;
    case WM_VSCROLL      :rtn = IOWND_ON_VSCROLL      ;break;
    case WM_SETFOCUS     :rtn = IOWND_ON_SETFOCUS     ;break;
    case WM_KILLFOCUS    :rtn = IOWND_ON_KILLFOCUS    ;break;
    case WM_MOUSEACTIVATE:rtn = IOWND_ON_MOUSEACTIVATE;break;
    case WM_MOUSEWHEEL   :rtn = IOWND_ON_MOUSEWHEEL   ;break;
    default              :rtn = IOWND_ON_DEFAULT      ;break;
    }
    /* *INDENT-ON* */

    return rtn;
}

/********************************************************************************
 * 内容  : 表示バッファに出力する
 * 引数  : int   x 表示バッファの出力位置(X)
 * 引数  : int   y 表示バッファの出力位置(Y)
 * 引数  : TCHAR c 出力する文字列
 * 戻り値: なし
 ***************************************/
static void
ioWndBufferOut( int x, int y, TCHAR c )
{
    if( ioWndData.bufferPtr != NULL )
    {
        *(ioWndData.bufferPtr + y * ioWndData.cxBuffer + x) = c;
    }
    else
    {
        /* do nothing */
    }
}

/********************************************************************************
 * 内容  : WM_CREATE の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    int iDelta;          /* for MouseWheel */
    ULONG ulScrollLines; /* for MouseWheel */

    ioWndData.bufferPtr  = NULL;
    ioWndData.bufferSize = 0;
    ioWndData.dwCharSet = DEFAULT_CHARSET;

    hdc = GetDC( hwnd );
    SelectObject( hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, ioWndData.dwCharSet, 0, 0, 0, FIXED_PITCH, NULL) );

    GetTextMetrics( hdc, &tm );
    ioWndData.cxChar = tm.tmAveCharWidth;
    ioWndData.cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * ioWndData.cxChar / 2;
    ioWndData.cyChar = tm.tmHeight + tm.tmExternalLeading;

    DeleteObject( SelectObject( hdc, GetStockObject(SYSTEM_FONT)) );
    ReleaseDC( hwnd, hdc );

    ioWndData.iMaxWidth = 40 * ioWndData.cxChar * 22 * ioWndData.cxCaps;

    /* for MouseWheel */
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0, &ulScrollLines, 0);
    iDelta = HIWORD(wParam);
    if( ulScrollLines )
    {
        ioWndData.iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
    }
    else
    {
        ioWndData.iDeltaPerLine = 0;
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_PAINT の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT  ps;
    HDC          hdc;
    SCROLLINFO  si;
    int         iHorzPos,iVertPos;
    int         iPaintBeg,iPaintEnd,x,y,columnSize,columnSum;
    int         xPaintBeg,xPaintEnd;
    TCHAR       szBuffer[10] ;
    TCHAR       *lineEndPtr;

    hdc = BeginPaint( hwnd, &ps );
    SelectObject( hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, ioWndData.dwCharSet, 0, 0, 0, FIXED_PITCH, NULL) );

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hwnd, SB_VERT, &si ); /* 縦スクロールバーの位置を取得 */
    iVertPos = si.nPos;
    GetScrollInfo(hwnd, SB_HORZ, &si);   /* 横スクロールバーの位置を取得 */
    iHorzPos = si.nPos;

    if( ioWndData.bufferSize )
    { /* データがあれば */
        iPaintBeg = max(0, iVertPos + ps.rcPaint.top / ioWndData.cyChar);
        iPaintEnd = min(ioWndData.lineMax,iVertPos + ps.rcPaint.bottom / ioWndData.cyChar);

        for( y=0,columnSum=0; y<=iPaintEnd; y++ )
        { /* バッファの最初から、再描画領域の最後まで1行ずつ処理 */
            /* 1行の文字数取得 */
            lineEndPtr = strchr( ioWndData.bufferPtr+columnSum, '\n' );
            if( lineEndPtr != NULL )
            { /* 改行コード有 */
                columnSize = lineEndPtr - (ioWndData.bufferPtr+columnSum);
            }
            else
            { /* 改行コード無 */
                columnSize = ioWndData.bufferSize - columnSum;
            }

            if( iPaintBeg <= y )
            { /* 再描画領域 */
                TextOut( hdc, 0, (y-iVertPos) * ioWndData.cyChar, ioWndData.bufferPtr + columnSum + iHorzPos, min((columnSize - iHorzPos),ioWndData.cxBuffer) );
            }
            else
            { /* 再描画領域でない */
                nop();
            }
            columnSum += columnSize+1; /* 文字数を加算 */
        }
    }
    else
    {
        nop();
    }

    DeleteObject( SelectObject(hdc, GetStockObject(SYSTEM_FONT)) );
    EndPaint( hwnd, &ps );

    ioWndData.iVertPos = iVertPos;
    ioWndData.iHorzPos = iHorzPos;

    return 0;
}

/********************************************************************************
 * 内容  : WM_SIZE の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    int x,y;

    ioWndData.cxClient = LOWORD( lParam );
    ioWndData.cyClient = HIWORD( lParam );

    setAllScrollInfo();

    ioWndData.cxBuffer = max( 1, ioWndData.cxClient / ioWndData.cxChar );
    ioWndData.cyBuffer = max( 1, ioWndData.cyClient / ioWndData.cyChar );

    if( hwnd == GetFocus() )
    {
        SetCaretPos( ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar );
        printCaretPos();
    }
    else
    {
        /* do nothing */
    }

    return 0;
}

/********************************************************************************
 * 内容  : WM_CLOSE の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyWindow( hwnd );

    return 0;
}

/********************************************************************************
 * 内容  : WM_DESTROY の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( ioWndData.bufferPtr != NULL )
    {
        free( ioWndData.bufferPtr );
    }
    else
    {
        /* do nothing */
    }

    PostQuitMessage(0); /* WM_QUITメッセージをポストする */
    return 0;
}

/********************************************************************************
 * 内容  : WM_COMMAND の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_KEYUP の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_KEYDOWN の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    switch(wParam)
    {
    case VK_LEFT:
        ioWndData.xCaret = max( ioWndData.xCaret - 1, 0 );
        break;
    case VK_RIGHT:
        ioWndData.xCaret = min( ioWndData.xCaret + 1, ioWndData.cxBuffer - 1 );
        break;
    case VK_UP:
        ioWndData.yCaret = max( ioWndData.yCaret - 1, 0 );
        break;
    case VK_DOWN:
        ioWndData.yCaret = min( ioWndData.yCaret + 1, ioWndData.cyBuffer - 1 );
        break;
    default:
        break;
    }

    SetCaretPos( ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar);
    printCaretPos();

    return rtn;
}

/********************************************************************************
 * 内容  : WM_CHAR の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_HSCROLL の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SCROLLINFO si;
    int iHorzPos;

    /* Get all the vertial scroll bar information */
    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;

    /* Save the position for comparison later on */
    GetScrollInfo( hwnd, SB_HORZ, &si );
    iHorzPos = si.nPos;

    switch(LOWORD(wParam))
    {
    case SB_LINELEFT:
        si.nPos -= 1;
        break;
    case SB_LINERIGHT:
        si.nPos += 1;
        break;
    case SB_PAGELEFT:
        si.nPos -= si.nPage;
        break;
    case SB_PAGERIGHT:
        si.nPos += si.nPage;
        break;
    case SB_THUMBPOSITION:
        si.nPos = si.nTrackPos;
        break;
    default :
        break;
    }

    /* Set the position and then retrieve it.  Due to adjustments */
    /* by Windows it may not be the same as the value set. */
    si.fMask = SIF_POS;
    SetScrollInfo( hwnd, SB_HORZ, &si, TRUE );
    GetScrollInfo( hwnd, SB_HORZ, &si );

    /* If the position has changed, scroll the window */
    if( si.nPos != iHorzPos )
    {
        ScrollWindow( hwnd, ioWndData.cxChar * (iHorzPos - si.nPos), 0, NULL, NULL );
        InvalidateRect( hWndIo, NULL, TRUE );
    }

    GetScrollInfo( hwnd, SB_HORZ, &si );
    ioWndData.iHorzPos = si.nPos;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_VSCROLL を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SCROLLINFO si;
    int iVertPos;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;
    GetScrollInfo( hwnd, SB_VERT, &si ); /* スクロールバー情報取得 */
    iVertPos = si.nPos;                  /* 現在の位置               */

    switch( LOWORD(wParam) )
    {
    case SB_TOP:
        si.nPos = si.nMin;
        break;
    case SB_BOTTOM:
        si.nPos = si.nMax;
        break;
    case SB_LINEUP:
        si.nPos -= 1;
        break;
    case SB_LINEDOWN:
        si.nPos += 1;
        break;
    case SB_PAGEUP:
        si.nPos -= si.nPage;
        break;
    case SB_PAGEDOWN:
        si.nPos += si.nPage;
        break;
    case SB_THUMBTRACK:
        si.nPos = si.nTrackPos;
        break;
    default:
        break;
    }
    si.fMask = SIF_POS;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE); /* スクロールバー情報セット＆再描画 */
    GetScrollInfo(hwnd, SB_VERT, &si);       /* スクロールバー情報再取得         */

    if( si.nPos != iVertPos )
    {
        ScrollWindow(hwnd, 0, ioWndData.cyChar * (iVertPos - si.nPos),NULL, NULL);
        UpdateWindow(hwnd);
    }

    GetScrollInfo(hwnd, SB_VERT, &si);
    ioWndData.iVertPos = si.nPos;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_SETFOCUS を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnSetFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    CreateCaret( hwnd,NULL,ioWndData.cxChar,ioWndData.cyChar );
    SetCaretPos( ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar);
    ShowCaret( hwnd );

    return rtn;
}

/********************************************************************************
 * 内容  : WM_KILLFOCUS を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnKillFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    HideCaret( hwnd );
    DestroyCaret();

    return rtn;
}

/********************************************************************************
 * 内容  : WM_MOUSEACTIVATE を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnMouseActivate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SetFocus(hwnd);

    return rtn;
}

/********************************************************************************
 * 内容  : WM_MOUSEWHEEL を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnMouseWheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    if( ioWndData.iDeltaPerLine )
    {
        ioWndData.iAccumDelta += (short)HIWORD(wParam);

        while( ioWndData.iAccumDelta >= ioWndData.iDeltaPerLine )
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
            ioWndData.iAccumDelta -= ioWndData.iDeltaPerLine;
        }
        while( ioWndData.iAccumDelta <= -ioWndData.iDeltaPerLine)
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
            ioWndData.iAccumDelta += ioWndData.iDeltaPerLine;
        }
    }
    else
    {
        /* do nothing */
    }

    return rtn;
}

/********************************************************************************
 * 内容  : メッセージのデフォルト処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 内容  : スクロール情報のセット
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
static void
setAllScrollInfo( void )
{
    SCROLLINFO si;

    /* Set vertical scroll bar range and page size */
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;                                                                /* 範囲の最小値 */
    si.nMax   = max(ioWndData.lineMax,(ioWndData.cyClient / ioWndData.cyChar))-1; /* 範囲の最大値 */
    si.nPage  = (ioWndData.cyClient / ioWndData.cyChar); /* ページサイズ */
    SetScrollInfo( hWndIo, SB_VERT, &si, TRUE );

    /* Set horizontal scroll bar range and page size*/
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = max( ioWndData.columnMax,(ioWndData.cxClient/ioWndData.cxChar))-1;
    si.nPage  = (ioWndData.cxClient/ioWndData.cxChar);
    SetScrollInfo( hWndIo, SB_HORZ, &si, TRUE );
}

/********************************************************************************
 * 内容  : カーソル位置をステータスバーに出力
 * 引数  : void
 * 戻り値: void
 ***************************************/
static void
printCaretPos( void )
{
    StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",ioWndData.iVertPos + ioWndData.yCaret,ioWndData.iHorzPos + ioWndData.xCaret);
}
