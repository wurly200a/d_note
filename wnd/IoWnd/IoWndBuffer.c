/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"
static DWORD getLineLength( TCHAR *dataPtr, DWORD maxLength );
static void addLinkedList( S_BUFF_LINE_DATA *dataPtr );
static void clearLinkedList( void );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );

/* 内部変数定義 */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;

typedef struct
{
    TCHAR *DataPtr   ;
    DWORD DataSize   ;
    DWORD LineMax    ;
    DWORD ColumnMax  ;
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
    if( ioWndBuffData.DataPtr != NULL )
    {
        free( ioWndBuffData.DataPtr );
        ioWndBuffData.DataPtr = NULL;
    }
    else
    {
        nop();
    }
    clearLinkedList();

    ioWndBuffData.DataSize = 0;
    ioWndBuffData.LineMax = 0;
    ioWndBuffData.ColumnMax = 0;

    ioWndBuffData.NewLineType = IOWND_BUFF_NEWLINE_CRLF;
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
    S_BUFF_LINE_DATA *dataPtr;
    DWORD lineLengthSum = 0;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffData.DataSize = length;

        while( lineLengthSum < length )
        {
            dataPtr = (S_BUFF_LINE_DATA *)getBuffLine( strPtr+lineLengthSum, length - lineLengthSum );

            if( dataPtr != NULL )
            {
                addLinkedList( dataPtr );

                ioWndBuffData.ColumnMax = max( ioWndBuffData.ColumnMax, dataPtr->dataSize );
                (ioWndBuffData.LineMax)++; /* 改行の数をカウント */
                lineLengthSum += dataPtr->dataSize;
            }
            else
            {
                break;
            }
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
 * 引数  : S_BUFF_LINE_DATA *dataPtr
 * 戻り値: なし
 ***************************************/
static void
addLinkedList( S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

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
 * 内容  : バッファ行データを取得
 * 引数  : TCHAR *dataPtr  データの先頭
 * 引数  : DWORD maxLength 最大長さ
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
getBuffLine( TCHAR *dataPtr, DWORD maxLength )
{
    S_BUFF_LINE_DATA *listPtr = NULL;
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

    listPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (i * sizeof(TCHAR)) );
    if( listPtr != NULL )
    {
        listPtr->dataSize = i;
        listPtr->newLineCodeSize = newLineCodeSize;
        memcpy( listPtr->data, dataPtr, i );
    }
    else
    {
        nop();
    }

    return listPtr;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( void )
{
    return ioWndBuffData.DataSize;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大行サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetLineMaxSize( void )
{
    return ioWndBuffData.LineMax;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大文字サイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetColumnMaxSize( void )
{
    return ioWndBuffData.ColumnMax;
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
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSetNewLineCode( UINT newLineType )
{
    ioWndBuffData.NewLineType = newLineType;
}
