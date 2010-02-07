#ifndef IOWND_BUFFER_H

typedef struct tag_buffer_line_data
{
    S_LIST_HEADER header         ;
    DWORD         lineNum        ; /* Y位置            */
    DWORD         caretDataPos   ; /* X位置            */
    DWORD         dataSize       ; /* データサイズ     */
    INT           newLineCodeSize; /* 改行コードサイズ */
    TCHAR         data[]         ;
} S_BUFF_LINE_DATA;

/********************************************************************************
 * 内容  : IOウィンドウバッファの初期化
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndBuffInit( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファの終了
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndBuffEnd( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータセット
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit  (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void IoWndBuffDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    BUFF_ALL,
    BUFF_SELECTED
} IOWND_BUFF_REGION;

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータ取得
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndBuffDataGet( TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region );

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetBuffSize( IOWND_BUFF_REGION region );

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
 * 内容  : IOウィンドウバッファのキャレットX位置取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetCaretXpos( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndGetCaretYpos( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレット位置セット
 * 引数  : DWORD xPos
 * 引数  : DWORD lineNum
 * 戻り値: なし
 ***************************************/
void IoWndSetCaretPos( DWORD xPos, DWORD lineNum );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置加算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndIncCaretXpos( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置減算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndDecCaretXpos( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置加算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndIncCaretYpos( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置減算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndDecCaretYpos( void );

/********************************************************************************
 * 内容  : 指定行のデータアドレス取得
 * 引数  : DWORD lineNum
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffGetLinePtr( DWORD lineNum );

enum
{
    SINGLE_CHAR     ,
    DOUBLE_CHAR_HIGH,
    DOUBLE_CHAR_LOW ,
    TAB_CHAR        ,
    END_CHAR        ,
};
typedef int CHARSET_TYPE;

typedef struct
{
    TCHAR        data[8] ; /* データ(半角,全角,タブ)                   */
    INT          offset  ; /* 半角以外の場合、先頭からのオフセット位置 */
    INT          size    ; /* データサイズ                             */
    CHARSET_TYPE type    ;
    BOOL         bSelect ; /* 選択されているか否か                     */
    DWORD        dataPos ; /* データ位置                               */
    DWORD        caretPos; /* キャレット位置(区切りのいい位置)         */
} S_BUFF_DISP_DATA;

/********************************************************************************
 * 内容  : 指定行、指定列のデータを取得
 * 引数  : S_BUFF_LINE_DATA *lineDataPtr
 * 引数  : DWORD             dispPos     表示位置
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndBuffGetDispData( S_BUFF_LINE_DATA *lineDataPtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );

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
 * 戻り値: なし
 ***************************************/
void IoWndBuffSetNewLineCode( UINT newLineType );

/********************************************************************************
 * 内容  : データ削除
 * 引数  : BOOL bBackSpace
 * 戻り値: なし
 ***************************************/
void IoWndBuffRemoveData( BOOL bBackSpace );

/********************************************************************************
 * 内容  : 改行データ取得
 * 引数  : PTSTR *dataPtr
 * 戻り値: INT
 ***************************************/
INT IoWndBuffGetNewLineData( PTSTR dataPtr );

/********************************************************************************
 * 内容  : IOウィンドウバッファのタブサイズセット
 * 引数  : INT tabSize
 * 戻り値: BOOL (TRUE:変更された)
 ***************************************/
BOOL IoWndBuffSetTabSize( INT tabSize );

/********************************************************************************
 * 内容  : IOウィンドウバッファの範囲選択ON
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndBuffSelectOn( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファの範囲選択OFF
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void IoWndBuffSelectOff( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファの全範囲選択
 * 引数  : なし
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndBuffSelectAll( void );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
