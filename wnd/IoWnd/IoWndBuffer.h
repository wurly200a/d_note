#ifndef IOWND_BUFFER_H

typedef struct tag_buffer_line_data
{
    struct tag_buffer_line_data *prevPtr;
    struct tag_buffer_line_data *nextPtr;
    DWORD                       lineNum;
    DWORD                       caretPos;
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
 * ���e  : IO�E�B���h�E�o�b�t�@�̏I��
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffEnd( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�Z�b�g
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit  (TRUE:�����f�[�^���N���A,FALSE:�N���A���Ȃ�)
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    BUFF_ALL,
    BUFF_SELECTED
} IOWND_BUFF_REGION;

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�擾
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndBuffDataGet( TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�T�C�Y�擾
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetBuffSize( IOWND_BUFF_REGION region );

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
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetCaretXpos( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetCaretYpos( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�g�ʒu�Z�b�g
 * ����  : DWORD xPos
 * ����  : DWORD lineNum
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndSetCaretPos( DWORD xPos, DWORD lineNum );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndIncCaretXpos( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDecCaretXpos( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndIncCaretYpos( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDecCaretYpos( void );

/********************************************************************************
 * ���e  : �w��s�̃f�[�^�A�h���X�擾
 * ����  : DWORD lineNum
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffGetLinePtr( DWORD lineNum );

enum
{
    SINGLE_CHAR,
    DOUBLE_CHAR_HIGH,
    DOUBLE_CHAR_LOW,
    TAB_CHAR,
};
typedef int CHARSET_TYPE;

typedef struct
{
    TCHAR        data[8];
    INT          offset ;
    INT          size   ;
    CHARSET_TYPE type   ;
    BOOL         bSelect;
} S_BUFF_DISP_DATA;

/********************************************************************************
 * ���e  : �w��s�A�w���̃f�[�^���擾
 * ����  : DWORD  lineNum   �s
 * ����  : DWORD  dispPos   �\���ʒu
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndBuffGetDispData( DWORD lineNum, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );

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
 * ���e  : �f�[�^�폜
 * ����  : BOOL bBackSpace
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffRemoveData( BOOL bBackSpace );

/********************************************************************************
 * ���e  : ���s�ǉ�
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffAddNewLine( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃^�u�T�C�Y�Z�b�g
 * ����  : INT tabSize
 * �߂�l: BOOL (TRUE:�ύX���ꂽ)
 ***************************************/
BOOL IoWndBuffSetTabSize( INT tabSize );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�͈̔͑I��ON
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSelectOn( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�͈̔͑I��OFF
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSelectOff( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̑S�͈͑I��
 * ����  : �Ȃ�
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndBuffSelectAll( void );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
