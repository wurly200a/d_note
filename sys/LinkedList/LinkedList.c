/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "LinkedList.h"

/********************************************************************************
 * 内容  : 連結リストのデータをクリア
 * 引数  : S_LIST_HEADER **topPtr 先頭データがつないであるポインタ
 * 引数  : S_LIST_HEADER **endPtr 最終データがつないであるポインタ
 * 戻り値: なし
 ***************************************/
void
ClearLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr )
{
    S_LIST_HEADER *nowPtr,*nextPtr;

    for( nowPtr = nextPtr = *topPtr; nowPtr != NULL; nowPtr = nextPtr )
    {
        nextPtr = nowPtr->nextPtr;
        free( nowPtr );

        if( nextPtr == NULL )
        { /* 次につながれているデータ無し */
            *topPtr = NULL;
            *endPtr = NULL;
        }
        else
        { /* 次につながれているデータ有り */
            nop();
        }
    }
}

/********************************************************************************
 * 内容  : 連結リストにデータを追加する
 * 引数  : S_LIST_HEADER **topPtr 先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *dataPtr つなぐデータ
 * 戻り値: なし
 ***************************************/
void
AddLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
{
    if( *topPtr == NULL )
    {
        dataPtr->prevPtr = NULL;
        dataPtr->nextPtr = NULL;
        *topPtr = dataPtr;
        *endPtr = dataPtr;
    }
    else
    {
        if( *endPtr != NULL )
        {
            dataPtr->prevPtr   = *endPtr;
            dataPtr->nextPtr   = NULL;
            (*endPtr)->nextPtr = dataPtr;
            *endPtr            = dataPtr;
        }
        else
        {
            nop(); /* 異常 */
        }
    }
}

/********************************************************************************
 * 内容  : 連結リストにデータを挿入する
 * 引数  : S_LIST_HEADER **topPtr 先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *nowPtr  挿入位置
 * 引数  : S_LIST_HEADER *dataPtr 挿入するデータ
 * 戻り値: なし
 ***************************************/
void
InsertLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->prevPtr = nowPtr->prevPtr;
        dataPtr->nextPtr = nowPtr;

        if( dataPtr->prevPtr != NULL )
        { /* 前データ有り */
            (dataPtr->prevPtr)->nextPtr = dataPtr;
        }
        else
        { /* 前データ無し */
            (*topPtr) = dataPtr;
        }

        nowPtr->prevPtr = dataPtr;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 連結リストのデータを置き換える
 * 引数  : S_LIST_HEADER **topPtr 先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *nowPtr  置き換え対象のデータ
 * 引数  : S_LIST_HEADER *dataPtr 置き換えるデータ
 * 戻り値: 置き換えたデータ
 ***************************************/
S_LIST_HEADER *
ReplaceLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->prevPtr = nowPtr->prevPtr;
        dataPtr->nextPtr = nowPtr->nextPtr;

        if( dataPtr->prevPtr != NULL )
        {
            (dataPtr->prevPtr)->nextPtr = dataPtr;
        }
        else
        {
            *(topPtr) = dataPtr;
        }
        if( dataPtr->nextPtr != NULL )
        {
            (dataPtr->nextPtr)->prevPtr = dataPtr;
        }
        else
        {
            *(endPtr) = dataPtr;
        }
    }
    else
    {
        nop();
    }

    return dataPtr;
}

/********************************************************************************
 * 内容  : 連結リストからデータを削除する
 * 引数  : S_LIST_HEADER **topPtr 先頭データがつないであるポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつないであるポインタ
 * 引数  : S_LIST_HEADER *dataPtr 削除するデータ
 * 戻り値: S_LIST_HEADER *        削除した次のデータ
 ***************************************/
S_LIST_HEADER *
RemoveLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
{
    S_LIST_HEADER *nextPtr = NULL;

    if( (topPtr != NULL) && (endPtr != NULL) && (dataPtr != NULL) )
    {
        nextPtr = dataPtr->nextPtr; /* 戻り値 */

        if( dataPtr->prevPtr != NULL )
        { /* 前データ有り */
            (dataPtr->prevPtr)->nextPtr = dataPtr->nextPtr; /* 次データを、前データの次データとする */
            if( (dataPtr->prevPtr)->nextPtr != NULL )
            { /* 次データがあった場合 */
                ((dataPtr->prevPtr)->nextPtr)->prevPtr = dataPtr->prevPtr; /* 次データの前データは、前データ */
            }
            else
            { /* 次データ無し */
                (*endPtr) = dataPtr->prevPtr; /* 前データが最後のデータとなる */
            }
        }
        else
        { /* 前データ無し */
            *topPtr = dataPtr->nextPtr;
            if( *topPtr != NULL )
            { /* 次データがあった場合 */
                (*topPtr)->prevPtr = NULL;
            }
            else
            { /* 次データ無し */
                (*endPtr) = NULL; /* データは全部無くなった */
            }
        }
    }
    else
    {
        nop();
    }

    return nextPtr;
}

/********************************************************************************
 * 内容  : 連結リストの結合
 * 引数  : S_LIST_HEADER **topPtr 先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *nowPtr  挿入位置
 * 引数  : S_LIST_HEADER **mergeTopPtr
 * 引数  : S_LIST_HEADER **mergeEndPtr
 * 戻り値: void
 ***************************************/
void
MergeLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER **mergeTopPtr, S_LIST_HEADER **mergeEndPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (mergeTopPtr != NULL) && (mergeEndPtr != NULL) )
    {
        (*mergeTopPtr)->prevPtr = nowPtr->prevPtr;

        if( (*mergeTopPtr)->prevPtr != NULL )
        {
            ((*mergeTopPtr)->prevPtr)->nextPtr = (*mergeTopPtr);
        }
        else
        {
            (*topPtr) = (*mergeTopPtr);
        }

        (*mergeEndPtr)->nextPtr = nowPtr;
        if( (*mergeEndPtr)->nextPtr != NULL )
        {
            ((*mergeEndPtr)->nextPtr)->prevPtr = (*mergeEndPtr);
        }
        else
        {
            nop(); /* 有り得ない */
        }
    }
    else
    {
        nop();
    }
}
