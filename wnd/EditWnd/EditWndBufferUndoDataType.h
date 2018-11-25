#ifndef EDITWND_BUFFER_UNDO_DATA_TYPE_H

typedef enum
{
    UNDO_TYPE_NONE  ,
    UNDO_TYPE_SET   ,
    UNDO_TYPE_REMOVE,
    UNDO_TYPE_JOIN  ,
} UNDO_TYPE;

typedef struct S_BUFF_UNDO_POSITION
{
    DWORD lineNum ;
    DWORD caretPos;
} S_BUFF_UNDO_POSITION;

typedef struct tagS_BUFF_UNDO_DATA
{
    S_LIST_HEADER header         ;
    UNDO_TYPE     undoType       ;
    TCHAR         *dataPtr       ;
    DWORD         size           ;
    S_BUFF_UNDO_POSITION prePos  ;
    S_BUFF_UNDO_POSITION postPos ;
} S_BUFF_UNDO_DATA;

/********************************************************************************
 * 内容  : アンドゥデータを追加する
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 先頭データをつなぐポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 最終データをつなぐポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA *dataPtr つなぐデータ
 * 戻り値: なし
 ***************************************/
void EditWndBufferUndoDataAddLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr );

/********************************************************************************
 * 内容  : アンドゥデータを削除する
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 先頭データがつないであるポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA **endPtrPtr 最終データをつないであるポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA *dataPtr    削除するデータ
 * 戻り値: S_BUFF_UNDO_DATA *           削除した次のデータ
 ***************************************/
void EditWndBufferUndoDataRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr );

/********************************************************************************
 * 内容  : アンドゥデータのクリア
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 先頭データをつなぐポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA **endPtrPtr 最終データをつなぐポインタ(のポインタ)
 * 戻り値: なし
 ***************************************/
void EditWndBufferUndoDataAllRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr );

/********************************************************************************
 * 内容  : アンドゥデータの生成
 * 引数  : UNDO_TYPE undoType
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD length
 * 引数  : DWORD lineNum
 * 引数  : DWORD caretPos
 * 戻り値: S_BUFF_UNDO_DATA *
 ***************************************/
S_BUFF_UNDO_DATA *EditWndBufferUndoDataCreate( UNDO_TYPE undoType, TCHAR* dataPtr, DWORD length, DWORD lineNum, DWORD caretPos );

/********************************************************************************
 * 内容  : アンドゥデータの解放
 * 引数  : S_BUFF_UNDO_DATA *
 * 戻り値: なし
 ***************************************/
void EditWndBufferUndoDataDestroy( S_BUFF_UNDO_DATA *dataPtr );

#define EDITWND_BUFFER_UNDO_DATA_TYPE_H
#endif /* EDITWND_BUFFER_UNDO_DATA_TYPE_H */
