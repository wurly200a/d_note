#ifndef IOWND_H

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
/*                                                                              */
/*                                                                              */
/* [未対応]                                                                     */
/* WM_SETFONT                                                                   */
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
 * 内容  : IOウィンドウクラスの登録、ウィンドウの生成
 * 引数  : HINSTANCE hInst
 * 引数  : HWND hWndParent 親ウィンドウのハンドル
 * 戻り値: HWND
 ***************************************/
HWND IoWndCreate( HINSTANCE hInst, HWND hWndParent );

/********************************************************************************
 * 内容  : IOウィンドウのフォント変更
 * 引数  : HWND hwnd
 * 引数  : HFONT hFont
 * 戻り値: なし
 ***************************************/
void IoWndChangeFont( HWND hwnd, HFONT hFont );

/********************************************************************************
 * 内容  : IOウィンドウの初期化
 * 引数  : HWND hwnd
 * 戻り値: なし
 ***************************************/
void IoWndDataInit( HWND hwnd );

/********************************************************************************
 * 内容  : IOウィンドウのデータセット
 * 引数  : HWND hwnd
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void IoWndDataSet( HWND hwnd, TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    IOWND_ALL,
    IOWND_SELECTED
} IOWND_REGION;

/********************************************************************************
 * 内容  : IOウィンドウのデータ取得
 * 引数  : HWND hwnd
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : IOWND_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndDataGet( HWND hwnd, TCHAR *dataPtr, DWORD dataSize, IOWND_REGION region );

/********************************************************************************
 * 内容  : IOウィンドウのデータサイズ取得
 * 引数  : HWND hwnd
 * 引数  : IOWND_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetDataSize( HWND hwnd, IOWND_REGION region );

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大行サイズ取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetLineMaxSize( HWND hwnd );

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大文字サイズ取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetColumnMaxSize( HWND hwnd );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetCaretXpos( HWND hwnd );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置取得
 * 引数  : HWND hwnd
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetCaretYpos( HWND hwnd );

enum
{
    NEWLINECODE_CRLF = 0,
    NEWLINECODE_LF   ,
    NEWLINECODE_CR   ,
    NEWLINECODE_NONE ,
};
typedef UINT NEWLINECODE_TYPE;
/********************************************************************************
 * 内容  : IOウィンドウの改行コードセット
 * 引数  : HWND hwnd
 * 引数  : NEWLINECODE_TYPE newLineCodeType
 * 戻り値: BOOL (TRUE:表示書き換え必要)
 ***************************************/
BOOL IoWndNewLineCodeSet( HWND hwnd, NEWLINECODE_TYPE newLineCodeType );

#define IOWND_H
#endif /* IOWND_H */
