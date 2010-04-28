#ifndef IOWND_H

/********************************************************************************
 * ���e  : IO�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : HINSTANCE hInst
 * ����  : HWND hWndParent �e�E�B���h�E�̃n���h��
 * �߂�l: HWND
 ***************************************/
HWND IoWndCreate( HINSTANCE hInst, HWND hWndParent );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃t�H���g�ύX
 * ����  : HWND hwnd
 * ����  : LOGFONT *logFontPtr
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndChangeFont( HWND hwnd, LOGFONT *logFontPtr );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃T�C�Y�ύX
 * ����  : HWND hwnd
 * ����  : int x
 * ����  : int y
 * ����  : int cxClient
 * ����  : int cyClient
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndSize( HWND hwnd, int x, int y, int cxClient, int cyClient );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̔j��
 * ����  : HWND hwnd
 * ����  : int nCmdShow
 * �߂�l: BOOL
 ***************************************/
void IoWndDestroy( HWND hwnd );

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
