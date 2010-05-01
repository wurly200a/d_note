#ifndef IOWND_BUFFER_H

typedef struct tagS_IOWND_BUFF
{
    int unused;
} S_IOWND_BUFF;
typedef S_IOWND_BUFF * H_IOWND_BUFF;

typedef struct tagS_IOWND_BUFF_LINE
{
    int unused;
} S_IOWND_BUFF_LINE;
typedef S_IOWND_BUFF_LINE * H_IOWND_BUFF_LINE;

/********************************************************************************
 * 内容  : IOウィンドウバッファのオブジェクト生成
 * 引数  : なし
 * 戻り値: H_IOWND_BUFF
 ***************************************/
H_IOWND_BUFF IoWndBuffCreate( void );

/********************************************************************************
 * 内容  : IOウィンドウバッファのオブジェクト破棄
 * 引数  : H_IOWND_BUFF
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndBuffDestroy( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファの初期化
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffInit( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファの終了
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffEnd( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータセット
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit  (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void IoWndBuffDataSet( H_IOWND_BUFF hIoWndBuff, TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    BUFF_ALL,
    BUFF_SELECTED
} IOWND_BUFF_REGION;

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータ取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndBuffDataGet( H_IOWND_BUFF hIoWndBuff, TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region );

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndBuffGetDataSize( H_IOWND_BUFF hIoWndBuff, IOWND_BUFF_REGION region );

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大行サイズ取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndBuffGetLineMaxSize( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大文字サイズ取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndBuffGetColumnMaxSize( H_IOWND_BUFF hIoWndBuff);

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndBuffGetCaretXpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD IoWndBuffGetCaretYpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレット位置セット
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : DWORD xPos
 * 引数  : DWORD lineNum
 * 戻り値: なし
 ***************************************/
void IoWndBuffSetCaretPos( H_IOWND_BUFF hIoWndBuff, DWORD xPos, DWORD lineNum );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置加算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffIncCaretXpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置減算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffDecCaretXpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置加算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffIncCaretYpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置減算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffDecCaretYpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : 指定行データのハンドル取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : DWORD lineNum
 * 戻り値: H_IOWND_BUFF_LINE
 ***************************************/
H_IOWND_BUFF_LINE IoWndBuffGetLinePtr( H_IOWND_BUFF hIoWndBuff, DWORD lineNum );

/********************************************************************************
 * 内容  : 次の行データのハンドル取得
 * 引数  : H_IOWND_BUFF_LINE hLineData
 * 戻り値: H_IOWND_BUFF_LINE
 ***************************************/
H_IOWND_BUFF_LINE IoWndBuffGetLineNextPtr( H_IOWND_BUFF_LINE hLineData );

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
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : H_IOWND_BUFF_LINE hLineData
 * 引数  : DWORD             dispPos     表示位置
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndBuffGetDispData( H_IOWND_BUFF hIoWndBuff, H_IOWND_BUFF_LINE hLineData, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );

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
void IoWndBuffSetNewLineCode( H_IOWND_BUFF hIoWndBuff, UINT newLineType );

/********************************************************************************
 * 内容  : データ削除
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : BOOL bBackSpace
 * 戻り値: なし
 ***************************************/
void IoWndBuffRemoveData( H_IOWND_BUFF hIoWndBuff, BOOL bBackSpace );

/********************************************************************************
 * 内容  : 改行データ取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : PTSTR dataPtr
 * 戻り値: INT
 ***************************************/
INT IoWndBuffGetNewLineData( H_IOWND_BUFF hIoWndBuff, PTSTR dataPtr );

/********************************************************************************
 * 内容  : IOウィンドウバッファのタブサイズセット
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : INT tabSize
 * 戻り値: BOOL (TRUE:変更された)
 ***************************************/
BOOL IoWndBuffSetTabSize( H_IOWND_BUFF hIoWndBuff, INT tabSize );

/********************************************************************************
 * 内容  : IOウィンドウバッファの範囲選択ON
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffSelectOn( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファの範囲選択OFF
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void IoWndBuffSelectOff( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * 内容  : IOウィンドウバッファの全範囲選択
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: BOOL
 ***************************************/
BOOL IoWndBuffSelectAll( H_IOWND_BUFF hIoWndBuff );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
