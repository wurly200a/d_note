/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"
static void clearLinkedList ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr );
static void addLinkedList   ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void insertLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *replaceLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *removeLinkedList ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void mergeLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
static void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr );

static CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );
static BOOL getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );
static INT getNewLineSize( void );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

/* 内部変数定義 */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;    /* 先頭のデータ                           */
S_BUFF_LINE_DATA *ioWndBuffListEndPtr;    /* 最後のデータ                           */
S_BUFF_LINE_DATA *ioWndBuffLineNowPtr;    /* キャレットがあるデータ                 */
S_BUFF_LINE_DATA *ioWndBuffLineSelectPtr; /* 選択範囲の先頭のデータ                 */
DWORD selectCaretPos;                     /* 選択範囲の先頭のデータのキャレット位置 */

typedef struct
{
    UINT  NewLineType;
    DWORD xCaret;
    DWORD yCaret;
    INT   tabSize;
} S_IOWND_BUFF_DATA;

S_IOWND_BUFF_DATA ioWndBuffData = {IOWND_BUFF_NEWLINE_CRLF,0,0,8};

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

    ioWndBuffLineSelectPtr = NULL;
    selectCaretPos = 0;
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
 * 引数  : BOOL   bInit  (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void
IoWndBuffDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    S_BUFF_LINE_DATA *lineDataPtr;
    S_BUFF_LINE_DATA *tempTopPtr = NULL;
    S_BUFF_LINE_DATA *tempEndPtr = NULL;
    S_BUFF_LINE_DATA *newPtr,*targetPtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    DWORD lineLengthSum = 0;
    DWORD caretPos = 0;

    if( bInit )
    {
        IoWndBuffInit();
    }
    else
    {
        nop();
    }

    if( (dataPtr != NULL) && (length > 0) )
    { /* データ有りの場合 */

        /* 改行で分割したデータを仮連結リスト(tempTopPtr〜tempEndPtr)に登録(ここから) */
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
        /* 改行で分割したデータを仮連結リスト(tempTopPtr〜tempEndPtr)に登録(ここまで) */

        if( ioWndBuffLineNowPtr->caretPos == 0 )
        { /* 行の先頭に挿入 */
            mergeLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

            if( tempEndPtr->newLineCodeSize == 0 )
            { /* 挿入データの最後に改行がなければ */
                newPtr = joinData(tempEndPtr,ioWndBuffLineNowPtr); /* 追加データの最終行と現在の行と結合 */
                if( newPtr != NULL )
                {
                    targetPtr = ioWndBuffLineNowPtr;
                    ioWndBuffLineNowPtr = replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr ); /* 現在の行を置き換え */
                    free( targetPtr );                                                       /* 現在の行のデータを解放                   */
                    caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;           /* キャレット位置は追加データの最終行の最後 */
                    removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr); /* 最終行自体は不要なので連結リストから外す */
                    free( tempEndPtr );                                                      /* データ自体を解放                         */

                    ioWndBuffLineNowPtr = newPtr;                                            /* 結合したものを現在行とする */
                    ioWndBuffLineNowPtr->caretPos = caretPos;                                /* キャレット位置は結合位置     */
                }
                else
                {
                    nop();
                }
            }
            else
            { /* 挿入データの最後に改行があった */
                ioWndBuffLineNowPtr->caretPos = 0;
            }
        }
        else
        { /* 行の途中に挿入 */
            /* 現在の行をキャレット位置で分割(ここから) */
            divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr );
            if( dividePrePtr != NULL )
            {
                if( dividePostPtr != NULL )
                {
                    replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,dividePostPtr ); /* まず 古いデータを改行以降のデータに置き換え */
                    insertLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,dividePrePtr ); /* その前に改行前のデータを挿入 */
                    free( ioWndBuffLineNowPtr );
                    ioWndBuffLineNowPtr = dividePostPtr;
                    ioWndBuffLineNowPtr->caretPos = 0;
                    /* 現在の行をキャレット位置で分割(ここまで) */

                    if( tempTopPtr == tempEndPtr )
                    { /* 1行挿入の場合 */
                        insertLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,tempEndPtr );

                        /* 分割した前方データと、挿入データの先頭を結合(ここから) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
                            free( dividePrePtr );
                        }
                        else
                        {
                            nop();
                        }
                        tempTopPtr = tempEndPtr = newPtr;

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* 挿入データの最後に改行がなければ */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* 追加データの最終行と改行以降の行と結合 */
                            if( newPtr != NULL )
                            {
                                newPtr->caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
                                free( tempEndPtr );
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else
                        { /* 挿入データの最後に改行有り */
                            nop();
                        }
                    }
                    else
                    { /* 複数行挿入の場合 */
                        /* 分割した位置にデータを挿入(ここから) */
                        mergeLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* 挿入データの最後に改行がなければ */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* 追加データの最終行と改行以降の行と結合 */
                            if( newPtr != NULL )
                            {
                                newPtr->caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
                                free( tempEndPtr );
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else
                        { /* 挿入データの最後に改行有り */
                            nop();
                        }

                        /* 分割した前方データと、挿入データの先頭を結合(ここから) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
                            free( dividePrePtr );
                        }
                        else
                        {
                            nop();
                        }
                    }
                }
                else
                {
                    free( dividePrePtr );
                }
            }
            else
            {
                nop();
            }
        }
    }
    else
    { /* データ無しの場合 */
        nop();
    }

    if( bInit )
    {
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
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータ取得
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffDataGet( TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region )
{
    BOOL rtn = FALSE;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;
    DWORD size;

    if( region == BUFF_ALL )
    {
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
    }
    else
    {
        if( dataPtr != NULL )
        {
            if( ioWndBuffLineSelectPtr != NULL )
            { /* 選択開始位置有り */
                if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
                { /* 正方向に選択 */
                    for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = nowPtr->nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineSelectPtr )
                        {
                            size = (nowPtr->dataSize - selectCaretPos);
                            memcpy( dataPtr, (nowPtr->data + selectCaretPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = nowPtr->caretPos;
                            memcpy( dataPtr,nowPtr->data,size );
                            dataPtr += size;
                        }
                        else
                        {
                            size = nowPtr->dataSize;
                            memcpy( dataPtr, nowPtr->data,size );
                            dataPtr += size;
                        }
                        prevPtr = nowPtr;
                    }
                }
                else if( (ioWndBuffLineSelectPtr->lineNum) > (ioWndBuffLineNowPtr->lineNum) )
                { /* 負方向に選択 */
                    for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = nowPtr->nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = (nowPtr->dataSize - nowPtr->caretPos);
                            memcpy( dataPtr, (nowPtr->data + nowPtr->caretPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == ioWndBuffLineSelectPtr )
                        {
                            size = selectCaretPos;
                            memcpy( dataPtr,nowPtr->data,size );
                            dataPtr += size;
                        }
                        else
                        {
                            size = nowPtr->dataSize;
                            memcpy( dataPtr, nowPtr->data,size );
                            dataPtr += size;
                        }
                        prevPtr = nowPtr;
                    }
                }
                else
                { /* 同一行内で選択 */
                    if( ioWndBuffLineNowPtr == ioWndBuffLineSelectPtr )
                    {
                        if( ioWndBuffLineNowPtr->caretPos < selectCaretPos )
                        {
                            size = (selectCaretPos - ioWndBuffLineNowPtr->caretPos);
                            memcpy( dataPtr, (ioWndBuffLineNowPtr->data + ioWndBuffLineNowPtr->caretPos),size );
                            dataPtr += size;
                        }
                        else if( ioWndBuffLineNowPtr->caretPos > selectCaretPos )
                        {
                            size = (ioWndBuffLineNowPtr->caretPos - selectCaretPos);
                            memcpy( dataPtr, (ioWndBuffLineNowPtr->data + selectCaretPos),size );
                            dataPtr += size;
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
        else
        {
            nop();
        }
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
 * 内容  : 連結リストにデータを挿入する
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr  挿入位置
 * 引数  : S_BUFF_LINE_DATA *dataPtr 挿入するデータ
 * 戻り値: なし
 ***************************************/
static void
insertLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->prevPtr = nowPtr->prevPtr;
        dataPtr->nextPtr = nowPtr;
        dataPtr->lineNum = nowPtr->lineNum;

        if( dataPtr->prevPtr != NULL )
        { /* 前データ有り */
            (dataPtr->prevPtr)->nextPtr = dataPtr;
        }
        else
        { /* 前データ無し */
            (*topPtr) = dataPtr;
        }

        nowPtr->prevPtr = dataPtr;
        (nowPtr->lineNum)++;

        updateLineNum( nowPtr );
    }
    else
    {
        nop();
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
                updateLineNum( (dataPtr->prevPtr)->nextPtr );
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
                updateLineNum( (*topPtr) );
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
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr  挿入位置
 * 引数  : S_BUFF_LINE_DATA **mergeTopPtr
 * 引数  : S_BUFF_LINE_DATA **mergeEndPtr
 * 戻り値: void
 ***************************************/
static void
mergeLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (mergeTopPtr != NULL) && (mergeEndPtr != NULL) )
    {
        (*mergeTopPtr)->prevPtr = nowPtr->prevPtr;

        if( (*mergeTopPtr)->prevPtr != NULL )
        {
            (*mergeTopPtr)->lineNum = ((*mergeTopPtr)->prevPtr)->lineNum+1;
            ((*mergeTopPtr)->prevPtr)->nextPtr = (*mergeTopPtr);
        }
        else
        {
            (*mergeTopPtr)->lineNum = 0;
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

        updateLineNum( (*mergeTopPtr) );
    }
    else
    {
        nop();
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

            newPtr->prevPtr = NULL;
            newPtr->nextPtr = NULL;
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
 * 内容  : 行データの分割
 * 引数  : S_BUFF_LINE_DATA *dataPtr  分割するデータ
 * 引数  : S_BUFF_LINE_DATA **new1Ptr 分割後のデータ1 (メモリ確保する)
 * 引数  : S_BUFF_LINE_DATA **new2Ptr 分割後のデータ2 (メモリ確保する)
 * 戻り値: なし
 ***************************************/
static void
divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( (dataPtr != NULL) && (new1Ptr != NULL) && (new2Ptr != NULL) )
    {
        *new1Ptr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (dataPtr->caretPos) * sizeof(TCHAR));

        if( *new1Ptr != NULL )
        {
            *new2Ptr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((dataPtr->dataSize-dataPtr->caretPos) * sizeof(TCHAR)) );
            if( *new2Ptr != NULL )
            {
                /* 改行より前のデータ生成 */
                (*new1Ptr)->dataSize = dataPtr->caretPos;
                (*new1Ptr)->newLineCodeSize = 0;
                (*new1Ptr)->lineNum = dataPtr->lineNum;
                (*new1Ptr)->caretPos = dataPtr->caretPos;

                memcpy( (*new1Ptr)->data, dataPtr->data, (*new1Ptr)->dataSize );

                /* 改行以降のデータ生成 */
                (*new2Ptr)->dataSize = dataPtr->dataSize - dataPtr->caretPos;
                (*new2Ptr)->newLineCodeSize = dataPtr->newLineCodeSize;
                (*new2Ptr)->lineNum = (*new1Ptr)->lineNum+1;
                (*new2Ptr)->caretPos = 0;
                memcpy( (*new2Ptr)->data, dataPtr->data+dataPtr->caretPos, (*new2Ptr)->dataSize );
            }
            else
            {
                free( *new1Ptr );
                *new1Ptr = NULL;
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
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( IOWND_BUFF_REGION region )
{
    DWORD dataSize = 0;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;

    if( region == BUFF_ALL )
    {
        for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
        {
            dataSize += nowPtr->dataSize;
        }
    }
    else
    {
        if( ioWndBuffLineSelectPtr != NULL )
        { /* 選択開始位置有り */
            if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
            { /* 正方向に選択 */
                for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = nowPtr->nextPtr )
                {
                    if( nowPtr == ioWndBuffLineSelectPtr )
                    {
                        dataSize += (nowPtr->dataSize - selectCaretPos);
                    }
                    else if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += nowPtr->caretPos;
                    }
                    else
                    {
                        dataSize += nowPtr->dataSize;
                    }
                    prevPtr = nowPtr;
                }
            }
            else if( (ioWndBuffLineSelectPtr->lineNum) > (ioWndBuffLineNowPtr->lineNum) )
            { /* 負方向に選択 */
                for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = nowPtr->nextPtr )
                {
                    if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += (nowPtr->dataSize - nowPtr->caretPos);
                    }
                    else if( nowPtr == ioWndBuffLineSelectPtr )
                    {
                        dataSize += selectCaretPos;
                    }
                    else
                    {
                        dataSize += nowPtr->dataSize;
                    }
                    prevPtr = nowPtr;
                }
            }
            else
            { /* 同一行内で選択 */
                if( ioWndBuffLineNowPtr == ioWndBuffLineSelectPtr )
                {
                    if( ioWndBuffLineNowPtr->caretPos < selectCaretPos )
                    {
                        dataSize += (selectCaretPos - ioWndBuffLineNowPtr->caretPos);
                    }
                    else if( ioWndBuffLineNowPtr->caretPos > selectCaretPos )
                    {
                        dataSize += (ioWndBuffLineNowPtr->caretPos - selectCaretPos);
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
    DWORD i,j=0,k;
    DWORD literalMaxSize;
    int tab_offset;
    S_BUFF_LINE_DATA *linePtr;
    DWORD dispPos;
    CHARSET_TYPE charType;

    if( ioWndBuffLineNowPtr != NULL )
    {
        linePtr = ioWndBuffLineNowPtr;
        dispPos = linePtr->caretPos;
        charType = SINGLE_CHAR;
        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( i=0,j=0; (i<dispPos)&&(i<literalMaxSize); i++ )
        { /* 1行中のデータを1文字ずつ処理 */
            if( charType == DOUBLE_CHAR_HIGH )
            { /* 前文字が2byte文字の上位byteだったら */
                charType = DOUBLE_CHAR_LOW;
                j++;
            }
            else
            { /* 前文字が2byte文字の上位byte以外 */
                if( (*(linePtr->data+i)) == '\0' )
                {
                    charType = SINGLE_CHAR;
                    j++;
                }
                else if( (*(linePtr->data+i)) == '\t' )
                { /* 処理中の文字はTAB */
                    charType = TAB_CHAR;

                    tab_offset = ioWndBuffData.tabSize - (j % ioWndBuffData.tabSize);
                    j+=tab_offset;
                }
                else if( ( (BYTE)(*(linePtr->data+i)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xDF)) ||
                         (((BYTE)0xF0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xFF)) )
                { /* 処理中の文字は1byte文字 */
                    charType = SINGLE_CHAR;
                    j++;
                }
                else
                { /* 処理中の文字は2byte文字の上位byte */
                    charType = DOUBLE_CHAR_HIGH;
                    j++;
                }
            }
        }
    }
    else
    {
        nop();
    }

    return j;
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
 * 内容  : 指定行、指定列のデータを取得
 * 引数  : DWORD  lineNum   行
 * 引数  : DWORD  dispPos   表示位置
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffGetDispData( DWORD lineNum, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;
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
                getDispCharData( nowPtr, dispPos, dataPtr );
                break;
            }
            else
            {
                nop();
            }
        }
    }

    return TRUE;
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
        allDataSize = IoWndGetBuffSize(BUFF_ALL);
        dataTopPtr = malloc( sizeof(TCHAR) * allDataSize );
        if( dataTopPtr != NULL )
        {
            for( nowPtr=ioWndBuffListTopPtr,dataPtr=dataTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
            {
                memcpy( dataPtr, nowPtr->data, nowPtr->dataSize );
                dataPtr += nowPtr->dataSize;
            }
            IoWndBuffDataSet( dataTopPtr, allDataSize, TRUE );
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

                if( newPtr != NULL )
                {
                    /* 行番号、キャレット位置を前行データによって決まる */
                    newPtr->lineNum  = prevPtr->lineNum;
                    newPtr->caretPos = prevPtr->dataSize - prevPtr->newLineCodeSize;

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
    TCHAR data[2];
    INT size = 0;

    switch( ioWndBuffData.NewLineType )
    {
    case IOWND_BUFF_NEWLINE_CRLF:
        data[0] = '\r';
        data[1] = '\n';
        size = 2;
        break;
    case IOWND_BUFF_NEWLINE_LF  :
        data[0] = '\n';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_CR  :
        data[0] = '\r';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_NONE:
    default:
        break;
    }

    if( size )
    {
        IoWndBuffDataSet( data, size, FALSE );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのタブサイズセット
 * 引数  : INT tabSize
 * 戻り値: BOOL (TRUE:変更された)
 ***************************************/
BOOL
IoWndBuffSetTabSize( INT tabSize )
{
    BOOL bRtn = FALSE;

    if( tabSize != ioWndBuffData.tabSize )
    {
        ioWndBuffData.tabSize = tabSize;
        bRtn = TRUE;
    }
    else
    {
        nop();
    }

    return bRtn;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの範囲選択ON
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSelectOn( void )
{
    if( ioWndBuffLineSelectPtr )
    { /* 既に選択済みの場合 */
        nop(); /* 変更しない */
    }
    else
    {
        ioWndBuffLineSelectPtr = ioWndBuffLineNowPtr;
        selectCaretPos = ioWndBuffLineNowPtr->caretPos;
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの範囲選択OFF
 * 引数  : なし
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSelectOff( void )
{
    ioWndBuffLineSelectPtr = NULL;
    selectCaretPos = 0;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの全範囲選択
 * 引数  : なし
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffSelectAll( void )
{
    BOOL bRtn = FALSE;

    if( ioWndBuffListTopPtr != NULL )
    {
        if( (ioWndBuffLineSelectPtr == ioWndBuffListTopPtr) &&
            (selectCaretPos == 0) &&
            (ioWndBuffLineNowPtr == ioWndBuffListEndPtr) &&
            (ioWndBuffLineNowPtr->caretPos == (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) ) )
        {
            nop();
        }
        else
        {
            ioWndBuffLineSelectPtr = ioWndBuffListTopPtr;
            selectCaretPos = 0;
            ioWndBuffLineNowPtr = ioWndBuffListEndPtr;
            ioWndBuffLineNowPtr->caretPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
            bRtn = TRUE;
        }
    }
    else
    {
        nop();
    }

    return bRtn;
}

/********************************************************************************
 * 内容  : キャラクタセットの判断(Shift_JISの場合)
 * 引数  : S_BUFF_LINE_DATA *dataPtr
 * 引数  : DWORD            offset   判断したい文字の先頭からのオフセット(0 origin)
 * 戻り値: CHARSET_TYPE
 ***************************************/
static CHARSET_TYPE
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
 * 内容  : 指定位置の表示データを取得する
 * 引数  : S_BUFF_LINE_DATA *linePtr   行データ
 * 引数  : DWORD             dispPos   列位置(表示上の)
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
static BOOL
getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    DWORD i,j,k;
    DWORD literalMaxSize;
    int tab_offset;

    if( (linePtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->size    = 0;
        dataPtr->type    = SINGLE_CHAR;
        dataPtr->bSelect = FALSE;

        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( i=0,j=0; i<literalMaxSize; i++ )
        { /* 1行中のデータを1文字ずつ処理 */
            if( dataPtr->type == DOUBLE_CHAR_HIGH )
            { /* 前文字が2byte文字の上位byteだったら */
                dataPtr->type = DOUBLE_CHAR_LOW;
                *(dataPtr->data+1) = *(linePtr->data+i);
            }
            else
            { /* 前文字が2byte文字の上位byte以外 */
                if( (*(linePtr->data+i)) == '\0' )
                {
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = ' ';
                }
                else if( (*(linePtr->data+i)) == '\t' )
                { /* 処理中の文字はTAB */
                    dataPtr->type = TAB_CHAR;

                    tab_offset = ioWndBuffData.tabSize - (j % ioWndBuffData.tabSize);

                    if( dispPos < (j+tab_offset) )
                    {
                        for(k=0;k<tab_offset;k++)
                        {
                            *(dataPtr->data+k) = ' ';
                        }
                        dataPtr->size = tab_offset;
                        dataPtr->offset = dispPos - j;
                        break;
                    }
                    else
                    {
                        j+=tab_offset;
                        continue;
                    }
                }
                else if( ( (BYTE)(*(linePtr->data+i)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xDF)) ||
                         (((BYTE)0xF0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xFF)) )
                { /* 処理中の文字は1byte文字 */
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = *(linePtr->data+i);
                }
                else
                { /* 処理中の文字は2byte文字の上位byte */
                    dataPtr->type = DOUBLE_CHAR_HIGH;
                    *(dataPtr->data)   = *(linePtr->data+i);
                    *(dataPtr->data+1) = *(linePtr->data+i+1);
                }
            }

            if( j == dispPos )
            { /* 処理中の文字が指定された表示位置 */
                break;
            }
            else
            {
                nop();
            }

            j++;
        }

        if( i >= literalMaxSize )
        {
            nop();
        }
        else
        {
            switch( dataPtr->type )
            {
            case SINGLE_CHAR:
                dataPtr->size = 1;
                dataPtr->offset = 0;
                break;
            case DOUBLE_CHAR_HIGH:
                dataPtr->size = 2;
                dataPtr->offset = 0;
                break;
            case DOUBLE_CHAR_LOW:
                dataPtr->size = 2;
                dataPtr->offset = 1;
                break;
            defalut:
                break;
            }

            if( ioWndBuffLineSelectPtr != NULL )
            { /* 選択開始位置有り */
                if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
                { /* 正方向に選択 */
                    if( (ioWndBuffLineSelectPtr->lineNum < linePtr->lineNum) && (linePtr->lineNum < ioWndBuffLineNowPtr->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (selectCaretPos <= i) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (i < linePtr->caretPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else
                    {
                        nop();
                    }
                }
                else if( (ioWndBuffLineSelectPtr->lineNum) > (ioWndBuffLineNowPtr->lineNum) )
                { /* 負方向に選択 */
                    if( (ioWndBuffLineNowPtr->lineNum < linePtr->lineNum) && (linePtr->lineNum < ioWndBuffLineSelectPtr->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (linePtr->caretPos <= i) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (i < selectCaretPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else
                    {
                        nop();
                    }
                }
                else
                { /* 同一行内で選択 */
                    if( ioWndBuffLineNowPtr == linePtr )
                    {
                        if( linePtr->caretPos < selectCaretPos )
                        {
                            if( (linePtr->caretPos <= i) && (i < selectCaretPos) )
                            {
                                dataPtr->bSelect = TRUE;
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else if( linePtr->caretPos > selectCaretPos )
                        {
                            if( (selectCaretPos <= i) && (i < linePtr->caretPos) )
                            {
                                dataPtr->bSelect = TRUE;
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
                        nop();
                    }
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

    return TRUE;
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

