#ifndef MAINWND_H

/********************************************************************************
 * ���e  : ���C���E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : int nCmdShow
 * ����  : HACCEL *hAccelPtr
 * �߂�l: HWND
 ***************************************/
HWND MainWndCreate( int nCmdShow, HACCEL *hAccelPtr );

/********************************************************************************
 * ���e  : ���C���E�B���h�E���ŏ������郁�b�Z�[�W�𔻒肷��
 * ����  : MSG *msg
 * �߂�l: BOOL
 ***************************************/
BOOL IsMainWndMessage( MSG *msg );

#define MAINWND_H
#endif /* MAINWND_H */
