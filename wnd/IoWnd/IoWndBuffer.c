/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"
static void clearLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr );
static void addLinkedList  ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *replaceLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *insertLinkedList ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *removeLinkedList ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );

static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );

enum
{
    SINGLE_CHAR,
    DOUBLE_CHAR_HIGH,
    DOUBLE_CHAR_LOW,
};
typedef int CHARSET_TYPE;
CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );

static INT getNewLineSize( void );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

/* 内部変数定義 */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;
S_BUFF_LINE_DATA *ioWndBuffListEndPtr;
S_BUFF_LINE_DATA *ioWndBuffLineNowPtr;

typedef struct
{
    UINT  NewLineType;
    DWORD xCaret;
    DWORD yCaret;
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
    S_BUFF_LINE_DATA *lineDataPtr;

    clearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);

    /* データ0、改行コード0のデータを追加 */
    lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
    if( lineDataPtr != NULL )
    {
        lineDataPtr->dataSize = 0;
        lineDataPtr->newLineCodeSize = 0;
        lineDataPtr->lineNum = 0;
        addLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,lineDataPtr );
    }
    else
    {
        nop();
    }

    /* カーソル位置を先頭にする */
    if( ioWndBuffListTopPtr != NULL )
    {
        ioWndBuffLineNowPtr = ioWndBuffListTopPtr;
        ioWndBuffLineNowPtr->caretPos = 0;
    }
    else
    {
        nop();
    }

}

/********************************************************************************
 * 内容  : IOウィンドウバッファの終了
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndBuffEnd( void )
{
    clearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);
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
#if 0
    S_BUFF_LINE_DATA *lineDataPtr;
    S_BUFF_LINE_DATA *tempTopPtr = NULL; /* 一時的につないでおく */
    S_BUFF_LINE_DATA *tempEndPtr = NULL;
    S_BUFF_LINE_DATA *newPtr;
    DWORD lineLengthSum = 0;

    if( (dataPtr != NULL) && (length > 0) )
    { /* データ有りの場合 */
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                addLinkedList( &tempTopPtr,&tempEndPtr,lineDataPtr );
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
                addLinkedList( &tempTopPtr,&tempEndPtr,lineDataPtr );
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
        /* ここまでで、 追加データが行データに分割されて tempTopPtr につながれる */

        insertLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr, tempTopPtr );
    }
    else
    { /* データ無しの場合 */
        nop();
    }

#else
    S_BUFF_LINE_DATA *lineDataPtr;
    DWORD lineLengthSum = 0;
    DWORD lineNum = 0;

    clearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);

    if( (dataPtr != NULL) && (length > 0) )
    { /* データ有りの場合 */
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                lineDataPtr->lineNum = lineNum;
                addLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,lineDataPtr );
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
            lineNum++;
        }

        if( (lineDataPtr != NULL) &&
            (lineDataPtr->newLineCodeSize) )
        { /* 最後の行に改行コードがあった場合、データ0、改行コード0のデータを追加 */
            lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
            if( lineDataPtr != NULL )
            {
                lineDataPtr->dataSize = 0;
                lineDataPtr->newLineCodeSize = 0;
                lineDataPtr->lineNum = lineNum;
                addLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,lineDataPtr );
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
    { /* データ無しの場合 */
        lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
        if( lineDataPtr != NULL )
        {
            lineDataPtr->dataSize = 0;
            lineDataPtr->newLineCodeSize = 0;
            lineDataPtr->lineNum = 0;
            addLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,lineDataPtr );
        }
        else
        {
            nop();
        }
    }

    if( ioWndBuffListTopPtr != NULL )
    {
        ioWndBuffLineNowPtr = ioWndBuffListTopPtr;
        ioWndBuffLineNowPtr->caretPos = 0;
    }
    else
    {
        nop();
    }
#endif
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータ取得
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffDataGet( TCHAR *dataPtr, DWORD dataSize )
{
    BOOL rtn = FALSE;
    S_BUFF_LINE_DATA *nowPtr;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        if( dataPtr != NULL )
        {
            for( nowPtr=ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
            {
                memcpy( dataPtr, nowPtr->data, nowPtr->dataSize );
                dataPtr += nowPtr->dataSize;
            }
        }
        else
        {
            nop();
        }
        rtn = TRUE;
    }

    return rtn;
}

/********************************************************************************
 * 内容  : 連結リストのデータをクリア
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データがつないであるポインタ
 * 引数  : S_BUFF_LINE_DATA **endPtr 最終データがつないであるポインタ
 * 戻り値: なし
 ***************************************/
static void
clearLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

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
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *dataPtr つなぐデータ
 * 戻り値: なし
 ***************************************/
static void
addLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

    if( *topPtr == NULL )
    {
        dataPtr->prevPtr = NULL;
        dataPtr->nextPtr = NULL;
        dataPtr->lineNum = 0;
        *topPtr = dataPtr;
        *endPtr = dataPtr;
    }
    else
    {
        if( *endPtr != NULL )
        {
            dataPtr->prevPtr   = *endPtr;
            dataPtr->nextPtr   = NULL;
            dataPtr->lineNum   = (*endPtr)->lineNum+1;
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
 * 内容  : 連結リストのデータを置き換える
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr  置き換え対象のデータ
 * 引数  : S_BUFF_LINE_DATA *dataPtr 置き換えるデータ
 * 戻り値: 置き換えたデータ
 ***************************************/
static S_BUFF_LINE_DATA *
replaceLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->lineNum = nowPtr->lineNum;

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
 * 内容  : 連結リストにデータを挿入する
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr  挿入位置
 * 引数  : S_BUFF_LINE_DATA *dataPtr 挿入データの先頭
 * 戻り値: S_BUFF_LINE_DATA *        挿入データの先頭
 ***************************************/
static S_BUFF_LINE_DATA *
insertLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *lastPtr,*lastPrevPtr,*rtnPtr = NULL;
    DWORD lineNum;

    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        rtnPtr = dataPtr;

        dataPtr->prevPtr = nowPtr->prevPtr;

        if( dataPtr->prevPtr != NULL )
        {
            dataPtr->lineNum = (dataPtr->prevPtr)->lineNum+1;
            (dataPtr->prevPtr)->nextPtr = dataPtr;
        }
        else
        {
            dataPtr->lineNum = 0;
            (*topPtr) = dataPtr;
        }

        /* 挿入データの最後を探す */
        for( lastPtr = dataPtr,lineNum = dataPtr->lineNum; lastPtr->nextPtr != NULL; lastPtr = lastPtr->nextPtr,lineNum++ )
        {
            lastPtr->lineNum = lineNum;
        }

        if( lastPtr == dataPtr )
        {
            nowPtr->lineNum = lineNum+1;
        }
        else
        {
            nowPtr->lineNum = lineNum;
        }

        lastPtr->nextPtr = nowPtr;
        if( lastPtr->nextPtr != NULL )
        {
            (lastPtr->nextPtr)->prevPtr = lastPtr;
            updateLineNum( nowPtr );
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

    return rtnPtr;
}

/********************************************************************************
 * 内容  : 連結リストからデータを削除する
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データがつないであるポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつないであるポインタ
 * 引数  : S_BUFF_LINE_DATA *dataPtr 削除するデータ
 * 戻り値: S_BUFF_LINE_DATA *        削除した次のデータ
 ***************************************/
static S_BUFF_LINE_DATA *
removeLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nextPtr = NULL;

    if( (topPtr != NULL) && (endPtr != NULL) && (dataPtr != NULL) )
    {
        nextPtr = dataPtr->nextPtr; /* 戻り値 */

        if( dataPtr->prevPtr != NULL )
        { /* 前データ有り */
            (dataPtr->prevPtr)->nextPtr = dataPtr->nextPtr; /* 次データを、前データの次データとする */
            if( (dataPtr->prevPtr)->nextPtr != NULL )
            { /* 次データがあった場合 */
                ((dataPtr->prevPtr)->nextPtr)->lineNum = (dataPtr->prevPtr)->lineNum+1;
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
                (*topPtr)->lineNum = 0;
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
 * 内容  : 行データの結合
 * 引数  : S_BUFF_LINE_DATA *data1Ptr (改行コードは削除される)
 * 引数  : S_BUFF_LINE_DATA *data2Ptr
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( (data1Ptr != NULL) && (data2Ptr != NULL) )
    {
        newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((data1Ptr->dataSize-data1Ptr->newLineCodeSize+data2Ptr->dataSize) * sizeof(TCHAR)) );
        if( newPtr != NULL )
        {
            newPtr->dataSize = data1Ptr->dataSize - data1Ptr->newLineCodeSize + data2Ptr->dataSize;
            newPtr->newLineCodeSize = data2Ptr->newLineCodeSize;

            memcpy( newPtr->data, data1Ptr->data, data1Ptr->dataSize - data1Ptr->newLineCodeSize );
            memcpy( newPtr->data + data1Ptr->dataSize - data1Ptr->newLineCodeSize, data2Ptr->data, data2Ptr->dataSize );
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

    return newPtr;
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
    return ioWndBuffListEndPtr->lineNum;
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
 * 内容  : IOウィンドウバッファのキャレットX位置取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetCaretXpos( void )
{
    return ioWndBuffLineNowPtr->caretPos;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetCaretYpos( void )
{
    return ioWndBuffLineNowPtr->lineNum;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレット位置セット
 * 引数  : DWORD xPos
 * 引数  : DWORD lineNum
 * 戻り値: なし
 ***************************************/
void
IoWndSetCaretPos( DWORD xPos, DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr = NULL;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        for( nowPtr = ioWndBuffListTopPtr; (nowPtr->nextPtr != NULL); nowPtr=nowPtr->nextPtr )
        { /* 一致しなかった場合は nowPtr=最終行 となるように */
            if( nowPtr->lineNum == lineNum )
            {
                break;
            }
            else
            {
                nop();
            }
        }
    }

    if( nowPtr != NULL )
    {
        ioWndBuffLineNowPtr = nowPtr;

        if( xPos < (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
        {
            ioWndBuffLineNowPtr->caretPos = xPos;

            if( (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_LOW) )
            { /* 2byte文字の真ん中だったら */
                ioWndBuffLineNowPtr->caretPos += 1; /* 後ろへずらす */
            }
            else
            {
                nop();
            }
        }
        else
        {
            ioWndBuffLineNowPtr->caretPos = (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize);
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置加算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndIncCaretXpos( void )
{
    int moveAmount = 0;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_HIGH )
        { /* 次の文字で移動量を判断 */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( (ioWndBuffLineNowPtr->caretPos + moveAmount) > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
        { /* キャレットはすでに行の最右端 */
            if( (ioWndBuffLineNowPtr->nextPtr != NULL) )
            { /* 次行有り */
                ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->nextPtr;
                ioWndBuffLineNowPtr->caretPos = 0;
            }
            else
            { /* 次行無し */
                nop();
            }
        }
        else
        { /* まだ右へ移動可能 */
            ioWndBuffLineNowPtr->caretPos += moveAmount;
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置減算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndDecCaretXpos( void )
{
    int moveAmount = 0;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( (ioWndBuffLineNowPtr->caretPos>0) &&
            (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos-1) == DOUBLE_CHAR_LOW) )
        { /* 前の文字で移動量を判断 */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( ioWndBuffLineNowPtr->caretPos == 0 )
        { /* キャレットはすでに行の最左端 */
            if( (ioWndBuffLineNowPtr->prevPtr != NULL) )
            { /* 前行有り */
                ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->prevPtr;
                ioWndBuffLineNowPtr->caretPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
            }
            else
            { /* 前行無し */
                nop();
            }
        }
        else
        { /* まだ左へ移動可能 */
            ioWndBuffLineNowPtr->caretPos = max( ioWndBuffLineNowPtr->caretPos - moveAmount, 0 );
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置加算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndIncCaretYpos( void )
{
    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineNowPtr->nextPtr != NULL )
        { /* 次行有り */
            (ioWndBuffLineNowPtr->nextPtr)->caretPos = ioWndBuffLineNowPtr->caretPos;
            ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->nextPtr;

            if( ioWndBuffLineNowPtr->caretPos > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
            { /* 下は文字が無い */
                ioWndBuffLineNowPtr->caretPos = (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize);
            }
            else
            { /* 下は文字が有る */
                if( (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_LOW) )
                { /* 下は2byte文字の真ん中だったら */
                    ioWndBuffLineNowPtr->caretPos += 1; /* 後ろへずらす */
                }
                else
                {
                    nop();
                }
            }
        }
        else
        { /* 次行無し */
            nop();
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置減算
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndDecCaretYpos( void )
{
    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineNowPtr->prevPtr != NULL )
        { /* 前行有り */
            (ioWndBuffLineNowPtr->prevPtr)->caretPos = ioWndBuffLineNowPtr->caretPos;
            ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->prevPtr;

            if( ioWndBuffLineNowPtr->caretPos > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
            { /* 上は文字が無い */
                ioWndBuffLineNowPtr->caretPos = (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize);
            }
            else
            { /* 上は文字が有る */
                if( (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_LOW) )
                { /* 下は2byte文字の真ん中だったら */
                    ioWndBuffLineNowPtr->caretPos += 1; /* 後ろへずらす */
                }
                else
                {
                    nop();
                }
            }
        }
        else
        { /* 前行無し */
            nop();
        }
    }
    else
    {
        nop();
    }
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
            clearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);
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

/********************************************************************************
 * 内容  : データ追加
 * 引数  : TCHAR data
 * 戻り値: なし
 ***************************************/
void
IoWndBuffAddData( TCHAR data )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( ioWndBuffLineNowPtr != NULL )
    {
        /* 1文字追加した新しい行データを生成 */
        newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((ioWndBuffLineNowPtr->dataSize+1) * sizeof(TCHAR)) );
        if( newPtr != NULL )
        {
            newPtr->dataSize = ioWndBuffLineNowPtr->dataSize + 1;
            newPtr->newLineCodeSize = ioWndBuffLineNowPtr->newLineCodeSize;
            newPtr->lineNum  = ioWndBuffLineNowPtr->lineNum;
            newPtr->caretPos = ioWndBuffLineNowPtr->caretPos+1;

            memcpy( newPtr->data, ioWndBuffLineNowPtr->data, ioWndBuffLineNowPtr->caretPos );
            *(newPtr->data+ioWndBuffLineNowPtr->caretPos) = data;
            memcpy( newPtr->data+ioWndBuffLineNowPtr->caretPos+1, ioWndBuffLineNowPtr->data+ioWndBuffLineNowPtr->caretPos, (ioWndBuffLineNowPtr->dataSize-ioWndBuffLineNowPtr->caretPos) );

            /* 古い行データと置き換える */
            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
            free( ioWndBuffLineNowPtr );
            ioWndBuffLineNowPtr = newPtr;
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

/********************************************************************************
 * 内容  : データ削除
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndBuffRemoveData( void )
{
    int removeSize = 0;
    S_BUFF_LINE_DATA *newPtr = NULL,*prevPtr = NULL;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineNowPtr->caretPos > 0  )
        {
            /* 1文字削除した新しい行データを生成 */
            if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos-1) == DOUBLE_CHAR_LOW )
            { /* 次の文字で削除量を判断 */
                removeSize = 2;
            }
            else
            {
                removeSize = 1;
            }

            newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((ioWndBuffLineNowPtr->dataSize-removeSize) * sizeof(TCHAR)) );
            if( newPtr != NULL )
            {
                newPtr->dataSize        = ioWndBuffLineNowPtr->dataSize - removeSize;
                newPtr->newLineCodeSize = ioWndBuffLineNowPtr->newLineCodeSize;
                newPtr->lineNum         = ioWndBuffLineNowPtr->lineNum;
                newPtr->caretPos        = ioWndBuffLineNowPtr->caretPos - removeSize;

                memcpy( newPtr->data, ioWndBuffLineNowPtr->data,ioWndBuffLineNowPtr->caretPos-removeSize );
                memcpy( newPtr->data+ioWndBuffLineNowPtr->caretPos-removeSize, ioWndBuffLineNowPtr->data+ioWndBuffLineNowPtr->caretPos, (ioWndBuffLineNowPtr->dataSize-ioWndBuffLineNowPtr->caretPos) );

                /* 古い行データと置き換える */
                replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
                free( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = newPtr;
            }
            else
            {
                nop();
            }
        }
        else
        { /* キャレットが行の先頭位置。つまり、前行との結合。*/
            if( ioWndBuffLineNowPtr->prevPtr != NULL )
            {
                prevPtr = ioWndBuffLineNowPtr->prevPtr;
                newPtr = joinData( prevPtr,ioWndBuffLineNowPtr ); /* 前行と本行を結合した新しい行データを生成 */

                /* 行番号、キャレット位置を前行データによって決まる */
                newPtr->lineNum  = prevPtr->lineNum;
                newPtr->caretPos = prevPtr->dataSize - prevPtr->newLineCodeSize;

                if( newPtr != NULL )
                {
                    removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr ); /* 本行は削除 */
                    replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,prevPtr,newPtr );    /* 前行は新しい行データに置き換える */

                    free( ioWndBuffLineNowPtr );
                    free( prevPtr );
                    ioWndBuffLineNowPtr = newPtr;

                    updateLineNum( ioWndBuffLineNowPtr );
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
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 改行追加
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndBuffAddNewLine( void )
{
    S_BUFF_LINE_DATA *newPtr = NULL,*newNextPtr = NULL,*addPtr;

    if( ioWndBuffLineNowPtr != NULL )
    {
        newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (ioWndBuffLineNowPtr->caretPos+getNewLineSize()) * sizeof(TCHAR));
        if( newPtr != NULL )
        {
            newNextPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((ioWndBuffLineNowPtr->dataSize-ioWndBuffLineNowPtr->caretPos) * sizeof(TCHAR)) );
            if( newNextPtr != NULL )
            {
                /* 改行より前のデータ生成 */
                newPtr->dataSize = ioWndBuffLineNowPtr->caretPos+getNewLineSize();
                newPtr->newLineCodeSize = getNewLineSize();
                newPtr->lineNum = ioWndBuffLineNowPtr->lineNum;

                memcpy( newPtr->data, ioWndBuffLineNowPtr->data, newPtr->dataSize );
                switch( ioWndBuffData.NewLineType )
                {
                case IOWND_BUFF_NEWLINE_CRLF:
                    *(newPtr->data + newPtr->dataSize-2) = '\r';
                    *(newPtr->data + newPtr->dataSize-1) = '\n';
                    break;
                case IOWND_BUFF_NEWLINE_LF  :
                    *(newPtr->data + newPtr->dataSize-1) = '\n';
                    break;
                case IOWND_BUFF_NEWLINE_CR  :
                    *(newPtr->data + newPtr->dataSize-1) = '\r';
                    break;
                case IOWND_BUFF_NEWLINE_NONE:
                default:
                    break;
                }

                /* 改行以降のデータ生成 */
                newNextPtr->dataSize = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->caretPos;
                newNextPtr->newLineCodeSize = ioWndBuffLineNowPtr->newLineCodeSize;
                newNextPtr->lineNum = newPtr->lineNum+1;
                memcpy( newNextPtr->data, ioWndBuffLineNowPtr->data+ioWndBuffLineNowPtr->caretPos, newNextPtr->dataSize );

                /* 古いデータを置き換える */
                addPtr = removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr ); /* 元の行は削除 */
                free( ioWndBuffLineNowPtr );

                if( addPtr != NULL )
                {
                    /* 改行より前と、改行以降のデータをつないでおく */
                    newPtr->prevPtr     = NULL;
                    newPtr->nextPtr     = newNextPtr;
                    newNextPtr->prevPtr = newPtr;
                    newNextPtr->nextPtr = NULL;

                    insertLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,addPtr, newPtr );
                }
                else
                { /* 元々1行しか無かった場合 */
                    addLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,newPtr );
                    addLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,newNextPtr );
                }

                ioWndBuffLineNowPtr = newNextPtr;
                ioWndBuffLineNowPtr->caretPos = 0;

                updateLineNum( ioWndBuffLineNowPtr );
            }
            else
            {
                free( newPtr );
            }
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

/********************************************************************************
 * 内容  : キャラクタセットの判断(Shift_JISの場合)
 * 引数  : S_BUFF_LINE_DATA *dataPtr
 * 引数  : DWORD            offset   判断したい文字の先頭からのオフセット(0 origin)
 * 戻り値: CHARSET_TYPE
 ***************************************/
CHARSET_TYPE
detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset )
{
    DWORD i;
    int charType = SINGLE_CHAR;

    if( dataPtr != NULL )
    {
        for( i=0; i<(dataPtr->dataSize-dataPtr->newLineCodeSize); i++ )
        {
            if( charType == DOUBLE_CHAR_HIGH )
            {
                charType = DOUBLE_CHAR_LOW;
            }
            else
            {
                if( ( (BYTE)(*(dataPtr->data+i)) <= (BYTE)0x80) || (((BYTE)0xA0 <= (BYTE)(*(dataPtr->data+i))) && ((BYTE)(*(dataPtr->data+i)) <= (BYTE)0xDF)) )
                {
                    charType = SINGLE_CHAR;
                }
                else
                {
                    charType = DOUBLE_CHAR_HIGH;
                }
            }

            if( i==offset )
            {
                break;
            }
            else
            {
                nop();
            }
        }

        if( i==(dataPtr->dataSize-dataPtr->newLineCodeSize) )
        {
            charType = SINGLE_CHAR;
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

    return charType;
}

/********************************************************************************
 * 内容  : 改行コードサイズを取得
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
static INT
getNewLineSize( void )
{
    INT rtn = 0;

    switch( ioWndBuffData.NewLineType )
    {
    case IOWND_BUFF_NEWLINE_CRLF:
        rtn = 2;
        break;
    case IOWND_BUFF_NEWLINE_LF  :
    case IOWND_BUFF_NEWLINE_CR  :
        rtn = 1;
        break;
    case IOWND_BUFF_NEWLINE_NONE:
    default:
        break;
    }

    return rtn;
}

/********************************************************************************
 * 内容  : 行番号の更新
 * 引数  : S_BUFF_LINE_DATA *dataPtr
 * 戻り値: なし
 ***************************************/
static void
updateLineNum( S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr;
    DWORD i;

    if( dataPtr != NULL )
    {
        i = dataPtr->lineNum;

        for( nowPtr=dataPtr; (nowPtr != NULL); nowPtr=nowPtr->nextPtr,i++ )
        {
            nowPtr->lineNum = i;
        }
    }
    else
    {
        nop();
    }
}
