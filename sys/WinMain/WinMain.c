#include "common.h"

/* �O���֐���` */
#include "MainWnd.h"

/* �O���ϐ���` */
/* �����֐���` */
static HWND initializeApp( HINSTANCE hInst, int nCmdShow, HACCEL *hAccelPtr );

/* �����ϐ���` */
static HINSTANCE hInstance;    /* �C���X�^���X�̃n���h��     */
static TCHAR szAppName[] = TEXT("hdt"); /* �A�v���P�[�V�����̖��� */

/********************************************************************************
 * ���e  : WINDOWS�v���O�����̃G���g���|�C���g
 * ����  : HINSTANCE hInstance     �C���X�^���X�̃n���h��
 * ����  : HINSTANCE hPrevInstance ���̃v���O���������O�Ɏ��s����Ă����C���X�^���X�̃n���h��
 * ����  : PSTR szCmdLine          �v���O�����̋N���̂��߂Ɏg��ꂽ�R�}���h�s
 * ����  : int iCmdShow            �v���O�����̏����\���̌`�Ԃ̎w��
 * �߂�l: int
 ***************************************/
int WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow )
{
    MSG msg;
    HACCEL hAccel;
    HWND hWnd;

    hWnd = initializeApp( hInstance, nCmdShow, &hAccel );
    if( hWnd == NULL )
    {
        MessageBox(NULL, "couldn't start!", NULL, MB_OK);
        return 0;
    }

    while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        if( !IsMainWndMessage( &msg ) )
        {
            if( !TranslateAccelerator(hWnd,hAccel,&msg) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            else
            {
                nop();
            }
        }
    }

    return msg.wParam; /* PostQuitMessage()�̈��� */
}


/********************************************************************************
 * ���e   : �A�v���P�[�V�����̏�����
 * ����   : HINSTANCE hInst
 * ����   : int nCmdShow
 * ����   : HACCEL *hAccelPtr
 * �߂�l : HWND
 ***************************************/
static HWND
initializeApp( HINSTANCE hInst, int nCmdShow, HACCEL *hAccelPtr )
{
    hInstance = hInst;
    return MainWndCreate( nCmdShow, hAccelPtr ); /* ���C���E�B���h�E���� */
}

/********************************************************************************
 * ���e   : �C���X�^���X�n���h���̎擾
 * ����   : �Ȃ�
 * �߂�l : HINSTANCE
 ***************************************/
HINSTANCE
GetHinst( void )
{
    return hInstance;
}
/********************************************************************************
 * ���e   : �A�v���P�[�V�����̖��̕�����̎擾
 * ����   : �Ȃ�
 * �߂�l : PTSTR
 ***************************************/
PTSTR
GetAppName( void )
{
    return szAppName;
}
