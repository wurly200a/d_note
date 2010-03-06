#ifndef MAINWND_H

/********************************************************************************
 * 内容  : メインウィンドウクラスの登録、ウィンドウの生成
 * 引数  : int nCmdShow
 * 引数  : HACCEL *hAccelPtr
 * 戻り値: HWND
 ***************************************/
HWND MainWndCreate( int nCmdShow, HACCEL *hAccelPtr );

/********************************************************************************
 * 内容  : メインウィンドウ内で処理するメッセージを判定する
 * 引数  : MSG *msgPtr
 * 戻り値: BOOL
 ***************************************/
BOOL IsMainWndMessage( MSG *msgPtr );

#define MAINWND_H
#endif /* MAINWND_H */
