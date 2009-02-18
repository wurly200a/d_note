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
 * ���e  : ������o��
 * ����  : TCHAR* strPtr
 * ����  : int length
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndPrint( TCHAR* strPtr, int length );

#define IOWND_H
#endif /* IOWND_H */
