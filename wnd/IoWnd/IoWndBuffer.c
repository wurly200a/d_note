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

S_IOWND_BUFF_DATA ioWndBuffQueTop;
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
    if( ioWndBuffQueTop.dataPtr != NULL )
    {
        free( ioWndBuffQueTop.dataPtr );
        ioWndBuffQueTop.dataPtr  = NULL;
        ioWndBuffQueTop.dataSize = 0;
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
    DWORD i,j;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffQueTop.dataPtr = (TCHAR *) malloc( (length * sizeof(TCHAR)) );

        if( ioWndBuffQueTop.dataPtr != NULL )
        {
            ioWndBuffQueTop.dataSize = length;

            for( i=0,j=0; i<length; i++ )
            {
                *(ioWndBuffQueTop.dataPtr+i) = *(strPtr+i);
                if( *(strPtr+i) == '\n' )
                {
                    (ioWndBuffLineMax)++; /* 改行の数をカウント */
                    ioWndBuffColumnMax = max( ioWndBuffColumnMax, j );
                    j=0;
                }
                else
                {
                    j++;
                }
            }

            if( ioWndBuffLineMax==0 )
            { /* 改行が無かった場合 */
                ioWndBuffLineMax  = 1;
                ioWndBuffColumnMax = ioWndBuffQueTop.dataSize;
            }
        }
        else
        {
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
    return ioWndBuffQueTop.dataPtr;
}

/********************************************************************************
 * 内容  : IOウィンドウバッファのデータサイズ取得
 * 引数  : なし
 * 戻り値: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( void )
{
    return ioWndBuffQueTop.dataSize;
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
