#ifndef IOWND_BUFFER_H

typedef struct tag_buffer_line_data
{
    struct tag_buffer_line_data *prevPtr;
    struct tag_buffer_line_data *nextPtr;
    DWORD                       dataSize;
    INT                         newLineCodeSize;
    TCHAR                       data[];
} S_BUFF_LINE_DATA;

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
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffGetLinePtr( DWORD lineNum );

#define IOWND_BUFF_NEWLINE_CRLF 0
#define IOWND_BUFF_NEWLINE_LF   1
#define IOWND_BUFF_NEWLINE_CR   2
#define IOWND_BUFF_NEWLINE_NONE 3
/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̉��s�R�[�h�Z�b�g
 * ����  : UINT newLineType
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSetNewLineCode( UINT newLineType );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
