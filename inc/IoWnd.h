#ifndef IOWND_H

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
 * 引数  : LOGFONT *logFontPtr
 * 戻り値: なし
 ***************************************/
void IoWndChangeFont( HWND hwnd, LOGFONT *logFontPtr );

/********************************************************************************
 * 内容  : IOウィンドウのサイズ変更
 * 引数  : HWND hwnd
 * 引数  : int x
 * 引数  : int y
 * 引数  : int cxClient
 * 引数  : int cyClient
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndSize( HWND hwnd, int x, int y, int cxClient, int cyClient );

/********************************************************************************
 * 内容  : IOウィンドウの破壊
 * 引数  : HWND hwnd
 * 引数  : int nCmdShow
 * 戻り値: BOOL
 ***************************************/
void IoWndDestroy( HWND hwnd );

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
