#ifndef EDITWND_BUFFER_H

typedef struct tagS_EDITWND_BUFF
{
    int unused;
} S_EDITWND_BUFF;
typedef S_EDITWND_BUFF * H_EDITWND_BUFF;

typedef struct tagS_EDITWND_BUFF_LINE
{
    int unused;
} S_EDITWND_BUFF_LINE;
typedef S_EDITWND_BUFF_LINE * H_EDITWND_BUFF_LINE;

/********************************************************************************
 * 内容  : EDITウィンドウバッファのオブジェクト生成
 * 引数  : なし
 * 戻り値: H_EDITWND_BUFF
 ***************************************/
H_EDITWND_BUFF EditWndBuffCreate( void );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのオブジェクト破棄
 * 引数  : H_EDITWND_BUFF
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndBuffDestroy( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの初期化
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: なし
 ***************************************/
void EditWndBuffInit( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの終了
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: なし
 ***************************************/
void EditWndBuffEnd( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのデータセット
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit  (TRUE:既存データをクリア,FALSE:クリアしない)
 * 引数  : BOOL   bUndoEnable
 * 戻り値: なし
 ***************************************/
void EditWndBuffDataSet( H_EDITWND_BUFF hEditWndBuff, TCHAR* dataPtr, DWORD length, BOOL bInit, BOOL bUndoEnable );

typedef enum
{
    BUFF_ALL,
    BUFF_SELECTED
} EDITWND_BUFF_REGION;

/********************************************************************************
 * 内容  : EDITウィンドウバッファのデータ取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : EDITWND_BUFF_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndBuffDataGet( H_EDITWND_BUFF hEditWndBuff, TCHAR *dataPtr, DWORD dataSize, EDITWND_BUFF_REGION region );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのデータサイズ取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : EDITWND_BUFF_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndBuffGetDataSize( H_EDITWND_BUFF hEditWndBuff, EDITWND_BUFF_REGION region );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの最大行サイズ取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndBuffGetLineMaxSize( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの最大文字サイズ取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndBuffGetColumnMaxSize( H_EDITWND_BUFF hEditWndBuff);

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットX位置取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndBuffGetCaretXpos( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットY位置取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD EditWndBuffGetCaretYpos( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレット位置セット
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : DWORD xPos
 * 引数  : DWORD lineNum
 * 戻り値: BOOL (TRUE:キャレット位置が変わった,FALSE:変わらない)
 ***************************************/
BOOL EditWndBuffSetCaretPos( H_EDITWND_BUFF hEditWndBuff, DWORD xPos, DWORD lineNum );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットX位置加算
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: なし
 ***************************************/
void EditWndBuffIncCaretXpos( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットX位置減算
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: なし
 ***************************************/
void EditWndBuffDecCaretXpos( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットY位置加算
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: なし
 ***************************************/
void EditWndBuffIncCaretYpos( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのキャレットY位置減算
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: なし
 ***************************************/
void EditWndBuffDecCaretYpos( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : 指定行データのハンドル取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : DWORD lineNum
 * 戻り値: H_EDITWND_BUFF_LINE
 ***************************************/
H_EDITWND_BUFF_LINE EditWndBuffGetLinePtr( H_EDITWND_BUFF hEditWndBuff, DWORD lineNum );

/********************************************************************************
 * 内容  : 次の行データのハンドル取得
 * 引数  : H_EDITWND_BUFF_LINE hLineData
 * 戻り値: H_EDITWND_BUFF_LINE
 ***************************************/
H_EDITWND_BUFF_LINE EditWndBuffGetLineNextPtr( H_EDITWND_BUFF_LINE hLineData );

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
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : H_EDITWND_BUFF_LINE hLineData
 * 引数  : DWORD             dispPos     表示位置
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndBuffGetDispData( H_EDITWND_BUFF hEditWndBuff, H_EDITWND_BUFF_LINE hLineData, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );

enum
{
    EDITWND_BUFF_NEWLINE_CRLF= 0,
    EDITWND_BUFF_NEWLINE_LF  ,
    EDITWND_BUFF_NEWLINE_CR  ,
    EDITWND_BUFF_NEWLINE_NONE,
};
/********************************************************************************
 * 内容  : EDITウィンドウバッファの改行コードセット
 * 引数  : UINT newLineType
 * 戻り値: なし
 ***************************************/
void EditWndBuffSetNewLineCode( H_EDITWND_BUFF hEditWndBuff, UINT newLineType );

enum
{
    EDITWND_BUFF_REMOVE_DATA_RESULT_WITHIN_LINE,
    EDITWND_BUFF_REMOVE_DATA_RESULT_OTHER      ,
};
typedef INT EDITWND_BUFF_REMOVE_DATA_RESULT;
/********************************************************************************
 * 内容  : データ削除
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : BOOL bBackSpace
 * 引数  : BOOL bUndoEnable
 * 戻り値: EDITWND_BUFF_REMOVE_DATA_RESULT
 ***************************************/
EDITWND_BUFF_REMOVE_DATA_RESULT EditWndBuffRemoveData( H_EDITWND_BUFF hEditWndBuff, BOOL bBackSpace, BOOL bUndoEnable );

/********************************************************************************
 * 内容  : 改行データ取得
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : PTSTR dataPtr
 * 戻り値: INT
 ***************************************/
INT EditWndBuffGetNewLineData( H_EDITWND_BUFF hEditWndBuff, PTSTR dataPtr );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのタブサイズセット
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : INT tabSize
 * 戻り値: BOOL (TRUE:変更された)
 ***************************************/
BOOL EditWndBuffSetTabSize( H_EDITWND_BUFF hEditWndBuff, INT tabSize );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの範囲選択ON
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: BOOL (TRUE:変更された)
 ***************************************/
BOOL EditWndBuffSelectOn( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの範囲選択OFF
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: BOOL (TRUE: 選択範囲が解除された,FALSE: 選択範囲無し)
 ***************************************/
BOOL EditWndBuffSelectOff( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの全範囲選択
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndBuffSelectAll( H_EDITWND_BUFF hEditWndBuff );

/********************************************************************************
 * 内容  : EDITウィンドウバッファの検索データセット
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL bDirectionUp
 * 引数  : BOOL bMatchCase
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndBuffFindDataSet( H_EDITWND_BUFF hEditWndBuff, TCHAR* dataPtr, DWORD length, BOOL bDirectionUp, BOOL bMatchCase );

/********************************************************************************
 * 内容  : EDITウィンドウバッファのUndo
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndBuffUndo( H_EDITWND_BUFF hEditWndBuff);

/********************************************************************************
 * 内容  : EDITウィンドウバッファのRedo
 * 引数  : H_EDITWND_BUFF hEditWndBuff
 * 戻り値: BOOL
 ***************************************/
BOOL EditWndBuffRedo( H_EDITWND_BUFF hEditWndBuff);

#define EDITWND_BUFFER_H
#endif /* EDITWND_BUFFER_H */
