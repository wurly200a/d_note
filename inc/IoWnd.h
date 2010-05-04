#ifndef IOWND_H

/* [�Ή����b�Z�[�W] */
/*                                                                              */
/*                                                                              */
/*                                                                              */
/*                                                                              */
/* WM_CUT                                                                       */
/* WM_COPY                                                                      */
/* WM_PASTE                                                                     */
/* WM_CLEAR                                                                     */
/*                                                                              */
/* EM_SETSEL                                                                    */
/* WM_SETFONT                                                                   */
/*                                                                              */
/*                                                                              */
/* [���Ή�]                                                                     */
/*                                                                              */
/*                                                                              */
/* [�݊����̖�������]                                                           */
/*                                                                              */
/* GetWindowTextLength(HWND hWnd)                                               */
/* GetWindowText(HWND hWnd,LPTSTR lpString,int nMaxCount)                       */
/* SetWindowText(HWND hWnd,LPCTSTR lpString)                                    */
/*                                                                              */
/*                                                                              */
/*                                                                              */
/*                                                                              */

/********************************************************************************
 * ���e  : IO�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : HINSTANCE hInst
 * ����  : HWND hWndParent �e�E�B���h�E�̃n���h��
 * �߂�l: HWND
 ***************************************/
HWND IoWndCreate( HINSTANCE hInst, HWND hWndParent );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̏�����
 * ����  : HWND hwnd
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDataInit( HWND hwnd );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�Z�b�g
 * ����  : HWND hwnd
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit (TRUE:�����f�[�^���N���A,FALSE:�N���A���Ȃ�)
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDataSet( HWND hwnd, TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    IOWND_ALL,
    IOWND_SELECTED
} IOWND_REGION;

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�擾
 * ����  : HWND hwnd
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * ����  : IOWND_REGION region
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndDataGet( HWND hwnd, TCHAR *dataPtr, DWORD dataSize, IOWND_REGION region );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�T�C�Y�擾
 * ����  : HWND hwnd
 * ����  : IOWND_REGION region
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetDataSize( HWND hwnd, IOWND_REGION region );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő�s�T�C�Y�擾
 * ����  : HWND hwnd
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetLineMaxSize( HWND hwnd );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő啶���T�C�Y�擾
 * ����  : HWND hwnd
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetColumnMaxSize( HWND hwnd );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu�擾
 * ����  : HWND hwnd
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetCaretXpos( HWND hwnd );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu�擾
 * ����  : HWND hwnd
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetCaretYpos( HWND hwnd );

enum
{
    NEWLINECODE_CRLF = 0,
    NEWLINECODE_LF   ,
    NEWLINECODE_CR   ,
    NEWLINECODE_NONE ,
};
typedef UINT NEWLINECODE_TYPE;
/********************************************************************************
 * ���e  : IO�E�B���h�E�̉��s�R�[�h�Z�b�g
 * ����  : HWND hwnd
 * ����  : NEWLINECODE_TYPE newLineCodeType
 * �߂�l: BOOL (TRUE:�\�����������K�v)
 ***************************************/
BOOL IoWndNewLineCodeSet( HWND hwnd, NEWLINECODE_TYPE newLineCodeType );

#define IOWND_H
#endif /* IOWND_H */
