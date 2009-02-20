#ifndef IOWND_BUFFER_H

typedef struct tag_buffer_line_data
{
    struct tag_buffer_line_data *prevPtr;
    struct tag_buffer_line_data *nextPtr;
    DWORD                       dataSize;
    INT                         newLineCodeSize;
    TCHAR                       data[];
} S_BUFF_LINE_DATA;

/********************************************************************************
 * 内容  : IOウィンドウバッファの初期化
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndBuffInit( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファ(連結リスト)へのデータ追加
 * 引数  : TCHAR* strPtr
 * 引数  : DWORD  length
 * 戻り値: なし
 ***************************************/
void IoWndBuffSetLinkedList( TCHAR* strPtr, DWORD length );

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetBuffSize( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大行サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetLineMaxSize( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大文字サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetColumnMaxSize( void );

/********************************************************************************
 * 内容  : 指定行のデータアドレス取得
 * 引数  : DWORD lineNum
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffGetLinePtr( DWORD lineNum );

#define IOWND_BUFF_NEWLINE_CRLF 0
#define IOWND_BUFF_NEWLINE_LF   1
#define IOWND_BUFF_NEWLINE_CR   2
#define IOWND_BUFF_NEWLINE_NONE 3
/********************************************************************************
 * 内容  : IOウィンドウバッファの改行コードセット
 * 引数  : UINT newLineType
 * 戻り値: なし
 ***************************************/
void IoWndBuffSetNewLineCode( UINT newLineType );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
