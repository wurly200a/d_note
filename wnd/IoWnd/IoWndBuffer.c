/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"
static void addLinkedList( S_BUFF_LINE_DATA *dataPtr );
static void clearLinkedList( void );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );

/* 内部変数定義 */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;

typedef struct
{
    UINT  NewLineType;
} S_IOWND_BUFF_DATA;

S_IOWND_BUFF_DATA ioWndBuffData;

/********************************************************************************
 * 内容  : IOウィンドウバッファの初期化
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndBuffInit( void )
{
    clearLinkedList();
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータセット
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 戻り値: なし
 ***************************************/
void
IoWndBuffDataSet( TCHAR* dataPtr, DWORD length )
{
    S_BUFF_LINE_DATA *lineDataPtr;
    DWORD lineLengthSum = 0;

    IoWndBuffInit();

    if( (dataPtr != NULL) && (length > 0) )
    {
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                addLinkedList( lineDataPtr );
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
        }

        if( (lineDataPtr != NULL) &&
            (lineDataPtr->newLineCodeSize) )
        { /* 最後の行に改行コードがあった場合、データ0、改行コード0のデータを追加 */
            lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
            if( lineDataPtr != NULL )
            {
                lineDataPtr->dataSize = 0;
                lineDataPtr->newLineCodeSize = 0;
                addLinkedList( lineDataPtr );
            }
            else
            {
                nop();
            }
        }
        else
        {
            nop();
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
clearLinkedList( void )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

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

/********************************************************************************
 * 内容  : 連結リストにデータを追加する
 * 引数  : S_BUFF_LINE_DATA *dataPtr
 * 戻り値: なし
 ***************************************/
static void
addLinkedList( S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

    if( ioWndBuffListTopPtr == NULL )
    {
        dataPtr->prevPtr = NULL;
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
                dataPtr->prevPtr = nowPtr;
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
 * 内容  : バッファ行データを取得
 * 引数  : TCHAR *dataPtr  データの先頭
 * 引数  : DWORD maxLength 最大長さ
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
getBuffLine( TCHAR *dataPtr, DWORD maxLength )
{
    S_BUFF_LINE_DATA *lineDataPtr = NULL;
    DWORD i;
    INT   newLineCodeSize = 0;

    for( i=0; i<maxLength; i++ )
    {
        if( ioWndBuffData.NewLineType == IOWND_BUFF_NEWLINE_CRLF )
        {
            if( (i>0) &&
                (*(dataPtr+i-1) == '\r') &&
                (*(dataPtr+i)   == '\n') )
            {
                i++;
                newLineCodeSize = 2;
                break;
            }
            else
            {
                nop();
            }
        }
        else if( ioWndBuffData.NewLineType == IOWND_BUFF_NEWLINE_LF )
        {
            if( (*(dataPtr+i) == '\n') )
            {
                i++;
                newLineCodeSize = 1;
                break;
            }
            else
            {
                nop();
            }
        }
        else if( ioWndBuffData.NewLineType == IOWND_BUFF_NEWLINE_CR )
        {
            if( (*(dataPtr+i) == '\r') )
            {
                i++;
                newLineCodeSize = 1;
                break;
            }
            else
            {
                nop();
            }
        }
        else
        { /* IOWND_BUFF_NEWLINE_NONE */
            nop();
        }
    }

    lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (i * sizeof(TCHAR)) );
    if( lineDataPtr != NULL )
    {
        lineDataPtr->dataSize = i;
        lineDataPtr->newLineCodeSize = newLineCodeSize;
        memcpy( lineDataPtr->data, dataPtr, i );
    }
    else
    {
        nop();
    }

    return lineDataPtr;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( void )
{
    DWORD dataSize = 0;
    S_BUFF_LINE_DATA *nowPtr;

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
    {
        dataSize += nowPtr->dataSize;
    }

    return dataSize;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大行サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetLineMaxSize( void )
{
    DWORD lineMaxSize = 0;
    S_BUFF_LINE_DATA *nowPtr;

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
    {
        lineMaxSize++;
    }

    return lineMaxSize;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大文字サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetColumnMaxSize( void )
{
    DWORD columnMaxSize = 0;
    S_BUFF_LINE_DATA *nowPtr;

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
    {
        columnMaxSize = max( columnMaxSize, (nowPtr->dataSize - nowPtr->newLineCodeSize) );
    }

    return columnMaxSize;
}

/********************************************************************************
 * 内容  : 指定行のデータアドレス取得
 * 引数  : DWORD lineNum
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *
IoWndBuffGetLinePtr( DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr,*rtnPtr = NULL;
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

/********************************************************************************
 * 内容  : IOウィンドウバッファの改行コードセット
 * 引数  : UINT newLineType
 * 戻り値: BOOL (TRUE:データが変更された)
 ***************************************/
BOOL
IoWndBuffSetNewLineCode( UINT newLineType )
{
    BOOL rtn = FALSE;
    DWORD allDataSize;
    TCHAR *dataTopPtr,*dataPtr;
    S_BUFF_LINE_DATA *nowPtr;

    ioWndBuffData.NewLineType = newLineType;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        allDataSize = IoWndGetBuffSize();
        dataTopPtr = malloc( sizeof(TCHAR) * allDataSize );
        if( dataTopPtr != NULL )
        {
            for( nowPtr=ioWndBuffListTopPtr,dataPtr=dataTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
            {
                memcpy( dataPtr, nowPtr->data, nowPtr->dataSize );
                dataPtr += nowPtr->dataSize;
            }
            clearLinkedList();
            IoWndBuffDataSet( dataTopPtr, allDataSize );
            free( dataTopPtr );
        }
        else
        {
            nop();
        }
        rtn = TRUE;
    }

    return rtn;
}
