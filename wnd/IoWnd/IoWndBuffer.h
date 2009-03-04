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
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�Z�b�g
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffDataSet( TCHAR* dataPtr, DWORD length );

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

enum
{
    IOWND_BUFF_NEWLINE_CRLF= 0,
    IOWND_BUFF_NEWLINE_LF  ,
    IOWND_BUFF_NEWLINE_CR  ,
    IOWND_BUFF_NEWLINE_NONE,
};
/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̉��s�R�[�h�Z�b�g
 * ����  : UINT newLineType
 * �߂�l: BOOL (TRUE:�f�[�^���ύX���ꂽ)
 ***************************************/
BOOL IoWndBuffSetNewLineCode( UINT newLineType );

/********************************************************************************
 * ���e  : �w��s�ւ̃f�[�^�ǉ�
 * ����  : DWORD lineNum
 * ����  : DWORD addPos
 * ����  : TCHAR data
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffAddData( DWORD lineNum, DWORD addPos, TCHAR data );

/********************************************************************************
 * ���e  : �w��s�ւ̉��s�ǉ�
 * ����  : DWORD lineNum
 * ����  : DWORD addPos
 * ����  : TCHAR data
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffAddNewLine( DWORD lineNum, DWORD addPos );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
