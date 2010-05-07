#ifndef EDITWND_H

/* [対応メッセージ] */
/*                                                                              */
/*                                                                              */
/*                                                                              */
/*                                                                              */
/* WM_CUT                                                                       */
/* WM_COPY                                                                      */
/* WM_PASTE                                                                     */
/* WM_CLEAR                                                                     */
/*                                                                              */
/* EM_SETSEL                                                                    */
/* WM_SETFONT                                                                   */
/*                                                                              */
/*                                                                              */
/* [未対応]                                                                     */
/*                                                                              */
/*                                                                              */
/* [互換性の無いもの]                                                           */
/*                                                                              */
/* GetWindowTextLength(HWND hWnd)                                               */
/* GetWindowText(HWND hWnd,LPTSTR lpString,int nMaxCount)                       */
/* SetWindowText(HWND hWnd,LPCTSTR lpString)                                    */
/*                                                                              */
/*                                                                              */
/*                                                                              */
/*                                                                              */

/********************************************************************************
 * 内容  : EDITウィンドウクラスの登録
 * 引数  : HINSTANCE hInst
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndRegisterClass( HINSTANCE hInst );

/********************************************************************************
 * 内容  : EDITウィンドウの初期化
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
void EditWndDataInit( HWND hwnd );

/********************************************************************************
 * 内容  : EDITウィンドウのデータセット
 * 引数  : HWND hwnd
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void EditWndDataSet( HWND hwnd, TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    EDITWND_ALL,
    EDITWND_SELECTED
} EDITWND_REGION;

/********************************************************************************
 * 内容  : EDITウィンドウのデータ取得
 * 引数  : HWND hwnd
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : EDITWND_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndDataGet( HWND hwnd, TCHAR *dataPtr, DWORD dataSize, EDITWND_REGION region );

/********************************************************************************
 * 内容  : EDITウィンドウのデータサイズ取得
 * 引数  : HWND hwnd
 * 引数  : EDITWND_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndGetDataSize( HWND hwnd, EDITWND_REGION region );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの最大行サイズ取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndGetLineMaxSize( HWND hwnd );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの最大文字サイズ取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndGetColumnMaxSize( HWND hwnd );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットX位置取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndGetCaretXpos( HWND hwnd );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットY位置取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndGetCaretYpos( HWND hwnd );

enum
{
    NEWLINECODE_CRLF = 0,
    NEWLINECODE_LF   ,
    NEWLINECODE_CR   ,
    NEWLINECODE_NONE ,
};
typedef UINT NEWLINECODE_TYPE;
/********************************************************************************
 * 内容  : EDITウィンドウの改行コードセット
 * 引数  : HWND hwnd
 * 引数  : NEWLINECODE_TYPE newLineCodeType
 * 戻り値: BOOL (TRUE:表示書き換え必要)
 ***************************************/
BOOL EditWndNewLineCodeSet( HWND hwnd, NEWLINECODE_TYPE newLineCodeType );

#define EDITWND_H
#endif /* EDITWND_H */
