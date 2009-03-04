/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "MainWndMenu.h"

/* �O���֐���` */
/* �O���ϐ���` */
/* �����֐���` */
/* �����ϐ���` */
HMENU hMenu;

/********************************************************************************
 * ���e  : ���j���[�̐���
 * ����  : �Ȃ�
 * �߂�l: HMENU
 ***************************************/
HMENU
MenuCreate( void )
{
    HMENU hMenuPopup,hMenuPopup2;

    hMenu = CreateMenu();

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_NEW        , TEXT("�V�K(&N)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_OPEN       , TEXT("�J��(&O)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE       , TEXT("�㏑���ۑ�(&S)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE_AS    , TEXT("���O��t���ĕۑ�(&A)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PAGE       , TEXT("�y�[�W�ݒ�(&U)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PRINT      , TEXT("���(&P)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_EXIT       , TEXT("�I��(&X)") );
    AppendMenu( hMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("�t�@�C��(&F)") );

    EnableMenuItem( hMenu, IDM_FILE_SAVE      , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_FILE_SAVE_AS   , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_FILE_PAGE      , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_FILE_PRINT     , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_UNDO       , TEXT("���ɖ߂�(&U)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_CUT        , TEXT("�؂���(&T)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_COPY       , TEXT("�R�s�[(&C)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_PASTE      , TEXT("�\��t��(&P)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DELETE     , TEXT("�폜(&L)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND       , TEXT("����(&F)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND_NEXT  , TEXT("��������(&N)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_REPLACE    , TEXT("�u��(&R)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_GOTO_LINE  , TEXT("�s�ֈړ�(&G)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("�S�đI��(&A)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DATETIME   , TEXT("���t�Ǝ���(&D)") );
    AppendMenu( hMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("�ҏW(&E)") );

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
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_WRAP     , TEXT("�E�[�Ő܂�Ԃ�(&W)") );
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_FONT     , TEXT("�t�H���g(&F)...") );
    AppendMenu( hMenu     , MF_POPUP , (UINT_PTR)hMenuPopup, TEXT("����(&O)") );

    EnableMenuItem( hMenu, IDM_FORMAT_WRAP , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_VIEW_STS_BAR, TEXT("�X�e�[�^�X �o�[(&S)") );
    AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�\��(&V)") );

    hMenuPopup2 = CreateMenu();
    AppendMenu( hMenuPopup2, MF_STRING, IDM_EXTEND_NEWLINE_CRLF, TEXT("CR+LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_EXTEND_NEWLINE_LF  , TEXT("LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_EXTEND_NEWLINE_CR  , TEXT("CR") );
    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_POPUP, (UINT_PTR)hMenuPopup2, TEXT("���s�R�[�h(&N)") );
    AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�g��(&X)") );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_HELP  , TEXT("�g�s�b�N�̌���(&H)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0              , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_ABOUT , TEXT("�o�[�W�������(&A)") );
    AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�w���v(&H)") );

    EnableMenuItem( hMenu, IDM_HELP_HELP  , MF_GRAYED );
    EnableMenuItem( hMenu, IDM_HELP_ABOUT , MF_GRAYED );

    return hMenu;
}

/********************************************************************************
 * ���e  : �w��̍��ڂ��`�F�b�N����Ă��邩�₢���킹
 * ����  : UINT itemID
 * �߂�l: BOOL
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
 * ���e  : �w��̍��ڂ̃`�F�b�N��t����
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
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
 * ���e  : �w��̍��ڂ̃`�F�b�N���O��
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
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