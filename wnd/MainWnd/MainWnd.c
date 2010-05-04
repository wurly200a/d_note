/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "MainWndDef.h"
#include "MenuId.h"
#include "MainWndMenu.h"

/* 外部関数定義 */
#include "WinMain.h"
#include "IoWnd.h"
#include "SomeCtrl.h"
#include "File.h"
#include "StsBar.h"
#include "Font.h"
#include "Config.h"
#include "DateTime.h"
#include "ModalDlg.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static MAINWND_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onPaint           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMove            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyUp           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyDown         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onChar            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onHscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onVscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMouseWheel      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSetFocus        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKillFocus       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDropFiles       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onInitMenuPopup   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onFindMsgString   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

void doCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave );
short AskAboutSave( HWND hwnd, TCHAR * szTitleName );

/* 内部変数定義 */
static HWND hwndMain; /* メインウィンドウのハンドラ */
static S_MAINWND_DATA mainWndData;

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[MAINWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate          , /* WM_CREATE                     */
    onPaint           , /* WM_PAINT                      */
    onSize            , /* WM_SIZE                       */
    onMove            , /* WM_MOVE                       */
    onWindowPosChanged, /* WM_WINDOWPOSCHANGED           */
    onClose           , /* WM_CLOSE                      */
    onDestroy         , /* WM_DESTROY                    */
    onCommand         , /* WM_COMMAND                    */
    onKeyUp           , /* WM_KEYUP                      */
    onKeyDown         , /* WM_KEYDOWN                    */
    onChar            , /* WM_CHAR                       */
    onHscroll         , /* WM_HSCROLL                    */
    onVscroll         , /* WM_VSCROLL                    */
    onMouseWheel      , /* WM_MOUSEWHEEL                 */
    onSetFocus        , /* WM_SETFOCUS                   */
    onKillFocus       , /* WM_KILLFOCUS                  */
    onDropFiles       , /* WM_DROPFILES                  */
    onInitMenuPopup   , /* WM_INITMENUPOPUP              */
    onFindMsgString   , /* FINDMSGSTRINGの登録メッセージ */
    onApp             , /* WM_APP                        */
    onDefault           /* default                       */
};
/* *INDENT-ON* */

#if 0
#define USE_EDITCONTROL
#endif

/********************************************************************************
 * 内容  : メインウィンドウクラスの登録、ウィンドウの生成
 * 引数  : int nCmdShow
 * 引数  : HACCEL *hAccelPtr
 * 戻り値: HWND
 ***************************************/
HWND
MainWndCreate( int nCmdShow, HACCEL *hAccelPtr )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();
    HMENU hMenu = NULL;

    hwndMain = NULL;

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
    { /* ウィンドウクラスの登録失敗 */
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
    }
    else
    {
        /* 設定の初期化 */
        ConfigInit();

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
    case WM_PAINT           :rtn = MAINWND_ON_PAINT           ;break;
    case WM_SIZE            :rtn = MAINWND_ON_SIZE            ;break;
    case WM_MOVE            :rtn = MAINWND_ON_MOVE            ;break;
    case WM_WINDOWPOSCHANGED:rtn = MAINWND_ON_WINDOWPOSCHANGED;break;
    case WM_CLOSE           :rtn = MAINWND_ON_CLOSE           ;break;
    case WM_DESTROY         :rtn = MAINWND_ON_DESTROY         ;break;
    case WM_COMMAND         :rtn = MAINWND_ON_COMMAND         ;break;
    case WM_APP             :rtn = MAINWND_ON_APP             ;break;
    case WM_KEYUP           :rtn = MAINWND_ON_KEYUP           ;break;
    case WM_KEYDOWN         :rtn = MAINWND_ON_KEYDOWN         ;break;
    case WM_CHAR            :rtn = MAINWND_ON_CHAR            ;break;
    case WM_HSCROLL         :rtn = MAINWND_ON_HSCROLL         ;break;
    case WM_VSCROLL         :rtn = MAINWND_ON_VSCROLL         ;break;
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
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    mainWndData.cxChar = tm.tmAveCharWidth;
    mainWndData.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    mainWndData.messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

    ModalDlgInit();
    FileInitialize( hwnd ); /* ファイル初期化     */
    FontInit();

#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
    mainWndData.hWndIo = IoWndCreate( GetHinst(), hwnd );
#else                   /* [エディットコントロール使用] or  通常  */
    mainWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT ("edit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                         WS_BORDER | ES_LEFT | ES_MULTILINE |
                                         ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                         0, 0, 0, 0,
                                         hwnd, (HMENU)0, GetHinst(), NULL) ;
#endif                  /*  エディットコントロール使用  or  通常  */

#if 0
    SomeCtrlCreate( hwnd ); /* コントロールを生成 */
#endif
    StsBarCreate( hwnd, TRUE ); /* ステータスバー生成、デフォルト表示 */

    MenuCheckItem( IDM_VIEW_STS_BAR );
    MenuCheckItem( IDM_EXTEND_NEWLINE_CRLF );

    doCaption( hwnd, "" , FALSE );

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
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_X , mainWndData.xWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_Y , mainWndData.yWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DX, mainWndData.cxWindow   );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DY, mainWndData.cyWindow   );

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
onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyAcceleratorTable( mainWndData.hAccel );

    DestroyWindow( mainWndData.hWndIo );
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
onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD dwSize;
    PBYTE dataPtr;
    PTSTR strPtr;
    S_MODAL_DLG_DATA modalDlgData;
    static FINDREPLACE fr;
    static TCHAR strFind[80],strRep[80],strMsg[1024];

    if( (HWND)lParam == mainWndData.hWndIo )
    {
        switch( HIWORD(wParam) )
        {
        case EN_UPDATE:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            StsBarSetText( STS_BAR_0  ,"%d バイト、全 %d 行",IoWndGetDataSize(mainWndData.hWndIo, IOWND_ALL),IoWndGetLineMaxSize(mainWndData.hWndIo) );
            StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",IoWndGetCaretYpos(mainWndData.hWndIo)+1,IoWndGetCaretXpos(mainWndData.hWndIo)+1);
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
                IoWndDataInit(mainWndData.hWndIo);
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
                    IoWndDataSet( mainWndData.hWndIo, dataPtr,dwSize,TRUE );
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
            dwSize = IoWndGetDataSize(mainWndData.hWndIo,IOWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                IoWndDataGet( mainWndData.hWndIo, dataPtr,dwSize,IOWND_ALL );
                if( (FileWrite( FILE_ID_BIN, dataPtr, dwSize )) == FILE_NAME_NOT_SET )
                {
                    if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                    {
                        mainWndData.bNeedSave = FALSE;
                        doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
                    mainWndData.bNeedSave = FALSE;
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
        case IDM_FILE_SAVE_AS:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            dwSize = IoWndGetDataSize(mainWndData.hWndIo,IOWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                IoWndDataGet( mainWndData.hWndIo, dataPtr,dwSize,IOWND_ALL );
                if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                {
                    mainWndData.bNeedSave = FALSE;
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
            fr.lStructSize   = sizeof (FINDREPLACE);
            fr.hwndOwner     = hwnd;
            fr.Flags         = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat = strFind;
            fr.wFindWhatLen  = 80;
            mainWndData.hDlgModeless = FindText(&fr);
            break;

        case IDM_EDIT_REPLACE:
            fr.lStructSize      = sizeof(FINDREPLACE);
            fr.hwndOwner        = hwnd;
            fr.Flags            = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat    = strFind;
            fr.lpstrReplaceWith = strRep;
            fr.wReplaceWithLen  = fr.wFindWhatLen = 80;
            mainWndData.hDlgModeless = ReplaceText(&fr);
            break;

        case IDM_EDIT_FIND_NEXT:
            break;

        case IDM_EDIT_SELECT_ALL:
            SendMessage( mainWndData.hWndIo, EM_SETSEL, 0, -1 );
            break;

        case IDM_EDIT_DATETIME:
            strPtr = DateTimeGetString();
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            IoWndDataSet( mainWndData.hWndIo, strPtr, strlen(strPtr), FALSE );
#endif                  /*  エディットコントロール使用  or  通常  */
            break;

        case IDM_FORMAT_FONT:
#ifndef USE_EDITCONTROL /*  エディットコントロール使用  or [通常] */
            if( FontChooseFont( hwnd, FONT_ID_IO ) )
            {
                mainWndData.hFontIo = CreateFontIndirect( FontGetLogFont(FONT_ID_IO) );
                IoWndChangeFont( mainWndData.hWndIo, mainWndData.hFontIo );
            }
            else
            {
                nop();
            }
#endif                  /*  エディットコントロール使用  or  通常  */
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
            IoWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_CRLF);
            MenuCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuUnCheckItem( IDM_EXTEND_NEWLINE_LF   );
            MenuUnCheckItem( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR+LF");
            break;

        case IDM_EXTEND_NEWLINE_LF  :
            IoWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_LF);
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuCheckItem    ( IDM_EXTEND_NEWLINE_LF   );
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"LF");
            break;

        case IDM_EXTEND_NEWLINE_CR  :
            IoWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_CR);
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
            ModalDlg( MODAL_DLG_ID_ABOUT, &modalDlgData, hwnd, mainWndData.xPos, mainWndData.yPos );
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
        IoWndDataSet( mainWndData.hWndIo,dataPtr,dwSize,TRUE );
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
        if( IoWndGetDataSize(mainWndData.hWndIo,IOWND_SELECTED) )
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

    if( pfr->Flags & FR_DIALOGTERM )
    {
        mainWndData.hDlgModeless = NULL;
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
 * 引数  : BOOL bNeedSave
 * 戻り値: なし
 ***************************************/
void
doCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave )
{
     TCHAR szCaption[64 + MAX_PATH];

     if( bNeedSave )
     {
         wsprintf( szCaption, TEXT ("*%s - %s"), (szTitleName[0] ? szTitleName : TEXT("無題")),GetAppName() );
     }
     else
     {
         wsprintf( szCaption, TEXT ("%s - %s"), (szTitleName[0] ? szTitleName : TEXT("無題")),GetAppName() );
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

    iReturn = MessageBox( hwnd,szBuffer,GetAppName(),MB_YESNOCANCEL|MB_ICONEXCLAMATION );

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
