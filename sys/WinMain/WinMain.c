#include "common.h"

/* 外部関数定義 */
#include "MainWnd.h"

/* 外部変数定義 */
/* 内部関数定義 */
static BOOL initializeApp( HINSTANCE, int);

/* 内部変数定義 */
HINSTANCE ghInst;    /* インスタンスのハンドラ     */
TCHAR szAppName[] = TEXT("hdt"); /* アプリケーションの名称 */

/********************************************************************************
 * 関数名: WinMain()
 * 引数  : HINSTANCE hInstance     インスタンスのハンドラ
 *         HINSTANCE hPrevInstance このプログラムが直前に実行されていたインスタンスのハンドラ
 *         PSTR szCmdLine          プログラムの起動のために使われたコマンド行
 *         int iCmdShow            プログラムの初期表示の形態の指定
 * 戻り値: int
 * 内容  : WINDOWSプログラムのエントリポイント
 ***************************************/
int WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow )
{
    MSG msg;

    if( !initializeApp( hInstance, nCmdShow ) )
    {
        MessageBox(NULL, "couldn't start!", NULL, MB_OK);
        return 0;
    }

    while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        if( !IsMainWndMessage( &msg ) )
        {
            TranslateMessage( &msg ) ;
            DispatchMessage( &msg ) ;
        }
    }

    return msg.wParam; /* PostQuitMessage()の引数 */
}


/********************************************************************************
 * 関数名 : initializeApp()
 * 引数   : HINSTANCE hInst, int nCmdShow
 * 戻り値 : BOOL
 * 内容   : アプリケーションの初期化
 ***************************************/
static BOOL
initializeApp( HINSTANCE hInst, int nCmdShow )
{
    ghInst = hInst;
    return MainWndCreate( nCmdShow ); /* メインウィンドウ生成 */
}
