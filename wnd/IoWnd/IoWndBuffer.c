/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"

typedef struct tagS_LIST
{
    struct tagS_LIST *prevPtr;
    struct tagS_LIST *nextPtr;
} S_LIST_HEADER;

typedef struct tag_buffer_line_data
{
    S_LIST_HEADER header         ;
    DWORD         lineNum        ; /* Y位置            */
    DWORD         caretDataPos   ; /* X位置            */
    DWORD         dataSize       ; /* データサイズ     */
    INT           newLineCodeSize; /* 改行コードサイズ */
    TCHAR         data[]         ;
} S_BUFF_LINE_DATA;

typedef struct tagS_IOWND_BUFF_LOCAL
{
    struct
    {
        S_BUFF_LINE_DATA *topPtr       ; /* 先頭のデータ                           */
        S_BUFF_LINE_DATA *endPtr       ; /* 最後のデータ                           */
        S_BUFF_LINE_DATA *nowPtr       ; /* キャレットがあるデータ                 */
        S_BUFF_LINE_DATA *selectPtr    ; /* 選択範囲の先頭のデータ                 */
        DWORD            selectCaretPos; /* 選択範囲の先頭のデータのキャレット位置 */
    } lineData;
    UINT  NewLineType;
    DWORD xCaret     ;
    DWORD yCaret     ;
    INT   tabSize    ;
} S_IOWND_BUFF_LOCAL;
typedef S_IOWND_BUFF_LOCAL * H_IOWND_BUFF_LOCAL;

static S_BUFF_LINE_DATA *createBuffLineData( DWORD size, INT newLineCodeSize, TCHAR *dataPtr, DWORD lineNum, DWORD caretPos );
static void destroyBuffLineData( S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *getBuffLine( H_IOWND_BUFF_LOCAL h, TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
static void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr );
static S_BUFF_LINE_DATA *shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size );
static CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );
static DWORD getCaretDispXpos( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dataPos );
static BOOL getDispCharData( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

static void addLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void removeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **insertTopPtr, S_BUFF_LINE_DATA **insertEndPtr );
static S_BUFF_LINE_DATA *replaceLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static void clearBuffLineData( H_IOWND_BUFF_LOCAL h );

static void addLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr );
static S_LIST_HEADER *replaceLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr );
static S_LIST_HEADER *removeLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr );
static void insertLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER **insertTopPtr, S_LIST_HEADER **insertEndPtr );

/* 内部変数定義 */

/********************************************************************************
 * 内容  : IOウィンドウバッファのオブジェクト生成
 * 引数  : なし
 * 戻り値: H_IOWND_BUFF
 ***************************************/
H_IOWND_BUFF
IoWndBuffCreate( void )
{
    H_IOWND_BUFF_LOCAL h;

    h = (H_IOWND_BUFF_LOCAL)malloc( sizeof(S_IOWND_BUFF_LOCAL) );
    if( h != NULL )
    {
        h->lineData.topPtr         = NULL;
        h->lineData.endPtr         = NULL;
        h->lineData.nowPtr         = NULL;
        h->lineData.selectPtr      = NULL;
        h->lineData.selectCaretPos = 0;
        h->NewLineType = IOWND_BUFF_NEWLINE_CRLF;
        h->xCaret      = 0;
        h->yCaret      = 0;
        h->tabSize     = 8;
    }
    else
    {
        nop();
    }

    return (H_IOWND_BUFF)h;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのオブジェクト破棄
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffDestroy( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h != NULL) )
    {
        free( h );
    }
    else
    {
        nop();
    }

    return TRUE;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの初期化
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffInit( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;
    S_BUFF_LINE_DATA *lineDataPtr;

    clearBuffLineData(h);

    /* 空データを追加 */
    lineDataPtr = createBuffLineData( 0, 0, NULL, 0, 0 );
    if( lineDataPtr != NULL )
    {
        addLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),lineDataPtr );
        (h->lineData.nowPtr) = lineDataPtr;
    }
    else
    {
        nop();
    }

    (h->lineData.selectPtr) = NULL;
    (h->lineData.selectCaretPos) = 0;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの終了
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffEnd( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    clearBuffLineData(h);
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータセット
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : TCHAR* dataPtr
 * 引数  : DWORD  length
 * 引数  : BOOL   bInit  (TRUE:既存データをクリア,FALSE:クリアしない)
 * 戻り値: なし
 ***************************************/
void
IoWndBuffDataSet( H_IOWND_BUFF hIoWndBuff, TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    S_BUFF_LINE_DATA *lineDataPtr;
    S_BUFF_LINE_DATA *tempTopPtr = NULL;
    S_BUFF_LINE_DATA *tempEndPtr = NULL;
    S_BUFF_LINE_DATA *newPtr,*targetPtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    DWORD lineLengthSum = 0;
    DWORD caretPos = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( bInit )
    {
        IoWndBuffInit(hIoWndBuff);
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
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( h, dataPtr+lineLengthSum, length - lineLengthSum );

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

        if( (h->lineData.nowPtr)->caretDataPos == 0 )
        { /* 行の先頭に挿入 */
            insertLineData(&(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),&tempTopPtr,&tempEndPtr);

            if( tempEndPtr->newLineCodeSize == 0 )
            { /* 挿入データの最後に改行がなければ */
                newPtr = joinData(tempEndPtr,(h->lineData.nowPtr)); /* 追加データの最終行と現在の行と結合した新たなデータ生成 */
                if( newPtr != NULL )
                {
                    targetPtr = (h->lineData.nowPtr);
                    (h->lineData.nowPtr) = replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr ); /* 現在の行を置き換え */
                    destroyBuffLineData( targetPtr );                                                       /* 現在の行のデータを解放                   */

                    caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;           /* キャレット位置は追加データの最終行の最後 */
                    removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempEndPtr); /* 最終行自体は不要なので連結リストから外す */
                    destroyBuffLineData( tempEndPtr );

                    (h->lineData.nowPtr) = newPtr;                                            /* 結合したものを現在行とする */
                    (h->lineData.nowPtr)->caretDataPos = caretPos;                                /* キャレット位置は結合位置     */
                }
                else
                {
                    nop();
                }
            }
            else
            { /* 挿入データの最後に改行があった */
                (h->lineData.nowPtr)->caretDataPos = 0;
            }
        }
        else
        { /* 行の途中に挿入 */
            /* 現在の行をキャレット位置で分割(ここから) */
            divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr );
            if( dividePrePtr != NULL )
            {
                if( dividePostPtr != NULL )
                {
                    replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),dividePostPtr ); /* まず 古いデータを改行以降のデータに置き換え */
                    insertLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePostPtr,&dividePrePtr,&dividePrePtr ); /* その前に改行前のデータを挿入 */

                    destroyBuffLineData( (h->lineData.nowPtr) );
                    (h->lineData.nowPtr) = dividePostPtr;
                    (h->lineData.nowPtr)->caretDataPos = 0;
                    /* 現在の行をキャレット位置で分割(ここまで) */

                    if( tempTopPtr == tempEndPtr )
                    { /* 1行挿入の場合 */
                        insertLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),&tempEndPtr,&tempEndPtr );

                        /* 分割した前方データと、挿入データの先頭を結合(ここから) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempTopPtr,newPtr );
                            destroyBuffLineData( targetPtr );
                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePrePtr);
                            destroyBuffLineData( dividePrePtr );
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
                                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePostPtr,newPtr );
                                destroyBuffLineData( dividePostPtr );
                                (h->lineData.nowPtr) = newPtr;
                                removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempEndPtr);
                                destroyBuffLineData( tempEndPtr );
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
                        insertLineData(&(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),&tempTopPtr,&tempEndPtr);

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* 挿入データの最後に改行がなければ */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* 追加データの最終行と改行以降の行と結合 */
                            if( newPtr != NULL )
                            {
                                newPtr->caretDataPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePostPtr,newPtr );
                                destroyBuffLineData( dividePostPtr );
                                (h->lineData.nowPtr) = newPtr;
                                removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempEndPtr);
                                destroyBuffLineData( tempEndPtr );
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
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempTopPtr,newPtr );
                            destroyBuffLineData( targetPtr );
                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePrePtr);
                            destroyBuffLineData( dividePrePtr );
                        }
                        else
                        {
                            nop();
                        }
                    }
                }
                else
                {
                    destroyBuffLineData( dividePrePtr );
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
        if( (h->lineData.topPtr) != NULL )
        {
            (h->lineData.nowPtr) = (h->lineData.topPtr);
            (h->lineData.nowPtr)->caretDataPos = 0;
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
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : TCHAR *dataPtr
 * 引数  : DWORD dataSize
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffDataGet( H_IOWND_BUFF hIoWndBuff, TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region )
{
    BOOL rtn = FALSE;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;
    DWORD size;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( region == BUFF_ALL )
    {
        if( (h->lineData.topPtr) == NULL )
        {
            nop();
        }
        else
        {
            if( dataPtr != NULL )
            {
                for( nowPtr=(h->lineData.topPtr); nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
            if( (h->lineData.selectPtr) != NULL )
            { /* 選択開始位置有り */
                if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
                { /* 正方向に選択 */
                    for( nowPtr = (h->lineData.selectPtr),prevPtr = NULL; prevPtr != (h->lineData.nowPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == (h->lineData.selectPtr) )
                        {
                            size = (nowPtr->dataSize - (h->lineData.selectCaretPos));
                            memcpy( dataPtr, (nowPtr->data + (h->lineData.selectCaretPos)),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == (h->lineData.nowPtr) )
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
                else if( ((h->lineData.selectPtr)->lineNum) > ((h->lineData.nowPtr)->lineNum) )
                { /* 負方向に選択 */
                    for( nowPtr = (h->lineData.nowPtr),prevPtr = NULL; prevPtr != (h->lineData.selectPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == (h->lineData.nowPtr) )
                        {
                            size = (nowPtr->dataSize - nowPtr->caretDataPos);
                            memcpy( dataPtr, (nowPtr->data + nowPtr->caretDataPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == (h->lineData.selectPtr) )
                        {
                            size = (h->lineData.selectCaretPos);
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
                    if( (h->lineData.nowPtr) == (h->lineData.selectPtr) )
                    {
                        if( (h->lineData.nowPtr)->caretDataPos < (h->lineData.selectCaretPos) )
                        {
                            size = ((h->lineData.selectCaretPos) - (h->lineData.nowPtr)->caretDataPos);
                            memcpy( dataPtr, ((h->lineData.nowPtr)->data + (h->lineData.nowPtr)->caretDataPos),size );
                            dataPtr += size;
                        }
                        else if( (h->lineData.nowPtr)->caretDataPos > (h->lineData.selectCaretPos) )
                        {
                            size = ((h->lineData.nowPtr)->caretDataPos - (h->lineData.selectCaretPos));
                            memcpy( dataPtr, ((h->lineData.nowPtr)->data + (h->lineData.selectCaretPos)),size );
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
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : IOWND_BUFF_REGION region
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndBuffGetDataSize( H_IOWND_BUFF hIoWndBuff, IOWND_BUFF_REGION region )
{
    DWORD dataSize = 0;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( region == BUFF_ALL )
    {
        for( nowPtr = (h->lineData.topPtr); nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
        {
            dataSize += nowPtr->dataSize;
        }
    }
    else
    {
        if( (h->lineData.selectPtr) != NULL )
        { /* 選択開始位置有り */
            if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
            { /* 正方向に選択 */
                for( nowPtr = (h->lineData.selectPtr),prevPtr = NULL; prevPtr != (h->lineData.nowPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == (h->lineData.selectPtr) )
                    {
                        dataSize += (nowPtr->dataSize - (h->lineData.selectCaretPos));
                    }
                    else if( nowPtr == (h->lineData.nowPtr) )
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
            else if( ((h->lineData.selectPtr)->lineNum) > ((h->lineData.nowPtr)->lineNum) )
            { /* 負方向に選択 */
                for( nowPtr = (h->lineData.nowPtr),prevPtr = NULL; prevPtr != (h->lineData.selectPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == (h->lineData.nowPtr) )
                    {
                        dataSize += (nowPtr->dataSize - nowPtr->caretDataPos);
                    }
                    else if( nowPtr == (h->lineData.selectPtr) )
                    {
                        dataSize += (h->lineData.selectCaretPos);
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
                if( (h->lineData.nowPtr) == (h->lineData.selectPtr) )
                {
                    if( (h->lineData.nowPtr)->caretDataPos < (h->lineData.selectCaretPos) )
                    {
                        dataSize += ((h->lineData.selectCaretPos) - (h->lineData.nowPtr)->caretDataPos);
                    }
                    else if( (h->lineData.nowPtr)->caretDataPos > (h->lineData.selectCaretPos) )
                    {
                        dataSize += ((h->lineData.nowPtr)->caretDataPos - (h->lineData.selectCaretPos));
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
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndBuffGetLineMaxSize( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    return (h->lineData.endPtr)->lineNum;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの最大文字サイズ取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndBuffGetColumnMaxSize( H_IOWND_BUFF hIoWndBuff)
{
    DWORD columnMaxSize = 0;
    S_BUFF_LINE_DATA *nowPtr;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    for( nowPtr = (h->lineData.topPtr); nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
    {
        columnMaxSize = max( columnMaxSize, (nowPtr->dataSize - nowPtr->newLineCodeSize) );
    }

    return columnMaxSize;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndBuffGetCaretXpos( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    return getCaretDispXpos(h,(h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos);
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndBuffGetCaretYpos( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    return (h->lineData.nowPtr)->lineNum;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレット位置セット
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : DWORD xPos
 * 引数  : DWORD lineNum
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSetCaretPos( H_IOWND_BUFF hIoWndBuff, DWORD xPos, DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr = NULL;
    S_BUFF_DISP_DATA dispData;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    nowPtr = (S_BUFF_LINE_DATA *)IoWndBuffGetLinePtr( hIoWndBuff, lineNum );

    if( nowPtr != NULL )
    {
        (h->lineData.nowPtr) = nowPtr;

        getDispCharData( h,(h->lineData.nowPtr), xPos, &dispData );
        (h->lineData.nowPtr)->caretDataPos = dispData.caretPos;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置加算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffIncCaretXpos( H_IOWND_BUFF hIoWndBuff )
{
    int moveAmount = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.nowPtr) != NULL )
    {
        if( detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos) == DOUBLE_CHAR_HIGH )
        { /* 次の文字で移動量を判断 */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( ((h->lineData.nowPtr)->caretDataPos + moveAmount) > ((h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize) )
        { /* キャレットはすでに行の最右端 */
            if( ((h->lineData.nowPtr)->header.nextPtr != NULL) )
            { /* 次行有り */
                (h->lineData.nowPtr) = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.nextPtr;
                (h->lineData.nowPtr)->caretDataPos = 0;
            }
            else
            { /* 次行無し */
                nop();
            }
        }
        else
        { /* まだ右へ移動可能 */
            (h->lineData.nowPtr)->caretDataPos += moveAmount;
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットX位置減算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffDecCaretXpos( H_IOWND_BUFF hIoWndBuff )
{
    int moveAmount = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.nowPtr) != NULL )
    {
        if( ((h->lineData.nowPtr)->caretDataPos>0) &&
            (detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos-1) == DOUBLE_CHAR_LOW) )
        { /* 前の文字で移動量を判断 */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( (h->lineData.nowPtr)->caretDataPos == 0 )
        { /* キャレットはすでに行の最左端 */
            if( ((h->lineData.nowPtr)->header.prevPtr != NULL) )
            { /* 前行有り */
                (h->lineData.nowPtr) = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.prevPtr;
                (h->lineData.nowPtr)->caretDataPos = (h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize;
            }
            else
            { /* 前行無し */
                nop();
            }
        }
        else
        { /* まだ左へ移動可能 */
            (h->lineData.nowPtr)->caretDataPos = max( (h->lineData.nowPtr)->caretDataPos - moveAmount, 0 );
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置加算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffIncCaretYpos( H_IOWND_BUFF hIoWndBuff )
{
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *nextPtr;
    S_BUFF_DISP_DATA dispData;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    nextPtr = (S_BUFF_LINE_DATA *)((h->lineData.nowPtr)->header.nextPtr);

    if( ((h->lineData.nowPtr) != NULL) && (nextPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(h,(h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos);
        getDispCharData( h, nextPtr, preDispPos, &dispData );
        nextPtr->caretDataPos = dispData.caretPos;
        (h->lineData.nowPtr) = nextPtr;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのキャレットY位置減算
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffDecCaretYpos( H_IOWND_BUFF hIoWndBuff )
{
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *prevPtr;
    S_BUFF_DISP_DATA dispData;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    prevPtr = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.prevPtr;

    if( ((h->lineData.nowPtr) != NULL) && (prevPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(h,(h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos);
        getDispCharData( h, prevPtr, preDispPos, &dispData );
        prevPtr->caretDataPos = dispData.caretPos;
        (h->lineData.nowPtr) = prevPtr;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 指定行データのハンドル取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : DWORD lineNum
 * 戻り値: H_IOWND_BUFF_LINE
 ***************************************/
H_IOWND_BUFF_LINE
IoWndBuffGetLinePtr( H_IOWND_BUFF hIoWndBuff, DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr,*rtnPtr = NULL;
    DWORD i;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.topPtr) == NULL )
    {
        nop();
    }
    else
    {
        for( i=0,nowPtr = nextPtr = (h->lineData.topPtr); (nowPtr != NULL) && (i<=lineNum); nowPtr=nextPtr,i++ )
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

    return (H_IOWND_BUFF_LINE)rtnPtr;
}

/********************************************************************************
 * 内容  : 次の行データのハンドル取得
 * 引数  : H_IOWND_BUFF_LINE hLineData
 * 戻り値: H_IOWND_BUFF_LINE
 ***************************************/
H_IOWND_BUFF_LINE
IoWndBuffGetLineNextPtr( H_IOWND_BUFF_LINE hLineData )
{
    return (H_IOWND_BUFF_LINE)(((S_BUFF_LINE_DATA *)hLineData)->header.nextPtr);
}

/********************************************************************************
 * 内容  : 指定行、指定列のデータを取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : H_IOWND_BUFF_LINE hLineData
 * 引数  : DWORD             dispPos     表示位置
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffGetDispData( H_IOWND_BUFF hIoWndBuff, H_IOWND_BUFF_LINE hLineData, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    getDispCharData( h, (S_BUFF_LINE_DATA *)hLineData, dispPos, dataPtr );

    return TRUE;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの改行コードセット
 * 引数  : UINT newLineType
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSetNewLineCode( H_IOWND_BUFF hIoWndBuff, UINT newLineType )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    h->NewLineType = newLineType;
}

/********************************************************************************
 * 内容  : データ削除
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : BOOL bBackSpace
 * 戻り値: なし
 ***************************************/
void
IoWndBuffRemoveData( H_IOWND_BUFF hIoWndBuff, BOOL bBackSpace )
{
    DWORD removeSize = 0,saveCaretPos;
    S_BUFF_LINE_DATA *newPtr = NULL,*prevPtr = NULL,*nextPtr = NULL,*nowPtr = NULL,*savePtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    BOOL bValid = FALSE;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.nowPtr) != NULL )
    {
        if( (h->lineData.selectPtr) != NULL )
        { /* 選択開始位置有り */
            if( ((h->lineData.selectPtr)->lineNum) == ((h->lineData.nowPtr)->lineNum) )
            { /* 同一行内で選択 */
                if( (h->lineData.nowPtr) == (h->lineData.selectPtr) )
                {
                    if( (h->lineData.nowPtr)->caretDataPos < (h->lineData.selectCaretPos) )
                    {
                        removeSize += ((h->lineData.selectCaretPos) - (h->lineData.nowPtr)->caretDataPos);
                        saveCaretPos = (h->lineData.nowPtr)->caretDataPos;
                        (h->lineData.nowPtr)->caretDataPos = (h->lineData.selectCaretPos);
                        (h->lineData.selectCaretPos) = saveCaretPos;
                    }
                    else if( (h->lineData.nowPtr)->caretDataPos > (h->lineData.selectCaretPos) )
                    {
                        removeSize += ((h->lineData.nowPtr)->caretDataPos - (h->lineData.selectCaretPos));
                    }
                    else
                    {
                        nop();
                    }

                    divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                    newPtr = shortenData( dividePrePtr, removeSize );                 /* 分割後の前方データの末尾から所定量削除 */
                    destroyBuffLineData( dividePrePtr );                                 /* 前方データを */
                    dividePrePtr = newPtr;                                            /* 書き換え     */

                    newPtr = joinData( dividePrePtr, dividePostPtr );                 /* 再結合 */
                    newPtr->lineNum = (h->lineData.nowPtr)->lineNum;                   /* 行番号は同じ */
                    newPtr->caretDataPos = (h->lineData.nowPtr)->caretDataPos - removeSize;    /* キャレット位置は削除量分前方になる */
                    destroyBuffLineData( dividePrePtr );                                 /* 作業データを削除 */
                    destroyBuffLineData( dividePostPtr );                                /* 作業データを削除 */

                    /* 古い行データと置き換える */
                    replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr );
                    destroyBuffLineData( (h->lineData.nowPtr) );
                    (h->lineData.nowPtr) = newPtr;
                }
                else
                {
                    nop();
                }
            }
            else
            {
                if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
                { /* 正方向に選択 */
                    nop();
                }
                else
                { /* 負方向に選択 */
                    savePtr = (h->lineData.selectPtr);
                    saveCaretPos = (h->lineData.selectCaretPos);
                    (h->lineData.selectPtr) = (h->lineData.nowPtr);
                    (h->lineData.selectCaretPos) = (h->lineData.nowPtr)->caretDataPos;
                    (h->lineData.nowPtr) = savePtr;
                    (h->lineData.nowPtr)->caretDataPos = saveCaretPos;
                }

                /* 先頭から最後まで選択された行を削除 */
                for( nowPtr = (S_BUFF_LINE_DATA *)(h->lineData.selectPtr)->header.nextPtr; nowPtr != (h->lineData.nowPtr); nowPtr = nextPtr )
                {
                    nextPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr;
                    removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),nowPtr);
                    destroyBuffLineData( nowPtr );
                }
                /* 先頭行のうち、選択位置から前を残す */
                if( (h->lineData.selectCaretPos) == 0 )
                { /* 選択位置が先頭だったら、1行丸々削除 */
                    removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.selectPtr));
                }
                else
                {
                    (h->lineData.selectPtr)->caretDataPos = (h->lineData.selectCaretPos);
                    divideData( (h->lineData.selectPtr), &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                    replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.selectPtr),dividePrePtr );
                    destroyBuffLineData( (h->lineData.selectPtr) );
                }

                /* 最終行のうち、選択位置から後を残す */
                divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),dividePostPtr );
                destroyBuffLineData( (h->lineData.nowPtr) );
                (h->lineData.nowPtr) = dividePostPtr;
                (h->lineData.nowPtr)->caretDataPos = 0;
                updateLineNum( (h->lineData.nowPtr) );
            }

            (h->lineData.selectPtr) = NULL;
            (h->lineData.selectCaretPos) = 0;
        }
        else
        { /* 選択無し */
            if( bBackSpace )
            { /* BSキー */
                if( (h->lineData.nowPtr)->caretDataPos > 0  )
                {
                    bValid = TRUE;

                    if( detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos-1) == DOUBLE_CHAR_LOW )
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
                    if( (h->lineData.nowPtr)->header.prevPtr != NULL )
                    {
                        prevPtr = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.prevPtr;
                        newPtr = joinData( prevPtr,(h->lineData.nowPtr) ); /* 前行と本行を結合した新しい行データを生成 */

                        if( newPtr != NULL )
                        {
                            /* 行番号、キャレット位置を前行データによって決まる */
                            newPtr->lineNum  = prevPtr->lineNum;
                            newPtr->caretDataPos = prevPtr->dataSize - prevPtr->newLineCodeSize;

                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr) ); /* 本行は削除 */
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),prevPtr,newPtr );    /* 前行は新しい行データに置き換える */

                            destroyBuffLineData( (h->lineData.nowPtr) );
                            destroyBuffLineData( prevPtr );
                            (h->lineData.nowPtr) = newPtr;

                            updateLineNum( (h->lineData.nowPtr) );
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
                if( (h->lineData.nowPtr)->caretDataPos != ((h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize) )
                {
                    bValid = TRUE;

                    if( detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos) == DOUBLE_CHAR_HIGH )
                    { /* キャレット位置の文字で削除量を判断 */
                        removeSize = 2;
                    }
                    else
                    {
                        removeSize = 1;
                    }
                    (h->lineData.nowPtr)->caretDataPos += removeSize;
                }
                else
                { /* キャレットが行の最終位置。つまり、次行との結合。*/
                    if( (h->lineData.nowPtr)->header.nextPtr != NULL )
                    {
                        nextPtr = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.nextPtr;
                        newPtr = joinData( (h->lineData.nowPtr),nextPtr ); /* 本行と次行を結合した新しい行データを生成 */

                        if( newPtr != NULL )
                        {
                            /* 行番号、キャレット位置は本行データによって決まる */
                            newPtr->lineNum  = (h->lineData.nowPtr)->lineNum;
                            newPtr->caretDataPos = (h->lineData.nowPtr)->caretDataPos;

                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),nextPtr );         /* 次行は削除 */
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr ); /* 本行は新しい行データに置き換える */

                            destroyBuffLineData( (h->lineData.nowPtr) );
                            destroyBuffLineData( nextPtr );
                            (h->lineData.nowPtr) = newPtr;

                            updateLineNum( (h->lineData.nowPtr) );
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
                divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr ); /* キャレット位置で分割 */
                newPtr = shortenData( dividePrePtr, removeSize );                 /* 分割後の前方データの末尾から所定量削除 */
                destroyBuffLineData( dividePrePtr );                                 /* 前方データを */
                dividePrePtr = newPtr;                                            /* 書き換え     */

                newPtr = joinData( dividePrePtr, dividePostPtr );                 /* 再結合 */
                newPtr->lineNum = (h->lineData.nowPtr)->lineNum;                   /* 行番号は同じ */
                newPtr->caretDataPos = (h->lineData.nowPtr)->caretDataPos - removeSize;    /* キャレット位置は削除量分前方になる */
                destroyBuffLineData( dividePrePtr );                                 /* 作業データを削除 */
                destroyBuffLineData( dividePostPtr );                                /* 作業データを削除 */

                /* 古い行データと置き換える */
                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr );
                destroyBuffLineData( (h->lineData.nowPtr) );
                (h->lineData.nowPtr) = newPtr;
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
 * 内容  : 改行データ取得
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : PTSTR dataPtr
 * 戻り値: INT
 ***************************************/
INT
IoWndBuffGetNewLineData( H_IOWND_BUFF hIoWndBuff, PTSTR dataPtr )
{
    INT size = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    switch( h->NewLineType )
    {
    case IOWND_BUFF_NEWLINE_CRLF:
        *dataPtr     = '\r';
        *(dataPtr+1) = '\n';
        size = 2;
        break;
    case IOWND_BUFF_NEWLINE_LF  :
        *dataPtr = '\n';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_CR  :
        *dataPtr = '\r';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_NONE:
    default:
        break;
    }

    return size;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのタブサイズセット
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 引数  : INT tabSize
 * 戻り値: BOOL (TRUE:変更された)
 ***************************************/
BOOL
IoWndBuffSetTabSize( H_IOWND_BUFF hIoWndBuff, INT tabSize )
{
    BOOL bRtn = FALSE;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( tabSize != h->tabSize )
    {
        h->tabSize = tabSize;
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
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSelectOn( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.selectPtr) )
    { /* 既に選択済みの場合 */
        nop(); /* 変更しない */
    }
    else
    {
        (h->lineData.selectPtr) = (h->lineData.nowPtr);
        (h->lineData.selectCaretPos) = (h->lineData.nowPtr)->caretDataPos;
    }
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの範囲選択OFF
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: なし
 ***************************************/
void
IoWndBuffSelectOff( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    (h->lineData.selectPtr) = NULL;
    (h->lineData.selectCaretPos) = 0;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファの全範囲選択
 * 引数  : H_IOWND_BUFF hIoWndBuff
 * 戻り値: BOOL
 ***************************************/
BOOL
IoWndBuffSelectAll( H_IOWND_BUFF hIoWndBuff )
{
    BOOL bRtn = FALSE;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.topPtr) != NULL )
    {
        if( ((h->lineData.selectPtr) == (h->lineData.topPtr)) &&
            ((h->lineData.selectCaretPos) == 0) &&
            ((h->lineData.nowPtr) == (h->lineData.endPtr)) &&
            ((h->lineData.nowPtr)->caretDataPos == ((h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize) ) )
        {
            nop();
        }
        else
        {
            (h->lineData.selectPtr) = (h->lineData.topPtr);
            (h->lineData.selectCaretPos) = 0;
            (h->lineData.nowPtr) = (h->lineData.endPtr);
            (h->lineData.nowPtr)->caretDataPos = (h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize;
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

    newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (size * sizeof(TCHAR)) + 1 );
    if( newPtr != NULL )
    {
        memset( newPtr, sizeof(S_BUFF_LINE_DATA), 0 );
        newPtr->lineNum         = lineNum;
        newPtr->caretDataPos    = caretPos;
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
        newPtr->data[size] = '\0';
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
 * 引数  : H_IOWND_BUFF_LOCAL h
 * 引数  : TCHAR *dataPtr  データの先頭
 * 引数  : DWORD maxLength 最大長さ
 * 戻り値: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
getBuffLine( H_IOWND_BUFF_LOCAL h, TCHAR *dataPtr, DWORD maxLength )
{
    S_BUFF_LINE_DATA *lineDataPtr = NULL;
    DWORD i;
    INT   newLineCodeSize = 0;

    for( i=0; i<maxLength; i++ )
    {
        if( h->NewLineType == IOWND_BUFF_NEWLINE_CRLF )
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
        else if( h->NewLineType == IOWND_BUFF_NEWLINE_LF )
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
        else if( h->NewLineType == IOWND_BUFF_NEWLINE_CR )
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
 * 引数  : H_IOWND_BUFF_LOCAL h
 * 引数  : S_BUFF_LINE_DATA *linePtr   行データ
 * 引数  : DWORD             dataPos   列位置(データ上の)
 * 戻り値: DWORD
 ***************************************/
static DWORD
getCaretDispXpos( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dataPos )
{
    DWORD i,j=0;
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

                    tab_offset = h->tabSize - (j % h->tabSize);
                    j+=tab_offset;
                }
                else if( ( (BYTE)(*(linePtr->data+i)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xDF)) ||
                         ((BYTE)0xF0 <= (BYTE)(*(linePtr->data+i))) )
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
 * 引数  : H_IOWND_BUFF_LOCAL h
 * 引数  : S_BUFF_LINE_DATA *linePtr   行データ
 * 引数  : DWORD             dispPos   列位置(表示上の)
 * 引数  : S_BUFF_DISP_DATA *dataPtr
 * 戻り値: BOOL
 ***************************************/
static BOOL
getDispCharData( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
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

                    tab_offset = h->tabSize - (j % h->tabSize);

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
                         ((BYTE)0xF0 <= (BYTE)(*(linePtr->data+dataPos))) )
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

            if( (h->lineData.selectPtr) != NULL )
            { /* 選択開始位置有り */
                if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
                { /* 正方向に選択 */
                    if( ((h->lineData.selectPtr)->lineNum < linePtr->lineNum) && (linePtr->lineNum < (h->lineData.nowPtr)->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.selectPtr)->lineNum == linePtr->lineNum) && ((h->lineData.selectCaretPos) <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.nowPtr)->lineNum == linePtr->lineNum) && (dataPos < linePtr->caretDataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else
                    {
                        nop();
                    }
                }
                else if( ((h->lineData.selectPtr)->lineNum) > ((h->lineData.nowPtr)->lineNum) )
                { /* 負方向に選択 */
                    if( ((h->lineData.nowPtr)->lineNum < linePtr->lineNum) && (linePtr->lineNum < (h->lineData.selectPtr)->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.nowPtr)->lineNum == linePtr->lineNum) && (linePtr->caretDataPos <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.selectPtr)->lineNum == linePtr->lineNum) && (dataPos < (h->lineData.selectCaretPos)) )
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
                    if( (h->lineData.nowPtr) == linePtr )
                    {
                        if( linePtr->caretDataPos < (h->lineData.selectCaretPos) )
                        {
                            if( (linePtr->caretDataPos <= dataPos) && (dataPos < (h->lineData.selectCaretPos)) )
                            {
                                dataPtr->bSelect = TRUE;
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else if( linePtr->caretDataPos > (h->lineData.selectCaretPos) )
                        {
                            if( ((h->lineData.selectCaretPos) <= dataPos) && (dataPos < linePtr->caretDataPos) )
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

    addLinkedList( (S_LIST_HEADER **)topPtr, (S_LIST_HEADER **)endPtr, (S_LIST_HEADER *)dataPtr );
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

    removeNextPtr = (S_BUFF_LINE_DATA *)removeLinkedList( (S_LIST_HEADER **)topPtr,(S_LIST_HEADER **)endPtr,(S_LIST_HEADER *)dataPtr );
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
 * 内容  : 行データの挿入
 * 引数  : S_BUFF_LINE_DATA **topPtr       先頭データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA **topPtr       最終データをつなぐポインタ
 * 引数  : S_BUFF_LINE_DATA *nowPtr        挿入位置
 * 引数  : S_BUFF_LINE_DATA **insertTopPtr
 * 引数  : S_BUFF_LINE_DATA **insertEndPtr
 * 戻り値: void
 ***************************************/
static void
insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **insertTopPtr, S_BUFF_LINE_DATA **insertEndPtr )
{
    if( (nowPtr != NULL) && (insertTopPtr != NULL) )
    {
        (*insertTopPtr)->lineNum = nowPtr->lineNum;

        insertLinkedList((S_LIST_HEADER **)topPtr,(S_LIST_HEADER **)endPtr,(S_LIST_HEADER *)nowPtr,(S_LIST_HEADER **)insertTopPtr,(S_LIST_HEADER **)insertEndPtr);
        updateLineNum( (*insertTopPtr) );
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
        dataPtr = (S_BUFF_LINE_DATA *)replaceLinkedList((S_LIST_HEADER **)topPtr,(S_LIST_HEADER **)endPtr,(S_LIST_HEADER *)nowPtr,(S_LIST_HEADER *)dataPtr);
    }
    else
    {
        nop();
    }

    return dataPtr;
}

/********************************************************************************
 * 内容  : バッファデータのクリア
 * 引数  : H_IOWND_BUFF_LOCAL h
 * 戻り値: なし
 ***************************************/
static void
clearBuffLineData( H_IOWND_BUFF_LOCAL h )
{
    S_BUFF_LINE_DATA *lineDataPtr,*nextPtr;

    lineDataPtr = h->lineData.topPtr;
    while( lineDataPtr != NULL )
    {
        nextPtr = (S_BUFF_LINE_DATA *)removeLinkedList((S_LIST_HEADER **)&(h->lineData.topPtr),(S_LIST_HEADER **)&(h->lineData.endPtr),(S_LIST_HEADER *)lineDataPtr);
        destroyBuffLineData(lineDataPtr);
        lineDataPtr = nextPtr;
    }
}

/********************************************************************************
 * 内容  : 連結リストにデータを追加する
 * 引数  : S_LIST_HEADER **topPtr 先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *dataPtr つなぐデータ
 * 戻り値: なし
 ***************************************/
static void
addLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
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
 * 内容  : 連結リストのデータを置き換える
 * 引数  : S_LIST_HEADER **topPtr 先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr 最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *nowPtr  置き換え対象のデータ
 * 引数  : S_LIST_HEADER *dataPtr 置き換えるデータ
 * 戻り値: 置き換えたデータ
 ***************************************/
static S_LIST_HEADER *
replaceLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr )
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
static S_LIST_HEADER *
removeLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
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
 * 内容  : 連結リストへの挿入
 * 引数  : S_LIST_HEADER **topPtr       先頭データをつなぐポインタ
 * 引数  : S_LIST_HEADER **topPtr       最終データをつなぐポインタ
 * 引数  : S_LIST_HEADER *nowPtr        挿入位置
 * 引数  : S_LIST_HEADER **insertTopPtr 挿入する連結リストの先頭
 * 引数  : S_LIST_HEADER **insertEndPtr 挿入する連結リストの最後
 * 戻り値: void
 ***************************************/
static void
insertLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER **insertTopPtr, S_LIST_HEADER **insertEndPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (insertTopPtr != NULL) && (insertEndPtr != NULL) )
    {
        (*insertTopPtr)->prevPtr = nowPtr->prevPtr;

        if( (*insertTopPtr)->prevPtr != NULL )
        {
            ((*insertTopPtr)->prevPtr)->nextPtr = (*insertTopPtr);
        }
        else
        {
            (*topPtr) = (*insertTopPtr);
        }

        (*insertEndPtr)->nextPtr = nowPtr;
        nowPtr->prevPtr = (*insertEndPtr);
    }
    else
    {
        nop();
    }
}
