#ifndef IOWND_H

/********************************************************************************
 * 内容  : IOウィンドウクラスの登録、ウィンドウの生成
 * 引数  : HWND hWnd
 * 引数  : LOGFONT *logFontPtr
 * 戻り値: HWND
 ***************************************/
HWND IoWndCreate( HWND hWnd, LOGFONT *logFontPtr );

/********************************************************************************
 * 内容  : IOウィンドウのフォント変更
 * 引数  : LOGFONT *logFontPtr
 * 戻り値: なし
 ***************************************/
void IoWndChangeFont( LOGFONT *logFontPtr );

/********************************************************************************
 * 内容  : IOウィンドウのサイズ変更
 * 引数  : int x
 * 引数  : int y
 * 引数  : int cxClient
 * 引数  : int cyClient
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndSize( int x, int y, int cxClient, int cyClient );

/********************************************************************************
 * 内容  : IOウィンドウの破壊
 * 引数  : int nCmdShow
 * 戻り値: BOOL
 ***************************************/
void IoWndDestroy( void );

/********************************************************************************
 * 内容  : IOウィンドウの初期化
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndDataInit( void );

/********************************************************************************
 * 内容  : IOウィンドウのデータセット
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit
 * 戻り値: なし
 ***************************************/
void IoWndDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit );

/********************************************************************************
 * 内容  : IOウィンドウのデータ取得
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndDataGet( TCHAR *dataPtr, DWORD dataSize );

/********************************************************************************
 * 内容  : IOウィンドウのデータサイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetDataSize( void );

/********************************************************************************
 * 内容  : IOウィンドウの矩形無効化
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndInvalidateRect( void );

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
 * 引数  : NEWLINECODE_TYPE newLineCodeType
 * 戻り値: BOOL (TRUE:データが変更された)
 ***************************************/
BOOL IoWndNewLineCodeSet( NEWLINECODE_TYPE newLineCodeType );

#define IOWND_H
#endif /* IOWND_H */
