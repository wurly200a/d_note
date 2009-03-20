#ifndef MAINWND_MENU_H

#define IDM_FILE_NEW                    40001
#define IDM_FILE_OPEN                   40002
#define IDM_FILE_SAVE                   40003
#define IDM_FILE_SAVE_AS                40004
#define IDM_FILE_PAGE                   40005
#define IDM_FILE_PRINT                  40006
#define IDM_FILE_EXIT                   40007

#define IDM_EDIT_UNDO                   40011
#define IDM_EDIT_CUT                    40012
#define IDM_EDIT_COPY                   40013
#define IDM_EDIT_PASTE                  40014
#define IDM_EDIT_DELETE                 40015
#define IDM_EDIT_FIND                   40016
#define IDM_EDIT_FIND_NEXT              40017
#define IDM_EDIT_REPLACE                40018
#define IDM_EDIT_GOTO_LINE              40019
#define IDM_EDIT_SELECT_ALL             40020
#define IDM_EDIT_DATETIME               40021

#define IDM_FORMAT_WRAP                 40031
#define IDM_FORMAT_FONT                 40032

#define IDM_VIEW_STS_BAR                40041

#define IDM_EXTEND_NEWLINE_CRLF         40051
#define IDM_EXTEND_NEWLINE_LF           40052
#define IDM_EXTEND_NEWLINE_CR           40053

#define IDM_HELP_HELP                   40061
#define IDM_HELP_ABOUT                  40062

/********************************************************************************
 * ���e  : ���j���[�̐���
 * ����  : �Ȃ�
 * �߂�l: HMENU
 ***************************************/
HMENU MenuCreate( void );

/********************************************************************************
 * ���e  : �A�N�Z�����[�^�̐���
 * ����  : �Ȃ�
 * �߂�l: HACCEL
 ***************************************/
HACCEL AccelCreate( void );

/********************************************************************************
 * ���e  : �w��̍��ڂ��`�F�b�N����Ă��邩�₢���킹
 * ����  : UINT itemID
 * �߂�l: BOOL
 ***************************************/
BOOL MenuInqItemChecked( UINT itemID );

/********************************************************************************
 * ���e  : �w��̍��ڂ̃`�F�b�N��t����
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
 ***************************************/
void MenuCheckItem( UINT itemID );

/********************************************************************************
 * ���e  : �w��̍��ڂ̃`�F�b�N���O��
 * ����  : UINT itemID
 * �߂�l: �Ȃ�
 ***************************************/
void MenuUnCheckItem( UINT itemID );

#define MAINWND_MENU_H
#endif /* MAINWND_MENU_H */
