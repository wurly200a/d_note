#ifndef SOME_CTRL_H

typedef enum
{
    SOME_CTRL_FILENAME_STATIC,
    SOME_CTRL_FILENAME,
    SOME_CTRL_FILEOPEN_BUTTON,
    SOME_CTRL_MAX
}SOME_CTRL_ID;

#define SOME_CTRL_ID_OFFSET 10
#define MY_EX_STYLE (WS_EX_OVERLAPPEDWINDOW)

/********************************************************************************
 * ���e  : �o�^���ꂽ�S�ẴR���g���[���𐶐�����
 * ����  : HINSTANCE hInst
 * ����  : PTSTR szAppName
 * ����  : HWND hwnd �e�E�B���h�E�̃n���h��
 * �߂�l: BOOL
 ***************************************/
BOOL SomeCtrlCreate( HINSTANCE hInst, PTSTR szAppName, HWND hwnd );

/********************************************************************************
 * ���e  : �o�^���ꂽ�S�ẴR���g���[���̃T�C�Y�𒲐�����
 * ����  : int cxClient
 * ����  : int cyClient
 * �߂�l: LONG �R���g���[����z�u�����`�̏c�����̃T�C�Y
 ***************************************/
LONG SomeCtrlSize( int cxClient,int cyClient );

/********************************************************************************
 * ���e  : �R���g���[���̃E�B���h�E�n���h�����擾����
 * ����  : SOME_CTRL_ID id
 * �߂�l: HWND
 ***************************************/
HWND SomeCtrlGetHWND( SOME_CTRL_ID id );

/********************************************************************************
 * ���e  : �R���g���[����L���ɂ���
 * ����  : SOME_CTRL_ID id
 * �߂�l: �Ȃ�
 ***************************************/
void SomeCtrlEnable( SOME_CTRL_ID id );

/********************************************************************************
 * ���e  : �R���g���[���𖳌��ɂ���
 * ����  : SOME_CTRL_ID id
 * �߂�l: �Ȃ�
 ***************************************/
void SomeCtrlDisable( SOME_CTRL_ID id );

/********************************************************************************
 * ���e  : �R���g���[������e�L�X�g���擾����
 * ����  : SOME_CTRL_ID id
 * ����  : PTSTR ptstrText
 * �߂�l: BOOL
 ***************************************/
BOOL SomeCtrlGetText( SOME_CTRL_ID id, PTSTR ptstrText );

#define SOME_CTRL_H
#endif /* SOME_CTRL_H */
