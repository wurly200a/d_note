#ifndef IOWND_BUFFER_H

typedef struct tagS_IOWND_BUFF
{
    BOOL bValid;
} S_IOWND_BUFF;

typedef S_IOWND_BUFF * H_IOWND_BUFF;

typedef struct tag_buffer_line_data
{
    S_LIST_HEADER header         ;
    DWORD         lineNum        ; /* Y�ʒu            */
    DWORD         caretDataPos   ; /* X�ʒu            */
    DWORD         dataSize       ; /* �f�[�^�T�C�Y     */
    INT           newLineCodeSize; /* ���s�R�[�h�T�C�Y */
    TCHAR         data[]         ;
} S_BUFF_LINE_DATA;

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃I�u�W�F�N�g����
 * ����  : �Ȃ�
 * �߂�l: H_IOWND_BUFF
 ***************************************/
H_IOWND_BUFF IoWndBuffCreate( void );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃I�u�W�F�N�g�j��
 * ����  : H_IOWND_BUFF
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndBuffDestroy( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̏�����
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffInit( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̏I��
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffEnd( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�Z�b�g
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit  (TRUE:�����f�[�^���N���A,FALSE:�N���A���Ȃ�)
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffDataSet( H_IOWND_BUFF hIoWndBuff, TCHAR* dataPtr, DWORD length, BOOL bInit );

typedef enum
{
    BUFF_ALL,
    BUFF_SELECTED
} IOWND_BUFF_REGION;

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndBuffDataGet( H_IOWND_BUFF hIoWndBuff, TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�T�C�Y�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetBuffSize( H_IOWND_BUFF hIoWndBuff, IOWND_BUFF_REGION region );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő�s�T�C�Y�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetLineMaxSize( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő啶���T�C�Y�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetColumnMaxSize( H_IOWND_BUFF hIoWndBuff);

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetCaretXpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD IoWndGetCaretYpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�g�ʒu�Z�b�g
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : DWORD xPos
 * ����  : DWORD lineNum
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndSetCaretPos( H_IOWND_BUFF hIoWndBuff, DWORD xPos, DWORD lineNum );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndIncCaretXpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDecCaretXpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndIncCaretYpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndDecCaretYpos( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : �w��s�̃f�[�^�A�h���X�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : DWORD lineNum
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *IoWndBuffGetLinePtr( H_IOWND_BUFF hIoWndBuff, DWORD lineNum );

enum
{
    SINGLE_CHAR     ,
    DOUBLE_CHAR_HIGH,
    DOUBLE_CHAR_LOW ,
    TAB_CHAR        ,
    END_CHAR        ,
};
typedef int CHARSET_TYPE;

typedef struct
{
    TCHAR        data[8] ; /* �f�[�^(���p,�S�p,�^�u)                   */
    INT          offset  ; /* ���p�ȊO�̏ꍇ�A�擪����̃I�t�Z�b�g�ʒu */
    INT          size    ; /* �f�[�^�T�C�Y                             */
    CHARSET_TYPE type    ;
    BOOL         bSelect ; /* �I������Ă��邩�ۂ�                     */
    DWORD        dataPos ; /* �f�[�^�ʒu                               */
    DWORD        caretPos; /* �L�����b�g�ʒu(��؂�̂����ʒu)         */
} S_BUFF_DISP_DATA;

/********************************************************************************
 * ���e  : �w��s�A�w���̃f�[�^���擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : S_BUFF_LINE_DATA *lineDataPtr
 * ����  : DWORD             dispPos     �\���ʒu
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndBuffGetDispData( H_IOWND_BUFF hIoWndBuff, S_BUFF_LINE_DATA *lineDataPtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );

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
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSetNewLineCode( H_IOWND_BUFF hIoWndBuff, UINT newLineType );

/********************************************************************************
 * ���e  : �f�[�^�폜
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : BOOL bBackSpace
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffRemoveData( H_IOWND_BUFF hIoWndBuff, BOOL bBackSpace );

/********************************************************************************
 * ���e  : ���s�f�[�^�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : PTSTR dataPtr
 * �߂�l: INT
 ***************************************/
INT IoWndBuffGetNewLineData( H_IOWND_BUFF hIoWndBuff, PTSTR dataPtr );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃^�u�T�C�Y�Z�b�g
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : INT tabSize
 * �߂�l: BOOL (TRUE:�ύX���ꂽ)
 ***************************************/
BOOL IoWndBuffSetTabSize( H_IOWND_BUFF hIoWndBuff, INT tabSize );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�͈̔͑I��ON
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSelectOn( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�͈̔͑I��OFF
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void IoWndBuffSelectOff( H_IOWND_BUFF hIoWndBuff );

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̑S�͈͑I��
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: BOOL
 ***************************************/
BOOL IoWndBuffSelectAll( H_IOWND_BUFF hIoWndBuff );

#define IOWND_BUFFER_H
#endif /* IOWND_BUFFER_H */
