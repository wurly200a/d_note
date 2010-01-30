/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"
#include "LinkedList.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"
static S_BUFF_LINE_DATA *createBuffLineData( DWORD size, INT newLineCodeSize, TCHAR *dataPtr, DWORD lineNum, DWORD caretPos );
static void destroyBuffLineData( S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
static void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr );
static S_BUFF_LINE_DATA *shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size );
static CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );
static DWORD getCaretDispXpos( S_BUFF_LINE_DATA *linePtr, DWORD dataPos );
static BOOL getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );
static INT getNewLineSize( void );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

static void addLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void removeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void mergeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr );
static S_BUFF_LINE_DATA *replaceLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static void insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );

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

    ClearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);

    /* 空データを追加 */
    lineDataPtr = createBuffLineData( 0, 0, NULL, 0, 0 );
    if( lineDataPtr != NULL )
    {
        addLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,lineDataPtr );
        ioWndBuffLineNowPtr = lineDataPtr;
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
    ClearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);
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

        /* 改行で分割したデータを仮連結リスト(tempTopPtr～tempEndPtr)に登録(ここから) */
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                addLineData( &tempTopPtr,&tempEndPtr,lineDataPtr );
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
        }
        /* 改行で分割したデータを仮連結リスト(tempTopPtr～tempEndPtr)に登録(ここまで) */

        if( ioWndBuffLineNowPtr->caretDataPos == 0 )
        { /* 行の先頭に挿入 */
            mergeLineData(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

            if( tempEndPtr->newLineCodeSize == 0 )
            { /* 挿入データの最後に改行がなければ */
                newPtr = joinData(tempEndPtr,ioWndBuffLineNowPtr); /* 追加データの最終行と現在の行と結合した新たなデータ生成 */
                if( newPtr != NULL )
                {
                    targetPtr = ioWndBuffLineNowPtr;
                    ioWndBuffLineNowPtr = replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr ); /* 現在の行を置き換え */
                    free( targetPtr );                                                       /* 現在の行のデータを解放                   */

                    caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;           /* キャレット位置は追加データの最終行の最後 */
                    removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr); /* 最終行自体は不要なので連結リストから外す */
                    free( tempEndPtr );

                    ioWndBuffLineNowPtr = newPtr;                                            /* 結合したものを現在行とする */
                    ioWndBuffLineNowPtr->caretDataPos = caretPos;                                /* キャレット位置は結合位置     */
                }
                else
                {
                    nop();
                }
            }
            else
            { /* 挿入データの最後に改行があった */
                ioWndBuffLineNowPtr->caretDataPos = 0;
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
                    replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,dividePostPtr ); /* まず 古いデータを改行以降のデータに置き換え */
                    insertLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,dividePrePtr ); /* その前に改行前のデータを挿入 */

                    free( ioWndBuffLineNowPtr );
                    ioWndBuffLineNowPtr = dividePostPtr;
                    ioWndBuffLineNowPtr->caretDataPos = 0;
                    /* 現在の行をキャレット位置で分割(ここまで) */

                    if( tempTopPtr == tempEndPtr )
                    { /* 1行挿入の場合 */
                        insertLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,tempEndPtr );

                        /* 分割した前方データと、挿入データの先頭を結合(ここから) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
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
                                newPtr->caretDataPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
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
                        mergeLineData(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* 挿入データの最後に改行がなければ */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* 追加データの最終行と改行以降の行と結合 */
                            if( newPtr != NULL )
                            {
                                newPtr->caretDataPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
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
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
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
            ioWndBuffLineNowPtr->caretDataPos = 0;
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
                for( nowPtr=ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
                    for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineSelectPtr )
                        {
                            size = (nowPtr->dataSize - selectCaretPos);
                            memcpy( dataPtr, (nowPtr->data + selectCaretPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = nowPtr->caretDataPos;
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
                    for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = (nowPtr->dataSize - nowPtr->caretDataPos);
                            memcpy( dataPtr, (nowPtr->data + nowPtr->caretDataPos),size );
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
                        if( ioWndBuffLineNowPtr->caretDataPos < selectCaretPos )
                        {
                            size = (selectCaretPos - ioWndBuffLineNowPtr->caretDataPos);
                            memcpy( dataPtr, (ioWndBuffLineNowPtr->data + ioWndBuffLineNowPtr->caretDataPos),size );
                            dataPtr += size;
                        }
                        else if( ioWndBuffLineNowPtr->caretDataPos > selectCaretPos )
                        {
                            size = (ioWndBuffLineNowPtr->caretDataPos - selectCaretPos);
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
        for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
                for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == ioWndBuffLineSelectPtr )
                    {
                        dataSize += (nowPtr->dataSize - selectCaretPos);
                    }
                    else if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += nowPtr->caretDataPos;
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
                for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += (nowPtr->dataSize - nowPtr->caretDataPos);
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
                    if( ioWndBuffLineNowPtr->caretDataPos < selectCaretPos )
                    {
                        dataSize += (selectCaretPos - ioWndBuffLineNowPtr->caretDataPos);
                    }
                    else if( ioWndBuffLineNowPtr->caretDataPos > selectCaretPos )
                    {
                        dataSize += (ioWndBuffLineNowPtr->caretDataPos - selectCaretPos);
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

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
    return getCaretDispXpos(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos);
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
    S_BUFF_DISP_DATA dispData;

    nowPtr = IoWndBuffGetLinePtr( lineNum );

    if( nowPtr != NULL )
    {
        ioWndBuffLineNowPtr = nowPtr;

        getDispCharData( ioWndBuffLineNowPtr, xPos, &dispData );
        ioWndBuffLineNowPtr->caretDataPos = dispData.caretPos;
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
        if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos) == DOUBLE_CHAR_HIGH )
        { /* 次の文字で移動量を判断 */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( (ioWndBuffLineNowPtr->caretDataPos + moveAmount) > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
        { /* キャレットはすでに行の最右端 */
            if( (ioWndBuffLineNowPtr->header.nextPtr != NULL) )
            { /* 次行有り */
                ioWndBuffLineNowPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.nextPtr;
                ioWndBuffLineNowPtr->caretDataPos = 0;
            }
            else
            { /* 次行無し */
                nop();
            }
        }
        else
        { /* まだ右へ移動可能 */
            ioWndBuffLineNowPtr->caretDataPos += moveAmount;
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
        if( (ioWndBuffLineNowPtr->caretDataPos>0) &&
            (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos-1) == DOUBLE_CHAR_LOW) )
        { /* 前の文字で移動量を判断 */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( ioWndBuffLineNowPtr->caretDataPos == 0 )
        { /* キャレットはすでに行の最左端 */
            if( (ioWndBuffLineNowPtr->header.prevPtr != NULL) )
            { /* 前行有り */
                ioWndBuffLineNowPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.prevPtr;
                ioWndBuffLineNowPtr->caretDataPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
            }
            else
            { /* 前行無し */
                nop();
            }
        }
        else
        { /* まだ左へ移動可能 */
            ioWndBuffLineNowPtr->caretDataPos = max( ioWndBuffLineNowPtr->caretDataPos - moveAmount, 0 );
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
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *nextPtr;
    S_BUFF_DISP_DATA dispData;

    nextPtr = (S_BUFF_LINE_DATA *)(ioWndBuffLineNowPtr->header.nextPtr);

    if( (ioWndBuffLineNowPtr != NULL) && (nextPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos);
        getDispCharData( nextPtr, preDispPos, &dispData );
        nextPtr->caretDataPos = dispData.caretPos;
        ioWndBuffLineNowPtr = nextPtr;
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
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *prevPtr;
    S_BUFF_DISP_DATA dispData;

    prevPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.prevPtr;

    if( (ioWndBuffLineNowPtr != NULL) && (prevPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos);
        getDispCharData( prevPtr, preDispPos, &dispData );
        prevPtr->caretDataPos = dispData.caretPos;
        ioWndBuffLineNowPtr = prevPtr;
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
            nextPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr;

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
 * 引数  : S_BUFF_LINE_DATA *lineDataPtr
 * 引数  : DWORD             dispPos     表示位置
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffGetDispData( S_BUFF_LINE_DATA *lineDataPtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    getDispCharData( lineDataPtr, dispPos, dataPtr );

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
            for( nowPtr=ioWndBuffListTopPtr,dataPtr=dataTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
 * 引数  : BOOL bBackSpace
 * 戻り値: なし
 ***************************************/
void
IoWndBuffRemoveData( BOOL bBackSpace )
{
    DWORD removeSize = 0,saveCaretPos;
    S_BUFF_LINE_DATA *newPtr = NULL,*prevPtr = NULL,*nextPtr = NULL,*nowPtr = NULL,*savePtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    BOOL bValid = FALSE;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineSelectPtr != NULL )
        { /* 選択開始位置有り */
            if( (ioWndBuffLineSelectPtr->lineNum) == (ioWndBuffLineNowPtr->lineNum) )
            { /* 同一行内で選択 */
                if( ioWndBuffLineNowPtr == ioWndBuffLineSelectPtr )
                {
                    if( ioWndBuffLineNowPtr->caretDataPos < selectCaretPos )
                    {
                        removeSize += (selectCaretPos - ioWndBuffLineNowPtr->caretDataPos);
                        saveCaretPos = ioWndBuffLineNowPtr->caretDataPos;
                        ioWndBuffLineNowPtr->caretDataPos = selectCaretPos;
                        selectCaretPos = saveCaretPos;
                    }
                    else if( ioWndBuffLineNowPtr->caretDataPos > selectCaretPos )
                    {
                        removeSize += (ioWndBuffLineNowPtr->caretDataPos - selectCaretPos);
                    }
                    else
                    {
                        nop();
                    }

                    divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                    newPtr = shortenData( dividePrePtr, removeSize );                 /* 分割後の前方データの末尾から所定量削除 */
                    destroyBuffLineData( dividePrePtr );                                 /* 前方データを */
                    dividePrePtr = newPtr;                                            /* 書き換え     */

                    newPtr = joinData( dividePrePtr, dividePostPtr );                 /* 再結合 */
                    newPtr->lineNum = ioWndBuffLineNowPtr->lineNum;                   /* 行番号は同じ */
                    newPtr->caretDataPos = ioWndBuffLineNowPtr->caretDataPos - removeSize;    /* キャレット位置は削除量分前方になる */
                    destroyBuffLineData( dividePrePtr );                                 /* 作業データを削除 */
                    destroyBuffLineData( dividePostPtr );                                /* 作業データを削除 */

                    /* 古い行データと置き換える */
                    replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
                    destroyBuffLineData( ioWndBuffLineNowPtr );
                    ioWndBuffLineNowPtr = newPtr;
                }
                else
                {
                    nop();
                }
            }
            else
            {
                if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
                { /* 正方向に選択 */
                    nop();
                }
                else
                { /* 負方向に選択 */
                    savePtr = ioWndBuffLineSelectPtr;
                    saveCaretPos = selectCaretPos;
                    ioWndBuffLineSelectPtr = ioWndBuffLineNowPtr;
                    selectCaretPos = ioWndBuffLineNowPtr->caretDataPos;
                    ioWndBuffLineNowPtr = savePtr;
                    ioWndBuffLineNowPtr->caretDataPos = saveCaretPos;
                }

                /* 先頭から最後まで選択された行を削除 */
                for( nowPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineSelectPtr->header.nextPtr; nowPtr != ioWndBuffLineNowPtr; nowPtr = nextPtr )
                {
                    nextPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr;
                    removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,nowPtr);
                    destroyBuffLineData( nowPtr );
                }
                /* 先頭行のうち、選択位置から前を残す */
                if( selectCaretPos == 0 )
                { /* 選択位置が先頭だったら、1行丸々削除 */
                    removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineSelectPtr);
                }
                else
                {
                    ioWndBuffLineSelectPtr->caretDataPos = selectCaretPos;
                    divideData( ioWndBuffLineSelectPtr, &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                    replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineSelectPtr,dividePrePtr );
                    destroyBuffLineData( ioWndBuffLineSelectPtr );
                }

                /* 最終行のうち、選択位置から後を残す */
                divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,dividePostPtr );
                destroyBuffLineData( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = dividePostPtr;
                ioWndBuffLineNowPtr->caretDataPos = 0;
                updateLineNum( ioWndBuffLineNowPtr );
            }

            ioWndBuffLineSelectPtr = NULL;
            selectCaretPos = 0;
        }
        else
        { /* 選択無し */
            if( bBackSpace )
            { /* BSキー */
                if( ioWndBuffLineNowPtr->caretDataPos > 0  )
                {
                    bValid = TRUE;

                    if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos-1) == DOUBLE_CHAR_LOW )
                    { /* 次の文字で削除量を判断 */
                        removeSize = 2;
                    }
                    else
                    {
                        removeSize = 1;
                    }
                }
                else
                { /* キャレットが行の先頭位置。つまり、前行との結合。*/
                    if( ioWndBuffLineNowPtr->header.prevPtr != NULL )
                    {
                        prevPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.prevPtr;
                        newPtr = joinData( prevPtr,ioWndBuffLineNowPtr ); /* 前行と本行を結合した新しい行データを生成 */

                        if( newPtr != NULL )
                        {
                            /* 行番号、キャレット位置を前行データによって決まる */
                            newPtr->lineNum  = prevPtr->lineNum;
                            newPtr->caretDataPos = prevPtr->dataSize - prevPtr->newLineCodeSize;

                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr ); /* 本行は削除 */
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,prevPtr,newPtr );    /* 前行は新しい行データに置き換える */

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
            { /* DELキー */
                if( ioWndBuffLineNowPtr->caretDataPos != (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
                {
                    bValid = TRUE;

                    if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos) == DOUBLE_CHAR_HIGH )
                    { /* キャレット位置の文字で削除量を判断 */
                        removeSize = 2;
                    }
                    else
                    {
                        removeSize = 1;
                    }
                    ioWndBuffLineNowPtr->caretDataPos += removeSize;
                }
                else
                { /* キャレットが行の最終位置。つまり、次行との結合。*/
                    if( ioWndBuffLineNowPtr->header.nextPtr != NULL )
                    {
                        nextPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.nextPtr;
                        newPtr = joinData( ioWndBuffLineNowPtr,nextPtr ); /* 本行と次行を結合した新しい行データを生成 */

                        if( newPtr != NULL )
                        {
                            /* 行番号、キャレット位置は本行データによって決まる */
                            newPtr->lineNum  = ioWndBuffLineNowPtr->lineNum;
                            newPtr->caretDataPos = ioWndBuffLineNowPtr->caretDataPos;

                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,nextPtr );         /* 次行は削除 */
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr ); /* 本行は新しい行データに置き換える */

                            free( ioWndBuffLineNowPtr );
                            free( nextPtr );
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

            if( bValid )
            { /* 削除有効 */
                divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                newPtr = shortenData( dividePrePtr, removeSize );                 /* 分割後の前方データの末尾から所定量削除 */
                destroyBuffLineData( dividePrePtr );                                 /* 前方データを */
                dividePrePtr = newPtr;                                            /* 書き換え     */

                newPtr = joinData( dividePrePtr, dividePostPtr );                 /* 再結合 */
                newPtr->lineNum = ioWndBuffLineNowPtr->lineNum;                   /* 行番号は同じ */
                newPtr->caretDataPos = ioWndBuffLineNowPtr->caretDataPos - removeSize;    /* キャレット位置は削除量分前方になる */
                destroyBuffLineData( dividePrePtr );                                 /* 作業データを削除 */
                destroyBuffLineData( dividePostPtr );                                /* 作業データを削除 */

                /* 古い行データと置き換える */
                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
                destroyBuffLineData( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = newPtr;
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
        selectCaretPos = ioWndBuffLineNowPtr->caretDataPos;
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
            (ioWndBuffLineNowPtr->caretDataPos == (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) ) )
        {
            nop();
        }
        else
        {
            ioWndBuffLineSelectPtr = ioWndBuffListTopPtr;
            selectCaretPos = 0;
            ioWndBuffLineNowPtr = ioWndBuffListEndPtr;
            ioWndBuffLineNowPtr->caretDataPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
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
 * 内容  : 行データの生成
 * 引数  : DWORD size          データサイズ(改行コード含む)
 * 引数  : INT newLineCodeSize 改行コードサイズ
 * 引数  : TCHAR *dataPtr      データ(ポインタ)
 * 引数  : DWORD lineNum       行番号
 * 引数  : DWORD caretPos      キャレット位置
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
createBuffLineData( DWORD size, INT newLineCodeSize, TCHAR *dataPtr, DWORD lineNum, DWORD caretPos )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (size * sizeof(TCHAR)) );
    if( newPtr != NULL )
    {
        memset( newPtr, sizeof(S_BUFF_LINE_DATA), 0 );
        newPtr->lineNum         = lineNum;
        newPtr->caretDataPos        = caretPos;
        newPtr->dataSize        = size;
        newPtr->newLineCodeSize = newLineCodeSize;

        if( dataPtr != NULL )
        {
            memcpy( newPtr->data, dataPtr, size );
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
 * 内容  : 行データの解放
 * 引数  : S_BUFF_LINE_DATA *
 * 戻り値: なし
 ***************************************/
static void
destroyBuffLineData( S_BUFF_LINE_DATA *dataPtr )
{
    free( dataPtr );
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

    lineDataPtr = createBuffLineData( i, newLineCodeSize, dataPtr, 0, 0 );

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
        newPtr = createBuffLineData( data1Ptr->dataSize-data1Ptr->newLineCodeSize+data2Ptr->dataSize, data2Ptr->newLineCodeSize, data1Ptr->data, data1Ptr->lineNum, 0 );
        if( newPtr != NULL )
        {
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
        *new1Ptr = createBuffLineData( dataPtr->caretDataPos                  , 0                       , dataPtr->data                  , dataPtr->lineNum  , dataPtr->caretDataPos ); /* 改行より前 */
        *new2Ptr = createBuffLineData( dataPtr->dataSize-dataPtr->caretDataPos, dataPtr->newLineCodeSize, dataPtr->data+dataPtr->caretDataPos, dataPtr->lineNum+1, 0                 ); /* 改行より後 */
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 行データを短くする
 * 引数  : S_BUFF_LINE_DATA *dataPtr
 * 引数  : DWORD size
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( (dataPtr != NULL) &&
        (dataPtr->dataSize >= size) )
    {
        newPtr = createBuffLineData( dataPtr->dataSize-size, dataPtr->newLineCodeSize, dataPtr->data, dataPtr->lineNum, dataPtr->caretDataPos );
        if( newPtr != NULL )
        {
            /* 改行コードのコピー */
            memcpy( newPtr->data + newPtr->dataSize - newPtr->newLineCodeSize, dataPtr->data + dataPtr->dataSize - dataPtr->newLineCodeSize, dataPtr->newLineCodeSize );
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
 * 内容  : キャレットX表示位置取得
 * 引数  : S_BUFF_LINE_DATA *linePtr   行データ
 * 引数  : DWORD             dataPos   列位置(データ上の)
 * 戻り値: DWORD
 ***************************************/
static DWORD
getCaretDispXpos( S_BUFF_LINE_DATA *linePtr, DWORD dataPos )
{
    DWORD i,j=0,k;
    DWORD literalMaxSize;
    int tab_offset;
    CHARSET_TYPE charType;

    if( linePtr != NULL )
    {
        charType = SINGLE_CHAR;
        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( i=0,j=0; (i<dataPos)&&(i<literalMaxSize); i++ )
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
 * 内容  : 指定位置の表示データを取得する
 * 引数  : S_BUFF_LINE_DATA *linePtr   行データ
 * 引数  : DWORD             dispPos   列位置(表示上の)
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
static BOOL
getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    DWORD dataPos,j,k;
    DWORD literalMaxSize;
    int tab_offset;

    if( (linePtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->size    = 0;
        dataPtr->type    = SINGLE_CHAR;
        dataPtr->bSelect = FALSE;
        dataPtr->dataPos = 0;
        dataPtr->caretPos= 0;

        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( dataPos=0,j=0; dataPos<literalMaxSize; dataPos++ )
        { /* 1行中のデータを1文字ずつ処理 */
            if( dataPtr->type == DOUBLE_CHAR_HIGH )
            { /* 前文字が2byte文字の上位byteだったら */
                dataPtr->type = DOUBLE_CHAR_LOW;
                *(dataPtr->data+1) = *(linePtr->data+dataPos);
            }
            else
            { /* 前文字が2byte文字の上位byte以外 */
                if( (*(linePtr->data+dataPos)) == '\0' )
                {
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = ' ';
                }
                else if( (*(linePtr->data+dataPos)) == '\t' )
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
                else if( ( (BYTE)(*(linePtr->data+dataPos)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+dataPos))) && ((BYTE)(*(linePtr->data+dataPos)) <= (BYTE)0xDF)) ||
                         (((BYTE)0xF0 <= (BYTE)(*(linePtr->data+dataPos))) && ((BYTE)(*(linePtr->data+dataPos)) <= (BYTE)0xFF)) )
                { /* 処理中の文字は1byte文字 */
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = *(linePtr->data+dataPos);
                }
                else
                { /* 処理中の文字は2byte文字の上位byte */
                    dataPtr->type = DOUBLE_CHAR_HIGH;
                    *(dataPtr->data)   = *(linePtr->data+dataPos);
                    *(dataPtr->data+1) = *(linePtr->data+dataPos+1);
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

        if( dataPos == literalMaxSize )
        {
            dataPtr->type = END_CHAR;
        }
        else
        {
            nop();
        }

        dataPtr->dataPos = dataPos;

        if( dataPos >= literalMaxSize )
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
            default:
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
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (selectCaretPos <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (dataPos < linePtr->caretDataPos) )
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
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (linePtr->caretDataPos <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (dataPos < selectCaretPos) )
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
                        if( linePtr->caretDataPos < selectCaretPos )
                        {
                            if( (linePtr->caretDataPos <= dataPos) && (dataPos < selectCaretPos) )
                            {
                                dataPtr->bSelect = TRUE;
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else if( linePtr->caretDataPos > selectCaretPos )
                        {
                            if( (selectCaretPos <= dataPos) && (dataPos < linePtr->caretDataPos) )
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

        if( dataPtr->type == DOUBLE_CHAR_LOW )
        {
            dataPtr->caretPos = dataPtr->dataPos + 1;
        }
        else if( (dataPtr->type == TAB_CHAR) && (dataPtr->offset != 0)  )
        {
            dataPtr->caretPos = dataPtr->dataPos + 1;
        }
        else
        {
            dataPtr->caretPos = dataPtr->dataPos;
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

        for( nowPtr=dataPtr; (nowPtr != NULL); nowPtr=(S_BUFF_LINE_DATA *)nowPtr->header.nextPtr,i++ )
        {
            nowPtr->lineNum = i;
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 行データを追加する
 * 引数  : S_LIST_HEADER **topPtr 先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *dataPtr つなぐデータ
 * 戻り値: なし
 ***************************************/
static void
addLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( *topPtr == NULL )
    {
        dataPtr->lineNum = 0;
    }
    else
    {
        if( *endPtr != NULL )
        {
            dataPtr->lineNum = (*endPtr)->lineNum+1;
        }
        else
        {
            nop(); /* 異常 */
        }
    }

    AddLinkedList( topPtr, endPtr, dataPtr );
}

/********************************************************************************
 * 内容  : 行データを削除する
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データがつないであるポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつないであるポインタ
 * 引数  : S_BUFF_LINE_DATA *dataPtr 削除するデータ
 * 戻り値: S_BUFF_LINE_DATA *        削除した次のデータ
 ***************************************/
static void
removeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *removeNextPtr;

    removeNextPtr = RemoveLinkedList( topPtr,endPtr,dataPtr );
    if( removeNextPtr != NULL )
    {
        if( removeNextPtr->header.prevPtr != NULL )
        {
            updateLineNum( (S_BUFF_LINE_DATA *)removeNextPtr->header.prevPtr );
        }
        else
        {
            removeNextPtr->lineNum = 0;
            updateLineNum( removeNextPtr );
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 行データの結合
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr  挿入位置
 * 引数  : S_BUFF_LINE_DATA **mergeTopPtr
 * 引数  : S_BUFF_LINE_DATA **mergeEndPtr
 * 戻り値: void
 ***************************************/
void
mergeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr )
{
    if( (nowPtr != NULL) && (mergeTopPtr != NULL) )
    {
        if( nowPtr->header.prevPtr != NULL )
        { /* 前行があれば */
            (*mergeTopPtr)->lineNum = ((S_BUFF_LINE_DATA *)(nowPtr->header.prevPtr))->lineNum+1;
        }
        else
        {
            (*mergeTopPtr)->lineNum = 0;
        }

        MergeLinkedList(topPtr,endPtr,nowPtr,mergeTopPtr,mergeEndPtr);
        updateLineNum( (*mergeTopPtr) );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 行データを置き換える
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr  置き換え対象のデータ
 * 引数  : S_BUFF_LINE_DATA *dataPtr 置き換えるデータ
 * 戻り値: 置き換えたデータ
 ***************************************/
static S_BUFF_LINE_DATA *
replaceLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->lineNum = nowPtr->lineNum;
        dataPtr = ReplaceLinkedList(topPtr,endPtr,nowPtr,dataPtr);
    }
    else
    {
        nop();
    }

    return dataPtr;
}

/********************************************************************************
 * 内容  : 行データを挿入する
 * 引数  : S_BUFF_LINE_DATA **topPtr 先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr 最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr  挿入位置
 * 引数  : S_BUFF_LINE_DATA *dataPtr 挿入するデータ
 * 戻り値: なし
 ***************************************/
static void
insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->lineNum = nowPtr->lineNum;

        InsertLinkedList(topPtr,endPtr,nowPtr,dataPtr);

        (nowPtr->lineNum)++;
        updateLineNum( nowPtr );
    }
    else
    {
        nop();
    }
}
