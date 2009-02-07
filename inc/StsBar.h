#ifndef STS_BAR_H

typedef enum
{
    STS_BAR_0,
    STS_BAR_1,
    STS_BAR_2,
    STS_BAR_3,
    STS_BAR_MAX
}STS_BAR_ID;

/********************************************************************************
 * ���e  : �X�e�[�^�X�o�[����
 * ����  : HWND hwnd �e�E�B���h�E�̃n���h��
 * �߂�l: HWND
 ***************************************/
HWND StsBarCreate( HWND hwnd );

/********************************************************************************
 * ���e  : �X�e�[�^�X�o�[�̃T�C�Y����
 * ����  : int cxClient
 * ����  : int cyClient
 * �߂�l: LONG �c�����̃T�C�Y
 ***************************************/
LONG StsBarSize( int cxClient,int cyClient );

/********************************************************************************
 * ���e  : �X�e�[�^�X�o�[�ւ̕�����Z�b�g
 * ����  : STS_BAR_ID id
 * ����  : PTSTR ptstrFormat, ...
 * �߂�l: �Ȃ�
 ***************************************/
void StsBarSetText( STS_BAR_ID id, PTSTR ptstrFormat, ... );

#define STS_BAR_H
#endif /* STS_BAR_H */
