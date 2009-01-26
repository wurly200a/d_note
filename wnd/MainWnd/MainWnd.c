/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "MainWndDef.h"

/* 外部関数定義 */
#include "WinMain.h"
#include "SomeCtrl.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static MAINWND_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onPaint  ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose  ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyUp  ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onChar   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp    ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

/* 内部変数定義 */
static HWND hwndMain; /* メインウィンドウのハンドラ */
static HWND hwndSbar;
static S_MAINWND_DATA mainWndData;

static int sbarColWidth[5] = { 100, 200, 300, 400, -1 };

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[MAINWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate , /* WM_CREATE  */
    onPaint  , /* WM_PAINT   */
    onSize   , /* WM_SIZE    */
    onClose  , /* WM_CLOSE   */
    onDestroy, /* WM_DESTROY */
    onCommand, /* WM_COMMAND */
    onKeyUp  , /* WM_KEYUP   */
    onKeyDown, /* WM_KEYDOWN */
    onChar   , /* WM_CHAR    */
    onHscroll, /* WM_HSCROLL */
    onVscroll, /* WM_VSCROLL */
    onApp    , /* WM_APP     */
    onDefault  /* default    */
};
/* *INDENT-ON* */

/********************************************************************************
 * 内容  : メインウィンドウクラスの登録、ウィンドウの生成
 * 引数  : int nCmdShow
 * 戻り値: BOOL
 ***************************************/
BOOL
MainWndCreate( int nCmdShow )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();

    /* メインウィンドウクラス */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) WndProc;
    wc.cbClsExtra       = 0; /* クラス構造体の為の追加領域を予約する */
    wc.cbWndExtra       = 0; /* ウィンドウ構造体の為の追加領域を予約する */
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );          /* アイコン */
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1); /* 背景 */
    wc.lpszMenuName     = pAppName;
    wc.lpszClassName    = pAppName;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
        return FALSE;
    }

    /* メインウィンドウを作成 */
    InitCommonControls(); /* commctrl.hのインクルード、comctl32.libのプロジェクトへの参加が必要 */
    hwndMain = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                               pAppName, pAppName,
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               WND_WIDTH    , WND_HEIGHT,
                               NULL, NULL, hInst, NULL);

    if( hwndMain == NULL )
    {
        return FALSE;
    }

    ShowWindow( hwndMain, nCmdShow ); /* ->WM_SIZE   */ /* ウインドウを表示 */
    UpdateWindow( hwndMain );         /* ->WM_PAINT  */

    return TRUE;
}

/********************************************************************************
 * 内容  : メインウィンドウ内で処理するメッセージを判定する
 * 引数  : MSG *msg
 * 戻り値: BOOL
 ***************************************/
BOOL
IsMainWndMessage( MSG *msg )
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
WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return wndProcTbl[convertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 関数名 : ConvertMSGtoINDEX()
 * 引数   : UINT message
 * 戻り値 : WNDPRC_INDEX
 * 内容   : ウィンドウズメッセージから対応するインデックスに変換する
 ***************************************/
static MAINWND_INDEX
convertMSGtoINDEX( UINT message )
{
    MAINWND_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE :rtn = MAINWND_ON_CREATE ;break;
    case WM_PAINT  :rtn = MAINWND_ON_PAINT  ;break;
    case WM_SIZE   :rtn = MAINWND_ON_SIZE   ;break;
    case WM_CLOSE  :rtn = MAINWND_ON_CLOSE  ;break;
    case WM_DESTROY:rtn = MAINWND_ON_DESTROY;break;
    case WM_COMMAND:rtn = MAINWND_ON_COMMAND;break;
    case WM_APP    :rtn = MAINWND_ON_APP    ;break;
    case WM_KEYUP  :rtn = MAINWND_ON_KEYUP  ;break;
    case WM_KEYDOWN:rtn = MAINWND_ON_KEYDOWN;break;
    case WM_CHAR   :rtn = MAINWND_ON_CHAR   ;break;
    case WM_HSCROLL:rtn = MAINWND_ON_HSCROLL;break;
    case WM_VSCROLL:rtn = MAINWND_ON_VSCROLL;break;
    default        :rtn = MAINWND_ON_DEFAULT;break;
    }
    /* *INDENT-ON* */

    return rtn;
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
onCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    HFONT hFont,hOldFont;

    hdc = GetDC( hwnd );
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    mainWndData.cxChar = tm.tmAveCharWidth;
    mainWndData.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

#if 0
    IoWndCreate( hwnd );
    mainWndData.execute = 0;
#endif

    /* コントロールをまとめて生成 */
    SomeCtrlCreate( hwnd );

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
onPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
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
onSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    mainWndData.cxClient = LOWORD( lParam );
    mainWndData.cyClient = HIWORD( lParam );

    /* コントロールをまとめて調整 */
    SomeCtrlSize( mainWndData.cxClient, mainWndData.cyChar );

#if 0
    IoWndSize( mainWndData.cxClient, mainWndData.cyClient );
#endif
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
onClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#if 0
    IoWndDestroy();
#endif

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
onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
#if 0
    static TCHAR szComPort[50];
    static S_COMM_PARAM commParam;
    TCHAR szBuf[100];

    switch( LOWORD(wParam) )
    {
    case (WND_SOME_CTRL_COMPORT_BUTTON+WND_SOME_CTRL_ID_OFFSET):
        if( mainWndData.execute == 0 )
        {
            /* COMポート名取得 */
            SomeCtrlGetText( WND_SOME_CTRL_COMPORT_COMBOBOX, szComPort );
            commParam.portStr = &szComPort;
            if( COMMopen( &commParam ) )
            {
                mainWndData.execute = 1;

                SomeCtrlDisable( WND_SOME_CTRL_COMPORT_COMBOBOX );
                SetWindowText( SomeCtrlGetHWND(WND_SOME_CTRL_COMPORT_BUTTON), TEXT("DISCONNECT") );
            }
            else
            {
                chMB("Cannot Open.");
            }
        }
        else
        {
            COMMclose();
            mainWndData.execute = 0;

            SomeCtrlEnable( WND_SOME_CTRL_COMPORT_COMBOBOX );
            SetWindowText( SomeCtrlGetHWND(WND_SOME_CTRL_COMPORT_BUTTON), TEXT("CONNECT") );
        }
        break;

    case (WND_SOME_CTRL_PROPERTY_BUTTON+WND_SOME_CTRL_ID_OFFSET):
        /* COMポート名取得 */
        SomeCtrlGetText( WND_SOME_CTRL_COMPORT_COMBOBOX, szComPort );
        commParam.portStr = &szComPort;
        COMMproperty( hwnd, &commParam );
        break;

#if 0
    case IDM_APP_EXIT:
        SendMessage( hwnd, WM_CLOSE, 0, 0 );
        break;
    case IDM_APP_ABOUT:
        DialogBox( ghInst, TEXT("ABOUT_BOX"), hwnd, AboutDlgProc );
        /* ダイアログボックスがクローズされるまで制御を返さない */
        break;
#endif
    default:
        break;
    }
#endif

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
onKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
onKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

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
onChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
onHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_VSCROLL の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
onVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}


/********************************************************************************
 * 内容  : WM_APP の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
onApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    switch( LOWORD(wParam) )
    {
    default:
        break;
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
onDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}
