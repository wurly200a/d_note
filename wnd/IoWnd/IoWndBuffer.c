/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
#include "WinMain.h"

/* 外部変数定義 */

/* 内部関数定義 */
#include "IoWndBuffer.h"

/* 内部変数定義 */
typedef struct tag_iownd_buffer_data
{
    struct tag_iownd_buffer_data *prevPtr;
    struct tag_iownd_buffer_data *nextPtr;
    DWORD                         dataSize;
    TCHAR                        *dataPtr;
} S_IOWND_BUFF_DATA;

S_IOWND_BUFF_DATA ioWndBuffListTop;

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
