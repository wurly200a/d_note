/* 共通インクルードファイル */
#include "common.h"

/* 個別インクルードファイル */
#include "DebugWnd.h"
#include "LinkedList.h"
#include "EditWndBufferUndoDataType.h"

/* 外部関数定義 */

/* 外部変数定義 */

/* 内部関数定義 */

/********************************************************************************
 * 内容  : アンドゥデータを追加する
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 先頭データをつなぐポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 最終データをつなぐポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA *dataPtr つなぐデータ
 * 戻り値: なし
 ***************************************/
void
EditWndBufferUndoDataAddLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr )
{
    AddLinkedList( (S_LIST_HEADER **)topPtrPtr, (S_LIST_HEADER **)endPtrPtr, (S_LIST_HEADER *)dataPtr );
}

/********************************************************************************
 * 内容  : アンドゥデータを削除する
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 先頭データがつないであるポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA **endPtrPtr 最終データをつないであるポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA *dataPtr    削除するデータ
 * 戻り値: S_BUFF_UNDO_DATA *           削除した次のデータ
 ***************************************/
void
EditWndBufferUndoDataRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr )
{
    S_BUFF_UNDO_DATA *removeNextPtr;

    removeNextPtr = (S_BUFF_UNDO_DATA *)RemoveLinkedList( (S_LIST_HEADER **)topPtrPtr,(S_LIST_HEADER **)endPtrPtr,(S_LIST_HEADER *)dataPtr );
}

/********************************************************************************
 * 内容  : アンドゥデータのクリア
 * 引数  : S_BUFF_UNDO_DATA **topPtrPtr 先頭データをつなぐポインタ(のポインタ)
 * 引数  : S_BUFF_UNDO_DATA **endPtrPtr 最終データをつなぐポインタ(のポインタ)
 * 戻り値: なし
 ***************************************/
void
EditWndBufferUndoDataAllRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr )
{
    S_BUFF_UNDO_DATA *lineDataPtr,*nextPtr;

    lineDataPtr = *topPtrPtr;
    while( lineDataPtr != NULL )
    {
        nextPtr = (S_BUFF_UNDO_DATA *)RemoveLinkedList((S_LIST_HEADER **)topPtrPtr,(S_LIST_HEADER **)endPtrPtr,(S_LIST_HEADER *)lineDataPtr);
        EditWndBufferUndoDataDestroy(lineDataPtr);
        lineDataPtr = nextPtr;
    }
}

/********************************************************************************
 * 内容  : アンドゥデータの生成
 * 引数  : UNDO_TYPE undoType
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD length
 * 引数  : DWORD lineNum
 * 引数  : DWORD caretPos
 * 戻り値: S_BUFF_UNDO_DATA *
 ***************************************/
S_BUFF_UNDO_DATA *
EditWndBufferUndoDataCreate( UNDO_TYPE undoType, TCHAR* dataPtr, DWORD length, DWORD lineNum, DWORD caretPos )
{
    S_BUFF_UNDO_DATA *newPtr = NULL;
    DWORD dataSize = (DWORD)sizeof(S_BUFF_UNDO_DATA);

    if( undoType == UNDO_TYPE_REMOVE )
    {
        dataSize += length;
    }
    else
    {
        nop();
    }
    newPtr = (S_BUFF_UNDO_DATA *)malloc( dataSize );

    if( newPtr != NULL )
    {
        TCHAR cForLog = ' ';

        memset( newPtr, 0, dataSize );
        newPtr->undoType = undoType;
        newPtr->size     = length;
        newPtr->lineNum  = lineNum;
        newPtr->caretPos = caretPos;

        if( undoType == UNDO_TYPE_REMOVE )
        {
            newPtr->dataPtr = ((TCHAR *)newPtr)+sizeof(S_BUFF_UNDO_DATA);
            memcpy( newPtr->dataPtr, dataPtr, length );

            cForLog = *(newPtr->dataPtr);
        }
        else
        {
            nop();
        }
#if 0
        DebugWndPrintf("EditWndBufferUndoDataCreate,%d,%c...,%08lX,%08lX,%08lX\r\n",newPtr->undoType,cForLog,newPtr->size,newPtr->lineNum,newPtr->caretPos);
#endif
    }
    else
    {
        nop();
    }

    return newPtr;
}

/********************************************************************************
 * 内容  : アンドゥデータの解放
 * 引数  : S_BUFF_UNDO_DATA *
 * 戻り値: なし
 ***************************************/
void
EditWndBufferUndoDataDestroy( S_BUFF_UNDO_DATA *dataPtr )
{
    free( dataPtr );
}

