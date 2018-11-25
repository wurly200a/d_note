#ifndef DEBUGWND_H

enum
{
    DEBUG_WND_CTRL_ID_01,
    DEBUG_WND_CTRL_ID_02,
    DEBUG_WND_CTRL_ID_03,
    DEBUG_WND_CTRL_ID_04,
    DEBUG_WND_CTRL_ID_05,
    DEBUG_WND_CTRL_ID_06,
    DEBUG_WND_CTRL_ID_07,
    DEBUG_WND_CTRL_ID_08,
    DEBUG_WND_CTRL_ID_09,
    DEBUG_WND_CTRL_ID_MAX
};
typedef int DEBUG_WND_CTRL_ID ;

/********************************************************************************
 * ���e  : �f�o�b�O�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : HINSTANCE hInst
 * ����  : PTSTR szAppName
 * ����  : int nCmdShow
 * �߂�l: HWND
 ***************************************/
HWND DebugWndCreate( HINSTANCE hInst, PTSTR szAppName, int nCmdShow );

/********************************************************************************
 * ���e  : �f�o�b�O�E�B���h�E���ŏ������郁�b�Z�[�W�𔻒肷��
 * ����  : MSG *msgPtr
 * �߂�l: BOOL
 ***************************************/
BOOL IsDebugWndMessage( MSG *msgPtr );

/********************************************************************************
 * ���e   : �f�o�b�O�E�B���h�E�ւ� printf
 * ����   : PTSTR ptstrFormat, ...
 * �߂�l : BOOL
 ********************************************************************************/
BOOL DebugWndPrintf( PTSTR ptstrFormat, ...);

/********************************************************************************
 * ���e   : �f�o�b�O�E�B���h�E�ւ� printf (�㏑��)
 * ����   : PTSTR ptstrFormat, ...
 * �߂�l : BOOL
 ********************************************************************************/
BOOL DebugWndPrintfOW( PTSTR ptstrFormat, ...);

/********************************************************************************
 * ���e   : �f�o�b�O�E�B���h�E�̍ŏI�s�N���A
 * ����   : �Ȃ�
 * �߂�l : BOOL
 ********************************************************************************/
BOOL DebugWndClearLine( void );

/********************************************************************************
 * ���e   : �f�o�b�O�E�B���h�E�ւ� printf
 * ����   : DEBUG_WND_CTRL_ID ctrlId
 * ����   : PTSTR ptstrFormat, ...
 * �߂�l : BOOL
 ********************************************************************************/
BOOL DebugWndCtrlPrintf( DEBUG_WND_CTRL_ID ctrlId, PTSTR ptstrFormat, ...);

/********************************************************************************
 * ���e   : �f�o�b�O�E�B���h�E�ւ̃Z�b�g
 * ����   : DEBUG_WND_CTRL_ID ctrlId
 * ����   : DWORD value
 * �߂�l : BOOL
 ********************************************************************************/
BOOL DebugWndCtrlSet( DEBUG_WND_CTRL_ID ctrlId, DWORD value );

#define DEBUGWND_H
#endif /* DEBUGWND_H */
