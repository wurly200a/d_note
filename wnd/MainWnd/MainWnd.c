/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "MainWndDef.h"
#include "MenuId.h"
#include "MainWndMenu.h"

/* 外部関数定義 */
#include "EditWnd.h"
#include "SomeCtrl.h"
#include "File.h"
#include "StsBar.h"
#include "Font.h"
#include "Config.h"
#include "DateTime.h"
#include "ModalDlg.h"

#include "DebugWnd.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static MAINWND_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMove            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMouseWheel      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMouseHwheel     ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSetFocus        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKillFocus       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDropFiles       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onInitMenuPopup   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onFindMsgString   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

int okMessage( HWND hwnd, TCHAR *szMessageFormat, ... );
void doCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave );
short AskAboutSave( HWND hwnd, TCHAR * szTitleName );

/* 内部変数定義 */
static HWND hwndMain; /* メインウィンドウのハンドラ */
static S_MAINWND_DATA mainWndData;
static TCHAR szCmdLineLocal[1024];

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[MAINWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate          , /* WM_CREATE                     */
    onSize            , /* WM_SIZE                       */
    onMove            , /* WM_MOVE                       */
    onWindowPosChanged, /* WM_WINDOWPOSCHANGED           */
    onClose           , /* WM_CLOSE                      */
    onDestroy         , /* WM_DESTROY                    */
    onCommand         , /* WM_COMMAND                    */
    onMouseWheel      , /* WM_MOUSEWHEEL                 */
    onMouseHwheel     , /* WM_MOUSEHWHEEL                */
    onSetFocus        , /* WM_SETFOCUS                   */
    onKillFocus       , /* WM_KILLFOCUS                  */
    onDropFiles       , /* WM_DROPFILES                  */
    onInitMenuPopup   , /* WM_INITMENUPOPUP              */
    onFindMsgString   , /* FINDMSGSTRINGの登録メッセージ */
    onDefault           /* default                       */
};
/* *INDENT-ON* */

#if 0
#define USE_EDITCONTROL
#endif

/********************************************************************************
 * 内容  : メインウィンドウクラスの登録、ウィンドウの生成
 * 引数  : HINSTANCE hInst
 * 引数  : PTSTR szAppName
 * 引数  : LPSTR szCmdLine
 * 引数  : int nCmdShow
 * 引数  : HACCEL *hAccelPtr
 * 戻り値: HWND
 ***************************************/
HWND
MainWndCreate( HINSTANCE hInst, PTSTR szAppName, LPSTR szCmdLine, int nCmdShow, HACCEL *hAccelPtr )
{
    WNDCLASS wc = {0};
    PTSTR pAppName = szAppName;
    HMENU hMenu = NULL;

    hwndMain = NULL;

    /* メインウィンドウクラス */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) WndProc;
    wc.cbClsExtra       = 0; /* クラス構造体の為の追加領域を予約する */
    wc.cbWndExtra       = 0; /* ウィンドウ構造体の為の追加領域を予約する */
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, "MAIN_ICON" );          /* アイコン */
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
        memset( &mainWndData, 0, sizeof(mainWndData) );
        mainWndData.hInstance = hInst;
        mainWndData.szAppName = szAppName;

        /* 設定の初期化 */
        ConfigInit(mainWndData.hInstance,mainWndData.szAppName);

        /* メニューの生成 */
        hMenu = MenuCreate();

        /* アクセラレータの生成 */
        if( hAccelPtr != NULL )
        {
            *(hAccelPtr) = AccelCreate();
        }
        else
        {
            nop();
        }

        strncpy(szCmdLineLocal,szCmdLine,1024);

        /* メインウィンドウを作成 */
        InitCommonControls(); /* commctrl.hのインクルード、comctl32.libのプロジェクトへの参加が必要 */
        hwndMain = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                                   pAppName, pAppName,
                                   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_X) , ConfigLoadDword(CONFIG_ID_WINDOW_POS_Y) ,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_DX), ConfigLoadDword(CONFIG_ID_WINDOW_POS_DY),
                                   NULL, hMenu, hInst, NULL );

        if( hwndMain == NULL )
        { /* ウィンドウの生成失敗 */
            nop();
        }
        else
        {
            ShowWindow( hwndMain, nCmdShow ); /* ->WM_SIZE   */ /* ウインドウを表示 */
            UpdateWindow( hwndMain );         /* ->WM_PAINT  */
        }
    }

    return hwndMain;
}

/********************************************************************************
 * 内容  : メインウィンドウ内で処理するメッセージを判定する
 * 引数  : MSG *msgPtr
 * 戻り値: BOOL
 ***************************************/
BOOL
IsMainWndMessage( MSG *msgPtr )
{
    BOOL bRtn = FALSE;

    if( (mainWndData.hDlgModeless) &&                      /* モードレスダイアログボックス表示中で、 */
        IsDialogMessage(mainWndData.hDlgModeless,msgPtr) ) /* 本メッセージがモードレスダイアログボックスのウィンドウプロシージャで処理された */
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
WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#if 0
    DebugWndPrintf("WndProc,%X,%X,%X\r\n",message,wParam,lParam);
#endif
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
    case WM_CREATE          :rtn = MAINWND_ON_CREATE          ;break;
    case WM_SIZE            :rtn = MAINWND_ON_SIZE            ;break;
    case WM_MOVE            :rtn = MAINWND_ON_MOVE            ;break;
    case WM_WINDOWPOSCHANGED:rtn = MAINWND_ON_WINDOWPOSCHANGED;break;
    case WM_CLOSE           :rtn = MAINWND_ON_CLOSE           ;break;
    case WM_DESTROY         :rtn = MAINWND_ON_DESTROY         ;break;
    case WM_COMMAND         :rtn = MAINWND_ON_COMMAND         ;break;
    case WM_MOUSEWHEEL      :rtn = MAINWND_ON_MOUSEWHEEL      ;break;
    case WM_SETFOCUS        :rtn = MAINWND_ON_SETFOCUS        ;break;
    case WM_KILLFOCUS       :rtn = MAINWND_ON_KILLFOCUS       ;break;
    case WM_DROPFILES       :rtn = MAINWND_ON_DROPFILES       ;break;
    case WM_INITMENUPOPUP   :rtn = MAINWND_ON_INITMENUPOPUP   ;break;
    default                 :
        if( message == mainWndData.messageFindReplace )
        {
            rtn = MAINWND_ON_FINDMSGSTRING;
        }
        else
        {
            rtn = MAINWND_ON_DEFAULT;
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
onCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    HFONT hFont,hOldFont;

    hdc = GetDC( hwnd );
    hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = (HFONT)SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    mainWndData.cxChar = tm.tmAveCharWidth;
    mainWndData.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    mainWndData.messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

    if( ConfigLoadDebugValue() == 0xDDDDDDDD )
    {
        mainWndData.hWndDebug = DebugWndCreate(mainWndData.hInstance,mainWndData.szAppName,TRUE);
        DebugWndPrintf("DEBUG=%08lX\r\n",ConfigLoadDebugValue());
    }
    else
    {
        nop();
    }

    ModalDlgInit(mainWndData.hInstance,mainWndData.szAppName);
    FileInitialize( hwnd ); /* ファイル初期化     */
    FontInit();

#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
    EditWndRegisterClass( mainWndData.hInstance );
    mainWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT("teddedit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL ,
                                         CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
                                         hwnd, (HMENU)0, mainWndData.hInstance, NULL );
#else                   /* [エディットコントロール使用] or  通常  */
    mainWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT ("edit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                         ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                         0, 0, 0, 0,
                                         hwnd, (HMENU)0, mainWndData.hInstance, NULL) ;
#endif                  /*  エディットコントロール使用  or  通常  */
    mainWndData.hFontIo = NULL;

#if 0
    SomeCtrlCreate( mainWndData.hInstance, mainWndData.szAppName, hwnd ); /* コントロールを生成 */
#endif
    StsBarCreate( mainWndData.hInstance, mainWndData.szAppName, hwnd, TRUE ); /* ステータスバー生成、デフォルト表示 */

    MenuCheckItem( IDM_VIEW_STS_BAR );
    MenuCheckItem( IDM_EXTEND_NEWLINE_CRLF );

    if( (szCmdLineLocal[0] != '\0') &&
        (FileSetName(FILE_ID_BIN,szCmdLineLocal,FALSE)) )
    {
        DWORD dwSize;
        PBYTE dataPtr;

        dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        EditWndDataSet( mainWndData.hWndIo,(TCHAR *)dataPtr,dwSize,TRUE );
#endif                  /*  エディットコントロール使用  or  通常  */
        doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
    }
    else
    {
        doCaption( hwnd, "" , FALSE );
    }

    return rtn;
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

    MoveWindow( mainWndData.hWndIo, 0, topSizeSum, mainWndData.cxClient, mainWndData.cyClient - topSizeSum - bottomSizeSum, TRUE) ;

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
onMove( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    mainWndData.xPos = LOWORD( lParam );
    mainWndData.yPos = HIWORD( lParam );

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
onWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    mainWndData.xWindowPos = ((WINDOWPOS *)lParam)->x ;
    mainWndData.yWindowPos = ((WINDOWPOS *)lParam)->y ;
    mainWndData.cxWindow   = ((WINDOWPOS *)lParam)->cx;
    mainWndData.cyWindow   = ((WINDOWPOS *)lParam)->cy;

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
onClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        BOOL bCancel = (BOOL)FALSE;

        ConfigSaveDword( CONFIG_ID_WINDOW_POS_X , mainWndData.xWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_Y , mainWndData.yWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DX, mainWndData.cxWindow   );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DY, mainWndData.cyWindow   );

        if( ConfigIsSame((CONFIG_ID)0,CONFIG_ID_MAX) )
        {
            nop();
        }
        else
        {
            if( ConfigLoadDword(CONFIG_ID_BOOL_SAVE_CONFIRM) )
            {
                int iReturn;

                iReturn = MessageBox( hwnd,TEXT("設定は変更されています。\n\n変更を保存しますか?"),mainWndData.szAppName,MB_YESNOCANCEL|MB_ICONEXCLAMATION );
//            iReturn = MessageBox( hwnd,TEXT("Configuration is changed.\n\nSave Changes?"),mainWndData.szAppName,MB_YESNOCANCEL|MB_ICONEXCLAMATION );

                if( (iReturn == IDYES) )
                {
                    ConfigWrite((CONFIG_ID)0,CONFIG_ID_MAX);
                }
                else if( iReturn == IDCANCEL )
                {
                    bCancel = (BOOL)TRUE;
                }
                else
                {
                    nop();
                }
            }
            else
            {
                ConfigWrite((CONFIG_ID)0,CONFIG_ID_MAX);
            }
        }

        if( !bCancel )
        {
            DestroyWindow( hwnd );
        }
        else
        {
            nop();
        }
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
onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyAcceleratorTable( mainWndData.hAccel );

    DestroyWindow( mainWndData.hWndIo );
    FileEnd();

    DestroyWindow( mainWndData.hWndDebug );

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
onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD dwSize;
    PBYTE dataPtr;
    PTSTR strPtr;
    S_MODAL_DLG_PARAM modalDlgParam;
    static FINDREPLACE fr;
    static TCHAR strFind[80],strRep[80],strMsg[1024];
    HFONT hFontOld;

    if( (HWND)lParam == mainWndData.hWndIo )
    {
        switch( HIWORD(wParam) )
        {
        case EN_UPDATE:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            StsBarSetText( STS_BAR_0  ,"%d バイト、全 %d 行",EditWndGetDataSize(mainWndData.hWndIo, EDITWND_ALL),(DWORD)SendMessage(mainWndData.hWndIo,EM_GETLINECOUNT,0,0) );
            StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",(DWORD)(SendMessage(mainWndData.hWndIo,EM_LINEFROMCHAR,-1,0))+1,EditWndGetCaretXpos(mainWndData.hWndIo)+1);
#else                   /* [エディットコントロール使用] or  通常  */
            StsBarSetText( STS_BAR_0  ,"%d バイト、全 %d 行",GetWindowTextLength(mainWndData.hWndIo),(DWORD)SendMessage(mainWndData.hWndIo,EM_GETLINECOUNT,0,0) );
            StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",(DWORD)(SendMessage(mainWndData.hWndIo,EM_LINEFROMCHAR,-1,0))+1,0);
#endif                  /*  エディットコントロール使用  or  通常  */
            break;
        case EN_CHANGE:
            if( mainWndData.bNeedSave )
            {
                nop();
            }
            else
            {
                doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),TRUE );
                mainWndData.bNeedSave = TRUE;
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
        case IDM_FILE_NEW:
            if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                mainWndData.bNeedSave = FALSE;
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
                EditWndDataInit(mainWndData.hWndIo);
#endif                  /*  エディットコントロール使用  or  通常  */
                doCaption( hwnd, "", FALSE);
            }
            break;
        case IDM_FILE_OPEN:
            if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
                {
                    mainWndData.bNeedSave = FALSE;
                    dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
                    EditWndDataSet( mainWndData.hWndIo, (TCHAR *)dataPtr,dwSize,TRUE );
#endif                  /*  エディットコントロール使用  or  通常  */
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN), FALSE );
                }
                else
                {
                    /* キャンセルされた。又はエラー */
                }
            }
            break;
        case IDM_FILE_SAVE:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            dwSize = EditWndGetDataSize(mainWndData.hWndIo,EDITWND_ALL);
            dataPtr = (PBYTE)malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                EditWndDataGet( mainWndData.hWndIo, (TCHAR *)dataPtr,dwSize,EDITWND_ALL );
                if( (FileWrite( FILE_ID_BIN, (TCHAR *)dataPtr, dwSize )) == FILE_NAME_NOT_SET )
                {
                    if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                    {
                        mainWndData.bNeedSave = FALSE;
                        doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                        FileWrite( FILE_ID_BIN, (TCHAR *)dataPtr, dwSize );
                        rtn = 1;
                    }
                    else
                    { /* キャンセル */
                        nop();
                    }
                }
                else
                {
                    mainWndData.bNeedSave = FALSE;
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                    FileWrite( FILE_ID_BIN, (TCHAR *)dataPtr, dwSize );
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
        case IDM_FILE_SAVE_AS:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            dwSize = EditWndGetDataSize(mainWndData.hWndIo,EDITWND_ALL);
            dataPtr = (PBYTE)malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                EditWndDataGet( mainWndData.hWndIo, (TCHAR *)dataPtr,dwSize,EDITWND_ALL );
                if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                {
                    mainWndData.bNeedSave = FALSE;
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                    FileWrite( FILE_ID_BIN, (TCHAR *)dataPtr, dwSize );
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

        case IDM_EDIT_UNDO:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            SendMessage( mainWndData.hWndIo, WM_UNDO, 0, 0 );
#else                   /* [エディットコントロール使用] or  通常  */
            SendMessage( mainWndData.hWndIo, EM_UNDO, 0, 0 );
#endif                  /*  エディットコントロール使用  or  通常  */
            break;

        case IDM_EDIT_REDO:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            SendMessage( mainWndData.hWndIo, WM_REDO, 0, 0 );
#else                   /* [エディットコントロール使用] or  通常  */
#endif                  /*  エディットコントロール使用  or  通常  */
            break;
        case IDM_EDIT_CUT:
            SendMessage( mainWndData.hWndIo, WM_CUT, 0, 0 );
            break;

        case IDM_EDIT_COPY:
            SendMessage( mainWndData.hWndIo, WM_COPY, 0, 0 );
            break;

        case IDM_EDIT_PASTE:
            SendMessage( mainWndData.hWndIo, WM_PASTE, 0, 0 );
            break;

        case IDM_EDIT_DELETE:
            SendMessage( mainWndData.hWndIo, WM_CLEAR, 0, 0 );
            break;

        case IDM_EDIT_FIND:
            fr.lStructSize      = sizeof (FINDREPLACE);
            fr.hwndOwner        = hwnd;
            fr.Flags            = (mainWndData.bFrUp ? 0 : FR_DOWN)|(mainWndData.bFrMatchCase ? FR_MATCHCASE : 0)|FR_HIDEWHOLEWORD/*|FR_HIDEMATCHCASE*/;
            fr.lpstrFindWhat    = strFind;
            fr.lpstrReplaceWith = strRep;
            fr.wReplaceWithLen  = fr.wFindWhatLen = 80;
            mainWndData.hDlgModeless = FindText(&fr);
            break;

        case IDM_EDIT_REPLACE:
            fr.lStructSize      = sizeof(FINDREPLACE);
            fr.hwndOwner        = hwnd;
            fr.Flags            = (mainWndData.bFrUp ? 0 : FR_DOWN)|(mainWndData.bFrMatchCase ? FR_MATCHCASE : 0)|FR_HIDEWHOLEWORD/*|FR_HIDEMATCHCASE*/;
            fr.lpstrFindWhat    = strFind;
            fr.lpstrReplaceWith = strRep;
            fr.wReplaceWithLen  = fr.wFindWhatLen = 80;
            mainWndData.hDlgModeless = ReplaceText(&fr);
            break;

        case IDM_EDIT_FIND_NEXT:
            if( mainWndData.searchDataSize && mainWndData.searchDataPtr )
            {
                if( EditWndFindDataSet(mainWndData.hWndIo,mainWndData.searchDataPtr,mainWndData.searchDataSize,mainWndData.bFrUp,mainWndData.bFrMatchCase) )
                {
                    nop();
                }
                else
                {
                    okMessage(mainWndData.hDlgModeless, TEXT("\"%s\" が見つかりません。"),mainWndData.searchDataPtr);
                }
            }
            else
            {
                PostMessage( hwnd,WM_COMMAND,IDM_EDIT_FIND,0);
            }
            break;

        case IDM_EDIT_GOTO_LINE:
            modalDlgParam.dwData1 = (DWORD)(SendMessage(mainWndData.hWndIo,EM_LINEFROMCHAR,-1,0))+1;
            modalDlgParam.dwData2 = (DWORD)SendMessage(mainWndData.hWndIo,EM_GETLINECOUNT,0,0);
            ModalDlg( MODAL_DLG_ID_GO_TO_LINE, &modalDlgParam, hwnd, mainWndData.xPos, mainWndData.yPos );
            SendMessage( mainWndData.hWndIo, WM_GOTOLINE, 0, modalDlgParam.dwData1-1 );
            break;

        case IDM_EDIT_SELECT_ALL:
            SendMessage( mainWndData.hWndIo, EM_SETSEL, 0, -1 );
            break;

        case IDM_EDIT_DATETIME:
            strPtr = DateTimeGetString();
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            EditWndDataSet( mainWndData.hWndIo, strPtr, strlen(strPtr), FALSE );
#endif                  /*  エディットコントロール使用  or  通常  */
            break;

        case IDM_EDIT_GOTO_FILE_TOP:
            SendMessage( mainWndData.hWndIo, WM_GOTOLINE, 0, 0 );
            break;

        case IDM_EDIT_GOTO_FILE_END:
            SendMessage( mainWndData.hWndIo, WM_GOTOLINE, 0, SendMessage(mainWndData.hWndIo,EM_GETLINECOUNT,0,0)-1 );
            break;

        case IDM_FORMAT_FONT:
            if( FontChooseFont( hwnd, FONT_ID_IO ) )
            {
                hFontOld = mainWndData.hFontIo;
                mainWndData.hFontIo = CreateFontIndirect( FontGetLogFont(FONT_ID_IO) );
                SendMessage( mainWndData.hWndIo, WM_SETFONT, (WPARAM)mainWndData.hFontIo, (LPARAM)TRUE );

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

        case IDM_VIEW_STS_BAR:
            if( MenuInqItemChecked(IDM_VIEW_STS_BAR) )
            {
                MenuUnCheckItem( IDM_VIEW_STS_BAR );
                StsBarShowWindow( FALSE );
            }
            else
            {
                MenuCheckItem( IDM_VIEW_STS_BAR );
                StsBarShowWindow( TRUE );
            }
            SendMessage(hwnd,WM_SIZE,0,MAKELONG(mainWndData.cxClient,mainWndData.cyClient));
            break;

#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        case IDM_EXTEND_NEWLINE_CRLF:
            EditWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_CRLF);
            MenuCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuUnCheckItem( IDM_EXTEND_NEWLINE_LF   );
            MenuUnCheckItem( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR+LF");
            break;

        case IDM_EXTEND_NEWLINE_LF  :
            EditWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_LF);
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuCheckItem    ( IDM_EXTEND_NEWLINE_LF   );
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"LF");
            break;

        case IDM_EXTEND_NEWLINE_CR  :
            EditWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_CR);
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_LF   );
            MenuCheckItem    ( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR");
            break;
#endif                  /*  エディットコントロール使用  or  通常  */

        case IDM_FILE_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;

        case IDM_HELP_ABOUT:
            ModalDlg( MODAL_DLG_ID_ABOUT, &modalDlgParam, hwnd, mainWndData.xPos, mainWndData.yPos );
            break;

        default:
            break;
        }
    }

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
 * 内容  : WM_MOUSEHWHEEL を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
onMouseHwheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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

    if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        mainWndData.bNeedSave = FALSE;
        DragQueryFile( (HDROP)wParam, (UINT)0, (LPSTR)szFileName, (UINT)sizeof(szFileName) );

        FileSetName( FILE_ID_BIN, szFileName, FALSE );
        dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        EditWndDataSet( mainWndData.hWndIo,(TCHAR *)dataPtr,dwSize,TRUE );
#endif                  /*  エディットコントロール使用  or  通常  */
        doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
onInitMenuPopup( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    if( LOWORD(lParam) == 1 )
    { /* 「編集」 */
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
        if( EditWndGetDataSize(mainWndData.hWndIo,EDITWND_SELECTED) )
        {
            MenuEnableItem( IDM_EDIT_CUT    );
            MenuEnableItem( IDM_EDIT_COPY   );
            MenuEnableItem( IDM_EDIT_DELETE );
        }
        else
        {
            MenuUnEnableItem( IDM_EDIT_CUT    );
            MenuUnEnableItem( IDM_EDIT_COPY   );
            MenuUnEnableItem( IDM_EDIT_DELETE );
        }
#endif                  /*  エディットコントロール使用  or  通常  */

        if( IsClipboardFormatAvailable(CF_TEXT) )
        {
            MenuEnableItem( IDM_EDIT_PASTE );
        }
        else
        {
            MenuUnEnableItem( IDM_EDIT_PASTE );
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
onFindMsgString( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    LPFINDREPLACE pfr;

    pfr = (LPFINDREPLACE)lParam;

    if( pfr->Flags & FR_DOWN )
    {
        mainWndData.bFrUp = FALSE;
    }
    else
    {
        mainWndData.bFrUp = TRUE;
    }

    if( pfr->Flags & FR_MATCHCASE )
    {
        mainWndData.bFrMatchCase = TRUE;
    }
    else
    {
        mainWndData.bFrMatchCase = FALSE;
    }

    if( pfr->Flags & FR_DIALOGTERM )
    {
        mainWndData.hDlgModeless = NULL;
    }
    else
    {
        nop();
    }

#if 0
    DebugWndPrintf("FindReplace: 0x%08lX\r\n",pfr->Flags);
    DebugWndPrintf("FindReplace: %s(%d),%s(%d)\r\n",pfr->lpstrFindWhat,pfr->wFindWhatLen,pfr->lpstrReplaceWith,pfr->wReplaceWithLen);
#endif

    mainWndData.searchDataPtr   = pfr->lpstrFindWhat;
    mainWndData.searchDataSize  = min(strlen(mainWndData.searchDataPtr),pfr->wFindWhatLen);
    mainWndData.replaceDataPtr  = pfr->lpstrReplaceWith;
    mainWndData.replaceDataSize = min(strlen(mainWndData.replaceDataPtr),pfr->wReplaceWithLen);

#if 0
    DebugWndPrintf("FindReplace: %s(%d),%s(%d)\r\n",mainWndData.searchDataPtr,mainWndData.searchDataSize,mainWndData.replaceDataPtr,mainWndData.replaceDataSize);
#endif

    if( (pfr->Flags & FR_FINDNEXT) )
    { /* 検索 */
        if( EditWndFindDataSet(mainWndData.hWndIo,mainWndData.searchDataPtr,mainWndData.searchDataSize,mainWndData.bFrUp,mainWndData.bFrMatchCase) )
        {
            nop();
        }
        else
        {
            okMessage(mainWndData.hDlgModeless, TEXT("\"%s\" が見つかりません。"),mainWndData.searchDataPtr);
        }
    }
    else if( pfr->Flags & FR_REPLACE )
    { /* 置換 */
        BOOL bProcFind = (BOOL)FALSE;

        bProcFind = EditWndReplaceData(mainWndData.hWndIo,mainWndData.searchDataPtr,mainWndData.replaceDataPtr,mainWndData.replaceDataSize,mainWndData.bFrUp,mainWndData.bFrMatchCase);

        if( bProcFind )
        { /* 検索 */
            if( EditWndFindDataSet(mainWndData.hWndIo,mainWndData.searchDataPtr,mainWndData.searchDataSize,mainWndData.bFrUp,mainWndData.bFrMatchCase) )
            {
                nop();
            }
            else
            {
                okMessage(mainWndData.hDlgModeless, TEXT("\"%s\" が見つかりません。"),mainWndData.searchDataPtr);
            }
        }
        else
        {
            nop();
        }
    }
    else if( pfr->Flags & FR_REPLACEALL )
    { /* 全て置換 */
        BOOL bProcFind = (BOOL)FALSE;

        bProcFind = EditWndReplaceData(mainWndData.hWndIo,mainWndData.searchDataPtr,mainWndData.replaceDataPtr,mainWndData.replaceDataSize,mainWndData.bFrUp,mainWndData.bFrMatchCase);

        while( bProcFind )
        {
            if( EditWndFindDataSet(mainWndData.hWndIo,mainWndData.searchDataPtr,mainWndData.searchDataSize,mainWndData.bFrUp,mainWndData.bFrMatchCase) )
            {
                bProcFind = EditWndReplaceData(mainWndData.hWndIo,mainWndData.searchDataPtr,mainWndData.replaceDataPtr,mainWndData.replaceDataSize,mainWndData.bFrUp,mainWndData.bFrMatchCase);
            }
            else
            {
                bProcFind = FALSE;
            }
        }
    }
    else
    {
        nop();
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
 * 引数  : TCHAR *szMessageFormat, ...
 * 戻り値: int
 ***************************************/
int
okMessage( HWND hwnd, TCHAR *szMessageFormat, ... )
{
    va_list vaArgs;
    TCHAR szBuffer[64 + MAX_PATH] ;
    BOOL bResult = (BOOL)FALSE;

    va_start(vaArgs, szMessageFormat);
    if( wvsprintf(szBuffer, szMessageFormat, vaArgs) != EOF )
    {
        bResult = (BOOL)TRUE;
    }
    else
    {
        /* do nothing */
    }
    va_end(vaArgs);

    if( bResult )
    {
        /* do nothing */
    }
    else
    {
        szBuffer[0] = '\0';
    }

    return MessageBox( hwnd, szBuffer, mainWndData.szAppName, MB_OK | MB_ICONINFORMATION );
}

/********************************************************************************
 * 内容  :
 * 引数  : HWND hwnd
 * 引数  : TCHAR* szTitleName
 * 引数  : BOOL bNeedSave
 * 戻り値: なし
 ***************************************/
void
doCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave )
{
     TCHAR szCaption[64 + MAX_PATH];

     if( bNeedSave )
     {
         wsprintf( szCaption, TEXT ("*%s - %s"), (szTitleName[0] ? szTitleName : TEXT("無題")),mainWndData.szAppName );
     }
     else
     {
         wsprintf( szCaption, TEXT ("%s - %s"), (szTitleName[0] ? szTitleName : TEXT("無題")),mainWndData.szAppName );
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
AskAboutSave( HWND hwnd, TCHAR * szTitleName )
{
    TCHAR szBuffer[64 + MAX_PATH];
    int   iReturn;

    wsprintf(szBuffer, TEXT("ファイル %s の内容は変更されています。\n\n変更を保存しますか?"), szTitleName[0] ? szTitleName : TEXT("無題") );

    iReturn = MessageBox( hwnd,szBuffer,mainWndData.szAppName,MB_YESNOCANCEL|MB_ICONEXCLAMATION );

    if( iReturn == IDYES )
    {
        if( !SendMessage( hwnd,WM_COMMAND,IDM_FILE_SAVE,0) )
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
