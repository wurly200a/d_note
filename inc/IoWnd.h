#ifndef IOWND_H

/********************************************************************************
 * ���e  : IO�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : HWND hWnd
 * ����  : LOGFONT *logFontPtr
 * �߂�l: HWND
 ***************************************/
HWND IoWndCreate( HWND hWnd, LOGFONT *logFontPtr );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃t�H���g�ύX
 * ����  : LOGFONT *logFontPtr
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndChangeFont( LOGFONT *logFontPtr );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃T�C�Y�ύX
 * ����  : int x
 * ����  : int y
 * ����  : int cxClient
 * ����  : int cyClient
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndSize( int x, int y, int cxClient, int cyClient );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̔j��
 * ����  : int nCmdShow
 * �߂�l: BOOL
 ***************************************/
void IoWndDestroy( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̏�����
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDataInit( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�Z�b�g
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    IOWND_ALL,
    IOWND_SELECTED
} IOWND_REGION;

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�擾
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * ����  : IOWND_REGION region
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndDataGet( TCHAR *dataPtr, DWORD dataSize, IOWND_REGION region );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�T�C�Y�擾
 * ����  : IOWND_REGION region
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetDataSize( IOWND_REGION region );

/********************************************************************************
 * ���e  : IO�E�B���h�E�̋�`������
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndInvalidateRect( void );

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
 * ����  : NEWLINECODE_TYPE newLineCodeType
 * �߂�l: BOOL (TRUE:�f�[�^���ύX���ꂽ)
 ***************************************/
BOOL IoWndNewLineCodeSet( NEWLINECODE_TYPE newLineCodeType );

#define IOWND_H
#endif /* IOWND_H */
