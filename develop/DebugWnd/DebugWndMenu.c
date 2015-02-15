/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "DebugMenuId.h"
#include "DebugWndMenu.h"

/* �O���֐���` */
/* �O���ϐ���` */
/* �����֐���` */
/* �����ϐ���` */
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
 * ���e  : ���j���[�̐���
 * ����  : �Ȃ�
 * �߂�l: HMENU
 ***************************************/
HMENU
DebugMenuCreate( void )
{
    HMENU hMenuPopup,hMenuPopup2;

    hDebugMenu = CreateMenu();

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_NEW        , TEXT("�V�K(&N)\tCtrl+N") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_OPEN       , TEXT("�J��(&O)...\tCtrl+O") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_SAVE       , TEXT("�㏑���ۑ�(&S)\tCtrl+S") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_SAVE_AS    , TEXT("���O��t���ĕۑ�(&A)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_PAGE       , TEXT("�y�[�W�ݒ�(&U)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_PRINT      , TEXT("���(&P)...\tCtrl+P") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_FILE_EXIT       , TEXT("�I��(&X)") );
    AppendMenu( hDebugMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("�t�@�C��(&F)") );

    EnableMenuItem( hDebugMenu, IDM_DEBUG_FILE_PAGE      , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_FILE_PRINT     , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_UNDO       , TEXT("���ɖ߂�(&U)\tCtrl+Z") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_CUT        , TEXT("�؂���(&T)\tCtrl+X") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_COPY       , TEXT("�R�s�[(&C)\tCtrl+C") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_PASTE      , TEXT("�\��t��(&P)\tCtrl+V") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_DELETE     , TEXT("�폜(&L)\tDel") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_FIND       , TEXT("����(&F)...\tCtrl+F") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_FIND_NEXT  , TEXT("��������(&N)\tF3") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_REPLACE    , TEXT("�u��(&R)...\tCtrl+H") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_GOTO_LINE  , TEXT("�s�ֈړ�(&G)...\tCtrl+G") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_SELECT_ALL , TEXT("���ׂđI��(&A)\tCtrl+A") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_EDIT_DATETIME   , TEXT("���t�Ǝ���(&D)\tF5") );
    AppendMenu( hDebugMenu     , MF_POPUP    , (UINT_PTR)hMenuPopup, TEXT("�ҏW(&E)") );

#if 0
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_UNDO       , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_FIND       , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_FIND_NEXT  , MF_GRAYED );
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_REPLACE    , MF_GRAYED );
#endif
    EnableMenuItem( hDebugMenu, IDM_DEBUG_EDIT_GOTO_LINE  , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_DEBUG_FORMAT_WRAP     , TEXT("�E�[�Ő܂�Ԃ�(&W)") );
    AppendMenu( hMenuPopup, MF_STRING, IDM_DEBUG_FORMAT_FONT     , TEXT("�t�H���g(&F)...") );
    AppendMenu( hDebugMenu     , MF_POPUP , (UINT_PTR)hMenuPopup, TEXT("����(&O)") );

    EnableMenuItem( hDebugMenu, IDM_DEBUG_FORMAT_WRAP , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_DEBUG_VIEW_STS_BAR, TEXT("�X�e�[�^�X �o�[(&S)") );
    AppendMenu( hDebugMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�\��(&V)") );

    hMenuPopup2 = CreateMenu();
    AppendMenu( hMenuPopup2, MF_STRING, IDM_DEBUG_EXTEND_NEWLINE_CRLF, TEXT("CR+LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_DEBUG_EXTEND_NEWLINE_LF  , TEXT("LF") );
    AppendMenu( hMenuPopup2, MF_STRING, IDM_DEBUG_EXTEND_NEWLINE_CR  , TEXT("CR") );
    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_POPUP, (UINT_PTR)hMenuPopup2, TEXT("���s�R�[�h(&N)") );
    AppendMenu( hDebugMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�g��(&X)") );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_HELP_HELP  , TEXT("�g�s�b�N�̌���(&H)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0              , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_DEBUG_HELP_ABOUT , TEXT("�o�[�W�������(&A)") );
    AppendMenu( hDebugMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�w���v(&H)") );

    EnableMenuItem( hDebugMenu, IDM_DEBUG_HELP_HELP  , MF_GRAYED );

    return hDebugMenu;
}

/********************************************************************************
 * ���e  : �A�N�Z�����[�^�̐���
 * ����  : �Ȃ�
 * �߂�l: HACCEL
 ***************************************/
HACCEL
DebugAccelCreate( void )
{
    return CreateAcceleratorTable((LPACCEL)debugAccelTbl,(int)(sizeof(debugAccelTbl)/sizeof(ACCEL)));
}

/********************************************************************************
 * ���e  : �w��̍��ڂ��`�F�b�N����Ă��邩�₢���킹
 * ����  : UINT itemID
 * �߂�l: BOOL
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
 * ���e  : �w��̍��ڂ̃`�F�b�N��t����
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
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
 * ���e  : �w��̍��ڂ̃`�F�b�N���O��
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
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
 * ���e  : �w��̍��ڂ�L���ɂ���
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
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
 * ���e  : �w��̍��ڂ𖳌��ɂ���
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
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
