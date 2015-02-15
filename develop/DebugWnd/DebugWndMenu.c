/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "DebugMenuId.h"
#include "DebugWndMenu.h"

/* 外部関数定義 */
/* 外部変数定義 */
/* 内部関数定義 */
/* 内部変数定義 */
HMENU hDebugMenu;

static const ACCEL debugAccelTbl[] =
{
    { (FVIRTKEY|FNOINVERT|FCONTROL),'N'       ,IDM_DEBUG_FILE_NEW        },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'O'       ,IDM_DEBUG_FILE_OPEN       },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'S'       ,IDM_DEBUG_FILE_SAVE       },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'P'       ,IDM_DEBUG_FILE_PRINT      },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'Z'       ,IDM_DEBUG_EDIT_UNDO       },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'X'       ,IDM_DEBUG_EDIT_CUT        },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'C'       ,IDM_DEBUG_EDIT_COPY       },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'V'       ,IDM_DEBUG_EDIT_PASTE      },
    { (FVIRTKEY|FNOINVERT)         ,VK_DELETE ,IDM_DEBUG_EDIT_DELETE     },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'F'       ,IDM_DEBUG_EDIT_FIND       },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'H'       ,IDM_DEBUG_EDIT_REPLACE    },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'G'       ,IDM_DEBUG_EDIT_GOTO_LINE  },
    { (FVIRTKEY|FNOINVERT|FCONTROL),'A'       ,IDM_DEBUG_EDIT_SELECT_ALL },
    { (FVIRTKEY|FNOINVERT)         ,VK_F5     ,IDM_DEBUG_EDIT_DATETIME   },
};

/********************************************************************************
 * 内容  : メニューの生成
 * 引数  : なし
 * 戻り値: HMENU
 ***************************************/
HMENU
DebugMenuCreate( void )
{
    HMENU hMenuPopup,hMenuPopup2;

    hDebugMenu = CreateMenu();

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_NEW        , TEXT("新規(&N)\tCtrl+N") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_OPEN       , TEXT("開く(&O)...\tCtrl+O") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_SAVE       , TEXT("上書き保存(&S)\tCtrl+S") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_SAVE_AS    , TEXT("名前を付けて保存(&A)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_PAGE       , TEXT("ページ設定(&U)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_PRINT      , TEXT("印刷(&P)...\tCtrl+P") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_EXIT       , TEXT("終了(&X)") );
    AppendMenu( hDebugMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("ファイル(&F)") );

    EnableMenuItem( hDebugMenu, IDM_DEBUG_FILE_PAGE      , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_FILE_PRINT     , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_UNDO       , TEXT("元に戻す(&U)\tCtrl+Z") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_CUT        , TEXT("切り取り(&T)\tCtrl+X") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_COPY       , TEXT("コピー(&C)\tCtrl+C") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_PASTE      , TEXT("貼り付け(&P)\tCtrl+V") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_DELETE     , TEXT("削除(&L)\tDel") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_FIND       , TEXT("検索(&F)...\tCtrl+F") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_FIND_NEXT  , TEXT("次を検索(&N)\tF3") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_REPLACE    , TEXT("置換(&R)...\tCtrl+H") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_GOTO_LINE  , TEXT("行へ移動(&G)...\tCtrl+G") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_SELECT_ALL , TEXT("すべて選択(&A)\tCtrl+A") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_DATETIME   , TEXT("日付と時刻(&D)\tF5") );
    AppendMenu( hDebugMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("編集(&E)") );

#if 0
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_UNDO       , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_FIND       , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_FIND_NEXT  , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_REPLACE    , MF_GRAYED );
#endif
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_GOTO_LINE  , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_DEBUG_FORMAT_WRAP     , TEXT("右端で折り返す(&W)") );
    AppendMenu( hMenuPopup, MF_STRING, IDM_DEBUG_FORMAT_FONT     , TEXT("フォント(&F)...") );
    AppendMenu( hDebugMenu     , MF_POPUP , (UINT_PTR)hMenuPopup, TEXT("書式(&O)") );

    EnableMenuItem( hDebugMenu, IDM_DEBUG_FORMAT_WRAP , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_DEBUG_VIEW_STS_BAR, TEXT("ステータス バー(&S)") );
    AppendMenu( hDebugMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("表示(&V)") );

    hMenuPopup2 = CreateMenu();
    AppendMenu( hMenuPopup2, MF_STRING, IDM_DEBUG_EXTEND_NEWLINE_CRLF, TEXT("CR+LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_DEBUG_EXTEND_NEWLINE_LF  , TEXT("LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_DEBUG_EXTEND_NEWLINE_CR  , TEXT("CR") );
    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_POPUP, (UINT_PTR)hMenuPopup2, TEXT("改行コード(&N)") );
    AppendMenu( hDebugMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("拡張(&X)") );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_HELP_HELP  , TEXT("トピックの検索(&H)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0              , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_HELP_ABOUT , TEXT("バージョン情報(&A)") );
    AppendMenu( hDebugMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("ヘルプ(&H)") );

    EnableMenuItem( hDebugMenu, IDM_DEBUG_HELP_HELP  , MF_GRAYED );

    return hDebugMenu;
}

/********************************************************************************
 * 内容  : アクセラレータの生成
 * 引数  : なし
 * 戻り値: HACCEL
 ***************************************/
HACCEL
DebugAccelCreate( void )
{
    return CreateAcceleratorTable((LPACCEL)debugAccelTbl,(int)(sizeof(debugAccelTbl)/sizeof(ACCEL)));
}

/********************************************************************************
 * 内容  : 指定の項目がチェックされているか問い合わせ
 * 引数  : UINT itemID
 * 戻り値: BOOL
 ***************************************/
BOOL
DebugMenuInqItemChecked( UINT itemID )
{
    INT iMenuFlags;
    BOOL rtn = FALSE;

    if( hDebugMenu != NULL )
    {
        iMenuFlags = GetMenuState( hDebugMenu, itemID, MF_BYCOMMAND );
        if( iMenuFlags & MF_CHECKED )
        {
            rtn = TRUE;
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

    return rtn;
}

/********************************************************************************
 * 内容  : 指定の項目のチェックを付ける
 * 引数  : UINT itemID
 * 戻り値: なし
 ***************************************/
void
DebugMenuCheckItem( UINT itemID )
{
    if( hDebugMenu != NULL )
    {
        CheckMenuItem( hDebugMenu, itemID, MF_CHECKED );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 指定の項目のチェックを外す
 * 引数  : UINT itemID
 * 戻り値: なし
 ***************************************/
void
DebugMenuUnCheckItem( UINT itemID )
{
    if( hDebugMenu != NULL )
    {
        CheckMenuItem( hDebugMenu, itemID, MF_UNCHECKED );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 指定の項目を有効にする
 * 引数  : UINT itemID
 * 戻り値: なし
 ***************************************/
void
DebugMenuEnableItem( UINT itemID )
{
    if( hDebugMenu != NULL )
    {
        EnableMenuItem( hDebugMenu, itemID, MF_ENABLED );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * 内容  : 指定の項目を無効にする
 * 引数  : UINT itemID
 * 戻り値: なし
 ***************************************/
void
DebugMenuUnEnableItem( UINT itemID )
{
    if( hDebugMenu != NULL )
    {
        EnableMenuItem( hDebugMenu, itemID, MF_GRAYED );
    }
    else
    {
        nop();
    }
}
