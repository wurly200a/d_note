/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "MainWndDef.h"
#include "MainWndMenu.h"

/* 外部関数定義 */
#include "WinMain.h"
#include "IoWnd.h"
#include "SomeCtrl.h"
#include "File.h"
#include "StsBar.h"
#include "Font.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static MAINWND_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onPaint        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyUp        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyDown      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onChar         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onHscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onVscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMouseWheel   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSetFocus     ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKillFocus    ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDropFiles    ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

void doCaption( HWND hwnd, TCHAR* szTitleName );

/* 内部変数定義 */
static HWND hwndMain; /* メインウィンドウのハンドラ */
static S_MAINWND_DATA mainWndData;

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[MAINWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate       , /* WM_CREATE           */
    onPaint        , /* WM_PAINT            */
    onSize         , /* WM_SIZE             */
    onClose        , /* WM_CLOSE            */
    onDestroy      , /* WM_DESTROY          */
    onCommand      , /* WM_COMMAND          */
    onKeyUp        , /* WM_KEYUP            */
    onKeyDown      , /* WM_KEYDOWN          */
    onChar         , /* WM_CHAR             */
    onHscroll      , /* WM_HSCROLL          */
    onVscroll      , /* WM_VSCROLL          */
    onMouseWheel   , /* WM_MOUSEWHEEL       */
    onSetFocus     , /* WM_SETFOCUS         */
    onKillFocus    , /* WM_KILLFOCUS        */
    onDropFiles    , /* WM_DROPFILES        */
    onApp          , /* WM_APP              */
    onDefault        /* default             */
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
    HMENU hMenuPopup;

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

    mainWndData.hMenu = CreateMenu();

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_NEW       , TEXT("新規(&N)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_OPEN      , TEXT("開く(&O)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE      , TEXT("上書き保存(&S)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE_AS   , TEXT("名前を付けて保存(&A)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                  , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PAGE      , TEXT("ページ設定(&U)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PRINT     , TEXT("印刷(&P)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                  , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_EXIT      , TEXT("終了(&X)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("ファイル(&F)") );

    EnableMenuItem( mainWndData.hMenu, IDM_FILE_SAVE      , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_FILE_SAVE_AS   , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_FILE_PAGE      , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_FILE_PRINT     , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_UNDO       , TEXT("元に戻す(&U)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_CUT        , TEXT("切り取り(&T)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_COPY       , TEXT("コピー(&C)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_PASTE      , TEXT("貼り付け(&P)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DELETE     , TEXT("削除(&L)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND       , TEXT("検索(&F)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND_NEXT  , TEXT("次を検索(&N)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_REPLACE    , TEXT("置換(&R)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_GOTO_LINE  , TEXT("行へ移動(&G)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("全て選択(&A)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DATETIME   , TEXT("日付と時刻(&D)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("編集(&E)") );

    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_UNDO       , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_CUT        , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_COPY       , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_PASTE      , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_DELETE     , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_FIND       , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_FIND_NEXT  , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_REPLACE    , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_GOTO_LINE  , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_SELECT_ALL , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_DATETIME   , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_WRAP , TEXT("右端で折り返す(&W)") );
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_FONT , TEXT("フォント(&F)...") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("書式(&O)") );

    EnableMenuItem( mainWndData.hMenu, IDM_FORMAT_WRAP , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_VIEW_STS_BAR, TEXT("ステータス バー(&S)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("表示(&V)") );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_HELP  , TEXT("トピックの検索(&H)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0              , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_ABOUT , TEXT("バージョン情報(&A)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("ヘルプ(&H)") );

    EnableMenuItem( mainWndData.hMenu, IDM_HELP_HELP  , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_HELP_ABOUT , MF_GRAYED );

    /* メインウィンドウを作成 */
    InitCommonControls(); /* commctrl.hのインクルード、comctl32.libのプロジェクトへの参加が必要 */
    hwndMain = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                               pAppName, pAppName,
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               WND_WIDTH    , WND_HEIGHT,
                               NULL, mainWndData.hMenu, hInst, NULL);

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
    case WM_CREATE       :rtn = MAINWND_ON_CREATE       ;break;
    case WM_PAINT        :rtn = MAINWND_ON_PAINT        ;break;
    case WM_SIZE         :rtn = MAINWND_ON_SIZE         ;break;
    case WM_CLOSE        :rtn = MAINWND_ON_CLOSE        ;break;
    case WM_DESTROY      :rtn = MAINWND_ON_DESTROY      ;break;
    case WM_COMMAND      :rtn = MAINWND_ON_COMMAND      ;break;
    case WM_APP          :rtn = MAINWND_ON_APP          ;break;
    case WM_KEYUP        :rtn = MAINWND_ON_KEYUP        ;break;
    case WM_KEYDOWN      :rtn = MAINWND_ON_KEYDOWN      ;break;
    case WM_CHAR         :rtn = MAINWND_ON_CHAR         ;break;
    case WM_HSCROLL      :rtn = MAINWND_ON_HSCROLL      ;break;
    case WM_VSCROLL      :rtn = MAINWND_ON_VSCROLL      ;break;
    case WM_MOUSEWHEEL   :rtn = MAINWND_ON_MOUSEWHEEL   ;break;
    case WM_SETFOCUS     :rtn = MAINWND_ON_SETFOCUS     ;break;
    case WM_KILLFOCUS    :rtn = MAINWND_ON_KILLFOCUS    ;break;
    case WM_DROPFILES    :rtn = MAINWND_ON_DROPFILES    ;break;
    default              :rtn = MAINWND_ON_DEFAULT      ;break;
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


    FileInitialize( hwnd ); /* ファイル初期化     */
    FontInit();

    mainWndData.hWndIo = IoWndCreate( hwnd, FontGetLogFont(FONT_ID_IO) );
#if 0
    SomeCtrlCreate( hwnd ); /* コントロールを生成 */
#endif
    StsBarCreate( hwnd, TRUE ); /* ステータスバー生成、デフォルト表示 */
    CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_CHECKED );

    doCaption( hwnd, "" );

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
    LONG topSizeSum=0,bottomSizeSum=0;

    mainWndData.cxClient = LOWORD( lParam );
    mainWndData.cyClient = HIWORD( lParam );

#if 0
    topSizeSum += SomeCtrlSize( mainWndData.cxClient, mainWndData.cyChar ); /* コントロール   */
#endif
    bottomSizeSum += StsBarSize( mainWndData.cxClient, mainWndData.cyChar ); /* ステータスバー */
    IoWndSize( 0, topSizeSum, mainWndData.cxClient, mainWndData.cyClient - topSizeSum - bottomSizeSum );

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
    IoWndDestroy();
    FileEnd();

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
    DWORD dwSize;
    PBYTE dataPtr;
    INT iMenuFlags;
    MENUITEMINFO menuItemInfo;

    switch( LOWORD(wParam) )
    {
    case IDM_FILE_NEW:
        IoWndPrint( NULL, 0 );
        doCaption( hwnd, "" );
        break;
    case IDM_FILE_OPEN:
        if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
        {
            dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
            IoWndPrint( dataPtr,dwSize );
            doCaption( hwnd, FileGetTitleName(FILE_ID_BIN) );
        }
        else
        {
            /* キャンセルされた。又はエラー */
        }
        break;
#if 0
    case (SOME_CTRL_FILEOPEN_BUTTON+SOME_CTRL_ID_OFFSET):
        if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
        {
            SetWindowText( SomeCtrlGetHWND(SOME_CTRL_FILENAME), FileGetName(FILE_ID_BIN) );
            dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
            IoWndPrint( dataPtr,dwSize );
        }
        else
        {
            /* キャンセルされた。又はエラー */
        }
        break;
#endif

    case IDM_FORMAT_FONT:
        if( FontChooseFont( hwnd, FONT_ID_IO ) )
        {
            IoWndChangeFont( FontGetLogFont(FONT_ID_IO) );
        }
        else
        {
            nop();
        }
        break;

    case IDM_VIEW_STS_BAR:
#if 1
        iMenuFlags = GetMenuState( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_BYCOMMAND );
        if( iMenuFlags & MF_CHECKED )
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_UNCHECKED );
            StsBarShowWindow( FALSE );
        }
        else
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_CHECKED );
            StsBarShowWindow( TRUE );
        }
#else
        menuItemInfo.cbSize = sizeof(MENUITEMINFO);
        menuItemInfo.fMask  = MIIM_STATE;
        GetMenuItemInfo( mainWndData.hMenu, IDM_VIEW_STS_BAR, FALSE, &menuItemInfo );
        if( menuItemInfo.fState & MFS_CHECKED )
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_UNCHECKED );
            StsBarShowWindow( FALSE );
        }
        else
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_CHECKED );
            StsBarShowWindow( TRUE );
        }
#endif
        SendMessage(hwnd,WM_SIZE,0,MAKELONG(mainWndData.cxClient,mainWndData.cyClient));
        break;

    case IDM_FILE_EXIT:
        SendMessage( hwnd, WM_CLOSE, 0, 0 );
        break;
    default:
        break;
    }

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
 * 内容  : WM_MOUSEWHEEL を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
onMouseWheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(mainWndData.hWndIo,message,wParam,lParam);

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
onSetFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SetFocus(mainWndData.hWndIo);

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
onKillFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(mainWndData.hWndIo,message,wParam,lParam);

    return rtn;
}

/********************************************************************************
 * 内容  : WM_DROPFILES を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
onDropFiles( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    static TCHAR szFileName[1024];
    DWORD dwSize;
    PBYTE dataPtr;

    DragQueryFile( wParam, 0, szFileName, sizeof(szFileName) );

    FileSetName( FILE_ID_BIN, szFileName, FALSE );
    dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
    IoWndPrint( dataPtr,dwSize );
    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN) );

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

/********************************************************************************
 * 内容  :
 * 引数  : HWND hwnd
 * 引数  : TCHAR* szTitleName
 * 戻り値: なし
 ***************************************/
void
doCaption( HWND hwnd, TCHAR* szTitleName )
{
     TCHAR szCaption[64 + MAX_PATH];

     wsprintf( szCaption, TEXT ("%s - %s"), (szTitleName[0] ? szTitleName : TEXT("無題")),GetAppName() );

     SetWindowText( hwnd, szCaption );
}
