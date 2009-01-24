/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
/* 外部変数定義 */
extern HINSTANCE ghInst;      /* インスタンスのハンドラ     */
extern TCHAR     szAppName[]; /* アプリケーションの名称 */

/* 内部関数定義 */
#include "SomeCtrl.h"
/* 内部変数定義 */

typedef struct
{
    BOOL  exist;       /* ウィンドウが生成されているかどうか         */
    HWND  hwnd;        /* ウィンドウのハンドラ                       */
    PTSTR class;       /* ウィンドウクラス名                         */
    PTSTR value;       /* ウィンドウテキスト                         */
    int   x;           /* クライアント領域の x座標                   */
    int   y;           /* クライアント領域の y座標                   */
    int   width;       /* 初期幅                                     */
    int   height;      /* 初期高さ                                   */
    BOOL  bSizeAdj;    /* サイズ調整を行うか                         */
    int   widthOfst;   /* クライアント領域の幅サイズからのオフセット */
    int   heightOfst;  /* 1文字分の高さからのオフセット              */
    DWORD exStyle;     /* 拡張スタイル                               */
    DWORD style;       /* スタイル                                   */
} S_SOME_CTRL;

S_SOME_CTRL ctrlListTbl[SOME_CTRL_MAX] =
{
    /* exist,  hwnd, class             , value              , x , y  , w  , h  ,bSAdj ,wOfst, hOfst,exStyle,style  */
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("file")        ,  5, 15, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 10, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT)             },
};


/********************************************************************************
 * 内容   : 登録された全てのコントロールを生成する
 * 引数   : HWND hwnd親ウィンドウのハンドラ
 * 戻り値 : BOOL
 ********************************************************************************/
BOOL
SomeCtrlCreate(HWND hwnd)
{
    int i,j;
    TCHAR buf[10];

    for(i=0; i<SOME_CTRL_MAX; i++)
    {
        ctrlListTbl[i].hwnd = CreateWindowEx( ctrlListTbl[i].exStyle,                  /* 拡張スタイル       */
                                              ctrlListTbl[i].class,                    /* クラス名           */
                                              ctrlListTbl[i].value,                    /* ウィンドウテキスト */
                                              ctrlListTbl[i].style,                    /* スタイル           */
                                              ctrlListTbl[i].x,ctrlListTbl[i].y,       /* x座標,y座標        */
                                              ctrlListTbl[i].width,                    /* 幅                 */
                                              ctrlListTbl[i].height,                   /* 高さ               */
                                              hwnd,(HMENU)(SOME_CTRL_ID_OFFSET+i),     /* 親ウィンドウ,子ウィンドウID */
                                              ghInst,NULL );                           /* インスタンスハンドル,補助引数 */
        if( ctrlListTbl[i].hwnd != NULL )
        {
            SendMessage(ctrlListTbl[i].hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));

            /* 個別に処理するもの */
            switch( i )
            {
            default:
                break;
            }

            ShowWindow(ctrlListTbl[i].hwnd, SW_SHOW);
            ctrlListTbl[i].exist = TRUE;
        }
        else
        {
            /* do nothing */
        }
    }

    return TRUE;
}


/********************************************************************************
 * 内容   : 登録された全てのコントロールのサイズを調整する
 * 引数   : HWND hwnd親ウィンドウのハンドラ
 * 戻り値 : BOOL
 ********************************************************************************/
BOOL
SomeCtrlSize(int cxClient, int cyClient)
{
    int i;

    for(i=0; i<SOME_CTRL_MAX; i++)
    {
        if( ctrlListTbl[i].bSizeAdj )
        {
            MoveWindow( ctrlListTbl[i].hwnd, ctrlListTbl[i].x,ctrlListTbl[i].y, (cxClient + ctrlListTbl[i].widthOfst),cyClient + ctrlListTbl[i].heightOfst, TRUE);
        }
        else
        {
            /* do nothing */
        }
    }

    return TRUE;
}

/********************************************************************************
 * 内容   : コントロールのウィンドウハンドラを取得する
 * 引数   : SOME_CTRL_ID id
 * 戻り値 : HWND
 ********************************************************************************/
HWND
SomeCtrlGetHWND(SOME_CTRL_ID id)
{
    HWND rtn;

    if( ctrlListTbl[id].exist == TRUE )
    {
        rtn = ctrlListTbl[id].hwnd;
    }
    else
    {
        rtn = (HWND)0;
    }

    return rtn;
}

/********************************************************************************
 * 内容   :
 * 引数   :
 * 戻り値 :
 ********************************************************************************/
void
SomeCtrlEnable(SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, TRUE);
}

/********************************************************************************
 * 内容   :
 * 引数   :
 * 戻り値 :
 ********************************************************************************/
void
SomeCtrlDisable(SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, FALSE);
}

/********************************************************************************
 * 内容   : ウィンドウからテキストを取得する
 *
 * 引数   : SOME_CTRL_ID id
 *        : PTSTR ptstrText
 *
 * 戻り値 : BOOL
 ********************************************************************************/
BOOL
SomeCtrlGetText( SOME_CTRL_ID id, PTSTR ptstrText )
{
    BOOL rtn = FALSE;
    INT iLength;

    if( ctrlListTbl[id].exist == TRUE )
    {
        iLength = GetWindowTextLength( ctrlListTbl[id].hwnd );
        GetWindowText( ctrlListTbl[id].hwnd, ptstrText, iLength + 1 );
        rtn = TRUE;
    }
    else
    {
        /* error */
    }

    return rtn;
}
