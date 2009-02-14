/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"
static DWORD getLineLength( TCHAR *dataPtr, DWORD maxLength );
static void ioWndBuffAddLinkedList( S_IOWND_BUFF_DATA *dataPtr );
static void ioWndBuffClearLinkedList( void );

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
    }
    else
    {
        nop();
    }
    ioWndBuffClearLinkedList();

    ioWndBuffDataSize = 0;
    ioWndBuffLineMax = 0;
    ioWndBuffColumnMax = 0;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファ(連結リスト)へのデータ追加
 * 引数  : TCHAR* strPtr
 * 引数  : DWORD  length
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSetLinkedList( TCHAR* strPtr, DWORD length )
{
    S_IOWND_BUFF_DATA *dataPtr;
    DWORD lineLength,lineLengthSum = 0;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffDataSize = length;

        while( lineLengthSum < length )
        {
            lineLength = getLineLength( strPtr+lineLengthSum, length - lineLengthSum );
            dataPtr = (S_IOWND_BUFF_DATA *)malloc( sizeof(S_IOWND_BUFF_DATA) + (lineLength * sizeof(TCHAR)) );

            if( dataPtr != NULL )
            {
                dataPtr->dataSize = lineLength;
                memcpy( dataPtr->data, (strPtr+lineLengthSum), lineLength );
                ioWndBuffAddLinkedList( dataPtr );
            }
            else
            {
                nop(); /* error */
            }

            ioWndBuffColumnMax = max( ioWndBuffColumnMax, lineLength );
            (ioWndBuffLineMax)++; /* 改行の数をカウント */
            lineLengthSum += lineLength;
        }
    }
    else
    {
        nop(); /* error */
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
    DWORD i,ColumnCnt;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffDataPtr = (TCHAR *) malloc( (length * sizeof(TCHAR)) );

        if( ioWndBuffDataPtr != NULL )
        { /* メモリ獲得できた */
            ioWndBuffDataSize = length;

            for( i=0,ColumnCnt=0; i<length; i++ )
            {
                *(ioWndBuffDataPtr+i) = *(strPtr+i);
                if( *(strPtr+i) == '\n' )
                {
                    (ioWndBuffLineMax)++; /* 改行の数をカウント */
                    ioWndBuffColumnMax = max( ioWndBuffColumnMax, ColumnCnt );
                    ColumnCnt=0;
                }
                else
                {
                    ColumnCnt++;
                }
            }

            if( ioWndBuffLineMax==0 )
            { /* 改行が無かった場合 */
                ioWndBuffLineMax  = 1;
                ioWndBuffColumnMax = ioWndBuffDataSize;
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
 * 内容  : 連結リストのデータをクリア
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
static void
ioWndBuffClearLinkedList( void )
{
    S_IOWND_BUFF_DATA *nowPtr,*nextPtr;

    if( ioWndBuffListTopPtr == NULL )
    { /* 1つもつながれてなければ */
        nop();
    }
    else
    {
        for( nowPtr = nextPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nextPtr )
        {
            nextPtr = nowPtr->nextPtr;
            free( nowPtr );

            if( nextPtr == NULL )
            { /* 次につながれているデータ無し */
                ioWndBuffListTopPtr = NULL;
            }
            else
            { /* 次につながれているデータ有り */
                nop();
            }
        }
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
 * 内容  : 改行までの長さを取得
 * 引数  : TCHAR *dataPtr  データの先頭
 * 引数  : DWORD maxLength 最大長さ
 * 戻り値: DWORD
 ***************************************/
static DWORD
getLineLength( TCHAR *dataPtr, DWORD maxLength )
{
    DWORD i;

    for( i=0; i<maxLength; i++ )
    {
        if( (i>0) &&
            (*(dataPtr+i-1) == '\r') &&
            (*(dataPtr+i)   == '\n') )
        {
            break;
        }
        else
        {
            nop();
        }
    }

    if( i>0 )
    {
        i++;
    }
    else
    {
        nop();
    }

    return i;
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

/********************************************************************************
 * 内容  : 指定行のデータアドレス取得
 * 引数  : DWORD lineNum
 * 戻り値: S_IOWND_BUFF_DATA *
 ***************************************/
S_IOWND_BUFF_DATA *
IoWndBuffGetLinePtr( DWORD lineNum )
{
    S_IOWND_BUFF_DATA *nowPtr,*nextPtr,*rtnPtr = NULL;
    DWORD i;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        for( i=0,nowPtr = nextPtr = ioWndBuffListTopPtr; (nowPtr != NULL) && (i<=lineNum); nowPtr=nextPtr,i++ )
        {
            nextPtr = nowPtr->nextPtr;

            if( i == lineNum )
            {
                rtnPtr = nowPtr;
                break;
            }
            else
            {
                nop();
            }
        }
    }

    return rtnPtr;
}
