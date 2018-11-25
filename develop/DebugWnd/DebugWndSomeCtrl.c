/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */
/* 外部変数定義 */

/* 内部関数定義 */
#include "DebugWndSomeCtrl.h"
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

static S_SOME_CTRL ctrlListTbl[DEBUG_WND_SOME_CTRL_MAX] =
{
    /* exist,  hwnd, class            , value               , x , y , w  , h  ,bSAdj ,wOfst, hOfst,exStyle       ,style  */
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("PARAM1")      ,  5, 15, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 10, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("PARAM2")      ,  5, 40, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 35, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("PARAM3")      ,  5, 65, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 60, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },

    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("4")           ,  5, 90, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 85, 55 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("5")           ,130, 90, 25 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,160, 85, 80 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("6")           ,245, 90, 25 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,275, 85, 80 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("7")           ,360, 90, 25 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,390, 85, 80 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("8")           ,480, 90, 30 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,515, 85, 40 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("9")           ,565, 90, 40 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,610, 85, 40 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },

    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("combo")       ,  5,115, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("combobox"), TEXT("")            , 70,110, 60 , 300,FALSE ,0    , 0    ,0            ,(WS_CHILD|CBS_SORT|CBS_DROPDOWNLIST)},
    {FALSE  ,(HWND)0, PROGRESS_CLASS  , TEXT("")            , 70,160, 0  , 15 ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|ES_LEFT)                       },
    {FALSE  ,(HWND)0, TEXT("button")  , TEXT("Exec")        , 70,135, 100, 20 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_LEFT)                       },
};


/********************************************************************************
 * 内容   : 登録された全てのコントロールを生成する
 * 引数   : HINSTANCE hInst
 * 引数   : HWND hwnd親ウィンドウのハンドラ
 * 戻り値 : BOOL
 ********************************************************************************/
BOOL
DebugWndSomeCtrlCreate(HINSTANCE hInst,HWND hwnd)
{
    int i;

    for(i=0; i<DEBUG_WND_SOME_CTRL_MAX; i++)
    {
        ctrlListTbl[i].hwnd = CreateWindowEx( ctrlListTbl[i].exStyle,                  /* 拡張スタイル       */
                                              ctrlListTbl[i].class,                    /* クラス名           */
                                              ctrlListTbl[i].value,                    /* ウィンドウテキスト */
                                              ctrlListTbl[i].style,                    /* スタイル           */
                                              ctrlListTbl[i].x,ctrlListTbl[i].y,       /* x座標,y座標        */
                                              ctrlListTbl[i].width,                    /* 幅                 */
                                              ctrlListTbl[i].height,                   /* 高さ               */
                                              hwnd,(HMENU)(DEBUG_WND_SOME_CTRL_ID_OFFSET+i), /* 親ウィンドウ,子ウィンドウID */
                                              hInst,NULL );                            /* インスタンスハンドル,補助引数 */
        if( ctrlListTbl[i].hwnd != NULL )
        {
            SendMessage(ctrlListTbl[i].hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));

            /* 個別に処理するもの */
            switch( i )
            {
            case DEBUG_WND_SOME_CTRL_COMBOBOX1:
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("2"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("3"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("4"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("5"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("6"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("7"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("8"));
                SendMessage(ctrlListTbl[i].hwnd, CB_SELECTSTRING, 0, (LPARAM)TEXT("1"));
                break;
            case DEBUG_WND_SOME_CTRL_PROGRESS_BAR:
#if 0
                SendMessage(ctrlListTbl[i].hwnd, PBM_SETRANGE,(WPARAM)0 ,MAKELPARAM(0, 100) ); /* range 0-100 */
                SendMessage(ctrlListTbl[i].hwnd, PBM_SETPOS  ,(WPARAM)50,0                  ); /* set 50      */
                SendMessage(ctrlListTbl[i].hwnd, PBM_SETSTEP ,(WPARAM)1 ,0                  ); /* 1/1step     */
                SendMessage(ctrlListTbl[i].hwnd, PBM_STEPIT  ,0         ,0                  ); /* 1step add   */
#endif
                break;
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
DebugWndSomeCtrlSize(int cxClient, int cyChar)
{
    int i;

    for(i=0; i<DEBUG_WND_SOME_CTRL_MAX; i++)
    {
        if( ctrlListTbl[i].bSizeAdj )
        {
            MoveWindow( ctrlListTbl[i].hwnd, ctrlListTbl[i].x,ctrlListTbl[i].y, (cxClient + ctrlListTbl[i].widthOfst),cyChar + ctrlListTbl[i].heightOfst, TRUE);
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
 * 引数   : DEBUG_WND_SOME_CTRL_ID id
 * 戻り値 : HWND
 ********************************************************************************/
HWND
DebugWndSomeCtrlGetHWND(DEBUG_WND_SOME_CTRL_ID id)
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
DebugWndSomeCtrlEnable(DEBUG_WND_SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, TRUE);
}

/********************************************************************************
 * 内容   :
 * 引数   :
 * 戻り値 :
 ********************************************************************************/
void
DebugWndSomeCtrlDisable(DEBUG_WND_SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, FALSE);
}

/********************************************************************************
 * 内容   : ウィンドウからテキストを取得する
 *
 * 引数   : DEBUG_WND_SOME_CTRL_ID id
 *        : PTSTR ptstrText
 *
 * 戻り値 : BOOL
 ********************************************************************************/
BOOL
DebugWndSomeCtrlGetText( DEBUG_WND_SOME_CTRL_ID id, PTSTR ptstrText )
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
