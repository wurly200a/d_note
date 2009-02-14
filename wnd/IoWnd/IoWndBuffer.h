#ifndef IOWND_BUFFER_H

typedef struct tag_iownd_buffer_data
{
    struct tag_iownd_buffer_data *prevPtr;
    struct tag_iownd_buffer_data *nextPtr;
    DWORD                         dataSize;
    TCHAR                         data[];
} S_IOWND_BUFF_DATA;

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̏�����
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffInit( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@(�A�����X�g)�ւ̃f�[�^�ǉ�
 * ����  : TCHAR* strPtr
 * ����  : DWORD  length
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSetLinkedList( TCHAR* strPtr, DWORD length );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�ւ̃f�[�^�ǉ�
 * ����  : TCHAR* strPtr
 * ����  : DWORD  length
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSet( TCHAR* strPtr, DWORD length );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�|�C���^�擾
 * ����  : �Ȃ�
 * �߂�l: TCHAR *
 ***************************************/
TCHAR * IoWndGetBuffPtr( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetBuffSize( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő�s�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetLineMaxSize( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő啶���T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetColumnMaxSize( void );

/********************************************************************************
 * ���e  : �w��s�̃f�[�^�A�h���X�擾
 * ����  : DWORD lineNum
 * �߂�l: S_IOWND_BUFF_DATA *
 ***************************************/
S_IOWND_BUFF_DATA *IoWndBuffGetLinePtr( DWORD lineNum );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
