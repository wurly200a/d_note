#ifndef SOME_CTRL_H

typedef enum
{
    SOME_CTRL_FILENAME_STATIC,
    SOME_CTRL_FILENAME,
    SOME_CTRL_FILEOPEN_BUTTON,
    SOME_CTRL_MAX
}SOME_CTRL_ID;

#define SOME_CTRL_ID_OFFSET 10
#define MY_EX_STYLE (WS_EX_OVERLAPPEDWINDOW)

/********************************************************************************
 * 内容  : 登録された全てのコントロールを生成する
 * 引数  : HINSTANCE hInst
 * 引数  : PTSTR szAppName
 * 引数  : HWND hwnd 親ウィンドウのハンドラ
 * 戻り値: BOOL
 ***************************************/
BOOL SomeCtrlCreate( HINSTANCE hInst, PTSTR szAppName, HWND hwnd );

/********************************************************************************
 * 内容  : 登録された全てのコントロールのサイズを調整する
 * 引数  : int cxClient
 * 引数  : int cyClient
 * 戻り値: LONG コントロールを配置する矩形の縦方向のサイズ
 ***************************************/
LONG SomeCtrlSize( int cxClient,int cyClient );

/********************************************************************************
 * 内容  : コントロールのウィンドウハンドラを取得する
 * 引数  : SOME_CTRL_ID id
 * 戻り値: HWND
 ***************************************/
HWND SomeCtrlGetHWND( SOME_CTRL_ID id );

/********************************************************************************
 * 内容  : コントロールを有効にする
 * 引数  : SOME_CTRL_ID id
 * 戻り値: なし
 ***************************************/
void SomeCtrlEnable( SOME_CTRL_ID id );

/********************************************************************************
 * 内容  : コントロールを無効にする
 * 引数  : SOME_CTRL_ID id
 * 戻り値: なし
 ***************************************/
void SomeCtrlDisable( SOME_CTRL_ID id );

/********************************************************************************
 * 内容  : コントロールからテキストを取得する
 * 引数  : SOME_CTRL_ID id
 * 引数  : PTSTR ptstrText
 * 戻り値: BOOL
 ***************************************/
BOOL SomeCtrlGetText( SOME_CTRL_ID id, PTSTR ptstrText );

#define SOME_CTRL_H
#endif /* SOME_CTRL_H */
