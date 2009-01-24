#include "common.h"

/* �O���֐���` */
#include "MainWnd.h"

/* �O���ϐ���` */
/* �����֐���` */
static BOOL initializeApp( HINSTANCE, int);

/* �����ϐ���` */
HINSTANCE ghInst;    /* �C���X�^���X�̃n���h��     */
TCHAR szAppName[] = TEXT("hdt"); /* �A�v���P�[�V�����̖��� */

/********************************************************************************
 * �֐���: WinMain()
 * ����  : HINSTANCE hInstance     �C���X�^���X�̃n���h��
 *         HINSTANCE hPrevInstance ���̃v���O���������O�Ɏ��s����Ă����C���X�^���X�̃n���h��
 *         PSTR szCmdLine          �v���O�����̋N���̂��߂Ɏg��ꂽ�R�}���h�s
 *         int iCmdShow            �v���O�����̏����\���̌`�Ԃ̎w��
 * �߂�l: int
 * ���e  : WINDOWS�v���O�����̃G���g���|�C���g
 ***************************************/
int WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow )
{
    MSG msg;

    if( !initializeApp( hInstance, nCmdShow ) )
    {
        MessageBox(NULL, "couldn't start!", NULL, MB_OK);
        return 0;
    }

    while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        if( !IsMainWndMessage( &msg ) )
        {
            TranslateMessage( &msg ) ;
            DispatchMessage( &msg ) ;
        }
    }

    return msg.wParam; /* PostQuitMessage()�̈��� */
}


/********************************************************************************
 * �֐��� : initializeApp()
 * ����   : HINSTANCE hInst, int nCmdShow
 * �߂�l : BOOL
 * ���e   : �A�v���P�[�V�����̏�����
 ***************************************/
static BOOL
initializeApp( HINSTANCE hInst, int nCmdShow )
{
    ghInst = hInst;
    return MainWndCreate( nCmdShow ); /* ���C���E�B���h�E���� */
}
