/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include <winnls.h>

/* 外部関数定義 */
#include "Version.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "ModalDlg.h"

BOOL CALLBACK AboutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK GoToLineDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

/* 内部変数定義 */

typedef struct
{
    HINSTANCE hInstance;
    PTSTR     szAppName;
} S_MODAL_DLG__DATA;

static S_MODAL_DLG__DATA modalDlgData;

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
    /* className         , titleName             , wndPrc         , x , y , w  , h*/
    { TEXT("ABOUTDLG")   , TEXT("バージョン情報"), AboutDlgProc   , 25, 25, 487, 327 },
    { TEXT("GOTOLINEDLG"), TEXT("行へ移動")      , GoToLineDlgProc, 25, 25, 297, 147 },
};

/********************************************************************************
 * 内容  : モーダルダイアログボックス初期化
 * 引数  : HINSTANCE hInst
 * 引数  : PTSTR szAppName
 * 戻り値: BOOL
 ***************************************/
BOOL
ModalDlgInit( HINSTANCE hInst, PTSTR szAppName )
{
    INT i;
    WNDCLASS wc = {0};

    modalDlgData.hInstance = hInst;
    modalDlgData.szAppName = szAppName;

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
        DialogBox(modalDlgData.hInstance, modalDlgInfoTbl[id].className, hwnd, (DLGPROC)modalDlgInfoTbl[id].wndPrc);
    }
    else
    {
        rtn = FALSE;
    }

    return rtn;
}

/********************************************************************************
 * 内容  : バージョン情報のモーダルダイアログボックス
 * 引数  : HWND   hDlg
 * 引数  : UINT   message
 * 引数  : WPARAM wParam
 * 引数  : LPARAM lParam
 * 戻り値: BOOL
 ***************************************/
BOOL CALLBACK
AboutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    BOOL rtn = FALSE;
    HWND hCtrl;
    MEMORYSTATUSEX memSts;
    TCHAR szTemp[256];
    TCHAR szTemp2[256];
    TCHAR szTemp3[256];
    INT x,y;
#if 0
    static HBITMAP hBitmap;
#endif
    static HICON hIcon;
    HINSTANCE hInst = modalDlgData.hInstance;

    NUMBERFMT numberFormat = {0};
    numberFormat.NumDigits = 0;
    numberFormat.LeadingZero = 1;
    numberFormat.Grouping = 3;
    numberFormat.lpDecimalSep = ".";
    numberFormat.lpThousandSep = ",";
    numberFormat.NegativeOrder = 1;

#if 0
    DebugWndPrintf("message=:0x%04X\r\n",message);
#endif

    switch( message )
    {
    case WM_INITDIALOG:
#if 0
        /* 白矩形 */
        CreateWindow(TEXT("static"),TEXT(""),WS_CHILD|WS_VISIBLE|SS_WHITERECT,0,0,481,90,hDlg,(HMENU)-1,hInst,NULL);
#endif
#if 0
        /* bar画像 */
        hCtrl = CreateWindow( TEXT("Static"), TEXT(""), WS_CHILD|WS_VISIBLE|SS_BITMAP,0,0,481,90, hDlg, (HMENU)-1, hInst, NULL);
        hBitmap = (HBITMAP)LoadImage(hInst, TEXT("BAR"), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR );
        SendMessage(hCtrl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
#endif
        /* アイコン画像 */
        hCtrl = CreateWindow( TEXT("Static"), TEXT(""), WS_CHILD|WS_VISIBLE|SS_ICON,55,60,0,0, hDlg, (HMENU)-1, hInst, NULL);
        hIcon = (HICON)LoadImage(hInst, modalDlgData.szAppName, IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );
        SendMessage(hCtrl, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

        x = 60;
        y = 100;
        /* アプリケーション名 */
        hCtrl = CreateWindow( TEXT("static"), modalDlgData.szAppName, WS_CHILD|WS_VISIBLE, x, y, 400,19, hDlg, (HMENU)-1, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );

#if 0
        /* アプリケーション名(補足) */
        hCtrl = CreateWindow( TEXT("static"), TEXT("(Text EDitor for Developers)"), WS_CHILD|WS_VISIBLE, x+35, y, 400,19, hDlg, (HMENU)-1, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
#endif

        /* バージョン */
        y += 15;
        wsprintf( szTemp, TEXT("Version %s"),VersionStringGet() );
        hCtrl = CreateWindow( TEXT("static"), szTemp, WS_CHILD|WS_VISIBLE, x, y, 400,19, hDlg, (HMENU)-1, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );

        /* Copyright */
        y += 15;
        hCtrl = CreateWindow( TEXT("static"), TEXT("Copyright (C) 2009-2017 Wurly"), WS_CHILD|WS_VISIBLE, x, y, 400,19, hDlg, (HMENU)-1, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );

        /* ライセンス */
        y = 180;
        hCtrl = CreateWindow( TEXT("static"), TEXT("この製品はフリーソフトウェアです。"), WS_CHILD|WS_VISIBLE, x, y, 400,19, hDlg, (HMENU)-1, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );

        /* 線 */
        hCtrl = CreateWindow( TEXT("static"), TEXT(""), WS_CHILD|WS_VISIBLE|SS_SUNKEN, 60, 242,410,2, hDlg, (HMENU)-1, hInst, NULL );

        /* 物理メモリサイズ */
        memSts.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx( &memSts );
        wsprintf( szTemp3, TEXT("%ld"),(memSts.ullTotalPhys/1024) );
        GetNumberFormat( LOCALE_SYSTEM_DEFAULT, 0, szTemp3, &numberFormat, szTemp2, 256 );
        wsprintf( szTemp, TEXT("Windows が使用できる物理メモリ:\t        %s KB"),szTemp2 );
        hCtrl = CreateWindow( TEXT("static"), szTemp, WS_CHILD|WS_VISIBLE, 60, 252,400,19, hDlg, (HMENU)-1, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
        break;

    case WM_COMMAND:
        switch( LOWORD(wParam) )
        {
        case IDOK:
            EndDialog(hDlg,TRUE);
            rtn = TRUE;
            break;
        case IDCANCEL:
            EndDialog(hDlg,FALSE);
            rtn = TRUE;
            break;
        default:
            nop();
            break;
        }
        break;
    default:
        nop();
        break;
    }

    return rtn;
}

/********************************************************************************
 * 内容  : 「行へ移動」のモーダルダイアログボックス
 * 引数  : HWND   hDlg
 * 引数  : UINT   message
 * 引数  : WPARAM wParam
 * 引数  : LPARAM lParam
 * 戻り値: BOOL
 ***************************************/
BOOL CALLBACK
GoToLineDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    BOOL rtn = FALSE;
    HWND hCtrl;
    HINSTANCE hInst = modalDlgData.hInstance;

    switch( message )
    {
    case WM_INITDIALOG:
        /* 文字列 */
        hCtrl = CreateWindow( TEXT("static"), TEXT("行番号(L):"), WS_CHILD|WS_VISIBLE, 10, 10, 100,20, hDlg, (HMENU)-1, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );

        /* テキストボックス */
        hCtrl = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW|WS_EX_CONTROLPARENT, TEXT("edit"), TEXT(""), WS_CHILD|WS_VISIBLE, 10, 35, 270,25, hDlg, (HMENU)2, hInst, NULL );
        SendMessage( hCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
        break;

    case WM_COMMAND:
        switch( LOWORD(wParam) )
        {
        case IDOK:
            EndDialog(hDlg,TRUE);
            rtn = TRUE;
            break;
        case IDCANCEL:
            EndDialog(hDlg,FALSE);
            rtn = TRUE;
            break;
        default:
            nop();
            break;
        }
        break;

    default:
        nop();
        break;
    }

    return rtn;
}
