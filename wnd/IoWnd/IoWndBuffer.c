/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

typedef struct tag_iownd_buffer_data
{
    struct tag_iownd_buffer_data *prevPtr;
    struct tag_iownd_buffer_data *nextPtr;
    DWORD                         dataSize;
    TCHAR                         data[];
} S_IOWND_BUFF_DATA;

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"
static void ioWndBuffAddLinkedList( S_IOWND_BUFF_DATA *dataPtr );

/* 内部変数定義 */
S_IOWND_BUFF_DATA *ioWndBuffListTopPtr;

TCHAR *ioWndBuffDataPtr;
DWORD ioWndBuffDataSize;
DWORD ioWndBuffLineMax;
DWORD ioWndBuffColumnMax;

/********************************************************************************
 * 内容  : IOウィンドウバッファの初期化
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndBuffInit( void )
{
    if( ioWndBuffDataPtr != NULL )
    {
        free( ioWndBuffDataPtr );

        ioWndBuffDataPtr = NULL;
        ioWndBuffDataSize = 0;
        ioWndBuffLineMax = 0;
        ioWndBuffColumnMax = 0;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファへのデータ追加
 * 引数  : TCHAR* strPtr
 * 引数  : DWORD  length
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSet( TCHAR* strPtr, DWORD length )
{
    DWORD i,columnCnt;
    S_IOWND_BUFF_DATA *dataPtr;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffDataPtr = (TCHAR *) malloc( (length * sizeof(TCHAR)) );

        if( ioWndBuffDataPtr != NULL )
        { /* メモリ獲得できた */
            ioWndBuffDataSize = length;
            for( i=0,columnCnt=0; i<length; i++ )
            {
                *(ioWndBuffDataPtr+i) = *(strPtr+i);
                if( *(strPtr+i) == '\n' )
                {
                    dataPtr = (S_IOWND_BUFF_DATA *)malloc( sizeof(S_IOWND_BUFF_DATA) + (columnCnt * sizeof(TCHAR)) );
                    if( dataPtr != NULL )
                    {
                        dataPtr->dataSize = columnCnt;
                        memcpy( dataPtr->data, (strPtr+i-columnCnt), columnCnt );
                        ioWndBuffAddLinkedList( dataPtr );
                    }
                    else
                    {
                        nop(); /* error */
                    }

                    (ioWndBuffLineMax)++; /* 改行の数をカウント */
                    ioWndBuffColumnMax = max( ioWndBuffColumnMax, columnCnt );
                    columnCnt=0;
                }
                else
                {
                    columnCnt++;
                }
            }

            if( ioWndBuffLineMax==0 )
            { /* 改行が無かった場合 */
                ioWndBuffLineMax  = 1;
                ioWndBuffColumnMax = ioWndBuffDataSize;

                dataPtr = (S_IOWND_BUFF_DATA *)malloc( sizeof(S_IOWND_BUFF_DATA) + (ioWndBuffDataSize * sizeof(TCHAR)) );
                if( dataPtr != NULL )
                {
                    ioWndBuffAddLinkedList( dataPtr );
                }
                else
                {
                    nop(); /* error */
                }
            }
            else
            {
                nop();
            }
         }
        else
        { /* メモリ獲得できなかった */
            nop(); /* error */
        }
   }
    else
    {
        nop(); /* error */
    }
}

/********************************************************************************
 * 内容  : 連結リストにデータを追加する
 * 引数  : S_IOWND_BUFF_DATA *dataPtr
 * 戻り値: なし
 ***************************************/
static void
ioWndBuffAddLinkedList( S_IOWND_BUFF_DATA *dataPtr )
{
    S_IOWND_BUFF_DATA *nowPtr,*nextPtr;

    if( ioWndBuffListTopPtr == NULL )
    {
        dataPtr->nextPtr = NULL;
        ioWndBuffListTopPtr = dataPtr;
    }
    else
    {
        for( nowPtr = nextPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nextPtr )
        {
            nextPtr = nowPtr->nextPtr;

            if( nextPtr == NULL )
            { /* 次につながれているデータ無し */
                dataPtr->nextPtr = NULL;
                nowPtr->nextPtr = dataPtr;
            }
            else
            { /* 次につながれているデータ有り */
                nop();
            }
        }
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータポインタ取得
 * 引数  : なし
 * 戻り値: TCHAR *
 ***************************************/
TCHAR *
IoWndGetBuffPtr( void )
{
    return ioWndBuffDataPtr;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( void )
{
    return ioWndBuffDataSize;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大行サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetLineMaxSize( void )
{
    return ioWndBuffLineMax;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大文字サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetColumnMaxSize( void )
{
    return ioWndBuffColumnMax;
}
