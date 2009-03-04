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
 * 内容  : IOウィンドウバッファのデータセット
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 戻り値: なし
 ***************************************/
void IoWndBuffDataSet( TCHAR* dataPtr, DWORD length );

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

enum
{
    IOWND_BUFF_NEWLINE_CRLF= 0,
    IOWND_BUFF_NEWLINE_LF  ,
    IOWND_BUFF_NEWLINE_CR  ,
    IOWND_BUFF_NEWLINE_NONE,
};
/********************************************************************************
 * 内容  : IOウィンドウバッファの改行コードセット
 * 引数  : UINT newLineType
 * 戻り値: BOOL (TRUE:データが変更された)
 ***************************************/
BOOL IoWndBuffSetNewLineCode( UINT newLineType );

/********************************************************************************
 * 内容  : 指定行へのデータ追加
 * 引数  : DWORD lineNum
 * 引数  : DWORD addPos
 * 引数  : TCHAR data
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffAddData( DWORD lineNum, DWORD addPos, TCHAR data );

/********************************************************************************
 * 内容  : 指定行への改行追加
 * 引数  : DWORD lineNum
 * 引数  : DWORD addPos
 * 引数  : TCHAR data
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffAddNewLine( DWORD lineNum, DWORD addPos );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
