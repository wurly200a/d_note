/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "EditWndDef.h"

/* 外部関数定義 */
#include "EditWndBuffer.h"
#include "DebugWnd.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "EditWnd.h"

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
    int    iHorzAccumDelta;
    int    iDeltaPerLine;
    int    iDeltaPerChar;
    int    iHorzPos;     /* スクロールバーの横位置  */
    int    iVertPos;     /* スクロールバーの縦位置  */
    HFONT  hFont;
    BOOL   bShiftKeyOn;
    H_EDITWND_BUFF hEditWndBuff;
    LONG   csStyle;
} S_EDITWND_DATA;

LRESULT CALLBACK EditWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static EDITWND_INDEX editWndConvertMSGtoINDEX( UINT message );
static LRESULT editOnCreate             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnPaint              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnSize               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnClose              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnDestroy            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnCommand            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnKeyUp              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnKeyDown            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnChar               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnHscroll            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnVscroll            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnSetFocus           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnKillFocus          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnMouseActivate      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnMouseWheel         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnMouseHwheel        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnMouseMove          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnLbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnMbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnRbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnLbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnMbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnRbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnImeStartComposition( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnCut                ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnCopy               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnPaste              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnClear              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnUndo               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnRedo               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnSetSel             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnSetFont            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnGetLineCount       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnLineFromChar       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnGoToLine           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT editOnDefault            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static BOOL editWndRemoveData( HWND hwnd, BOOL bBackSpace );
static void editWndCaretPosUpdate( S_EDITWND_DATA *editWndDataPtr );
static BOOL editWndCaretPosOutScroll( HWND hwnd, S_EDITWND_DATA *editWndDataPtr );
static void updateTextMetrics( HWND hwnd );
static void setAllScrollInfo( HWND hwnd );
static void getAllScrollInfo( HWND hwnd );
static void setScrollPos( HWND hwnd, int fnBar, DWORD nPos );

enum
{
    EDITWND_INVALID_RECT_KIND_1LINE_NOWPOS_AFTER,
};
typedef INT EDITWND_INVALID_RECT_KIND;
static void editWndGetInvalidateRect( S_EDITWND_DATA *editWndDataPtr, RECT *rectPtr, EDITWND_INVALID_RECT_KIND kind );
static void editWndInvalidateRect( HWND hWnd, RECT *rectPtr, BOOL bErase, TCHAR *strDebugMsg );

/* 内部変数定義 */

/* *INDENT-OFF* */
static LRESULT (*editWndProcTbl[EDITWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    editOnCreate             , /* WM_CREATE              */
    editOnPaint              , /* WM_PAINT               */
    editOnSize               , /* WM_SIZE                */
    editOnClose              , /* WM_CLOSE               */
    editOnDestroy            , /* WM_DESTROY             */
    editOnCommand            , /* WM_COMMAND             */
    editOnKeyUp              , /* WM_KEYUP               */
    editOnKeyDown            , /* WM_KEYDOWN             */
    editOnChar               , /* WM_CHAR                */
    editOnHscroll            , /* WM_HSCROLL             */
    editOnVscroll            , /* WM_VSCROLL             */
    editOnSetFocus           , /* WM_SETFOCUS            */
    editOnKillFocus          , /* WM_KILLFOCUS           */
    editOnMouseActivate      , /* WM_MOUSEACTIVATE       */
    editOnMouseWheel         , /* WM_MOUSEWHEEL          */
    editOnMouseHwheel        , /* WM_MOUSEHWHEEL         */
    editOnMouseMove          , /* WM_MOUSEMOVE           */
    editOnLbuttonDown        , /* WM_LBUTTONDOWN         */
    editOnMbuttonDown        , /* WM_MBUTTONDOWN         */
    editOnRbuttonDown        , /* WM_RBUTTONDOWN         */
    editOnLbuttonUp          , /* WM_LBUTTONUP           */
    editOnMbuttonUp          , /* WM_MBUTTONUP           */
    editOnRbuttonUp          , /* WM_RBUTTONUP           */
    editOnImeStartComposition, /* WM_IME_STARTCOMPOSITION*/
    editOnCut                , /* WM_CUT                 */
    editOnCopy               , /* WM_COPY                */
    editOnPaste              , /* WM_PASTE               */
    editOnClear              , /* WM_CLEAR               */
    editOnUndo               , /* WM_UNDO                */
    editOnRedo               , /* WM_REDO                */
    editOnSetSel             , /* EM_SETSEL              */
    editOnSetFont            , /* WM_SETFONT             */
    editOnGetLineCount       , /* EM_GETLINECOUNT        */
    editOnLineFromChar       , /* EM_LINEFROMCHAR        */
    editOnGoToLine           , /* WM_GOTOLINE            */
    editOnDefault              /* default                */
};
/* *INDENT-ON* */

WORD DebugInvalidRectPaintRed;

/********************************************************************************
 * 内容  : EDITウィンドウクラスの登録
 * 引数  : HINSTANCE hInst
 * 戻り値: BOOL
 ***************************************/
BOOL
EditWndRegisterClass( HINSTANCE hInst )
{
    BOOL rtn = TRUE;
    WNDCLASS wc = {0};

    wc.lpfnWndProc      = (WNDPROC) EditWndProc;
    wc.hInstance        = hInst;
    wc.hIcon            = (HICON)0;
    wc.hCursor          = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground    = (HBRUSH) CreateSolidBrush( BG_COLOR_RGB ); /* 上書きされるので何でもいい */
    wc.lpszClassName    = "teddedit";
    wc.lpszMenuName     = NULL;
    wc.cbWndExtra       = sizeof(S_EDITWND_DATA);

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("RegisterClass Failed!"), TEXT("teddedit"), MB_ICONERROR );
        rtn = FALSE;
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : EDITウィンドウの初期化
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
void
EditWndDataInit( HWND hwnd )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    EditWndBuffInit(editWndDataPtr->hEditWndBuff);

    setAllScrollInfo(hwnd);
    editWndInvalidateRect( hwnd, NULL, TRUE, "EditWndDataInit" );
}

/********************************************************************************
 * 内容  : EDITウィンドウのデータセット
 * 引数  : HWND hwnd
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void
EditWndDataSet( HWND hwnd, TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    EditWndBuffDataSet( editWndDataPtr->hEditWndBuff, dataPtr, length, bInit, bInit ? FALSE : TRUE );

    if( bInit )
    {
        nop();
    }
    else
    {
        SendMessage(GetParent(hwnd), (UINT)WM_COMMAND, MAKEWPARAM(0,EN_CHANGE), (LPARAM)hwnd);
    }

    EditWndBuffSelectOff(editWndDataPtr->hEditWndBuff);
    setAllScrollInfo(hwnd);
    editWndInvalidateRect( hwnd, NULL, TRUE, "EditWndDataSet" );
}

/********************************************************************************
 * 内容  : EDITウィンドウのデータ取得
 * 引数  : HWND hwnd
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : EDITWND_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL
EditWndDataGet( HWND hwnd, TCHAR *dataPtr, DWORD dataSize, EDITWND_REGION region )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    return EditWndBuffDataGet( editWndDataPtr->hEditWndBuff, dataPtr, dataSize, region );
}

/********************************************************************************
 * 内容  : EDITウィンドウのデータサイズ取得
 * 引数  : HWND hwnd
 * 引数  : EDITWND_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD
EditWndGetDataSize( HWND hwnd, EDITWND_REGION region )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    return EditWndBuffGetDataSize( editWndDataPtr->hEditWndBuff, region );
}

/********************************************************************************
 * 内容  : EDITウィンドウバッファの最大文字サイズ取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD
EditWndGetColumnMaxSize( HWND hwnd )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    return EditWndBuffGetColumnMaxSize( editWndDataPtr->hEditWndBuff );
}

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットX位置取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD
EditWndGetCaretXpos( HWND hwnd )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    return EditWndBuffGetCaretXpos( editWndDataPtr->hEditWndBuff );
}

/********************************************************************************
 * 内容  : EDITウィンドウの改行コードセット
 * 引数  : HWND hwnd
 * 引数  : NEWLINECODE_TYPE newLineCodeType
 * 戻り値: BOOL (TRUE:表示書き換え必要)
 ***************************************/
BOOL
EditWndNewLineCodeSet( HWND hwnd, NEWLINECODE_TYPE newLineCodeType )
{
    DWORD allDataSize;
    TCHAR *dataTopPtr;
    BOOL bRtn = FALSE;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    EditWndBuffSetNewLineCode( editWndDataPtr->hEditWndBuff, newLineCodeType );

    allDataSize = EditWndBuffGetDataSize(editWndDataPtr->hEditWndBuff, BUFF_ALL);
    dataTopPtr  = malloc( sizeof(TCHAR) * allDataSize );
    if( dataTopPtr != NULL )
    {
        EditWndBuffDataGet( editWndDataPtr->hEditWndBuff, dataTopPtr, allDataSize, BUFF_ALL );
        EditWndBuffDataSet( editWndDataPtr->hEditWndBuff, dataTopPtr, allDataSize, TRUE, FALSE );
        setAllScrollInfo(hwnd);
        editWndInvalidateRect( hwnd, NULL, TRUE, "EditWndNewLineCodeSet" );

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
 * 内容  : EDITウィンドウの検索文字列データセット
 * 引数  : HWND hwnd
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL bDirectionUp
 * 引数  : BOOL bMatchCase
 * 戻り値: BOOL
 ***************************************/
BOOL
EditWndFindDataSet( HWND hwnd, TCHAR* dataPtr, DWORD length, BOOL bDirectionUp, BOOL bMatchCase )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    BOOL rtn = (BOOL)FALSE;

    rtn = EditWndBuffFindDataSet( editWndDataPtr->hEditWndBuff, dataPtr, length, bDirectionUp, bMatchCase );

    setAllScrollInfo(hwnd);

    editWndCaretPosOutScroll(hwnd,editWndDataPtr);

    editWndInvalidateRect( hwnd, NULL, TRUE, "EditWndFindDataSet" );

    return rtn;
}

#ifdef _MSC_VER
#define STRNCASECMP strnicmp
#else
#define STRNCASECMP strncasecmp
#endif
/********************************************************************************
 * 内容  : EDITウィンドウの文字列置換
 * 引数  : HWND hwnd
 * 引数  : TCHAR* searchDataPtr
 * 引数  : TCHAR* replaceDataPtr
 * 引数  : DWORD  replaceDataSize
 * 引数  : BOOL bDirectionUp
 * 引数  : BOOL bMatchCase
 * 戻り値: BOOL
 ***************************************/
BOOL
EditWndReplaceData( HWND hwnd, TCHAR* searchDataPtr, TCHAR* replaceDataPtr, DWORD replaceDataSize, BOOL bDirectionUp, BOOL bMatchCase )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    BOOL rtn = (BOOL)FALSE;

    DWORD dwTmpSize;
    TCHAR *tmpDataPtr;

    dwTmpSize = EditWndGetDataSize(hwnd,EDITWND_SELECTED);

    if( dwTmpSize )
    {
        tmpDataPtr = malloc( dwTmpSize + 1 );
        *(tmpDataPtr + dwTmpSize) = '\0';

        EditWndDataGet( hwnd,tmpDataPtr,dwTmpSize,EDITWND_SELECTED );
        if( ( bMatchCase && !strncmp( tmpDataPtr,searchDataPtr,dwTmpSize)) ||
            (!bMatchCase && !STRNCASECMP(tmpDataPtr,searchDataPtr,dwTmpSize)) )
        {
            /* エディットコントロールのメッセージに従うなら EM_REPLACESEL だが */
#if 0
            okMessage(hwnd, TEXT("\"%s\" と一致したので、\"%s\"(%d文字)に置換します"),pfr->lpstrFindWhat, pfr->lpstrReplaceWith, min(strlen(pfr->lpstrReplaceWith),pfr->wReplaceWithLen) );
#endif
            editWndRemoveData( hwnd, FALSE );
            EditWndBuffDataSet( editWndDataPtr->hEditWndBuff,replaceDataPtr,replaceDataSize,FALSE, TRUE );

            setAllScrollInfo(hwnd);

            editWndCaretPosOutScroll(hwnd,editWndDataPtr);

            editWndInvalidateRect( hwnd, NULL, TRUE, "EditWndReplaceData" );
            rtn = (BOOL)TRUE; /* 次の文字列を探せ */
        }
        else
        { /* 選択領域の文字列が不一致 */
            rtn = (BOOL)TRUE; /* 先に文字列を探せ */
        }
    }
    else
    { /* 選択領域の文字列がない */
        rtn = (BOOL)TRUE; /* 先に文字列を探せ */
    }

    return rtn;
}

/********************************************************************************
 * 内容  : メインウィンドウ内で処理するメッセージを判定する
 * 引数  : MSG *msg
 * 戻り値: BOOL
 ***************************************/
BOOL
IsEditWndMessage( MSG *msg )
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
EditWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#if 0
    DebugWndPrintf("EditWndProc,%X,%X,%X\r\n",message,wParam,lParam);
#endif
    return editWndProcTbl[editWndConvertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 内容  : ウィンドウズメッセージから対応するインデックスに変換する
 * 引数  : UINT message
 * 戻り値: MAIN_WNDPRC_INDEX
 ***************************************/
static EDITWND_INDEX
editWndConvertMSGtoINDEX( UINT message )
{
    EDITWND_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE              :rtn = EDITWND_ON_CREATE              ;break;
    case WM_PAINT               :rtn = EDITWND_ON_PAINT               ;break;
    case WM_SIZE                :rtn = EDITWND_ON_SIZE                ;break;
    case WM_CLOSE               :rtn = EDITWND_ON_CLOSE               ;break;
    case WM_DESTROY             :rtn = EDITWND_ON_DESTROY             ;break;
    case WM_COMMAND             :rtn = EDITWND_ON_COMMAND             ;break;
    case WM_KEYUP               :rtn = EDITWND_ON_KEYUP               ;break;
    case WM_KEYDOWN             :rtn = EDITWND_ON_KEYDOWN             ;break;
    case WM_CHAR                :rtn = EDITWND_ON_CHAR                ;break;
    case WM_HSCROLL             :rtn = EDITWND_ON_HSCROLL             ;break;
    case WM_VSCROLL             :rtn = EDITWND_ON_VSCROLL             ;break;
    case WM_SETFOCUS            :rtn = EDITWND_ON_SETFOCUS            ;break;
    case WM_KILLFOCUS           :rtn = EDITWND_ON_KILLFOCUS           ;break;
    case WM_MOUSEACTIVATE       :rtn = EDITWND_ON_MOUSEACTIVATE       ;break;
    case WM_MOUSEWHEEL          :rtn = EDITWND_ON_MOUSEWHEEL          ;break;
    case WM_MOUSEHWHEEL         :rtn = EDITWND_ON_MOUSEHWHEEL         ;break;
    case WM_MOUSEMOVE           :rtn = EDITWND_ON_MOUSEMOVE           ;break;
    case WM_LBUTTONDOWN         :rtn = EDITWND_ON_LBUTTONDOWN         ;break;
    case WM_MBUTTONDOWN         :rtn = EDITWND_ON_MBUTTONDOWN         ;break;
    case WM_RBUTTONDOWN         :rtn = EDITWND_ON_RBUTTONDOWN         ;break;
    case WM_LBUTTONUP           :rtn = EDITWND_ON_LBUTTONUP           ;break;
    case WM_MBUTTONUP           :rtn = EDITWND_ON_MBUTTONUP           ;break;
    case WM_RBUTTONUP           :rtn = EDITWND_ON_RBUTTONUP           ;break;
    case WM_IME_STARTCOMPOSITION:rtn = EDITWND_ON_IME_STARTCOMPOSITION;break;
    case WM_CUT                 :rtn = EDITWND_ON_CUT                 ;break;
    case WM_COPY                :rtn = EDITWND_ON_COPY                ;break;
    case WM_PASTE               :rtn = EDITWND_ON_PASTE               ;break;
    case WM_CLEAR               :rtn = EDITWND_ON_CLEAR               ;break;
    case WM_UNDO                :rtn = EDITWND_ON_UNDO                ;break;
    case WM_REDO                :rtn = EDITWND_ON_REDO                ;break;
    case EM_SETSEL              :rtn = EDITWND_ON_SETSEL              ;break;
    case WM_SETFONT             :rtn = EDITWND_ON_SETFONT             ;break;
    case EM_GETLINECOUNT        :rtn = EDITWND_ON_GETLINECOUNT        ;break;
    case EM_LINEFROMCHAR        :rtn = EDITWND_ON_LINEFROMCHAR        ;break;
    case WM_GOTOLINE            :rtn = EDITWND_ON_GOTOLINE            ;break;

    case WM_NCMOUSEMOVE         :rtn = EDITWND_ON_DEFAULT             ;break;
    case WM_SETCURSOR           :rtn = EDITWND_ON_DEFAULT             ;break;
    case WM_NCPAINT             :rtn = EDITWND_ON_DEFAULT             ;break;
    case WM_NCHITTEST           :rtn = EDITWND_ON_DEFAULT             ;break;
    case WM_ERASEBKGND          :rtn = EDITWND_ON_DEFAULT             ;break;

    case WM_IME_COMPOSITION     :rtn = EDITWND_ON_DEFAULT             ;break;
    case WM_IME_NOTIFY          :rtn = EDITWND_ON_DEFAULT             ;break;
    case WM_IME_SETCONTEXT      :rtn = EDITWND_ON_DEFAULT             ;break;
    case WM_IME_COMPOSITIONFULL :rtn = EDITWND_ON_DEFAULT             ;break;

    default                     :rtn = EDITWND_ON_DEFAULT             ;break;
    }
    /* *INDENT-ON* */

    return rtn;
}

/********************************************************************************
 * 内容  : WM_CREATE の処理
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (LPCREATESTRUCT)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
editOnCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    int iDelta;          /* for MouseWheel */
    ULONG ulScrollLines; /* for MouseWheel */
    ULONG ulScrollChars; /* for MouseWheel */
    S_EDITWND_DATA *editWndDataPtr;
    LOGFONT logFont;
    LPCREATESTRUCT csPtr = (LPCREATESTRUCT)lParam;

    editWndDataPtr = (S_EDITWND_DATA *)malloc( sizeof(S_EDITWND_DATA) );
    memset( editWndDataPtr, 0, sizeof(S_EDITWND_DATA) );

    editWndDataPtr->csStyle = csPtr->style;

    GetObject( GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), (PTSTR)&(logFont) ); /* 本家エディットコントロールと異なる。固定幅のみなので */
    editWndDataPtr->hFont = CreateFontIndirect(&logFont);

    editWndDataPtr->hEditWndBuff = EditWndBuffCreate();
    SetWindowLongPtr(hwnd,0,(LONG_PTR)editWndDataPtr);

    updateTextMetrics( hwnd );

    EditWndBuffInit(editWndDataPtr->hEditWndBuff);

    /* for MouseWheel */
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0, &ulScrollLines, 0);
    iDelta = HIWORD(wParam);
    if( ulScrollLines )
    {
        editWndDataPtr->iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
    }
    else
    {
        editWndDataPtr->iDeltaPerLine = 0;
    }

    SystemParametersInfo(SPI_GETWHEELSCROLLCHARS,0, &ulScrollChars, 0);
    iDelta = HIWORD(wParam);
    if( ulScrollChars )
    {
        editWndDataPtr->iDeltaPerChar = WHEEL_DELTA / ulScrollChars;
    }
    else
    {
        editWndDataPtr->iDeltaPerChar = 0;
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
editOnPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC         hdc;
    int         iHorzPos,iVertPos;
    int         iPaintBeg,iPaintEnd,y;
    int         x;
    COLORREF    bkColor,textColor;
    S_BUFF_DISP_DATA buffDispData;
    H_EDITWND_BUFF_LINE hLineData;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    hdc = BeginPaint( hwnd, &ps );
    SelectObject( hdc, editWndDataPtr->hFont );

    getAllScrollInfo(hwnd);
    iHorzPos = editWndDataPtr->iHorzPos;
    iVertPos = editWndDataPtr->iVertPos;

#if 0
    DebugWndPrintf("WM_PAINT,left:%d,top:%d,right:%d,bottom:%d\r\n",ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right,ps.rcPaint.bottom);
#endif

    iPaintBeg = max(0, ps.rcPaint.top / editWndDataPtr->cyChar);
    iPaintEnd = ps.rcPaint.bottom / editWndDataPtr->cyChar;

    for( y=iPaintBeg,hLineData=EditWndBuffGetLinePtr(editWndDataPtr->hEditWndBuff,y+iVertPos); y<=iPaintEnd; y++ )
    { /* 再描画領域のみ1行ずつ処理 */
        for( x=0; x<(ps.rcPaint.right/editWndDataPtr->cxChar)+1;x++ )
        {
            if( hLineData != NULL )
            {
                EditWndBuffGetDispData(editWndDataPtr->hEditWndBuff,hLineData,x+iHorzPos,&buffDispData );
            }
            else
            {
                buffDispData.size = 0;
            }

            if( buffDispData.size )
            {
                bkColor = GetBkColor(hdc);
                textColor = GetTextColor(hdc);

                if( editWndDataPtr->csStyle & ES_READONLY )
                {
                    SetBkColor(hdc, GetSysColor(COLOR_BTNFACE) );
                }
                else
                {
                    if( buffDispData.type == TAB_CHAR )
                    {
                        SetBkColor(hdc, TAB_BK_COLOR_RGB );
                    }
                    else
                    {
                        SetBkColor(hdc, BK_COLOR_RGB );
                    }
                }

                if( DebugInvalidRectPaintRed )
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
                        (x*editWndDataPtr->cxChar) - (buffDispData.offset*editWndDataPtr->cxChar), /* x座標 */
                        y * editWndDataPtr->cyChar, /* y座標 */
                        buffDispData.data,               /* 文字列へのポインタ */
                        buffDispData.size                /* 文字数 */
                    );

                SetTextColor(hdc,textColor);
                SetBkColor(hdc,bkColor);
            }
            else
            {
                bkColor = GetBkColor(hdc);
                textColor = GetTextColor(hdc);

                if( editWndDataPtr->csStyle & ES_READONLY )
                {
                    SetBkColor(hdc, GetSysColor(COLOR_BTNFACE) );
                }
                else
                {
                    SetBkColor(hdc, BK_COLOR_RGB );
                }
                SetTextColor(hdc, TEXT_COLOR_RGB );

                TextOut(hdc,
                        (x*editWndDataPtr->cxChar), /* x座標 */
                        y * editWndDataPtr->cyChar, /* y座標 */
                        " ",               /* 文字列へのポインタ */
                        1                  /* 文字数 */
                    );

                SetTextColor(hdc,textColor);
                SetBkColor(hdc,bkColor);
            }
        }

        if( hLineData != NULL )
        {
            hLineData = EditWndBuffGetLineNextPtr(hLineData);
        }
        else
        {
            nop();
        }
    }

    EndPaint( hwnd, &ps );

    if( hwnd == GetFocus() )
    {
        editWndCaretPosUpdate(editWndDataPtr);
        SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0,EN_UPDATE), (LPARAM)hwnd);
    }
    else
    {
        /* do nothing */
    }

    DebugInvalidRectPaintRed = 0; /* デバッグ用 */

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
editOnSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    RECT        rc;

    GetClientRect(hwnd, &rc);
#if 0
    DebugWndPrintf("WM_SIZE,left:%d,top:%d,right:%d,bottom:%d\r\n",rc.left,rc.top,rc.right,rc.bottom);
#endif
    editWndDataPtr->cxClient = LOWORD( lParam );
    editWndDataPtr->cyClient = HIWORD( lParam );

    setAllScrollInfo(hwnd);

    editWndDataPtr->cxBuffer = max( 1, editWndDataPtr->cxClient / editWndDataPtr->cxChar );
    editWndDataPtr->cyBuffer = max( 1, editWndDataPtr->cyClient / editWndDataPtr->cyChar );

    if( hwnd == GetFocus() )
    {
        SetCaretPos( EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff) * editWndDataPtr->cxChar, EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff) * editWndDataPtr->cyChar );
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
editOnClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
editOnDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    EditWndBuffEnd(editWndDataPtr->hEditWndBuff);
    EditWndBuffDestroy(editWndDataPtr->hEditWndBuff);
    free( editWndDataPtr );

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
editOnCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    switch( LOWORD(wParam) )
    {
    case IDM_EDIT_CUT:
        rtn = editOnCut( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_COPY:
        rtn = editOnCopy( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_PASTE:
        rtn = editOnPaste( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_DELETE:
        rtn = editOnClear( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_SELECT_ALL:
        rtn = editOnSetSel( hwnd, message, 0, -1 );
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
editOnKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    switch(wParam)
    {
    case VK_SHIFT:
        editWndDataPtr->bShiftKeyOn = FALSE;
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
editOnKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    BOOL bErase = TRUE;
    BOOL bProc = TRUE;
    BOOL bRemoveWithinLine = FALSE;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( wParam == VK_SHIFT )
    {
        editWndDataPtr->bShiftKeyOn = TRUE;
        EditWndBuffSelectOn(editWndDataPtr->hEditWndBuff);
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
            if( editWndDataPtr->bShiftKeyOn )
            {
                EditWndBuffSelectOn(editWndDataPtr->hEditWndBuff);
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
            EditWndBuffDecCaretXpos(editWndDataPtr->hEditWndBuff);
            break;
        case VK_RIGHT:
            bErase = FALSE;
            EditWndBuffIncCaretXpos(editWndDataPtr->hEditWndBuff);
            break;
        case VK_UP:
            bErase = FALSE;
            EditWndBuffDecCaretYpos(editWndDataPtr->hEditWndBuff);
            break;
        case VK_DOWN:
            bErase = FALSE;
            EditWndBuffIncCaretYpos(editWndDataPtr->hEditWndBuff);
            break;
        case VK_DELETE:
            if( editWndDataPtr->csStyle & ES_READONLY )
            {
                bProc = FALSE;
            }
            else
            {
                bRemoveWithinLine = editWndRemoveData( hwnd, FALSE );
            }
            break;
        case VK_BACK:
            if( editWndDataPtr->csStyle & ES_READONLY )
            {
                bProc = FALSE;
            }
            else
            {
                bRemoveWithinLine = editWndRemoveData( hwnd, TRUE );
            }
            break;

        case VK_HOME:
//            DebugWndPrintf("VK_HOME\r\n");
            bErase = FALSE;
            EditWndBuffSetCaretPos(editWndDataPtr->hEditWndBuff,0,EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff));
            break;

        case VK_END :
//            DebugWndPrintf("VK_END\r\n");
            bErase = FALSE;
            EditWndBuffSetCaretPos(editWndDataPtr->hEditWndBuff,EditWndBuffGetColumnMaxSize(editWndDataPtr->hEditWndBuff),EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff));
            break;

        case VK_PRIOR: /* Pg Up */
            bProc = FALSE;
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, (LPARAM)0);
            break;

        case VK_NEXT : /* Pg Dn */
            bProc = FALSE;
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, (LPARAM)0);
            break;

        default:
//            DebugWndPrintf("WM_KEYDOWN:0x%04X\r\n",wParam);
            bProc = FALSE;
            break;
        }

        if( bProc )
        {
            RECT rect;
            BOOL bCaretPosOutScoll;

            if( editWndDataPtr->bShiftKeyOn )
            {
                nop();
            }
            else
            {
                EditWndBuffSelectOff(editWndDataPtr->hEditWndBuff);
            }

            bCaretPosOutScoll = editWndCaretPosOutScroll(hwnd,editWndDataPtr);

            HideCaret(hwnd);

            if( bRemoveWithinLine && !bCaretPosOutScoll )
            {
                editWndGetInvalidateRect(editWndDataPtr,&rect,EDITWND_INVALID_RECT_KIND_1LINE_NOWPOS_AFTER);
                editWndInvalidateRect( hwnd, &rect, bErase, "editOnKeyDown_bRectSelect" );
            }
            else
            {
                editWndInvalidateRect( hwnd, NULL, bErase, "editOnKeyDown" );
            }

            editWndCaretPosUpdate(editWndDataPtr);
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
editOnChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    static TCHAR data[3];
    int i;
    INT size;
    RECT rect;
    BOOL bRectSelect = FALSE;
    BOOL bLiteralFix;
    static BOOL bMultiByteHighExist;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( editWndDataPtr->csStyle & ES_READONLY )
    {
        nop();
    }
    else
    {
        EditWndBuffSelectOff(editWndDataPtr->hEditWndBuff);

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
                size = EditWndBuffGetNewLineData(editWndDataPtr->hEditWndBuff,data);
                break;
            case '\t':  /* tab */
            default:
//                DebugWndPrintf("WM_CHAR:0x%04X\r\n",wParam);
                /* 文字入力 */
                bLiteralFix = (BOOL)TRUE;

                if( bMultiByteHighExist )
                { /* 前文字が2byte文字の上位byteだったら */
                    bMultiByteHighExist = (BOOL)FALSE;
                    data[1] = (TCHAR)wParam;
                    size = 2;
                }
                else
                {
                    if( (TCHAR)wParam == '\0' )
                    {
                        data[0] = (TCHAR)wParam;
                        size = 1;
                    }
                    else if( (TCHAR)wParam == '\t' )
                    { /* TAB */
                        data[0] = (TCHAR)wParam;
                        size = 1;
                    }
                    else if( (BYTE)wParam < (BYTE)0x20 )
                    { /* その他のASCII制御コード */
                        bLiteralFix = (BOOL)FALSE;
                    }
                    else if( ( (BYTE)wParam <= (BYTE)0x80) ||
                             (((BYTE)0xA0 <= (BYTE)wParam) && ((BYTE)wParam <= (BYTE)0xDF)) ||
                             ((BYTE)0xF0 <= (BYTE)wParam) )
                    { /* 1byte文字 */
                        data[0] = (TCHAR)wParam;
                        size = 1;
                    }
                    else
                    {  /* 2byte文字の上位byte */
                        data[0] = (TCHAR)wParam;
                        bMultiByteHighExist = (BOOL)TRUE;
                        bLiteralFix = (BOOL)FALSE;
                    }
                }

                if( bLiteralFix )
                {
                    editWndGetInvalidateRect(editWndDataPtr,&rect,EDITWND_INVALID_RECT_KIND_1LINE_NOWPOS_AFTER);
                    bRectSelect = TRUE;
                }
                else
                {
                    nop();
                }
                break;
            }

            if( size )
            {
                EditWndBuffDataSet( editWndDataPtr->hEditWndBuff, data,size,FALSE, TRUE );
                if( bRectSelect )
                {
                    editWndInvalidateRect( hwnd, &rect, TRUE, "editOnChar_bRectSelect" );
                }
                else
                {
                    editWndInvalidateRect( hwnd, NULL, TRUE, "editOnChar" );
                }
                SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0,EN_CHANGE), (LPARAM)hwnd);
            }
            else
            {
                nop();
            }
        }

        editOnImeStartComposition( hwnd, message, wParam, lParam );

        editWndCaretPosOutScroll(hwnd,editWndDataPtr);
        editWndCaretPosUpdate(editWndDataPtr);
        setAllScrollInfo(hwnd);
    }

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
editOnHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SCROLLINFO si;
    int iHorzPos;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

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
    case SB_THUMBTRACK:
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
        ScrollWindow( hwnd, editWndDataPtr->cxChar * (iHorzPos - si.nPos), 0, NULL, NULL );
        UpdateWindow(hwnd);
    }

    GetScrollInfo( hwnd, SB_HORZ, &si );
    editWndDataPtr->iHorzPos = si.nPos;

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
editOnVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SCROLLINFO si;
    int iVertPos;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

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
    case SB_THUMBPOSITION:
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
        ScrollWindow(hwnd, 0, editWndDataPtr->cyChar * (iVertPos - si.nPos),NULL, NULL);
        UpdateWindow(hwnd);
    }

    GetScrollInfo(hwnd, SB_VERT, &si);
    editWndDataPtr->iVertPos = si.nPos;

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
editOnSetFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    CreateCaret( hwnd,NULL,1,editWndDataPtr->cyChar ); /* 幅=1,高さ=文字サイズ */
    SetCaretPos( EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff) * editWndDataPtr->cxChar, EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff) * editWndDataPtr->cyChar);
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
editOnKillFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
editOnMouseActivate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
editOnMouseWheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

#if 0
    DebugWndPrintf("editOnMouseWheel,%X,%X,%X\r\n",message,wParam,lParam);
#endif

    if( editWndDataPtr->iDeltaPerLine )
    {
        editWndDataPtr->iAccumDelta += (short)HIWORD(wParam);

        while( editWndDataPtr->iAccumDelta >= editWndDataPtr->iDeltaPerLine )
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
            editWndDataPtr->iAccumDelta -= editWndDataPtr->iDeltaPerLine;
        }
        while( editWndDataPtr->iAccumDelta <= -editWndDataPtr->iDeltaPerLine)
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
            editWndDataPtr->iAccumDelta += editWndDataPtr->iDeltaPerLine;
        }
    }
    else
    {
        /* do nothing */
    }

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
editOnMouseHwheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 1; /* チルトホイールを傾け続ける限り繰り返し WM_MOUSEHWHEEL メッセージが飛ぶようにするには1 */
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

//    DebugWndPrintf("editOnMouseHwheel,%X,%X,%X\r\n",message,wParam,lParam);

    if( editWndDataPtr->iDeltaPerChar )
    {
        editWndDataPtr->iHorzAccumDelta += (short)GET_WHEEL_DELTA_WPARAM(wParam);

        while( editWndDataPtr->iHorzAccumDelta >= editWndDataPtr->iDeltaPerChar )
        {
            SendMessage(hwnd, WM_HSCROLL, SB_LINERIGHT, 0);
            editWndDataPtr->iHorzAccumDelta -= editWndDataPtr->iDeltaPerChar;
        }
        while( editWndDataPtr->iHorzAccumDelta <= -editWndDataPtr->iDeltaPerChar)
        {
            SendMessage(hwnd, WM_HSCROLL, SB_LINELEFT, 0);
            editWndDataPtr->iHorzAccumDelta += editWndDataPtr->iDeltaPerChar;
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
editOnMouseMove( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SHORT x,y;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    BOOL bCaretPosChange;

    x = (SHORT)LOWORD(lParam);
    y = (SHORT)HIWORD(lParam);

//    DebugWndPrintf("x=%d,y=%d\r\n",x,y);

    if( (wParam & MK_LBUTTON) )
    {
        DWORD xPos    = max( 0, ((x + (editWndDataPtr->iHorzPos*editWndDataPtr->cxChar))/editWndDataPtr->cxChar) );
        DWORD lineNum = max( 0, ((y + (editWndDataPtr->iVertPos*editWndDataPtr->cyChar))/editWndDataPtr->cyChar) );

//        DebugWndPrintf("xPos=%d,lineNum=%d\r\n",xPos,lineNum);

        bCaretPosChange = EditWndBuffSetCaretPos( editWndDataPtr->hEditWndBuff, xPos, lineNum );
        if( bCaretPosChange )
        {
            editWndCaretPosOutScroll(hwnd,editWndDataPtr);
            editWndCaretPosUpdate(editWndDataPtr);
            editWndInvalidateRect( hwnd, NULL, FALSE, "editOnMouseMove" );
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
 * 内容  : WM_LBUTTONDOWN を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
editOnLbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    int x,y;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    BOOL bSelectOffChange = FALSE;
    BOOL bSelectOnChange = FALSE;
    BOOL bCaretPosChange = FALSE;

    getAllScrollInfo(hwnd);

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    if( wParam & MK_SHIFT )
    {
        nop(); /* WM_KEYDOWN の VK_SHIFT のところで範囲選択ONしているのでここでは処理していない */
    }
    else
    {
        bSelectOffChange = EditWndBuffSelectOff(editWndDataPtr->hEditWndBuff);
    }

    bCaretPosChange = EditWndBuffSetCaretPos( editWndDataPtr->hEditWndBuff, ((x + (editWndDataPtr->iHorzPos*editWndDataPtr->cxChar))/editWndDataPtr->cxChar), ((y + (editWndDataPtr->iVertPos*editWndDataPtr->cyChar))/editWndDataPtr->cyChar) );

    editWndCaretPosUpdate(editWndDataPtr);

    if( wParam & MK_SHIFT )
    {
        nop(); /* WM_KEYDOWN の VK_SHIFT のところで範囲選択ONしているのでここでは処理していない */
    }
    else
    {
        SetCapture(hwnd);
        bSelectOnChange = EditWndBuffSelectOn(editWndDataPtr->hEditWndBuff);
    }

//    DebugWndPrintf("%d,%d,%d\r\n",bSelectOffChange,bSelectOnChange,bCaretPosChange);

    if( bSelectOffChange || (bCaretPosChange && editWndDataPtr->bShiftKeyOn) )
    {
        editWndInvalidateRect( hwnd, NULL, TRUE, "editOnLbuttonDown" );
    }
    else
    {
        nop();
    }

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
editOnMbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
editOnRbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
editOnLbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
editOnMbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
editOnRbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HMENU hMenu;
    POINT point;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    point.x = LOWORD(lParam);
    point.y = HIWORD(lParam);

    ClientToScreen(hwnd,&point);

    hMenu = CreatePopupMenu();
    if( editWndDataPtr->csStyle & ES_READONLY )
    {
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_COPY       , TEXT("コピー(&C)") );
        AppendMenu( hMenu, MF_SEPARATOR, 0                   , NULL );
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("すべて選択(&A)") );

        if( EditWndGetDataSize(hwnd,EDITWND_SELECTED) )
        {
            EnableMenuItem( hMenu, IDM_EDIT_COPY  , MF_ENABLED );
        }
        else
        {
            EnableMenuItem( hMenu, IDM_EDIT_COPY  , MF_GRAYED );
        }
    }
    else
    {
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_UNDO       , TEXT("元に戻す(&U)") );
        AppendMenu( hMenu, MF_SEPARATOR, 0                   , NULL );
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_CUT        , TEXT("切り取り(&T)") );
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_COPY       , TEXT("コピー(&C)") );
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_PASTE      , TEXT("貼り付け(&P)") );
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_DELETE     , TEXT("削除(&D)") );
        AppendMenu( hMenu, MF_SEPARATOR, 0                   , NULL );
        AppendMenu( hMenu, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("すべて選択(&A)") );

        EnableMenuItem( hMenu, IDM_EDIT_UNDO, MF_GRAYED );

        if( EditWndGetDataSize(hwnd,EDITWND_SELECTED) )
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
editOnImeStartComposition( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HIMC hImc;
    COMPOSITIONFORM cf;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    LOGFONT logFont;

    GetObject( editWndDataPtr->hFont, sizeof(LOGFONT), (PTSTR)&(logFont) );

    cf.dwStyle = CFS_POINT;
    cf.ptCurrentPos.x = (EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff) - editWndDataPtr->iHorzPos) * editWndDataPtr->cxChar;
    cf.ptCurrentPos.y = (EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff) - editWndDataPtr->iVertPos) * editWndDataPtr->cyChar;

    hImc = ImmGetContext( hwnd );
    ImmSetCompositionWindow( hImc, &cf );
    ImmSetCompositionFont( hImc,&logFont );
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
editOnCut( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( editWndDataPtr->csStyle & ES_READONLY )
    {
        nop();
    }
    else
    {
        dwSize = EditWndGetDataSize(hwnd,EDITWND_SELECTED);
        if( dwSize )
        {
            hGlobal = GlobalAlloc( GHND|GMEM_SHARE, dwSize+1 );
            pGlobal = GlobalLock( hGlobal );
            GlobalUnlock(hGlobal);
            EditWndBuffDataGet( editWndDataPtr->hEditWndBuff, pGlobal,dwSize,EDITWND_SELECTED );
            OpenClipboard(hwnd);
            EmptyClipboard();
            SetClipboardData( CF_TEXT, hGlobal );
            CloseClipboard();

            /* CUTのみ */
            editWndRemoveData( hwnd,FALSE );
            editWndInvalidateRect( hwnd, NULL, TRUE, "editOnCut" );
        }
        else
        {
            nop();
        }
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
editOnCopy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    dwSize = EditWndGetDataSize(hwnd,EDITWND_SELECTED);
    if( dwSize )
    {
        hGlobal = GlobalAlloc( GHND|GMEM_SHARE, dwSize+1 );
        pGlobal = GlobalLock( hGlobal );
        GlobalUnlock(hGlobal);
        EditWndBuffDataGet( editWndDataPtr->hEditWndBuff, pGlobal,dwSize,EDITWND_SELECTED );
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
editOnPaste( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize,dwSize2;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    PBYTE   dataPtr;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( editWndDataPtr->csStyle & ES_READONLY )
    {
        nop();
    }
    else
    {
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
            EditWndDataSet( hwnd, dataPtr,dwSize,FALSE );
            free( dataPtr );

            editWndCaretPosOutScroll(hwnd,editWndDataPtr);
        }
        else
        {
            nop();
        }
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
editOnClear( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( editWndDataPtr->csStyle & ES_READONLY )
    {
        nop();
    }
    else
    {
        editWndRemoveData( hwnd,FALSE );
        editWndInvalidateRect( hwnd, NULL, TRUE, "editOnClear" );
    }

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
editOnUndo( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( EditWndBuffUndo(editWndDataPtr->hEditWndBuff) )
    {
        editWndCaretPosOutScroll(hwnd,editWndDataPtr);
        editWndInvalidateRect( hwnd, NULL, TRUE, "editOnUndo" );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_REDO を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam (内容はメッセージの種類により異なる)
 * 引数  : LPARAM lParam (内容はメッセージの種類により異なる)
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
editOnRedo( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( EditWndBuffRedo(editWndDataPtr->hEditWndBuff) )
    {
        editWndCaretPosOutScroll(hwnd,editWndDataPtr);
        editWndInvalidateRect( hwnd, NULL, TRUE, "editOnRedo" );
    }
    else
    {
        nop();
    }

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
editOnSetSel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    if( (wParam == 0) && (lParam == -1) )
    { /* ”すべて選択”のみ対応 */
        if( EditWndBuffSelectAll(editWndDataPtr->hEditWndBuff) )
        {
            EditWndBuffSetCaretPos(editWndDataPtr->hEditWndBuff, EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff),EditWndBuffGetLineMaxSize(editWndDataPtr->hEditWndBuff));
            setScrollPos( hwnd, SB_VERT, EditWndBuffGetLineMaxSize(editWndDataPtr->hEditWndBuff) );
            editWndInvalidateRect( hwnd, NULL, TRUE, "editOnSetSel" );
        }
        else
        {
            nop();
        }
    }
    else
    {
        /* 未対応。暫定で、最終位置を選択するようにしておく */
        HideCaret(hwnd);
        EditWndBuffSetCaretPos(editWndDataPtr->hEditWndBuff,0xffffffff,EditWndBuffGetLineMaxSize(editWndDataPtr->hEditWndBuff));
        setScrollPos( hwnd, SB_VERT, EditWndBuffGetLineMaxSize(editWndDataPtr->hEditWndBuff) );
        editWndInvalidateRect( hwnd, NULL, TRUE, "editOnSelSet" );
        editWndCaretPosUpdate(editWndDataPtr);
        ShowCaret(hwnd);
    }

    return rtn;
}

/********************************************************************************
 * 内容  : WM_SETFONT を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam フォントのハンドル
 * 引数  : LPARAM lParam フォント設定後に再描画するかどうか。TRUE で再描画
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
editOnSetFont( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    editWndDataPtr->hFont = (HFONT)wParam;

    updateTextMetrics( hwnd );

    if( lParam )
    {
        editWndInvalidateRect( hwnd, NULL, TRUE, "editOnSetFont" );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * 内容  : EM_GETLINECOUNT を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam
 * 引数  : LPARAM lParam
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
editOnGetLineCount( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    return (LRESULT)((EditWndBuffGetLineMaxSize(editWndDataPtr->hEditWndBuff)) + 1);
}

/********************************************************************************
 * 内容  : EM_LINEFROMCHAR を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam
 * 引数  : LPARAM lParam
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
editOnLineFromChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    DWORD result = (DWORD)0;

    if( (wParam == -1) )
    {
        result = EditWndBuffGetCaretYpos( editWndDataPtr->hEditWndBuff );
    }
    else
    {
        nop();
    }

    return (LRESULT)result;
}

/********************************************************************************
 * 内容  : WM_GOTOLINE を処理する
 * 引数  : HWND hwnd
 * 引数  : UINT message
 * 引数  : WPARAM wParam
 * 引数  : LPARAM lParam
 * 戻り値: LRESULT
 ***************************************/
static LRESULT
editOnGoToLine( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    DWORD result = (DWORD)0;

    EditWndBuffSelectOff(editWndDataPtr->hEditWndBuff);
    EditWndBuffSetCaretPos(editWndDataPtr->hEditWndBuff,0,lParam);

    editWndCaretPosOutScroll(hwnd,editWndDataPtr);

    editWndInvalidateRect( hwnd, NULL, TRUE, "editOnGoToLine" );

    return (LRESULT)result;
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
editOnDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * 内容  : データ削除
 * 引数  : HWND hwnd
 * 引数  : bBackSpace
 * 戻り値: BOOL
 ***************************************/
static BOOL
editWndRemoveData( HWND hwnd, BOOL bBackSpace )
{
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);
    BOOL bRemoveWithinLine = (BOOL)FALSE;
    EDITWND_BUFF_REMOVE_DATA_RESULT removeResult;

    removeResult = EditWndBuffRemoveData( editWndDataPtr->hEditWndBuff, bBackSpace, TRUE );
    if( removeResult == EDITWND_BUFF_REMOVE_DATA_RESULT_WITHIN_LINE )
    {
        bRemoveWithinLine = (BOOL)TRUE;
    }
    else
    {
        nop();
    }
    SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0,EN_CHANGE), (LPARAM)hwnd);

    return bRemoveWithinLine;
}

/********************************************************************************
 * 内容  : キャレット位置の更新
 * 引数  : S_EDITWND_DATA *editWndDataPtr
 * 戻り値: なし
 ***************************************/
static void
editWndCaretPosUpdate( S_EDITWND_DATA *editWndDataPtr )
{
    if( editWndDataPtr != NULL )
    {
        SetCaretPos( (EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff)-editWndDataPtr->iHorzPos)*editWndDataPtr->cxChar, (EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff)-editWndDataPtr->iVertPos)*editWndDataPtr->cyChar);
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : キャレットが表示範囲外に有った場合のスクロール処理
 * 引数  : HWND hwnd
 * 引数  : S_EDITWND_DATA *editWndDataPtr
 * 戻り値: BOOL
 ***************************************/
static BOOL
editWndCaretPosOutScroll( HWND hwnd, S_EDITWND_DATA *editWndDataPtr )
{
    BOOL bRtn = (BOOL)FALSE;

    if( editWndDataPtr != NULL )
    {
        /* キャレットが表示範囲外に有った場合の処理(横方向) */
        if( EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff) < editWndDataPtr->iHorzPos )
        {
            setScrollPos( hwnd, SB_HORZ, EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff) );
            bRtn = (BOOL)TRUE;
        }
        else if( (editWndDataPtr->iHorzPos+editWndDataPtr->cxBuffer-1) < EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff) )
        {
            setScrollPos( hwnd, SB_HORZ, (editWndDataPtr->iHorzPos+editWndDataPtr->cxBuffer-1) );
            bRtn = (BOOL)TRUE;
        }
        else
        {
            nop();
        }

        /* キャレットが表示範囲外に有った場合の処理(縦方向) */
        if( EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff) < editWndDataPtr->iVertPos )
        {
            setScrollPos( hwnd, SB_VERT, EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff) );
            bRtn = (BOOL)TRUE;
        }
        else if( (editWndDataPtr->iVertPos+editWndDataPtr->cyBuffer-1) < EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff) )
        {
            setScrollPos( hwnd, SB_VERT, EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff) - (editWndDataPtr->cyBuffer-1) );
            bRtn = (BOOL)TRUE;
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

    return bRtn;
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
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    hdc = GetDC( hwnd );
    SelectObject( hdc, editWndDataPtr->hFont );

    GetTextMetrics( hdc, &tm );
    editWndDataPtr->cxChar = tm.tmAveCharWidth;
    editWndDataPtr->cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * editWndDataPtr->cxChar / 2;
    editWndDataPtr->cyChar = tm.tmHeight + tm.tmExternalLeading;

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
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    /* Set vertical scroll bar range and page size */
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;                                                                /* 範囲の最小値 */
    si.nMax   = EditWndBuffGetLineMaxSize(editWndDataPtr->hEditWndBuff);
    si.nPage  = (editWndDataPtr->cyClient / editWndDataPtr->cyChar); /* ページサイズ */
    SetScrollInfo( hwnd, SB_VERT, &si, TRUE );

    /* Set horizontal scroll bar range and page size*/
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = max( EditWndBuffGetColumnMaxSize(editWndDataPtr->hEditWndBuff),(editWndDataPtr->cxClient/editWndDataPtr->cxChar))-1;
    si.nPage  = (editWndDataPtr->cxClient/editWndDataPtr->cxChar);
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
    S_EDITWND_DATA *editWndDataPtr = (S_EDITWND_DATA *)(LONG_PTR)GetWindowLongPtr(hwnd,0);

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hwnd, SB_VERT, &si ); /* 縦スクロールバーの位置を取得 */
    editWndDataPtr->iVertPos = si.nPos;

    GetScrollInfo( hwnd, SB_HORZ, &si);   /* 横スクロールバーの位置を取得 */
    editWndDataPtr->iHorzPos = si.nPos;
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
 * 内容  : 矩形無効化
 * 引数  : S_EDITWND_DATA *editWndDataPtr
 * 引数  : RECT *rectPtr
 * 引数  : EDITWND_INVALID_RECT_KIND kind
 * 戻り値: なし
 ***************************************/
static void
editWndGetInvalidateRect( S_EDITWND_DATA *editWndDataPtr, RECT *rectPtr, EDITWND_INVALID_RECT_KIND kind )
{
    if( rectPtr != NULL )
    {
        if( kind == EDITWND_INVALID_RECT_KIND_1LINE_NOWPOS_AFTER )
        {
            /* 今のキャレット位置から X座標：クライアント領域最後、Y座標：1行分 */
            rectPtr->left   = (EditWndBuffGetCaretXpos(editWndDataPtr->hEditWndBuff)-editWndDataPtr->iHorzPos)*editWndDataPtr->cxChar;
            rectPtr->top    = (EditWndBuffGetCaretYpos(editWndDataPtr->hEditWndBuff)-editWndDataPtr->iVertPos)*editWndDataPtr->cyChar;
            rectPtr->right  = editWndDataPtr->cxClient;
            rectPtr->bottom = rectPtr->top + editWndDataPtr->cyChar;
        }
        else
        {
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 矩形無効化
 * 引数  : HWND hWnd
 * 引数  : RECT *rectPtr
 * 引数  : BOOL bErase
 * 引数  : TCHAR *strDebugMsg
 * 戻り値: なし
 ***************************************/
static void
editWndInvalidateRect( HWND hWnd, RECT *rectPtr, BOOL bErase, TCHAR *strDebugMsg )
{
#if 0
    if( rectPtr != NULL )
    {
        DebugInvalidRectPaintRed = 1; /* 直後に無効化した矩形を描画したところを赤にする */
        DebugWndPrintf("InvalidateRect,%s,left:%d,top:%d,right:%d,bottom:%d,%s\r\n",strDebugMsg,rectPtr->left,rectPtr->top,rectPtr->right,rectPtr->bottom,(bErase?"TRUE":"FALSE"));
    }
    else
    {
        DebugWndPrintf("InvalidateRect,%s,NULL,%s\r\n",strDebugMsg,(bErase?"TRUE":"FALSE"));
    }
#endif
    InvalidateRect( hWnd, rectPtr, bErase );
}
