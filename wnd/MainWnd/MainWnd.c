/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "MainWndDef.h"

/* 外部関数定義 */
#include "SomeCtrl.h"

/* 外部変数定義 */
extern HINSTANCE ghInst;      /* インスタンスのハンドラ     */
extern TCHAR     szAppName[]; /* アプリケーションの名称 */

/* 内部関数定義 */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static WNDPROC_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyUp  ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onChar   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

/* 内部変数定義 */
static HWND hwndMain; /* メインウィンドウのハンドラ */
static HWND hwndSbar;
static S_MAIN_STATUS mainSts;

static int sbarColWidth[5] = { 100, 200, 300, 400, -1 };

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[WNDPROC_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
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
 * 関数名 : MainWndCreate()
 * 引数   : int nCmdShow
 * 戻り値 : BOOL
 * 内容   : メインウィンドウクラスの登録、ウィンドウの生成
 ********************************************************************************/
BOOL
MainWndCreate( int nCmdShow )
{
    WNDCLASS wc = {0};

    /* メインウィンドウクラス */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) WndProc;
    wc.cbClsExtra       = 0; /* クラス構造体の為の追加領域を予約する */
    wc.cbWndExtra       = 0; /* ウィンドウ構造体の為の追加領域を予約する */
    wc.hInstance        = ghInst;
    wc.hIcon            = LoadIcon( ghInst, szAppName );          /* アイコン */
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1); /* 背景 */
    wc.lpszMenuName     = szAppName;
    wc.lpszClassName    = szAppName;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), szAppName, MB_ICONERROR );
        return FALSE;
    }

    /* メインウィンドウを作成 */
    InitCommonControls(); /* commctrl.hのインクルード、comctl32.libのプロジェクトへの参加が必要 */
    hwndMain = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                               szAppName, szAppName,
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               WND_WIDTH    , WND_HEIGHT,
                               NULL, NULL, ghInst, NULL);

    if( hwndMain == NULL )
    {
        return FALSE;
    }

    ShowWindow( hwndMain, nCmdShow ); /* ->WM_SIZE   */ /* ウインドウを表示 */
    UpdateWindow( hwndMain );         /* ->WM_PAINT  */

    return TRUE;
}

/********************************************************************************
 * 関数名 : IsMainWndMessage()
 * 引数   : MSG *msg
 * 戻り値 : BOOL
 * 内容   : メインウィンドウ内で処理するメッセージを判定する
 ********************************************************************************/
BOOL
IsMainWndMessage( MSG *msg )
{
    return FALSE;
}

/********************************************************************************
 * 関数名 : WndProc()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : ウィンドウプロシージャ。WINDOWSから受け取ったメッセージを処理する
 ********************************************************************************/
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return wndProcTbl[convertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 関数名 : ConvertMSGtoINDEX()
 * 引数   : UINT message
 * 戻り値 : MAIN_WNDPRC_INDEX
 * 内容   : ウィンドウズメッセージから対応するインデックスに変換する
 ********************************************************************************/
static WNDPROC_INDEX convertMSGtoINDEX( UINT message )
{
    WNDPROC_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE :rtn = WNDPROC_ON_CREATE ;break;
    case WM_PAINT  :rtn = WNDPROC_ON_PAINT  ;break;
    case WM_SIZE   :rtn = WNDPROC_ON_SIZE   ;break;
    case WM_CLOSE  :rtn = WNDPROC_ON_CLOSE  ;break;
    case WM_DESTROY:rtn = WNDPROC_ON_DESTROY;break;
    case WM_COMMAND:rtn = WNDPROC_ON_COMMAND;break;
    case WM_APP    :rtn = WNDPROC_ON_APP    ;break;
    case WM_KEYUP  :rtn = WNDPROC_ON_KEYUP  ;break;
    case WM_KEYDOWN:rtn = WNDPROC_ON_KEYDOWN;break;
    case WM_CHAR   :rtn = WNDPROC_ON_CHAR   ;break;
    case WM_HSCROLL:rtn = WNDPROC_ON_HSCROLL;break;
    case WM_VSCROLL:rtn = WNDPROC_ON_VSCROLL;break;
    default        :rtn = WNDPROC_ON_DEFAULT;break;
    }
    /* *INDENT-ON* */

    return rtn;
}

/********************************************************************************
 * 関数名 : onCreate()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_CREATE を処理する
 ********************************************************************************/
static LRESULT onCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    HFONT hFont,hOldFont;

    hdc = GetDC( hwnd );
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    mainSts.cxChar = tm.tmAveCharWidth;
    mainSts.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

#if 0
    IoWndCreate( hwnd );
    mainSts.execute = 0;
#endif

    /* コントロールをまとめて生成 */
    SomeCtrlCreate( hwnd );

    return rtn;
}

/********************************************************************************
 * 関数名 : onPaint()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_PAINT を処理する
 ********************************************************************************/
static LRESULT onPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
  関数名 : onSize()
  引数   : ウィンドウハンドラ HWND hwnd
           メッセージ         UINT message
           パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
           パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
  戻り値 : LRESULT
  内容   : WM_SIZE を処理する
********************************************************************************/
static LRESULT onSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    mainSts.cxClient = LOWORD( lParam );
    mainSts.cyClient = HIWORD( lParam );

    /* コントロールをまとめて調整 */
    SomeCtrlSize( mainSts.cxClient, mainSts.cyChar );

#if 0
    IoWndSize( mainSts.cxClient, mainSts.cyClient );
#endif
    return 0;
}

/********************************************************************************
 * 関数名 : onClose()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_CLOSE を処理する
 ********************************************************************************/
static LRESULT onClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyWindow( hwnd );

    return 0;
}

/********************************************************************************
 * 関数名 : onDestroy()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_DESTROY を処理する
 ********************************************************************************/
static LRESULT onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#if 0
    IoWndDestroy();
#endif

    PostQuitMessage(0); /* WM_QUITメッセージをポストする */
    return 0;
}

/********************************************************************************
 * 関数名 : onCommand()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_COMMAND を処理する
 ********************************************************************************/
static LRESULT onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
#if 0
    static TCHAR szComPort[50];
    static S_COMM_PARAM commParam;
    TCHAR szBuf[100];

    switch( LOWORD(wParam) )
    {
    case (WND_SOME_CTRL_COMPORT_BUTTON+WND_SOME_CTRL_ID_OFFSET):
        if( mainSts.execute == 0 )
        {
            /* COMポート名取得 */
            SomeCtrlGetText( WND_SOME_CTRL_COMPORT_COMBOBOX, szComPort );
            commParam.portStr = &szComPort;
            if( COMMopen( &commParam ) )
            {
                mainSts.execute = 1;

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
            mainSts.execute = 0;

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
 * 関数名 : onKeyUp()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_KEYUP を処理する
 ********************************************************************************/
static LRESULT onKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 関数名 : onKeyDown()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_KEYDOWN を処理する
 ********************************************************************************/
static LRESULT onKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 関数名 : onChar()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_CHAR を処理する
 ********************************************************************************/
static LRESULT onChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 関数名 : onHscroll()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_HSCROLL を処理する
 ********************************************************************************/
static LRESULT onHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 関数名 : onVscroll()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_VSCROLL を処理する
 ********************************************************************************/
static LRESULT onVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}


/********************************************************************************
 * 関数名 : onApp()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : WM_APP を処理する
 ********************************************************************************/
static LRESULT onApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
 * 関数名 : onDefault()
 * 引数   : ウィンドウハンドラ HWND hwnd
 *          メッセージ         UINT message
 *          パラメータ1        WPARAM wParam (内容はメッセージの種類により異なる)
 *          パラメータ2        LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値 : LRESULT
 * 内容   : メッセージのデフォルト処理
 ********************************************************************************/
static LRESULT onDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}
