#ifndef STS_BAR_H

typedef enum
{
    STS_BAR_0,
    STS_BAR_1,
    STS_BAR_2,
    STS_BAR_3,
    STS_BAR_MAX
}STS_BAR_ID;

/********************************************************************************
 * 内容  : ステータスバー生成
 * 引数  : HWND hwnd 親ウィンドウのハンドラ
 * 戻り値: HWND
 ***************************************/
HWND StsBarCreate( HWND hwnd );

/********************************************************************************
 * 内容  : ステータスバーのサイズ調整
 * 引数  : int cxClient
 * 引数  : int cyClient
 * 戻り値: LONG 縦方向のサイズ
 ***************************************/
LONG StsBarSize( int cxClient,int cyClient );

/********************************************************************************
 * 内容  : ステータスバーへの文字列セット
 * 引数  : STS_BAR_ID id
 * 引数  : PTSTR ptstrFormat, ...
 * 戻り値: なし
 ***************************************/
void StsBarSetText( STS_BAR_ID id, PTSTR ptstrFormat, ... );

#define STS_BAR_H
#endif /* STS_BAR_H */
