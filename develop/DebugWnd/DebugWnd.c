/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "DebugWndDef.h"
#include "DebugMenuId.h"
#include "DebugWndMenu.h"

/* 外部関数定義 */
#include "WinMain.h"
#include "EditWnd.h"
#include "SomeCtrl.h"
#include "File.h"
#include "StsBar.h"
#include "Font.h"
#include "Config.h"
#include "DateTime.h"
#include "ModalDlg.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "DebugWnd.h"
LRESULT CALLBACK DebugWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static DEBUGWND_INDEX debugConvertMSGtoINDEX( UINT message );
static LRESULT debugOnCreate          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnPaint           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnSize            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnMove            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnClose           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnDestroy         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnCommand         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnKeyUp           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnKeyDown         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnChar            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnHscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnVscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnMouseWheel      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnSetFocus        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnKillFocus       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnDropFiles       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnInitMenuPopup   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnFindMsgString   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnApp             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnDefault         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static PTSTR getModuleString( void );
void debugDoCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave );
short debugAskAboutSave( HWND hwnd, TCHAR * szTitleName );

/* 内部変数定義 */
static HWND hDebugWnd; /* デバッグウィンドウのハンドラ */
static S_DEBUGWND_DATA debugWndData;
static TCHAR szCmdLineLocal[1024];

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[DEBUGWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    debugOnCreate          , /* WM_CREATE                     */
    debugOnPaint           , /* WM_PAINT                      */
    debugOnSize            , /* WM_SIZE                       */
    debugOnMove            , /* WM_MOVE                       */
    debugOnWindowPosChanged, /* WM_WINDOWPOSCHANGED           */
    debugOnClose           , /* WM_CLOSE                      */
    debugOnDestroy         , /* WM_DESTROY                    */
    debugOnCommand         , /* WM_COMMAND                    */
    debugOnKeyUp           , /* WM_KEYUP                      */
    debugOnKeyDown         , /* WM_KEYDOWN                    */
    debugOnChar            , /* WM_CHAR                       */
    debugOnHscroll         , /* WM_HSCROLL                    */
    debugOnVscroll         , /* WM_VSCROLL                    */
    debugOnMouseWheel      , /* WM_MOUSEWHEEL                 */
    debugOnSetFocus        , /* WM_SETFOCUS                   */
    debugOnKillFocus       , /* WM_KILLFOCUS                  */
    debugOnDropFiles       , /* WM_DROPFILES                  */
    debugOnInitMenuPopup   , /* WM_INITMENUPOPUP              */
    debugOnFindMsgString   , /* FINDMSGSTRINGの登録メッセージ */
    debugOnApp             , /* WM_APP                        */
    debugOnDefault           /* default                       */
};
/* *INDENT-ON* */

static TCHAR szModuleName[] = TEXT("Debug"); /* アプリケーションの名称 */

#define USE_EDITCONTROL

/********************************************************************************
 * 内容  : デバッグウィンドウクラスの登録、ウィンドウの生成
 * 引数  : LPSTR szCmdLine
 * 引数  : int nCmdShow
 * 引数  : HACCEL *hAccelPtr
 * 戻り値: HWND
 ***************************************/
HWND
DebugWndCreate( LPSTR szCmdLine, int nCmdShow, HACCEL *hAccelPtr )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = getModuleString();
    HMENU hMenu = NULL;

    hDebugWnd = NULL;

    /* デバッグウィンドウクラス */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) DebugWndProc;
    wc.cbClsExtra       = 0; /* クラス構造体の為の追加領域を予約する */
    wc.cbWndExtra       = 0; /* ウィンドウ構造体の為の追加領域を予約する */
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );          /* アイコン */
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1); /* 背景 */
    wc.lpszMenuName     = pAppName;
    wc.lpszClassName    = pAppName;

    if( !RegisterClass(&wc) )
    { /* ウィンドウクラスの登録失敗 */
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
    }
    else
    {
        /* 設定の初期化 */
        ConfigInit();

        /* メニューの生成 */
        hMenu = DebugMenuCreate();

        /* アクセラレータの生成 */
        if( hAccelPtr != NULL )
        {
            *(hAccelPtr) = DebugAccelCreate();
        }
        else
        {
            nop();
        }

        strncpy(szCmdLineLocal,szCmdLine,1024);

        /* デバッグウィンドウを作成 */
        InitCommonControls(); /* commctrl.hのインクルード、comctl32.libのプロジェクトへの参加が必要 */
        hDebugWnd = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                                   pAppName, pAppName,
                                   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_X) , ConfigLoadDword(CONFIG_ID_WINDOW_POS_Y) ,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_DX), ConfigLoadDword(CONFIG_ID_WINDOW_POS_DY),
                                   NULL, hMenu, hInst, NULL );

        if( hDebugWnd == NULL )
        { /* ウィンドウの生成失敗 */
            nop();
        }
        else
        {
            ShowWindow( hDebugWnd, nCmdShow ); /* ->WM_SIZE   */ /* ウインドウを表示 */
            UpdateWindow( hDebugWnd );         /* ->WM_PAINT  */
        }
    }

    return hDebugWnd;
}

/********************************************************************************
 * 内容  : デバッグウィンドウ内で処理するメッセージを判定する
 * 引数  : MSG *msgPtr
 * 戻り値: BOOL
 ***************************************/
BOOL
IsDebugWndMessage( MSG *msgPtr )
{
    BOOL bRtn = FALSE;

    if( (debugWndData.hDlgModeless) &&                      /* モードレスダイアログボックス表示中で、 */
        IsDialogMessage(debugWndData.hDlgModeless,msgPtr) ) /* 本メッセージがモードレスダイアログボックスのウィンドウプロシージャで処理された */
    {
        bRtn = TRUE;
    }
    else
    {
        nop();
    }

    return bRtn;
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
DebugWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return wndProcTbl[debugConvertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 関数名 : ConvertMSGtoINDEX()
 * 引数   : UINT message
 * 戻り値 : WNDPRC_INDEX
 * 内容   : ウィンドウズメッセージから対応するインデックスに変換する
 ***************************************/
static DEBUGWND_INDEX
debugConvertMSGtoINDEX( UINT message )
{
    DEBUGWND_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE          :rtn = DEBUGWND_ON_CREATE          ;break;
    case WM_PAINT           :rtn = DEBUGWND_ON_PAINT           ;break;
    case WM_SIZE            :rtn = DEBUGWND_ON_SIZE            ;break;
    case WM_MOVE            :rtn = DEBUGWND_ON_MOVE            ;break;
    case WM_WINDOWPOSCHANGED:rtn = DEBUGWND_ON_WINDOWPOSCHANGED;break;
    case WM_CLOSE           :rtn = DEBUGWND_ON_CLOSE           ;break;
    case WM_DESTROY         :rtn = DEBUGWND_ON_DESTROY         ;break;
    case WM_COMMAND         :rtn = DEBUGWND_ON_COMMAND         ;break;
    case WM_APP             :rtn = DEBUGWND_ON_APP             ;break;
    case WM_KEYUP           :rtn = DEBUGWND_ON_KEYUP           ;break;
    case WM_KEYDOWN         :rtn = DEBUGWND_ON_KEYDOWN         ;break;
    case WM_CHAR            :rtn = DEBUGWND_ON_CHAR            ;break;
    case WM_HSCROLL         :rtn = DEBUGWND_ON_HSCROLL         ;break;
    case WM_VSCROLL         :rtn = DEBUGWND_ON_VSCROLL         ;break;
    case WM_MOUSEWHEEL      :rtn = DEBUGWND_ON_MOUSEWHEEL      ;break;
    case WM_SETFOCUS        :rtn = DEBUGWND_ON_SETFOCUS        ;break;
    case WM_KILLFOCUS       :rtn = DEBUGWND_ON_KILLFOCUS       ;break;
    case WM_DROPFILES       :rtn = DEBUGWND_ON_DROPFILES       ;break;
    case WM_INITMENUPOPUP   :rtn = DEBUGWND_ON_INITMENUPOPUP   ;break;
    default                 :
        if( message == debugWndData.messageFindReplace )
        {
            rtn = DEBUGWND_ON_FINDMSGSTRING;
        }
        else
        {
            rtn = DEBUGWND_ON_DEFAULT;
        }
        break;
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
debugOnCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    HFONT hFont,hOldFont;

    memset( &debugWndData, 0, sizeof(debugWndData) );

    hdc = GetDC( hwnd );
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    debugWndData.cxChar = tm.tmAveCharWidth;
    debugWndData.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    debugWndData.messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

    ModalDlgInit();
    FileInitialize( hwnd ); /* ファイル初期化     */
    FontInit();

#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
    EditWndRegisterClass( GetHinst() );
    debugWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT("teddedit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL ,
                                         CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
                                         hwnd, (HMENU)0, GetHinst(), NULL );
#else                   /* [エディットコントロール使用] or  通常  */
    debugWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT ("edit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                         ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                         0, 0, 0, 0,
                                         hwnd, (HMENU)0, GetHinst(), NULL) ;
#endif                  /*  エディットコントロール使用  or  通常  */
    debugWndData.hFontIo = NULL;

#if 0
    SomeCtrlCreate( hwnd ); /* コントロールを生成 */
#endif
    StsBarCreate( hwnd, TRUE ); /* ステータスバー生成、デフォルト表示 */

    DebugMenuCheckItem( IDM_DEBUG_VIEW_STS_BAR );
    DebugMenuCheckItem( IDM_DEBUG_EXTEND_NEWLINE_CRLF );

    if( (szCmdLineLocal[0] != '\0') &&
        (FileSetName(FILE_ID_BIN,szCmdLineLocal,FALSE)) )
    {
        DWORD dwSize;
        PBYTE dataPtr;

        dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        EditWndDataSet( debugWndData.hWndIo,dataPtr,dwSize,TRUE );
#endif                  /*  エディットコントロール使用  or  通常  */
        debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
    }
    else
    {
        debugDoCaption( hwnd, "" , FALSE );
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
debugOnPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LONG topSizeSum=0,bottomSizeSum=0;

    debugWndData.cxClient = LOWORD( lParam );
    debugWndData.cyClient = HIWORD( lParam );

#if 0
    topSizeSum += SomeCtrlSize( debugWndData.cxClient, debugWndData.cyChar ); /* コントロール   */
#endif
    bottomSizeSum += StsBarSize( debugWndData.cxClient, debugWndData.cyChar ); /* ステータスバー */

    MoveWindow( debugWndData.hWndIo, 0, topSizeSum, debugWndData.cxClient, debugWndData.cyClient - topSizeSum - bottomSizeSum, TRUE) ;

    return 0;
}

/********************************************************************************
 * 内容  : WM_MOVE の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
debugOnMove( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    debugWndData.xPos = LOWORD( lParam );
    debugWndData.yPos = HIWORD( lParam );

    return 0;
}

/********************************************************************************
 * 内容  : WM_WINDOWPOSCHANGED の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
debugOnWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    debugWndData.xWindowPos = ((WINDOWPOS *)lParam)->x ;
    debugWndData.yWindowPos = ((WINDOWPOS *)lParam)->y ;
    debugWndData.cxWindow   = ((WINDOWPOS *)lParam)->cx;
    debugWndData.cyWindow   = ((WINDOWPOS *)lParam)->cy;

    return DefWindowProc( hwnd, message, wParam, lParam );
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
debugOnClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_X , debugWndData.xWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_Y , debugWndData.yWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DX, debugWndData.cxWindow   );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DY, debugWndData.cyWindow   );

        DestroyWindow( hwnd );
    }

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
debugOnDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyAcceleratorTable( debugWndData.hAccel );

    DestroyWindow( debugWndData.hWndIo );
    FileEnd();

    PostQuitMessage(0); /* WM_QUITメッセージをポストする */
    return 0;
}

/********************************************************************************
 * 内容  : WM_COMMAND の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam 通常はLOWORDが通知コード。(コントロールからの通知時はHIWORDが通知コード、LOWORDがコントロールID)
 * 引数  : LPARAM lParam 通常はNULL。(コントロールからの通知時はウィンドウハンドル)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
debugOnCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD dwSize;
    PBYTE dataPtr;
    PTSTR strPtr;
    S_MODAL_DLG_DATA modalDlgData;
    static FINDREPLACE fr;
    static TCHAR strFind[80],strRep[80],strMsg[1024];
    HFONT hFontOld;

    if( (HWND)lParam == debugWndData.hWndIo )
    {
        switch( HIWORD(wParam) )
        {
        case EN_UPDATE:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            StsBarSetText( STS_BAR_0  ,"%d バイト、全 %d 行",EditWndGetDataSize(debugWndData.hWndIo, EDITWND_ALL),(DWORD)SendMessage(debugWndData.hWndIo,EM_GETLINECOUNT,0,0) );
            StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",(DWORD)(SendMessage(debugWndData.hWndIo,EM_LINEFROMCHAR,-1,0))+1,EditWndGetCaretXpos(debugWndData.hWndIo)+1);
#else                   /* [エディットコントロール使用] or  通常  */
            StsBarSetText( STS_BAR_0  ,"%d バイト、全 %d 行",GetWindowTextLength(debugWndData.hWndIo),(DWORD)SendMessage(debugWndData.hWndIo,EM_GETLINECOUNT,0,0) );
            StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",(DWORD)(SendMessage(debugWndData.hWndIo,EM_LINEFROMCHAR,-1,0))+1,0);
#endif                  /*  エディットコントロール使用  or  通常  */
            break;
        case EN_CHANGE:
            if( debugWndData.bNeedSave )
            {
                nop();
            }
            else
            {
                debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),TRUE );
                debugWndData.bNeedSave = TRUE;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch( LOWORD(wParam) )
        {
        case IDM_DEBUG_FILE_NEW:
            if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                debugWndData.bNeedSave = FALSE;
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
                EditWndDataInit(debugWndData.hWndIo);
#endif                  /*  エディットコントロール使用  or  通常  */
                debugDoCaption( hwnd, "", FALSE);
            }
            break;
        case IDM_DEBUG_FILE_OPEN:
            if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
                {
                    debugWndData.bNeedSave = FALSE;
                    dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
                    EditWndDataSet( debugWndData.hWndIo, dataPtr,dwSize,TRUE );
#endif                  /*  エディットコントロール使用  or  通常  */
                    debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN), FALSE );
                }
                else
                {
                    /* キャンセルされた。又はエラー */
                }
            }
            break;
        case IDM_DEBUG_FILE_SAVE:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            dwSize = EditWndGetDataSize(debugWndData.hWndIo,EDITWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                EditWndDataGet( debugWndData.hWndIo, dataPtr,dwSize,EDITWND_ALL );
                if( (FileWrite( FILE_ID_BIN, dataPtr, dwSize )) == FILE_NAME_NOT_SET )
                {
                    if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                    {
                        debugWndData.bNeedSave = FALSE;
                        debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                        FileWrite( FILE_ID_BIN, dataPtr, dwSize );
                        rtn = 1;
                    }
                    else
                    { /* キャンセル */
                        nop();
                    }
                }
                else
                {
                    debugWndData.bNeedSave = FALSE;
                    debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                    FileWrite( FILE_ID_BIN, dataPtr, dwSize );
                    rtn = 1;
                }
                free( dataPtr );
            }
            else
            {
                nop();
            }
#endif                  /*  エディットコントロール使用  or  通常  */
            break;
        case IDM_DEBUG_FILE_SAVE_AS:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            dwSize = EditWndGetDataSize(debugWndData.hWndIo,EDITWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                EditWndDataGet( debugWndData.hWndIo, dataPtr,dwSize,EDITWND_ALL );
                if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                {
                    debugWndData.bNeedSave = FALSE;
                    debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                    FileWrite( FILE_ID_BIN, dataPtr, dwSize );
                }
                else
                {
                    nop();
                }
                free( dataPtr );
            }
            else
            {
                nop();
            }
#endif                  /*  エディットコントロール使用  or  通常  */
            break;

        case IDM_DEBUG_EDIT_UNDO:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            SendMessage( debugWndData.hWndIo, WM_UNDO, 0, 0 );
#else                   /* [エディットコントロール使用] or  通常  */
            SendMessage( debugWndData.hWndIo, EM_UNDO, 0, 0 );
#endif                  /*  エディットコントロール使用  or  通常  */
            break;

        case IDM_DEBUG_EDIT_CUT:
            SendMessage( debugWndData.hWndIo, WM_CUT, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_COPY:
            SendMessage( debugWndData.hWndIo, WM_COPY, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_PASTE:
            SendMessage( debugWndData.hWndIo, WM_PASTE, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_DELETE:
            SendMessage( debugWndData.hWndIo, WM_CLEAR, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_FIND:
            fr.lStructSize   = sizeof (FINDREPLACE);
            fr.hwndOwner     = hwnd;
            fr.Flags         = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat = strFind;
            fr.wFindWhatLen  = 80;
            debugWndData.hDlgModeless = FindText(&fr);
            break;

        case IDM_DEBUG_EDIT_REPLACE:
            fr.lStructSize      = sizeof(FINDREPLACE);
            fr.hwndOwner        = hwnd;
            fr.Flags            = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat    = strFind;
            fr.lpstrReplaceWith = strRep;
            fr.wReplaceWithLen  = fr.wFindWhatLen = 80;
            debugWndData.hDlgModeless = ReplaceText(&fr);
            break;

        case IDM_DEBUG_EDIT_FIND_NEXT:
            break;

        case IDM_DEBUG_EDIT_SELECT_ALL:
            SendMessage( debugWndData.hWndIo, EM_SETSEL, 0, -1 );
            break;

        case IDM_DEBUG_EDIT_DATETIME:
            strPtr = DateTimeGetString();
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            EditWndDataSet( debugWndData.hWndIo, strPtr, strlen(strPtr), FALSE );
#endif                  /*  エディットコントロール使用  or  通常  */
            break;

        case IDM_DEBUG_FORMAT_FONT:
            if( FontChooseFont( hwnd, FONT_ID_IO ) )
            {
                hFontOld = debugWndData.hFontIo;
                debugWndData.hFontIo = CreateFontIndirect( FontGetLogFont(FONT_ID_IO) );
                SendMessage( debugWndData.hWndIo, WM_SETFONT, (WPARAM)debugWndData.hFontIo, (LPARAM)TRUE );

                if( hFontOld != NULL )
                {
                    DeleteObject(hFontOld);
                }
                else
                {
                    nop();
                }
            }
            else
            {
                nop();
            }
            break;

        case IDM_DEBUG_VIEW_STS_BAR:
            if( DebugMenuInqItemChecked(IDM_DEBUG_VIEW_STS_BAR) )
            {
                DebugMenuUnCheckItem( IDM_DEBUG_VIEW_STS_BAR );
                StsBarShowWindow( FALSE );
            }
            else
            {
                DebugMenuCheckItem( IDM_DEBUG_VIEW_STS_BAR );
                StsBarShowWindow( TRUE );
            }
            SendMessage(hwnd,WM_SIZE,0,MAKELONG(debugWndData.cxClient,debugWndData.cyClient));
            break;

#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        case IDM_DEBUG_EXTEND_NEWLINE_CRLF:
            EditWndNewLineCodeSet(debugWndData.hWndIo,NEWLINECODE_CRLF);
            DebugMenuCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CRLF );
            DebugMenuUnCheckItem( IDM_DEBUG_EXTEND_NEWLINE_LF   );
            DebugMenuUnCheckItem( IDM_DEBUG_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR+LF");
            break;

        case IDM_DEBUG_EXTEND_NEWLINE_LF  :
            EditWndNewLineCodeSet(debugWndData.hWndIo,NEWLINECODE_LF);
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CRLF );
            DebugMenuCheckItem    ( IDM_DEBUG_EXTEND_NEWLINE_LF   );
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"LF");
            break;

        case IDM_DEBUG_EXTEND_NEWLINE_CR  :
            EditWndNewLineCodeSet(debugWndData.hWndIo,NEWLINECODE_CR);
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CRLF );
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_LF   );
            DebugMenuCheckItem    ( IDM_DEBUG_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR");
            break;
#endif                  /*  エディットコントロール使用  or  通常  */

        case IDM_DEBUG_FILE_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;

        case IDM_DEBUG_HELP_ABOUT:
            ModalDlg( MODAL_DLG_ID_ABOUT, &modalDlgData, hwnd, debugWndData.xPos, debugWndData.yPos );
            break;

        default:
            break;
        }
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
debugOnKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnMouseWheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(debugWndData.hWndIo,message,wParam,lParam);

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
debugOnSetFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SetFocus(debugWndData.hWndIo);

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
debugOnKillFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(debugWndData.hWndIo,message,wParam,lParam);

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
debugOnDropFiles( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    static TCHAR szFileName[1024];
    DWORD dwSize;
    PBYTE dataPtr;

    if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        debugWndData.bNeedSave = FALSE;
        DragQueryFile( (HDROP)wParam, (UINT)0, (LPSTR)szFileName, (UINT)sizeof(szFileName) );

        FileSetName( FILE_ID_BIN, szFileName, FALSE );
        dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        EditWndDataSet( debugWndData.hWndIo,dataPtr,dwSize,TRUE );
#endif                  /*  エディットコントロール使用  or  通常  */
        debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_INITMENUPOPUP を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
debugOnInitMenuPopup( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    if( LOWORD(lParam) == 1 )
    { /* 「編集」 */
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        if( EditWndGetDataSize(debugWndData.hWndIo,EDITWND_SELECTED) )
        {
            DebugMenuEnableItem( IDM_DEBUG_EDIT_CUT    );
            DebugMenuEnableItem( IDM_DEBUG_EDIT_COPY   );
            DebugMenuEnableItem( IDM_DEBUG_EDIT_DELETE );
        }
        else
        {
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_CUT    );
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_COPY   );
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_DELETE );
        }
#endif                  /*  エディットコントロール使用  or  通常  */

        if( IsClipboardFormatAvailable(CF_TEXT) )
        {
            DebugMenuEnableItem( IDM_DEBUG_EDIT_PASTE );
        }
        else
        {
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_PASTE );
        }
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : FINDMSGSTRINGの登録メッセージ の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
debugOnFindMsgString( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    LPFINDREPLACE pfr;

    pfr = (LPFINDREPLACE)lParam;

    if( pfr->Flags & FR_DIALOGTERM )
    {
        debugWndData.hDlgModeless = NULL;
    }
    else
    {
        nop();
    }

#if 0
    if( (pfr->Flags & FR_FINDNEXT) && !PopFindFindText(hwndEdit,&iOffset,pfr) )
    {
        OkMessage(hwnd, TEXT ("Text not found!"),TEXT ("\0")) ;
    }
    else
    {
        nop();
    }

    if( ((pfr->Flags & FR_REPLACE) || (pfr->Flags & FR_REPLACEALL)) && (!PopFindReplaceText(hwndEdit,&iOffset,pfr)) )
    {
        OkMessage (hwnd, TEXT ("Text not found!"),TEXT ("\0")) ;
    }
    else
    {
        nop();
    }

    if( pfr->Flags & FR_REPLACEALL )
    {
        while( PopFindReplaceText(hwndEdit, &iOffset, pfr) );
    }
    else
    {
        nop();
    }
#endif

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
debugOnApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 内容   : モジュールの名称文字列の取得
 * 引数   : なし
 * 戻り値 : PTSTR
 ***************************************/
static PTSTR
getModuleString( void )
{
    return szModuleName;
}

/********************************************************************************
 * 内容  :
 * 引数  : HWND hwnd
 * 引数  : TCHAR* szTitleName
 * 引数  : BOOL bNeedSave
 * 戻り値: なし
 ***************************************/
void
debugDoCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave )
{
     TCHAR szCaption[64 + MAX_PATH];

     if( bNeedSave )
     {
         wsprintf( szCaption, TEXT ("*%s - %s"), (szTitleName[0] ? szTitleName : TEXT("無題")),getModuleString() );
     }
     else
     {
         wsprintf( szCaption, TEXT ("%s - %s"), (szTitleName[0] ? szTitleName : TEXT("無題")),getModuleString() );
     }

     SetWindowText( hwnd, szCaption );
}

/********************************************************************************
 * 内容  :
 * 引数  : HWND hwnd
 * 引数  : TCHAR* szTitleName
 * 戻り値: なし
 ***************************************/
short
debugAskAboutSave( HWND hwnd, TCHAR * szTitleName )
{
    TCHAR szBuffer[64 + MAX_PATH];
    int   iReturn;

    wsprintf(szBuffer, TEXT("ファイル %s の内容は変更されています。\n\n変更を保存しますか?"), szTitleName[0] ? szTitleName : TEXT("無題") );

    iReturn = MessageBox( hwnd,szBuffer,getModuleString(),MB_YESNOCANCEL|MB_ICONEXCLAMATION );

    if( iReturn == IDYES )
    {
        if( !SendMessage( hwnd,WM_COMMAND,IDM_DEBUG_FILE_SAVE,0) )
        {
            iReturn = IDCANCEL;
        }
        else
        {
            nop();
        }
    }
    else
    {
        nop();
    }

    return iReturn;
}
