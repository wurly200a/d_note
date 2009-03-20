/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */
#include "MainWndMenu.h"

/* 外部関数定義 */
/* 外部変数定義 */
/* 内部関数定義 */
/* 内部変数定義 */
HMENU hMenu;

/********************************************************************************
 * 内容  : メニューの生成
 * 引数  : なし
 * 戻り値: HMENU
 ***************************************/
HMENU
MenuCreate( void )
{
    HMENU hMenuPopup,hMenuPopup2;

    hMenu = CreateMenu();

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_NEW        , TEXT("新規(&N)\tCtrl+N") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_OPEN       , TEXT("開く(&O)...\tCtrl+O") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE       , TEXT("上書き保存(&S)\tCtrl+S") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE_AS    , TEXT("名前を付けて保存(&A)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PAGE       , TEXT("ページ設定(&U)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PRINT      , TEXT("印刷(&P)...\tCtrl+P") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_EXIT       , TEXT("終了(&X)") );
    AppendMenu( hMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("ファイル(&F)") );

    EnableMenuItem( hMenu, IDM_FILE_PAGE      , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_FILE_PRINT     , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_UNDO       , TEXT("元に戻す(&U)\tCtrl+Z") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_CUT        , TEXT("切り取り(&T)\tCtrl+X") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_COPY       , TEXT("コピー(&C)\tCtrl+C") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_PASTE      , TEXT("貼り付け(&P)\tCtrl+V") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DELETE     , TEXT("削除(&L)\tDel") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND       , TEXT("検索(&F)...\tCtrl+F") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND_NEXT  , TEXT("次を検索(&N)\tF3") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_REPLACE    , TEXT("置換(&R)...\tCtrl+H") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_GOTO_LINE  , TEXT("行へ移動(&G)...\tCtrl+G") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("全て選択(&A)\tCtrl+A") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DATETIME   , TEXT("日付と時刻(&D)") );
    AppendMenu( hMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("編集(&E)") );

    EnableMenuItem( hMenu, IDM_EDIT_UNDO       , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_CUT        , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_COPY       , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_PASTE      , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_DELETE     , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_FIND       , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_FIND_NEXT  , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_REPLACE    , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_GOTO_LINE  , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_SELECT_ALL , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_EDIT_DATETIME   , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_WRAP     , TEXT("右端で折り返す(&W)") );
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_FONT     , TEXT("フォント(&F)...") );
    AppendMenu( hMenu     , MF_POPUP , (UINT_PTR)hMenuPopup, TEXT("書式(&O)") );

    EnableMenuItem( hMenu, IDM_FORMAT_WRAP , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_VIEW_STS_BAR, TEXT("ステータス バー(&S)") );
    AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("表示(&V)") );

    hMenuPopup2 = CreateMenu();
    AppendMenu( hMenuPopup2, MF_STRING, IDM_EXTEND_NEWLINE_CRLF, TEXT("CR+LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_EXTEND_NEWLINE_LF  , TEXT("LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_EXTEND_NEWLINE_CR  , TEXT("CR") );
    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_POPUP, (UINT_PTR)hMenuPopup2, TEXT("改行コード(&N)") );
    AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("拡張(&X)") );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_HELP  , TEXT("トピックの検索(&H)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0              , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_ABOUT , TEXT("バージョン情報(&A)") );
    AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("ヘルプ(&H)") );

    EnableMenuItem( hMenu, IDM_HELP_HELP  , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_HELP_ABOUT , MF_GRAYED );

    return hMenu;
}

/********************************************************************************
 * 内容  : 指定の項目がチェックされているか問い合わせ
 * 引数  : UINT itemID
 * 戻り値: BOOL
 ***************************************/
BOOL
MenuInqItemChecked( UINT itemID )
{
    INT iMenuFlags;
    MENUITEMINFO menuItemInfo;
    BOOL rtn = FALSE;

    if( hMenu != NULL )
    {
#if 1
        iMenuFlags = GetMenuState( hMenu, itemID, MF_BYCOMMAND );
        if( iMenuFlags & MF_CHECKED )
        {
            rtn = TRUE;
        }
        else
        {
            nop();
        }
#else
        menuItemInfo.cbSize = sizeof(MENUITEMINFO);
        menuItemInfo.fMask  = MIIM_STATE;
        GetMenuItemInfo( hMenu, IDM_VIEW_STS_BAR, FALSE, &menuItemInfo );
        if( menuItemInfo.fState & MFS_CHECKED )
        {
            rtn = TRUE;
        }
        else
        {
            nop();
        }
#endif
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
MenuCheckItem( UINT itemID )
{
    if( hMenu != NULL )
    {
        CheckMenuItem( hMenu, itemID, MF_CHECKED );
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
MenuUnCheckItem( UINT itemID )
{
    if( hMenu != NULL )
    {
        CheckMenuItem( hMenu, itemID, MF_UNCHECKED );
    }
    else
    {
        nop();
    }
}
