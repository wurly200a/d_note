/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "MenuId.h"
#include "IoWndDef.h"

/* 外部関数定義 */
#include "StsBar.h"
#include "IoWndBuffer.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWnd.h"
LRESULT CALLBACK IoWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static IOWND_INDEX ioWndConvertMSGtoINDEX( UINT message );
static LRESULT ioOnCreate             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnPaint              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSize               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnClose              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDestroy            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCommand            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyUp              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyDown            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnChar               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnHscroll            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnVscroll            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSetFocus           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKillFocus          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseActivate      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseWheel         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseMove          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnLbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnRbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnLbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnRbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnImeStartComposition( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCut                ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCopy               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnPaste              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnClear              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnUndo               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSetSel             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDefault            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static void ioWndRemoveData( HWND hwnd, BOOL bBackSpace );
static void updateTextMetrics( HWND hwnd );
static void setAllScrollInfo( HWND hwnd );
static void getAllScrollInfo( HWND hwnd );
static void setScrollPos( HWND hwnd, int fnBar, DWORD nPos );
static void printCaretPos( HWND hwnd );

/* 内部変数定義 */
typedef struct
{
    int    cxChar;    /* 文字サイズ(X)             */
    int    cyChar;    /* 文字サイズ(Y)             */
    int    cxClient;  /* クライアント領域サイズ(X) */
    int    cyClient;  /* クライアント領域サイズ(Y) */
    int    cxBuffer;  /* 表示可能な文字数(X)       */
    int    cyBuffer;  /* 表示可能な文字数(Y)       */
    DWORD  dwCharSet;
    int    cxCaps;    /*  */
    int    iAccumDelta;
    int    iDeltaPerLine;
    int    iHorzPos;     /* スクロールバーの横位置  */
    int    iVertPos;     /* スクロールバーの縦位置  */
    LOGFONT logFont;
    BOOL   bShiftKeyOn;
    H_IOWND_BUFF hIoWndBuff;
} S_IOWND_DATA;

/* *INDENT-OFF* */
static LRESULT (*ioWndProcTbl[IOWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    ioOnCreate             , /* WM_CREATE              */
    ioOnPaint              , /* WM_PAINT               */
    ioOnSize               , /* WM_SIZE                */
    ioOnClose              , /* WM_CLOSE               */
    ioOnDestroy            , /* WM_DESTROY             */
    ioOnCommand            , /* WM_COMMAND             */
    ioOnKeyUp              , /* WM_KEYUP               */
    ioOnKeyDown            , /* WM_KEYDOWN             */
    ioOnChar               , /* WM_CHAR                */
    ioOnHscroll            , /* WM_HSCROLL             */
    ioOnVscroll            , /* WM_VSCROLL             */
    ioOnSetFocus           , /* WM_SETFOCUS            */
    ioOnKillFocus          , /* WM_KILLFOCUS           */
    ioOnMouseActivate      , /* WM_MOUSEACTIVATE       */
    ioOnMouseWheel         , /* WM_MOUSEWHEEL          */
    ioOnMouseMove          , /* WM_MOUSEMOVE           */
    ioOnLbuttonDown        , /* WM_LBUTTONDOWN         */
    ioOnMbuttonDown        , /* WM_MBUTTONDOWN         */
    ioOnRbuttonDown        , /* WM_RBUTTONDOWN         */
    ioOnLbuttonUp          , /* WM_LBUTTONUP           */
    ioOnMbuttonUp          , /* WM_MBUTTONUP           */
    ioOnRbuttonUp          , /* WM_RBUTTONUP           */
    ioOnImeStartComposition, /* WM_IME_STARTCOMPOSITION*/
    ioOnCut                , /* WM_CUT                 */
    ioOnCopy               , /* WM_COPY                */
    ioOnPaste              , /* WM_PASTE               */
    ioOnClear              , /* WM_CLEAR               */
    ioOnUndo               , /* WM_UNDO                */
    ioOnSetSel             , /* EM_SETSEL              */
    ioOnDefault              /* default                */
};
/* *INDENT-ON* */

WORD DebugIoWndRect;

/********************************************************************************
 * 内容  : IOウィンドウクラスの登録、ウィンドウの生成
 * 引数  : HINSTANCE hInst
 * 引数  : HWND hWndParent 親ウィンドウのハンドル
 * 戻り値: HWND
 ***************************************/
HWND
IoWndCreate( HINSTANCE hInst, HWND hWndParent )
{
    WNDCLASS wc = {0};
    HWND hwnd;

    wc.lpfnWndProc      = (WNDPROC) IoWndProc;
    wc.hInstance        = hInst;
    wc.hIcon            = (HICON)0;
    wc.hCursor          = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground    = (HBRUSH) CreateSolidBrush( BG_COLOR_RGB );
    wc.lpszClassName    = "ioWndClass";
    wc.lpszMenuName     = NULL;
    wc.cbWndExtra       = sizeof(S_IOWND_DATA);

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("RegisterClass Failed!"), TEXT("IoWnd"), MB_ICONERROR );
        return (HWND)NULL;
    }

    hwnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW /* | WS_EX_ACCEPTFILES*/,
                             "ioWndClass", "IO Window",
                             WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             0, 0,
                             hWndParent, 0, hInst, NULL );

    if( hwnd != NULL )
    {
        SetFocus(hwnd);
    }

    return hwnd;
}

/********************************************************************************
 * 内容  : IOウィンドウのフォント変更
 * 引数  : HWND hwnd
 * 引数  : LOGFONT *logFontPtr
 * 戻り値: なし
 ***************************************/
void
IoWndChangeFont( HWND hwnd, LOGFONT *logFontPtr )
{
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    memcpy( &(ioWndDataPtr->logFont),logFontPtr, sizeof(LOGFONT));

    updateTextMetrics( hwnd );
    SendMessage(hwnd,(UINT)WM_SIZE,(WPARAM)0,MAKELPARAM(ioWndDataPtr->cyClient,ioWndDataPtr->cxClient));
    InvalidateRect( hwnd, NULL, TRUE );
}

/********************************************************************************
 * 内容  : IOウィンドウのサイズ変更
 * 引数  : HWND hwnd
 * 引数  : int x
 * 引数  : int y
 * 引数  : int cxClient
 * 引数  : int cyClient
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndSize( HWND hwnd, int x, int y, int cxClient, int cyClient )
{
    MoveWindow( hwnd,x,y,cxClient,cyClient, TRUE);

    return TRUE;
}

/********************************************************************************
 * 内容  : IOウィンドウの破壊
 * 引数  : HWND hwnd
 * 引数  : int nCmdShow
 * 戻り値: なし
 ***************************************/
void
IoWndDestroy( HWND hwnd )
{
    DestroyWindow( hwnd );
}

/********************************************************************************
 * 内容  : IOウィンドウの初期化
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
void
IoWndDataInit( HWND hwnd )
{
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    IoWndBuffInit(ioWndDataPtr->hIoWndBuff);

    setAllScrollInfo(hwnd);
    InvalidateRect( hwnd, NULL, TRUE );
}

/********************************************************************************
 * 内容  : IOウィンドウのデータセット
 * 引数  : HWND hwnd
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void
IoWndDataSet( HWND hwnd, TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    IoWndBuffDataSet( ioWndDataPtr->hIoWndBuff, dataPtr, length, bInit );

    if( bInit )
    {
        nop();
    }
    else
    {
        SendMessage(GetParent(hwnd), (UINT)WM_COMMAND, MAKEWPARAM(0,EN_UPDATE), (LPARAM)hwnd);
    }

    IoWndBuffSelectOff(ioWndDataPtr->hIoWndBuff);
    setAllScrollInfo(hwnd);
    InvalidateRect( hwnd, NULL, TRUE );
}

/********************************************************************************
 * 内容  : IOウィンドウのデータ取得
 * 引数  : HWND hwnd
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : IOWND_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndDataGet( HWND hwnd, TCHAR *dataPtr, DWORD dataSize, IOWND_REGION region )
{
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    return IoWndBuffDataGet( ioWndDataPtr->hIoWndBuff, dataPtr, dataSize, region );
}

/********************************************************************************
 * 内容  : IOウィンドウのデータサイズ取得
 * 引数  : HWND hwnd
 * 引数  : IOWND_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetDataSize( HWND hwnd, IOWND_REGION region )
{
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    return IoWndGetBuffSize( ioWndDataPtr->hIoWndBuff, region );
}

/********************************************************************************
 * 内容  : IOウィンドウの改行コードセット
 * 引数  : HWND hwnd
 * 引数  : NEWLINECODE_TYPE newLineCodeType
 * 戻り値: BOOL (TRUE:表示書き換え必要)
 ***************************************/
BOOL
IoWndNewLineCodeSet( HWND hwnd, NEWLINECODE_TYPE newLineCodeType )
{
    DWORD allDataSize;
    TCHAR *dataTopPtr;
    BOOL bRtn = FALSE;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    IoWndBuffSetNewLineCode( ioWndDataPtr->hIoWndBuff, newLineCodeType );

    allDataSize = IoWndGetBuffSize(ioWndDataPtr->hIoWndBuff, BUFF_ALL);
    dataTopPtr  = malloc( sizeof(TCHAR) * allDataSize );
    if( dataTopPtr != NULL )
    {
        IoWndBuffDataGet( ioWndDataPtr->hIoWndBuff, dataTopPtr, allDataSize, BUFF_ALL );
        IoWndBuffDataSet( ioWndDataPtr->hIoWndBuff, dataTopPtr, allDataSize, TRUE );
        setAllScrollInfo(hwnd);
        InvalidateRect( hwnd, NULL, TRUE );

        free( dataTopPtr );
        bRtn = TRUE;
    }
    else
    {
        nop();
    }

    return bRtn;
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
    case WM_CREATE              :rtn = IOWND_ON_CREATE              ;break;
    case WM_PAINT               :rtn = IOWND_ON_PAINT               ;break;
    case WM_SIZE                :rtn = IOWND_ON_SIZE                ;break;
    case WM_CLOSE               :rtn = IOWND_ON_CLOSE               ;break;
    case WM_DESTROY             :rtn = IOWND_ON_DESTROY             ;break;
    case WM_COMMAND             :rtn = IOWND_ON_COMMAND             ;break;
    case WM_KEYUP               :rtn = IOWND_ON_KEYUP               ;break;
    case WM_KEYDOWN             :rtn = IOWND_ON_KEYDOWN             ;break;
    case WM_CHAR                :rtn = IOWND_ON_CHAR                ;break;
    case WM_HSCROLL             :rtn = IOWND_ON_HSCROLL             ;break;
    case WM_VSCROLL             :rtn = IOWND_ON_VSCROLL             ;break;
    case WM_SETFOCUS            :rtn = IOWND_ON_SETFOCUS            ;break;
    case WM_KILLFOCUS           :rtn = IOWND_ON_KILLFOCUS           ;break;
    case WM_MOUSEACTIVATE       :rtn = IOWND_ON_MOUSEACTIVATE       ;break;
    case WM_MOUSEWHEEL          :rtn = IOWND_ON_MOUSEWHEEL          ;break;
    case WM_MOUSEMOVE           :rtn = IOWND_ON_MOUSEMOVE           ;break;
    case WM_LBUTTONDOWN         :rtn = IOWND_ON_LBUTTONDOWN         ;break;
    case WM_MBUTTONDOWN         :rtn = IOWND_ON_MBUTTONDOWN         ;break;
    case WM_RBUTTONDOWN         :rtn = IOWND_ON_RBUTTONDOWN         ;break;
    case WM_LBUTTONUP           :rtn = IOWND_ON_LBUTTONUP           ;break;
    case WM_MBUTTONUP           :rtn = IOWND_ON_MBUTTONUP           ;break;
    case WM_RBUTTONUP           :rtn = IOWND_ON_RBUTTONUP           ;break;
    case WM_IME_STARTCOMPOSITION:rtn = IOWND_ON_IME_STARTCOMPOSITION;break;
    case WM_CUT                 :rtn = IOWND_ON_CUT                 ;break;
    case WM_COPY                :rtn = IOWND_ON_COPY                ;break;
    case WM_PASTE               :rtn = IOWND_ON_PASTE               ;break;
    case WM_CLEAR               :rtn = IOWND_ON_CLEAR               ;break;
    case WM_UNDO                :rtn = IOWND_ON_UNDO                ;break;
    case EM_SETSEL              :rtn = IOWND_ON_SETSEL              ;break;

    case WM_NCMOUSEMOVE         :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_SETCURSOR           :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_NCPAINT             :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_NCHITTEST           :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_ERASEBKGND          :rtn = IOWND_ON_DEFAULT             ;break;

    case WM_IME_COMPOSITION     :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_IME_NOTIFY          :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_IME_SETCONTEXT      :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_IME_COMPOSITIONFULL :rtn = IOWND_ON_DEFAULT             ;break;

    default                     :rtn = IOWND_ON_DEFAULT             ;break;
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
ioOnCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    int iDelta;          /* for MouseWheel */
    ULONG ulScrollLines; /* for MouseWheel */
    S_IOWND_DATA *ioWndDataPtr;

    ioWndDataPtr = (S_IOWND_DATA *)malloc( sizeof(S_IOWND_DATA) );
    memset( ioWndDataPtr, 0, sizeof(S_IOWND_DATA) );
    GetObject( GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), (PTSTR)&(ioWndDataPtr->logFont) );
    ioWndDataPtr->hIoWndBuff = IoWndBuffCreate();
    SetWindowLongPtr(hwnd,0,(LONG_PTR)ioWndDataPtr);

    updateTextMetrics( hwnd );

    IoWndBuffInit(ioWndDataPtr->hIoWndBuff);

    /* for MouseWheel */
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0, &ulScrollLines, 0);
    iDelta = HIWORD(wParam);
    if( ulScrollLines )
    {
        ioWndDataPtr->iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
    }
    else
    {
        ioWndDataPtr->iDeltaPerLine = 0;
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
    PAINTSTRUCT ps;
    HDC         hdc;
    int         iHorzPos,iVertPos;
    int         iPaintBeg,iPaintEnd,y;
    int         x;
    COLORREF    bkColor,textColor;
    S_BUFF_DISP_DATA buffDispData;
    H_IOWND_BUFF_LINE hLineData;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    hdc = BeginPaint( hwnd, &ps );
    SelectObject( hdc, CreateFontIndirect(&(ioWndDataPtr->logFont)) );

    getAllScrollInfo(hwnd);
    iHorzPos = ioWndDataPtr->iHorzPos;
    iVertPos = ioWndDataPtr->iVertPos;

    iPaintBeg = max(0, iVertPos + ps.rcPaint.top / ioWndDataPtr->cyChar);
    iPaintEnd = min(IoWndGetLineMaxSize(ioWndDataPtr->hIoWndBuff),iVertPos + ps.rcPaint.bottom / ioWndDataPtr->cyChar);

    for( y=iPaintBeg,hLineData=IoWndBuffGetLinePtr(ioWndDataPtr->hIoWndBuff,y); (y<=iPaintEnd)&&(hLineData != NULL); y++,hLineData = IoWndBuffGetLineNextPtr(hLineData) )
    { /* 再描画領域のみ1行ずつ処理 */
        for( x=0; x<ioWndDataPtr->cxBuffer+1;x++ )
        {
            IoWndBuffGetDispData(ioWndDataPtr->hIoWndBuff,hLineData,x+iHorzPos,&buffDispData );

            if( buffDispData.size )
            {
                bkColor = GetBkColor(hdc);
                textColor = GetTextColor(hdc);

                if( buffDispData.type == TAB_CHAR )
                {
                    SetBkColor(hdc, TAB_BK_COLOR_RGB );
                }
                else
                {
                    SetBkColor(hdc, BK_COLOR_RGB );
                }

                if( DebugIoWndRect )
                { /* デバッグ用 */
                    SetBkColor(hdc, RGB(0xFF,0x00,0x00) );
                }

                if( buffDispData.bSelect )
                {
                    SetBkColor(hdc, SELECT_BK_COLOR_RGB );
                    SetTextColor(hdc, SELECT_TEXT_COLOR_RGB );
                }
                else
                {
                    SetTextColor(hdc, TEXT_COLOR_RGB );
                }

                TextOut(hdc,
                        (x*ioWndDataPtr->cxChar) - (buffDispData.offset*ioWndDataPtr->cxChar), /* x座標 */
                        (y-iVertPos) * ioWndDataPtr->cyChar, /* y座標 */
                        buffDispData.data,               /* 文字列へのポインタ */
                        buffDispData.size                /* 文字数 */
                    );

                SetTextColor(hdc,textColor);
                SetBkColor(hdc,bkColor);
            }
            else
            {
                break;
            }
        }
    }

    DeleteObject( SelectObject(hdc, GetStockObject(SYSTEM_FONT)) );
    EndPaint( hwnd, &ps );

    SetCaretPos( (IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iHorzPos)*ioWndDataPtr->cxChar, (IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iVertPos)*ioWndDataPtr->cyChar);
    printCaretPos(hwnd);

    DebugIoWndRect = 0; /* デバッグ用 */

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
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    ioWndDataPtr->cxClient = LOWORD( lParam );
    ioWndDataPtr->cyClient = HIWORD( lParam );

    setAllScrollInfo(hwnd);

    ioWndDataPtr->cxBuffer = max( 1, ioWndDataPtr->cxClient / ioWndDataPtr->cxChar );
    ioWndDataPtr->cyBuffer = max( 1, ioWndDataPtr->cyClient / ioWndDataPtr->cyChar );

    if( hwnd == GetFocus() )
    {
        SetCaretPos( IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff) * ioWndDataPtr->cxChar, IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff) * ioWndDataPtr->cyChar );
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
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    IoWndBuffEnd(ioWndDataPtr->hIoWndBuff);
    IoWndBuffDestroy(ioWndDataPtr->hIoWndBuff);
    free( ioWndDataPtr );

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

    switch( LOWORD(wParam) )
    {
    case IDM_EDIT_CUT:
        rtn = ioOnCut( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_COPY:
        rtn = ioOnCopy( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_PASTE:
        rtn = ioOnPaste( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_DELETE:
        rtn = ioOnClear( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_SELECT_ALL:
        rtn = ioOnSetSel( hwnd, message, 0, -1 );
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
ioOnKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    switch(wParam)
    {
    case VK_SHIFT:
        ioWndDataPtr->bShiftKeyOn = FALSE;
        break;
    default:
        break;
    }

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
    BOOL bErase = TRUE;
    BOOL bProc = TRUE;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( wParam == VK_SHIFT )
    {
        ioWndDataPtr->bShiftKeyOn = TRUE;
        IoWndBuffSelectOn(ioWndDataPtr->hIoWndBuff);
    }
    else if( wParam == VK_CONTROL )
    {
        nop();
    }
    else
    {
        getAllScrollInfo(hwnd);

        if( (wParam==VK_LEFT)||(wParam==VK_RIGHT)||(wParam==VK_UP)||(wParam==VK_DOWN) )
        {
            if( ioWndDataPtr->bShiftKeyOn )
            {
                IoWndBuffSelectOn(ioWndDataPtr->hIoWndBuff);
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

        switch(wParam)
        {
        case VK_LEFT:
            bErase = FALSE;
            IoWndDecCaretXpos(ioWndDataPtr->hIoWndBuff);
            break;
        case VK_RIGHT:
            bErase = FALSE;
            IoWndIncCaretXpos(ioWndDataPtr->hIoWndBuff);
            break;
        case VK_UP:
            bErase = FALSE;
            IoWndDecCaretYpos(ioWndDataPtr->hIoWndBuff);
            break;
        case VK_DOWN:
            bErase = FALSE;
            IoWndIncCaretYpos(ioWndDataPtr->hIoWndBuff);
            break;
        case VK_DELETE:
            ioWndRemoveData( hwnd, FALSE );
            break;
        case VK_BACK:
            ioWndRemoveData( hwnd, TRUE );
            break;
        default:
            bProc = FALSE;
            break;
        }

        if( bProc )
        {
            if( ioWndDataPtr->bShiftKeyOn )
            {
                nop();
            }
            else
            {
                IoWndBuffSelectOff(ioWndDataPtr->hIoWndBuff);
            }

            /* キャレットが表示範囲外に有った場合の処理(横方向) */
            if( IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff) < ioWndDataPtr->iHorzPos )
            {
                setScrollPos( hwnd, SB_HORZ, IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff) );
                bErase = TRUE;
            }
            else if( (ioWndDataPtr->iHorzPos+ioWndDataPtr->cxBuffer-1) < IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff) )
            {
                setScrollPos( hwnd, SB_HORZ, (ioWndDataPtr->iHorzPos+ioWndDataPtr->cxBuffer-1) );
                bErase = TRUE;
            }
            else
            {
                nop();
            }

            /* キャレットが表示範囲外に有った場合の処理(縦方向) */
            if( IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff) < ioWndDataPtr->iVertPos )
            {
                setScrollPos( hwnd, SB_VERT, IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff) );
                bErase = TRUE;
            }
            else if( (ioWndDataPtr->iVertPos+ioWndDataPtr->cyBuffer-1) < IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff) )
            {
                setScrollPos( hwnd, SB_VERT, IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff) - (ioWndDataPtr->cyBuffer-1) );
                bErase = TRUE;
            }
            else
            {
                nop();
            }

            HideCaret(hwnd);
            InvalidateRect( hwnd, NULL, bErase );
            SetCaretPos( (IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iHorzPos)*ioWndDataPtr->cxChar, (IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iVertPos)*ioWndDataPtr->cyChar);
            ShowCaret(hwnd);
        }
        else
        {
            nop();
        }
    }

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
    TCHAR data[2];
    int i;
    INT size;
    RECT rect;
    BOOL bRectSelect = FALSE;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    IoWndBuffSelectOff(ioWndDataPtr->hIoWndBuff);

    for( i=0; i<(int) LOWORD(lParam); i++ )
    {
        size = 0;

        switch( wParam )
        {
        case '\b':  /* backspace */
            break;
        case '\x1B':/* escape */
            break;
        case '\n':  /* line feed */
            break;
        case '\r':  /* carriage return */
            size = IoWndBuffGetNewLineData(ioWndDataPtr->hIoWndBuff,data);
            break;
        case '\t':  /* tab */
        default:
            /* 文字入力 */
            data[0] = (TCHAR)wParam;
            size = 1;
            rect.left   = (IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iHorzPos)*ioWndDataPtr->cxChar;
            rect.top    = (IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iVertPos)*ioWndDataPtr->cyChar;
            rect.right  = ioWndDataPtr->cxClient;
            rect.bottom = rect.top + ioWndDataPtr->cyChar;
            bRectSelect = TRUE;
            break;
        }

        if( size )
        {
            IoWndBuffDataSet( ioWndDataPtr->hIoWndBuff, data,size,FALSE );
            if( bRectSelect )
            {
#if 0 /* デバッグ用 */
                DebugIoWndRect = 1;
#endif
                InvalidateRect( hwnd, &rect, TRUE );
            }
            else
            {
                InvalidateRect( hwnd, NULL, TRUE );
            }
            SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0,EN_UPDATE), (LPARAM)hwnd);
        }
        else
        {
            nop();
        }
    }

    ioOnImeStartComposition( hwnd, message, wParam, lParam );

    SetCaretPos( (IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iHorzPos)*ioWndDataPtr->cxChar, (IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iVertPos)*ioWndDataPtr->cyChar);
    setAllScrollInfo(hwnd);

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
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

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
        ScrollWindow( hwnd, ioWndDataPtr->cxChar * (iHorzPos - si.nPos), 0, NULL, NULL );
        InvalidateRect( hwnd, NULL, TRUE );
    }

    GetScrollInfo( hwnd, SB_HORZ, &si );
    ioWndDataPtr->iHorzPos = si.nPos;

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
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

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
        ScrollWindow(hwnd, 0, ioWndDataPtr->cyChar * (iVertPos - si.nPos),NULL, NULL);
        UpdateWindow(hwnd);
    }

    GetScrollInfo(hwnd, SB_VERT, &si);
    ioWndDataPtr->iVertPos = si.nPos;

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
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    CreateCaret( hwnd,NULL,1,ioWndDataPtr->cyChar ); /* 幅=1,高さ=文字サイズ */
    SetCaretPos( IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff) * ioWndDataPtr->cxChar, IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff) * ioWndDataPtr->cyChar);
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
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( ioWndDataPtr->iDeltaPerLine )
    {
        ioWndDataPtr->iAccumDelta += (short)HIWORD(wParam);

        while( ioWndDataPtr->iAccumDelta >= ioWndDataPtr->iDeltaPerLine )
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
            ioWndDataPtr->iAccumDelta -= ioWndDataPtr->iDeltaPerLine;
        }
        while( ioWndDataPtr->iAccumDelta <= -ioWndDataPtr->iDeltaPerLine)
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
            ioWndDataPtr->iAccumDelta += ioWndDataPtr->iDeltaPerLine;
        }
    }
    else
    {
        /* do nothing */
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_MOUSEMOVE を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnMouseMove( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SHORT x,y;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    x = max(0,(SHORT)LOWORD(lParam));
    y = max(0,(SHORT)HIWORD(lParam));

    if( (wParam & MK_LBUTTON) )
    {
        IoWndSetCaretPos( ioWndDataPtr->hIoWndBuff, ((x + (ioWndDataPtr->iHorzPos*ioWndDataPtr->cxChar))/ioWndDataPtr->cxChar), ((y + (ioWndDataPtr->iVertPos*ioWndDataPtr->cyChar))/ioWndDataPtr->cyChar) );
        SetCaretPos( (IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iHorzPos)*ioWndDataPtr->cxChar, (IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iVertPos)*ioWndDataPtr->cyChar);
        InvalidateRect( hwnd, NULL, FALSE );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_LBUTTONDOWN を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnLbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    int x,y;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    getAllScrollInfo(hwnd);

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    IoWndSetCaretPos( ioWndDataPtr->hIoWndBuff, ((x + (ioWndDataPtr->iHorzPos*ioWndDataPtr->cxChar))/ioWndDataPtr->cxChar), ((y + (ioWndDataPtr->iVertPos*ioWndDataPtr->cyChar))/ioWndDataPtr->cyChar) );

    SetCaretPos( (IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iHorzPos)*ioWndDataPtr->cxChar, (IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff)-ioWndDataPtr->iVertPos)*ioWndDataPtr->cyChar);

    if( wParam & MK_SHIFT )
    {
        nop();
    }
    else
    {
        SetCapture(hwnd);
        IoWndBuffSelectOff(ioWndDataPtr->hIoWndBuff);
        IoWndBuffSelectOn(ioWndDataPtr->hIoWndBuff);
    }
    InvalidateRect( hwnd, NULL, TRUE );

    return rtn;
}

/********************************************************************************
 * 内容  : WM_MBUTTONDOWN を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnMbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_RBUTTONDOWN を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnRbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_LBUTTONUP を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnLbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    ReleaseCapture();

    return rtn;
}

/********************************************************************************
 * 内容  : WM_MBUTTONUP を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnMbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : WM_RBUTTONUP を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnRbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HMENU hMenu;
    POINT point;

    point.x = LOWORD(lParam);
    point.y = HIWORD(lParam);

    ClientToScreen(hwnd,&point);

    hMenu = CreatePopupMenu();
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_UNDO       , TEXT("元に戻す(&U)") );
    AppendMenu( hMenu, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_CUT        , TEXT("切り取り(&T)") );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_COPY       , TEXT("コピー(&C)") );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_PASTE      , TEXT("貼り付け(&P)") );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_DELETE     , TEXT("削除(&D)") );
    AppendMenu( hMenu, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("すべて選択(&A)") );

    EnableMenuItem( hMenu, IDM_EDIT_UNDO, MF_GRAYED );

    if( IoWndGetDataSize(hwnd,IOWND_SELECTED) )
    {
        EnableMenuItem( hMenu, IDM_EDIT_CUT   , MF_ENABLED );
        EnableMenuItem( hMenu, IDM_EDIT_COPY  , MF_ENABLED );
        EnableMenuItem( hMenu, IDM_EDIT_DELETE, MF_ENABLED );
    }
    else
    {
        EnableMenuItem( hMenu, IDM_EDIT_CUT   , MF_GRAYED );
        EnableMenuItem( hMenu, IDM_EDIT_COPY  , MF_GRAYED );
        EnableMenuItem( hMenu, IDM_EDIT_DELETE, MF_GRAYED );
    }

    if( IsClipboardFormatAvailable(CF_TEXT) )
    {
        EnableMenuItem( hMenu, IDM_EDIT_PASTE, MF_ENABLED );
    }
    else
    {
        EnableMenuItem( hMenu, IDM_EDIT_PASTE, MF_GRAYED );
    }

    TrackPopupMenu(hMenu,TPM_RIGHTBUTTON,point.x,point.y,0,hwnd,NULL);
    DestroyMenu(hMenu);

    return rtn;
}

/********************************************************************************
 * 内容  : WM_IME_STARTCOMPOSTION を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnImeStartComposition( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HIMC hImc;
    COMPOSITIONFORM cf;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    cf.dwStyle = CFS_POINT;
    cf.ptCurrentPos.x = IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff) * ioWndDataPtr->cxChar;
    cf.ptCurrentPos.y = IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff) * ioWndDataPtr->cyChar;

    hImc = ImmGetContext( hwnd );
    ImmSetCompositionWindow( hImc, &cf );
    ImmSetCompositionFont( hImc,&(ioWndDataPtr->logFont) );
    ImmReleaseContext( hwnd, hImc );

    return DefWindowProc(hwnd,message, wParam, lParam);
}

/********************************************************************************
 * 内容  : WM_CUT を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnCut( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    dwSize = IoWndGetDataSize(hwnd,IOWND_SELECTED);
    if( dwSize )
    {
        hGlobal = GlobalAlloc( GHND|GMEM_SHARE, dwSize+1 );
        pGlobal = GlobalLock( hGlobal );
        GlobalUnlock(hGlobal);
        IoWndBuffDataGet( ioWndDataPtr->hIoWndBuff, pGlobal,dwSize,IOWND_SELECTED );
        OpenClipboard(hwnd);
        EmptyClipboard();
        SetClipboardData( CF_TEXT, hGlobal );
        CloseClipboard();

        /* CUTのみ */
        ioWndRemoveData( hwnd,FALSE );
        InvalidateRect( hwnd, NULL, TRUE );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_COPY を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnCopy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    dwSize = IoWndGetDataSize(hwnd,IOWND_SELECTED);
    if( dwSize )
    {
        hGlobal = GlobalAlloc( GHND|GMEM_SHARE, dwSize+1 );
        pGlobal = GlobalLock( hGlobal );
        GlobalUnlock(hGlobal);
        IoWndBuffDataGet( ioWndDataPtr->hIoWndBuff, pGlobal,dwSize,IOWND_SELECTED );
        OpenClipboard(hwnd);
        EmptyClipboard();
        SetClipboardData( CF_TEXT, hGlobal );
        CloseClipboard();
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_PASTE を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnPaste( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize,dwSize2;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    PBYTE   dataPtr;

    if( IsClipboardFormatAvailable(CF_TEXT) )
    {
        OpenClipboard(hwnd);

        hGlobal = GetClipboardData(CF_TEXT);
        dwSize = GlobalSize(hGlobal) - 1;
        dataPtr = (PBYTE)malloc(dwSize);
        pGlobal = GlobalLock( hGlobal );
        strncpy( dataPtr, pGlobal, dwSize );
        GlobalUnlock(hGlobal);
        CloseClipboard();
        dwSize2 = strlen( dataPtr );
        if( dwSize2 < dwSize )
        { /* クリップボードの最大サイズに達する前にNULLがあれば */
            dwSize = dwSize2; /* NULLまでのデータを有効とする (アプリケーションによっては 余分なNULLが大量に入っていることがある) */
        }
        else
        {
            nop();
        }
        IoWndDataSet( hwnd, dataPtr,dwSize,FALSE );
        free( dataPtr );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_CLEAR を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnClear( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    ioWndRemoveData( hwnd,FALSE );
    InvalidateRect( hwnd, NULL, TRUE );

    return rtn;
}

/********************************************************************************
 * 内容  : WM_UNDO を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnUndo( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * 内容  : EM_SETSEL を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
ioOnSetSel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( (wParam == 0) && (lParam == -1) )
    { /* ”すべて選択”のみ対応 */
        if( IoWndBuffSelectAll(ioWndDataPtr->hIoWndBuff) )
        {
            IoWndSetCaretPos(ioWndDataPtr->hIoWndBuff, IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff),IoWndGetLineMaxSize(ioWndDataPtr->hIoWndBuff));
            setScrollPos( hwnd, SB_VERT, IoWndGetLineMaxSize(ioWndDataPtr->hIoWndBuff) );
            InvalidateRect( hwnd, NULL, TRUE );
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
 * 内容  : データ削除
 * 引数  : HWND hwnd
 * 引数  : bBackSpace
 * 戻り値: なし
 ***************************************/
static void
ioWndRemoveData( HWND hwnd, BOOL bBackSpace )
{
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    IoWndBuffRemoveData( ioWndDataPtr->hIoWndBuff, bBackSpace );
    SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0,EN_UPDATE), (LPARAM)hwnd);
}

/********************************************************************************
 * 内容  : テキストメトリクスの更新
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
static void
updateTextMetrics( HWND hwnd )
{
    HDC        hdc;
    TEXTMETRIC tm;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    hdc = GetDC( hwnd );
    SelectObject( hdc, CreateFontIndirect(&(ioWndDataPtr->logFont)) );

    GetTextMetrics( hdc, &tm );
    ioWndDataPtr->cxChar = tm.tmAveCharWidth;
    ioWndDataPtr->cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * ioWndDataPtr->cxChar / 2;
    ioWndDataPtr->cyChar = tm.tmHeight + tm.tmExternalLeading;

    DeleteObject( SelectObject( hdc, GetStockObject(SYSTEM_FONT) ) );
    ReleaseDC( hwnd, hdc );
}

/********************************************************************************
 * 内容  : スクロール情報のセット
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
static void
setAllScrollInfo( HWND hwnd )
{
    SCROLLINFO si;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    /* Set vertical scroll bar range and page size */
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;                                                                /* 範囲の最小値 */
    si.nMax   = max(IoWndGetLineMaxSize(ioWndDataPtr->hIoWndBuff),(ioWndDataPtr->cyClient / ioWndDataPtr->cyChar))-1; /* 範囲の最大値 */
    si.nPage  = (ioWndDataPtr->cyClient / ioWndDataPtr->cyChar); /* ページサイズ */
    SetScrollInfo( hwnd, SB_VERT, &si, TRUE );

    /* Set horizontal scroll bar range and page size*/
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = max( IoWndGetColumnMaxSize(ioWndDataPtr->hIoWndBuff),(ioWndDataPtr->cxClient/ioWndDataPtr->cxChar))-1;
    si.nPage  = (ioWndDataPtr->cxClient/ioWndDataPtr->cxChar);
    SetScrollInfo( hwnd, SB_HORZ, &si, TRUE );
}

/********************************************************************************
 * 内容  : スクロール情報の取得
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
static void
getAllScrollInfo( HWND hwnd )
{
    SCROLLINFO si;
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hwnd, SB_VERT, &si ); /* 縦スクロールバーの位置を取得 */
    ioWndDataPtr->iVertPos = si.nPos;

    GetScrollInfo( hwnd, SB_HORZ, &si);   /* 横スクロールバーの位置を取得 */
    ioWndDataPtr->iHorzPos = si.nPos;
}

/********************************************************************************
 * 内容  : スクロール位置のセット
 * 引数  : HWND hwnd
 * 引数  : int   fnBar スクロールバーのタイプ
 * 引数  : DWORD nPos  位置
 * 戻り値: なし
 ***************************************/
static void
setScrollPos( HWND hwnd, int fnBar, DWORD nPos )
{
    SCROLLINFO si;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hwnd, fnBar, &si );
    si.nPos   = nPos;
    SetScrollInfo( hwnd, fnBar, &si, TRUE );
}

/********************************************************************************
 * 内容  : カーソル位置をステータスバーに出力
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
static void
printCaretPos( HWND hwnd )
{
    S_IOWND_DATA *ioWndDataPtr = (S_IOWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    StsBarSetText( STS_BAR_0  ,"%d バイト、全 %d 行",IoWndGetBuffSize(ioWndDataPtr->hIoWndBuff, BUFF_ALL),IoWndGetLineMaxSize(ioWndDataPtr->hIoWndBuff) );
    StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",IoWndGetCaretYpos(ioWndDataPtr->hIoWndBuff)+1,IoWndGetCaretXpos(ioWndDataPtr->hIoWndBuff)+1);
}
