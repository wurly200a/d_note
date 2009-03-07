/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "mbctype.h"
#include "IoWndDef.h"

/* 外部関数定義 */
#include "WinMain.h"
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
static LRESULT ioOnDefault            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static void updateTextMetrics( HWND hwnd );
static void getAllScrollInfo( void );
static void setAllScrollInfo( void );
static void setScrollPos( int fnBar, DWORD nPos );
static void printCaretPos( void );

/* 内部変数定義 */
static HWND hWndIo; /* ウィンドウのハンドラ */
static S_IOWND_DATA ioWndData;

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
    ioOnDefault              /* default                */
};
/* *INDENT-ON* */


/********************************************************************************
 * 内容  : IOウィンドウクラスの登録、ウィンドウの生成
 * 引数  : HWND hWnd
 * 引数  : LOGFONT *logFontPtr
 * 戻り値: HWND
 ***************************************/
HWND
IoWndCreate( HWND hWnd, LOGFONT *logFontPtr )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();

    ioWndData.logFontPtr = logFontPtr;

    wc.lpfnWndProc      = (WNDPROC) IoWndProc;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );
    wc.hCursor          = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground    = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszClassName    = "ioWndClass";
    wc.lpszMenuName     = NULL;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
        return (HWND)NULL;
    }

    hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW /* | WS_EX_ACCEPTFILES*/,
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
 * 内容  : IOウィンドウのフォント変更
 * 引数  : LOGFONT *logFontPtr
 * 戻り値: なし
 ***************************************/
void IoWndChangeFont( LOGFONT *logFontPtr )
{
    ioWndData.logFontPtr = logFontPtr;

    updateTextMetrics( hWndIo );
    SendMessage(hWndIo,WM_SIZE,0,MAKELONG(ioWndData.cxClient,ioWndData.cyClient));
    InvalidateRect( hWndIo, NULL, TRUE );
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

    return TRUE;
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
 * 内容  : IOウィンドウのデータセット
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 戻り値: なし
 ***************************************/
void
IoWndDataSet( TCHAR* dataPtr, DWORD length )
{
    IoWndBuffDataSet( dataPtr, length );

    StsBarSetText( STS_BAR_0,"%d bytes,%d lines",IoWndGetBuffSize(),IoWndGetLineMaxSize() );

    ioWndData.xCaret = 0;
    ioWndData.yCaret = 0;

    setAllScrollInfo();
    IoWndInvalidateRect();
}

/********************************************************************************
 * 内容  : IOウィンドウの矩形無効化
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndInvalidateRect( void )
{
    InvalidateRect( hWndIo, NULL, TRUE );
}

/********************************************************************************
 * 内容  : IOウィンドウの改行コードセット
 * 引数  : NEWLINECODE_TYPE newLineCodeType
 * 戻り値: BOOL (TRUE:データが変更された)
 ***************************************/
BOOL
IoWndNewLineCodeSet( NEWLINECODE_TYPE newLineCodeType )
{
    return IoWndBuffSetNewLineCode( newLineCodeType );
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

    updateTextMetrics( hwnd );

    IoWndBuffInit();
    IoWndBuffDataSet( (TCHAR*)NULL, 0 );

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

#define SINGLE_CHAR      0
#define DOUBLE_CHAR_HIGH 1
#define DOUBLE_CHAR_LOW  2
/********************************************************************************
 * 内容  : キャラクタセットの判断(Shift_JISの場合)
 * 引数  : TCHAR *dataPtr  データの先頭ポインタ
 * 引数  : DWORD maxLength データの最大長さ
 * 引数  : DWORD offset    判断したい文字の先頭からのオフセット(0 origin)
 * 戻り値: int
 ***************************************/
static int
detectCharSet( TCHAR *dataPtr, DWORD maxLength, DWORD offset )
{
    DWORD i;
    int charType;

    for( i=0,charType=SINGLE_CHAR; i<maxLength; i++ )
    {
        if( charType == DOUBLE_CHAR_HIGH )
        {
            charType = DOUBLE_CHAR_LOW;
        }
        else
        {
            if( ( (BYTE)(*(dataPtr+i)) <= (BYTE)0x80) || (((BYTE)0xA0 <= (BYTE)(*(dataPtr+i))) && ((BYTE)(*(dataPtr+i)) <= (BYTE)0xDF)) )
            {
                charType = SINGLE_CHAR;
            }
            else
            {
                charType = DOUBLE_CHAR_HIGH;
            }
        }

        if( i==offset )
        {
            break;
        }
        else
        {
            nop();
        }
    }

    if( i==maxLength )
    {
        charType = SINGLE_CHAR;
    }
    else
    {
        nop();
    }

    return charType;
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
    S_BUFF_LINE_DATA *lineBuffPtr;
    int         x,charType;
    DWORD       offset,lineLength,dispLength;
    TCHAR       *dataPtr;

    hdc = BeginPaint( hwnd, &ps );
    SelectObject( hdc, CreateFontIndirect(ioWndData.logFontPtr) );

    getAllScrollInfo();
    iHorzPos = ioWndData.iHorzPos;
    iVertPos = ioWndData.iVertPos;

    iPaintBeg = max(0, iVertPos + ps.rcPaint.top / ioWndData.cyChar);
    iPaintEnd = min(IoWndGetLineMaxSize(),iVertPos + ps.rcPaint.bottom / ioWndData.cyChar);

    for( y=iPaintBeg; y<=iPaintEnd; y++ )
    { /* 再描画領域のみ1行ずつ処理 */
        lineBuffPtr = IoWndBuffGetLinePtr(y); /* 対象行のデータ取得 */
        if( lineBuffPtr != NULL )
        {
            lineLength = lineBuffPtr->dataSize-lineBuffPtr->newLineCodeSize; /* 1行内の文字数 */

            if( iHorzPos < lineLength )
            { /* 横スクロール位置が文字数未満(表示範囲に出力する文字有り) */
                dispLength = min(lineLength-iHorzPos,ioWndData.cxBuffer+1);

                if( (detectCharSet(lineBuffPtr->data,lineLength,iHorzPos)) == DOUBLE_CHAR_LOW )
                { /* 表示開始文字が DOUBLE下位 */
                    x = -(ioWndData.cxChar);              /* 上位byteから出力        */
                    offset = iHorzPos - 1;                /* 上位byteから出力        */
                    dataPtr = lineBuffPtr->data + offset;
                    dispLength++;                         /* 表示文字数が1個多くなる */
                }
                else
                { /* 表示開始文字はSINGLE or DOUBLE上位 */
                    x = 0;
                    offset = iHorzPos;
                    dataPtr = lineBuffPtr->data + offset;
                }

                if( (detectCharSet(dataPtr,(lineLength-offset),dispLength-1)) == DOUBLE_CHAR_HIGH )
                { /* 表示終了文字が DOUBLE上位 */
                    dispLength++; /* DOUBLE下位も表示 */
                }
                else
                {
                    nop();
                }

                TextOut(
                    hdc,
                    x,                               /* x座標 */
                    (y-iVertPos) * ioWndData.cyChar, /* y座標 */
                    dataPtr,                         /* 文字列へのポインタ */
                    dispLength                       /* 文字数 */
                    );
            }
            else
            {  /* 横スクロール位置が文字数以上(表示範囲に出力する文字無し) */
                nop();
            }
        }
        else
        {
            break;
        }
    }

    DeleteObject( SelectObject(hdc, GetStockObject(SYSTEM_FONT)) );
    EndPaint( hwnd, &ps );

    SetCaretPos( (ioWndData.xCaret-ioWndData.iHorzPos)*ioWndData.cxChar, (ioWndData.yCaret-ioWndData.iVertPos)*ioWndData.cyChar);

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
    IoWndBuffInit();

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
    S_BUFF_LINE_DATA *lineBuffPtr;
    SCROLLINFO si;
    int moveAmount = 0;

    getAllScrollInfo();

    lineBuffPtr = IoWndBuffGetLinePtr( ioWndData.yCaret );

    switch(wParam)
    {
    case VK_LEFT:
        if( lineBuffPtr == NULL )
        {
            nop();
        }
        else
        {
            if( (ioWndData.xCaret>0) &&
                (detectCharSet(lineBuffPtr->data,(lineBuffPtr->dataSize - lineBuffPtr->newLineCodeSize),ioWndData.xCaret-1) == DOUBLE_CHAR_LOW) )
            { /* 前の文字で移動量を判断 */
                moveAmount = 2;
            }
            else
            {
                moveAmount = 1;
            }

            if( ioWndData.xCaret == 0 )
            { /* キャレットはすでに行の最左端 */
                if( (lineBuffPtr->prevPtr != NULL) )
                { /* 前行有り */
                    ioWndData.xCaret = (lineBuffPtr->prevPtr->dataSize - lineBuffPtr->prevPtr->newLineCodeSize); /* 前行の最終位置 */
                    ioWndData.yCaret = max( ioWndData.yCaret - 1, 0 );
                }
                else
                { /* 前行無し */
                    nop();
                }
            }
            else
            { /* まだ左へ移動可能 */
                ioWndData.xCaret = max( ioWndData.xCaret - moveAmount, 0 );
            }
        }
        break;
    case VK_RIGHT:
        if( lineBuffPtr == NULL )
        {
            nop();
        }
        else
        {
            if( detectCharSet(lineBuffPtr->data,(lineBuffPtr->dataSize - lineBuffPtr->newLineCodeSize),ioWndData.xCaret) == DOUBLE_CHAR_HIGH )
            { /* 次の文字で移動量を判断 */
                moveAmount = 2;
            }
            else
            {
                moveAmount = 1;
            }

            if( (ioWndData.xCaret + moveAmount) > (lineBuffPtr->dataSize - lineBuffPtr->newLineCodeSize) )
            { /* キャレットはすでに行の最右端 */
                if( (lineBuffPtr->nextPtr != NULL) )
                { /* 次行有り */
                    ioWndData.xCaret = 0;
                    (ioWndData.yCaret)++;
                }
                else
                { /* 次行無し */
                    nop();
                }
            }
            else
            { /* まだ右へ移動可能 */
                ioWndData.xCaret += moveAmount;
            }
        }
        break;
    case VK_UP:
        if( (lineBuffPtr != NULL) &&
            (lineBuffPtr->prevPtr != NULL) )
        { /* 前行有り */
            if( ioWndData.xCaret > (lineBuffPtr->prevPtr->dataSize - lineBuffPtr->prevPtr->newLineCodeSize) )
            { /* 上は文字が無い */
                ioWndData.xCaret = (lineBuffPtr->prevPtr->dataSize - lineBuffPtr->prevPtr->newLineCodeSize); /* 前行の最終位置へ */
            }
            else
            { /* 上は文字が有る */
                if( (detectCharSet(lineBuffPtr->prevPtr->data,(lineBuffPtr->prevPtr->dataSize - lineBuffPtr->prevPtr->newLineCodeSize),ioWndData.xCaret) == DOUBLE_CHAR_LOW) )
                { /* 上は2byte文字の真ん中だったら */
                    ioWndData.xCaret += 1; /* 後ろへずらす */
                }
                else
                {
                    nop();
                }
            }
            ioWndData.yCaret = max( ioWndData.yCaret - 1, 0 );
        }
        else
        { /* 前行無し */
            nop();
        }
        break;
    case VK_DOWN:
        if( (lineBuffPtr != NULL) &&
            (lineBuffPtr->nextPtr != NULL) )
        { /* 次行有り */
            if( ioWndData.xCaret > (lineBuffPtr->nextPtr->dataSize - lineBuffPtr->nextPtr->newLineCodeSize) )
            { /* 下は文字が無い */
                ioWndData.xCaret = (lineBuffPtr->nextPtr->dataSize - lineBuffPtr->nextPtr->newLineCodeSize);
            }
            else
            { /* 下は文字が有る */
                if( (detectCharSet(lineBuffPtr->nextPtr->data,(lineBuffPtr->nextPtr->dataSize - lineBuffPtr->nextPtr->newLineCodeSize),ioWndData.xCaret) == DOUBLE_CHAR_LOW) )
                { /* 下は2byte文字の真ん中だったら */
                    ioWndData.xCaret += 1; /* 後ろへずらす */
                }
                else
                {
                    nop();
                }
            }
            (ioWndData.yCaret)++;
        }
        else
        { /* 次行無し */
            nop();
        }
        break;
    default:
        break;
    }

    /* キャレットが表示範囲外に有った場合の処理(横方向) */
    if( ioWndData.xCaret < ioWndData.iHorzPos )
    {
        setScrollPos( SB_HORZ, ioWndData.xCaret );
    }
    else if( (ioWndData.iHorzPos+ioWndData.cxBuffer-1) < ioWndData.xCaret )
    {
        setScrollPos( SB_HORZ, (ioWndData.iHorzPos+ioWndData.cxBuffer-1) );
    }
    else
    {
        nop();
    }

    /* キャレットが表示範囲外に有った場合の処理(縦方向) */
    if( ioWndData.yCaret < ioWndData.iVertPos )
    {
        setScrollPos( SB_VERT, ioWndData.yCaret );
    }
    else if( (ioWndData.iVertPos+ioWndData.cyBuffer-1) < ioWndData.yCaret )
    {
        setScrollPos( SB_VERT, ioWndData.yCaret - (ioWndData.cyBuffer-1) );
    }
    else
    {
        nop();
    }

    SetCaretPos( (ioWndData.xCaret-ioWndData.iHorzPos)*ioWndData.cxChar, (ioWndData.yCaret-ioWndData.iVertPos)*ioWndData.cyChar);
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
    int i;

    for( i=0; i<(int) LOWORD(lParam); i++ )
    {
        switch( wParam )
        {
        case '\b':  /* backspace */
            IoWndBuffRemoveData( ioWndData.yCaret, ioWndData.xCaret, 1 );
            if( ioWndData.xCaret > 0 )
            {
                (ioWndData.xCaret)--;
            }
            else
            {
                
            }
            IoWndInvalidateRect();
            break;
        case '\t':  /* tab */
        case '\x1B':/* escape */
            break;
        case '\n':  /* line feed */
            break;
        case '\r':  /* carriage return */
            IoWndBuffAddNewLine( ioWndData.yCaret, ioWndData.xCaret, wParam );
            ioWndData.xCaret = 0;
            (ioWndData.yCaret)++;
            IoWndInvalidateRect();
            break;
        default:    /* character codes */
            IoWndBuffAddData( ioWndData.yCaret, ioWndData.xCaret, wParam );
            (ioWndData.xCaret)++;
            IoWndInvalidateRect();
            break ;
        }
    }
    SetCaretPos( (ioWndData.xCaret-ioWndData.iHorzPos)*ioWndData.cxChar, (ioWndData.yCaret-ioWndData.iVertPos)*ioWndData.cyChar);
    setAllScrollInfo();

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

    CreateCaret( hwnd,NULL,1,ioWndData.cyChar ); /* 幅=1,高さ=文字サイズ */
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
    LRESULT rtn = 0;
    HIMC hImc;
    COMPOSITIONFORM cf;

    cf.dwStyle = CFS_POINT;
    cf.ptCurrentPos.x = ioWndData.xCaret * ioWndData.cxChar;
    cf.ptCurrentPos.y = ioWndData.yCaret * ioWndData.cyChar;

    hImc = ImmGetContext( hwnd );
    ImmSetCompositionWindow( hImc, &cf );
    ImmSetCompositionFont( hImc,ioWndData.logFontPtr );
    ImmReleaseContext( hwnd, hImc );

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
 * 内容  : テキストメトリクスの更新
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
static void
updateTextMetrics( HWND hwnd )
{
    HDC        hdc;
    TEXTMETRIC tm;

    hdc = GetDC( hwnd );
    SelectObject( hdc, CreateFontIndirect(ioWndData.logFontPtr) );

    GetTextMetrics( hdc, &tm );
    ioWndData.cxChar = tm.tmAveCharWidth;
    ioWndData.cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * ioWndData.cxChar / 2;
    ioWndData.cyChar = tm.tmHeight + tm.tmExternalLeading;

    DeleteObject( SelectObject( hdc, GetStockObject(SYSTEM_FONT) ) );
    ReleaseDC( hwnd, hdc );
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
    si.nMax   = max(IoWndGetLineMaxSize(),(ioWndData.cyClient / ioWndData.cyChar))-1; /* 範囲の最大値 */
    si.nPage  = (ioWndData.cyClient / ioWndData.cyChar); /* ページサイズ */
    SetScrollInfo( hWndIo, SB_VERT, &si, TRUE );

    /* Set horizontal scroll bar range and page size*/
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = max( IoWndGetColumnMaxSize(),(ioWndData.cxClient/ioWndData.cxChar))-1;
    si.nPage  = (ioWndData.cxClient/ioWndData.cxChar);
    SetScrollInfo( hWndIo, SB_HORZ, &si, TRUE );
}

/********************************************************************************
 * 内容  : スクロール情報の取得
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
static void
getAllScrollInfo( void )
{
    SCROLLINFO si;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hWndIo, SB_VERT, &si ); /* 縦スクロールバーの位置を取得 */
    ioWndData.iVertPos = si.nPos;

    GetScrollInfo( hWndIo, SB_HORZ, &si);   /* 横スクロールバーの位置を取得 */
    ioWndData.iHorzPos = si.nPos;
}

/********************************************************************************
 * 内容  : スクロール位置のセット
 * 引数  : int   fnBar スクロールバーのタイプ
 * 引数  : DWORD nPos  位置
 * 戻り値: なし
 ***************************************/
static void
setScrollPos( int fnBar, DWORD nPos )
{
    SCROLLINFO si;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hWndIo, fnBar, &si );
    si.nPos   = nPos;
    SetScrollInfo( hWndIo, fnBar, &si, TRUE );
    IoWndInvalidateRect();
}

/********************************************************************************
 * 内容  : カーソル位置をステータスバーに出力
 * 引数  : void
 * 戻り値: void
 ***************************************/
static void
printCaretPos( void )
{
    StsBarSetText( STS_BAR_MAX,"   %d 行、%d 列",ioWndData.yCaret,ioWndData.xCaret);
}
