#ifndef DEBUGWND_H

/********************************************************************************
 * ���e  : �f�o�b�O�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : int nCmdShow
 * �߂�l: HWND
 ***************************************/
HWND DebugWndCreate( int nCmdShow );

/********************************************************************************
 * ���e  : �f�o�b�O�E�B���h�E���ŏ������郁�b�Z�[�W�𔻒肷��
 * ����  : MSG *msgPtr
 * �߂�l: BOOL
 ***************************************/
BOOL IsDebugWndMessage( MSG *msgPtr );

#define DEBUGWND_H
#endif /* DEBUGWND_H */
