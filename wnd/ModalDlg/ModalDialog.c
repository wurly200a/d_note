/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "ModalDlg.h"

LRESULT CALLBACK AboutDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

/* 内部変数定義 */
static TCHAR szAboutTitleName[64];

typedef struct
{
    PTSTR   className;
    PTSTR   titleName;
    WNDPROC wndPrc   ;
    int     xOffset  ;
    int     yOffset  ;
    int     nWidth   ;
    int     nHeight  ;
} S_MODAL_DLG_INFO;

S_MODAL_DLG_INFO modalDlgInfoTbl[MODAL_DLG_ID_MAX] =
{
    /* className      , titleName             , wndPrc  */
    { TEXT("AboutDlg"), TEXT("バージョン情報"), AboutDlgProc, 25, 25, 487, 327 },
};

/********************************************************************************
 * 内容  : モーダルダイアログボックス初期化
 * 引数  : なし
 * 戻り値: BOOL
 ***************************************/
BOOL
ModalDlgInit( void )
{
    INT i;
    WNDCLASS wc = {0};

    for( i=0; i<MODAL_DLG_ID_MAX; i++ )
    {
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = modalDlgInfoTbl[i].wndPrc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = GetHinst();
        wc.hIcon         = NULL;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = modalDlgInfoTbl[i].className;

        switch( i )
        {
        case MODAL_DLG_ID_ABOUT:
            wsprintf( szAboutTitleName, "%s のバージョン情報",GetAppName() );
            modalDlgInfoTbl[i].titleName = szAboutTitleName;
            break;
        default:
            break;
        }

        RegisterClass(&wc); /* ウインドウクラス登録 */
    }

    return TRUE;
}

/********************************************************************************
 * 内容  : モーダルダイアログボックス
 * 引数  : MODAL_DLG_ID      id
 * 引数  : S_MODAL_DLG_DATA *dataPtr
 * 引数  : HWND              hwnd
 * 引数  : int               x
 * 引数  : int               y
 * 戻り値: BOOL
 ***************************************/
BOOL
ModalDlg( MODAL_DLG_ID id, S_MODAL_DLG_DATA *dataPtr, HWND hwnd, int x, int y )
{
    BOOL rtn = TRUE;
    HWND hDlg;
    MSG msg;

    if( id < MODAL_DLG_ID_MAX )
    {
        /* ダイアログボックス作成 */
        hDlg = CreateWindowEx( WS_EX_DLGMODALFRAME,
                               modalDlgInfoTbl[id].className,modalDlgInfoTbl[id].titleName,
                               WS_VISIBLE | (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU/*|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX */),
                               x+modalDlgInfoTbl[id].xOffset, y+modalDlgInfoTbl[id].yOffset,
                               modalDlgInfoTbl[id].nWidth   , modalDlgInfoTbl[id].nHeight  ,
                               hwnd , NULL, GetHinst(), NULL );

        EnableWindow(hwnd,FALSE); /* メインウインドウを無効化してモーダルに */

        while( GetMessage(&msg,NULL,0,0) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        EnableWindow(hwnd,TRUE); /* メインウインドウを有効にしてモーダル解除 */
        BringWindowToTop(hwnd);
    }
    else
    {
        rtn = FALSE;
    }

    return rtn;
}

LRESULT CALLBACK
AboutDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HWND hBtn;

    switch( message )
    {
    case WM_CREATE:
        hBtn = CreateWindow( TEXT("Button"), TEXT("OK"), WS_CHILD|WS_VISIBLE, 382,270,90,19, hwnd, (HMENU)0, GetHinst(), NULL );
        SendMessage( hBtn, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
        SetFocus( hBtn );
        break;
    case WM_DESTROY:
        PostQuitMessage(0); /* WM_QUITメッセージをポストする */
        break;
    case WM_COMMAND:
        switch( LOWORD(wParam) )
        {
        case 0:
            DestroyWindow( hwnd );
            break;
        defaut:
            break;
        }
        break;
    default:
        rtn = DefWindowProc( hwnd, message, wParam, lParam );
    }

    return rtn;
}
