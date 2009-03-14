/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWndBuffer.h"
static void addLinkedList( S_BUFF_LINE_DATA *dataPtr );
static void clearLinkedList( void );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );

enum
{
    SINGLE_CHAR,
    DOUBLE_CHAR_HIGH,
    DOUBLE_CHAR_LOW,
};
typedef int CHARSET_TYPE;
CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );

static INT getNewLineSize( void );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

/* �����ϐ���` */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;
S_BUFF_LINE_DATA *ioWndBuffLineNowPtr;

typedef struct
{
    UINT  NewLineType;
    DWORD xCaret;
    DWORD yCaret;
} S_IOWND_BUFF_DATA;

S_IOWND_BUFF_DATA ioWndBuffData;

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̏�����
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffInit( void )
{
    clearLinkedList();
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�Z�b�g
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffDataSet( TCHAR* dataPtr, DWORD length )
{
    S_BUFF_LINE_DATA *lineDataPtr;
    DWORD lineLengthSum = 0;
    DWORD lineNum = 0;

    IoWndBuffInit();

    if( (dataPtr != NULL) && (length > 0) )
    { /* �f�[�^�L��̏ꍇ */
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                lineDataPtr->lineNum = lineNum;
                addLinkedList( lineDataPtr );
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
            lineNum++;
        }

        if( (lineDataPtr != NULL) &&
            (lineDataPtr->newLineCodeSize) )
        { /* �Ō�̍s�ɉ��s�R�[�h���������ꍇ�A�f�[�^0�A���s�R�[�h0�̃f�[�^��ǉ� */
            lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
            if( lineDataPtr != NULL )
            {
                lineDataPtr->dataSize = 0;
                lineDataPtr->newLineCodeSize = 0;
                lineDataPtr->lineNum = lineNum;
                addLinkedList( lineDataPtr );
            }
            else
            {
                nop();
            }
        }
        else
        {
            nop();
        }
    }
    else
    { /* �f�[�^�����̏ꍇ */
        lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
        if( lineDataPtr != NULL )
        {
            lineDataPtr->dataSize = 0;
            lineDataPtr->newLineCodeSize = 0;
            lineDataPtr->lineNum = 0;
            addLinkedList( lineDataPtr );
        }
        else
        {
            nop();
        }
    }

    if( ioWndBuffListTopPtr != NULL )
    {
        ioWndBuffLineNowPtr = ioWndBuffListTopPtr;
        ioWndBuffLineNowPtr->caretPos = 0;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �A�����X�g�̃f�[�^���N���A
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
static void
clearLinkedList( void )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

    for( nowPtr = nextPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nextPtr )
    {
        nextPtr = nowPtr->nextPtr;
        free( nowPtr );

        if( nextPtr == NULL )
        { /* ���ɂȂ���Ă���f�[�^���� */
            ioWndBuffListTopPtr = NULL;
        }
        else
        { /* ���ɂȂ���Ă���f�[�^�L�� */
            nop();
        }
    }
}

/********************************************************************************
 * ���e  : �A�����X�g�Ƀf�[�^��ǉ�����
 * ����  : S_BUFF_LINE_DATA *dataPtr
 * �߂�l: �Ȃ�
 ***************************************/
static void
addLinkedList( S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

    if( ioWndBuffListTopPtr == NULL )
    {
        dataPtr->prevPtr = NULL;
        dataPtr->nextPtr = NULL;
        ioWndBuffListTopPtr = dataPtr;
    }
    else
    {
        for( nowPtr = nextPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nextPtr )
        {
            nextPtr = nowPtr->nextPtr;

            if( nextPtr == NULL )
            { /* ���ɂȂ���Ă���f�[�^���� */
                dataPtr->prevPtr = nowPtr;
                dataPtr->nextPtr = NULL;
                nowPtr->nextPtr = dataPtr;
            }
            else
            { /* ���ɂȂ���Ă���f�[�^�L�� */
                nop();
            }
        }
    }
}

/********************************************************************************
 * ���e  : �o�b�t�@�s�f�[�^���擾
 * ����  : TCHAR *dataPtr  �f�[�^�̐擪
 * ����  : DWORD maxLength �ő咷��
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
getBuffLine( TCHAR *dataPtr, DWORD maxLength )
{
    S_BUFF_LINE_DATA *lineDataPtr = NULL;
    DWORD i;
    INT   newLineCodeSize = 0;

    for( i=0; i<maxLength; i++ )
    {
        if( ioWndBuffData.NewLineType == IOWND_BUFF_NEWLINE_CRLF )
        {
            if( (i>0) &&
                (*(dataPtr+i-1) == '\r') &&
                (*(dataPtr+i)   == '\n') )
            {
                i++;
                newLineCodeSize = 2;
                break;
            }
            else
            {
                nop();
            }
        }
        else if( ioWndBuffData.NewLineType == IOWND_BUFF_NEWLINE_LF )
        {
            if( (*(dataPtr+i) == '\n') )
            {
                i++;
                newLineCodeSize = 1;
                break;
            }
            else
            {
                nop();
            }
        }
        else if( ioWndBuffData.NewLineType == IOWND_BUFF_NEWLINE_CR )
        {
            if( (*(dataPtr+i) == '\r') )
            {
                i++;
                newLineCodeSize = 1;
                break;
            }
            else
            {
                nop();
            }
        }
        else
        { /* IOWND_BUFF_NEWLINE_NONE */
            nop();
        }
    }

    lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (i * sizeof(TCHAR)) );
    if( lineDataPtr != NULL )
    {
        lineDataPtr->dataSize = i;
        lineDataPtr->newLineCodeSize = newLineCodeSize;
        memcpy( lineDataPtr->data, dataPtr, i );
    }
    else
    {
        nop();
    }

    return lineDataPtr;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( void )
{
    DWORD dataSize = 0;
    S_BUFF_LINE_DATA *nowPtr;

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
    {
        dataSize += nowPtr->dataSize;
    }

    return dataSize;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő�s�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetLineMaxSize( void )
{
    DWORD lineMaxSize = 0;
    S_BUFF_LINE_DATA *nowPtr;

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
    {
        lineMaxSize++;
    }

    return lineMaxSize;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő啶���T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetColumnMaxSize( void )
{
    DWORD columnMaxSize = 0;
    S_BUFF_LINE_DATA *nowPtr;

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
    {
        columnMaxSize = max( columnMaxSize, (nowPtr->dataSize - nowPtr->newLineCodeSize) );
    }

    return columnMaxSize;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetCaretXpos( void )
{
    return ioWndBuffLineNowPtr->caretPos;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetCaretYpos( void )
{
    return ioWndBuffLineNowPtr->lineNum;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�g�ʒu�Z�b�g
 * ����  : DWORD xPos
 * ����  : DWORD lineNum
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndSetCaretPos( DWORD xPos, DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr = NULL;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        for( nowPtr = ioWndBuffListTopPtr; (nowPtr->nextPtr != NULL); nowPtr=nowPtr->nextPtr )
        { /* ��v���Ȃ������ꍇ�� nowPtr=�ŏI�s �ƂȂ�悤�� */
            if( nowPtr->lineNum == lineNum )
            {
                break;
            }
            else
            {
                nop();
            }
        }
    }

    if( nowPtr != NULL )
    {
        ioWndBuffLineNowPtr = nowPtr;

        if( xPos < (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
        {
            ioWndBuffLineNowPtr->caretPos = xPos;

            if( (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_LOW) )
            { /* 2byte�����̐^�񒆂������� */
                ioWndBuffLineNowPtr->caretPos += 1; /* ���ւ��炷 */
            }
            else
            {
                nop();
            }
        }
        else
        {
            ioWndBuffLineNowPtr->caretPos = (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize);
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndIncCaretXpos( void )
{
    int moveAmount = 0;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_HIGH )
        { /* ���̕����ňړ��ʂ𔻒f */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( (ioWndBuffLineNowPtr->caretPos + moveAmount) > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
        { /* �L�����b�g�͂��łɍs�̍ŉE�[ */
            if( (ioWndBuffLineNowPtr->nextPtr != NULL) )
            { /* ���s�L�� */
                ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->nextPtr;
                ioWndBuffLineNowPtr->caretPos = 0;
            }
            else
            { /* ���s���� */
                nop();
            }
        }
        else
        { /* �܂��E�ֈړ��\ */
            ioWndBuffLineNowPtr->caretPos += moveAmount;
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndDecCaretXpos( void )
{
    int moveAmount = 0;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( (ioWndBuffLineNowPtr->caretPos>0) &&
            (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos-1) == DOUBLE_CHAR_LOW) )
        { /* �O�̕����ňړ��ʂ𔻒f */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( ioWndBuffLineNowPtr->caretPos == 0 )
        { /* �L�����b�g�͂��łɍs�̍ō��[ */
            if( (ioWndBuffLineNowPtr->prevPtr != NULL) )
            { /* �O�s�L�� */
                ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->prevPtr;
                ioWndBuffLineNowPtr->caretPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
            }
            else
            { /* �O�s���� */
                nop();
            }
        }
        else
        { /* �܂����ֈړ��\ */
            ioWndBuffLineNowPtr->caretPos = max( ioWndBuffLineNowPtr->caretPos - moveAmount, 0 );
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndIncCaretYpos( void )
{
    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineNowPtr->nextPtr != NULL )
        { /* ���s�L�� */
            (ioWndBuffLineNowPtr->nextPtr)->caretPos = ioWndBuffLineNowPtr->caretPos;
            ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->nextPtr;

            if( ioWndBuffLineNowPtr->caretPos > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
            { /* ���͕��������� */
                ioWndBuffLineNowPtr->caretPos = (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize);
            }
            else
            { /* ���͕������L�� */
                if( (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_LOW) )
                { /* ����2byte�����̐^�񒆂������� */
                    ioWndBuffLineNowPtr->caretPos += 1; /* ���ւ��炷 */
                }
                else
                {
                    nop();
                }
            }
        }
        else
        { /* ���s���� */
            nop();
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndDecCaretYpos( void )
{
    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineNowPtr->prevPtr != NULL )
        { /* �O�s�L�� */
            (ioWndBuffLineNowPtr->prevPtr)->caretPos = ioWndBuffLineNowPtr->caretPos;
            ioWndBuffLineNowPtr = ioWndBuffLineNowPtr->prevPtr;

            if( ioWndBuffLineNowPtr->caretPos > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
            { /* ��͕��������� */
                ioWndBuffLineNowPtr->caretPos = (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize);
            }
            else
            { /* ��͕������L�� */
                if( (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_LOW) )
                { /* ����2byte�����̐^�񒆂������� */
                    ioWndBuffLineNowPtr->caretPos += 1; /* ���ւ��炷 */
                }
                else
                {
                    nop();
                }
            }
        }
        else
        { /* �O�s���� */
            nop();
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �w��s�̃f�[�^�A�h���X�擾
 * ����  : DWORD lineNum
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
S_BUFF_LINE_DATA *
IoWndBuffGetLinePtr( DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr,*rtnPtr = NULL;
    DWORD i;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        for( i=0,nowPtr = nextPtr = ioWndBuffListTopPtr; (nowPtr != NULL) && (i<=lineNum); nowPtr=nextPtr,i++ )
        {
            nextPtr = nowPtr->nextPtr;

            if( i == lineNum )
            {
                rtnPtr = nowPtr;
                break;
            }
            else
            {
                nop();
            }
        }
    }

    return rtnPtr;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̉��s�R�[�h�Z�b�g
 * ����  : UINT newLineType
 * �߂�l: BOOL (TRUE:�f�[�^���ύX���ꂽ)
 ***************************************/
BOOL
IoWndBuffSetNewLineCode( UINT newLineType )
{
    BOOL rtn = FALSE;
    DWORD allDataSize;
    TCHAR *dataTopPtr,*dataPtr;
    S_BUFF_LINE_DATA *nowPtr;

    ioWndBuffData.NewLineType = newLineType;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        allDataSize = IoWndGetBuffSize();
        dataTopPtr = malloc( sizeof(TCHAR) * allDataSize );
        if( dataTopPtr != NULL )
        {
            for( nowPtr=ioWndBuffListTopPtr,dataPtr=dataTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
            {
                memcpy( dataPtr, nowPtr->data, nowPtr->dataSize );
                dataPtr += nowPtr->dataSize;
            }
            clearLinkedList();
            IoWndBuffDataSet( dataTopPtr, allDataSize );
            free( dataTopPtr );
        }
        else
        {
            nop();
        }
        rtn = TRUE;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �f�[�^�ǉ�
 * ����  : TCHAR data
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffAddData( TCHAR data )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( ioWndBuffLineNowPtr != NULL )
    {
        newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((ioWndBuffLineNowPtr->dataSize+1) * sizeof(TCHAR)) );
        if( newPtr != NULL )
        {
            newPtr->dataSize = ioWndBuffLineNowPtr->dataSize + 1;
            newPtr->newLineCodeSize = ioWndBuffLineNowPtr->newLineCodeSize;
            newPtr->lineNum  = ioWndBuffLineNowPtr->lineNum;
            newPtr->caretPos = ioWndBuffLineNowPtr->caretPos+1;

            memcpy( newPtr->data, ioWndBuffLineNowPtr->data, ioWndBuffLineNowPtr->caretPos );
            *(newPtr->data+ioWndBuffLineNowPtr->caretPos) = data;
            memcpy( newPtr->data+ioWndBuffLineNowPtr->caretPos+1, ioWndBuffLineNowPtr->data+ioWndBuffLineNowPtr->caretPos, (ioWndBuffLineNowPtr->dataSize-ioWndBuffLineNowPtr->caretPos) );
            newPtr->prevPtr = ioWndBuffLineNowPtr->prevPtr;
            newPtr->nextPtr = ioWndBuffLineNowPtr->nextPtr;

            if( newPtr->prevPtr != NULL )
            {
                (newPtr->prevPtr)->nextPtr = newPtr;
            }
            else
            {
                ioWndBuffListTopPtr = newPtr;
            }
            if( newPtr->nextPtr != NULL )
            {
                (newPtr->nextPtr)->prevPtr = newPtr;
            }
            else
            {
                nop();
            }
            free( ioWndBuffLineNowPtr );
            ioWndBuffLineNowPtr = newPtr;
        }
        else
        {
            nop();
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �f�[�^�폜
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffRemoveData( void )
{
    int removeSize = 0;
    S_BUFF_LINE_DATA *newPtr = NULL,*prevPtr = NULL;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineNowPtr->caretPos > 0  )
        {
            if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos-1) == DOUBLE_CHAR_LOW )
            { /* ���̕����ňړ��ʂ𔻒f */
                removeSize = 2;
            }
            else
            {
                removeSize = 1;
            }

            newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((ioWndBuffLineNowPtr->dataSize-removeSize) * sizeof(TCHAR)) );
            if( newPtr != NULL )
            {
                newPtr->dataSize        = ioWndBuffLineNowPtr->dataSize - removeSize;
                newPtr->newLineCodeSize = ioWndBuffLineNowPtr->newLineCodeSize;
                newPtr->lineNum         = ioWndBuffLineNowPtr->lineNum;
                newPtr->caretPos        = ioWndBuffLineNowPtr->caretPos - removeSize;

                memcpy( newPtr->data, ioWndBuffLineNowPtr->data,ioWndBuffLineNowPtr->caretPos-removeSize );
                memcpy( newPtr->data+ioWndBuffLineNowPtr->caretPos-removeSize, ioWndBuffLineNowPtr->data+ioWndBuffLineNowPtr->caretPos, (ioWndBuffLineNowPtr->dataSize-ioWndBuffLineNowPtr->caretPos) );
                newPtr->prevPtr = ioWndBuffLineNowPtr->prevPtr;
                newPtr->nextPtr = ioWndBuffLineNowPtr->nextPtr;
                if( newPtr->prevPtr != NULL )
                {
                    (newPtr->prevPtr)->nextPtr = newPtr;
                }
                else
                {
                    ioWndBuffListTopPtr = newPtr;
                }
                if( newPtr->nextPtr != NULL )
                {
                    (newPtr->nextPtr)->prevPtr = newPtr;
                }
                else
                {
                    nop();
                }

                free( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = newPtr;
            }
            else
            {
                nop();
            }
        }
        else
        {
            if( ioWndBuffLineNowPtr->prevPtr != NULL )
            {
                prevPtr = ioWndBuffLineNowPtr->prevPtr;

                removeSize = 0;

                newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((prevPtr->dataSize-prevPtr->newLineCodeSize-removeSize+ioWndBuffLineNowPtr->dataSize) * sizeof(TCHAR)) );
                if( newPtr != NULL )
                {
                    newPtr->dataSize        = prevPtr->dataSize - prevPtr->newLineCodeSize - removeSize + ioWndBuffLineNowPtr->dataSize;
                    newPtr->newLineCodeSize = ioWndBuffLineNowPtr->newLineCodeSize;
                    newPtr->lineNum         = prevPtr->lineNum;
                    newPtr->caretPos        = prevPtr->dataSize - prevPtr->newLineCodeSize;

                    memcpy( newPtr->data, prevPtr->data, prevPtr->dataSize - prevPtr->newLineCodeSize - removeSize );
                    memcpy( newPtr->data + prevPtr->dataSize - prevPtr->newLineCodeSize - removeSize, ioWndBuffLineNowPtr->data, ioWndBuffLineNowPtr->dataSize );

                    newPtr->prevPtr = prevPtr->prevPtr;
                    newPtr->nextPtr = ioWndBuffLineNowPtr->nextPtr;
                    if( newPtr->prevPtr != NULL )
                    {
                        (newPtr->prevPtr)->nextPtr = newPtr;
                    }
                    else
                    {
                        ioWndBuffListTopPtr = newPtr;
                    }
                    if( newPtr->nextPtr != NULL )
                    {
                        (newPtr->nextPtr)->prevPtr = newPtr;
                    }
                    else
                    {
                        nop();
                    }

                    free( ioWndBuffLineNowPtr );
                    free( prevPtr );
                    ioWndBuffLineNowPtr = newPtr;

                    updateLineNum( ioWndBuffLineNowPtr );
                }
                else
                {
                    nop();
                }
            }
            else
            {
                nop();
            }
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : ���s�ǉ�
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffAddNewLine( void )
{
    S_BUFF_LINE_DATA *newPtr = NULL,*newNextPtr = NULL;

    if( ioWndBuffLineNowPtr != NULL )
    {
        newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (ioWndBuffLineNowPtr->caretPos+getNewLineSize()) * sizeof(TCHAR));
        if( newPtr != NULL )
        {
            newNextPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((ioWndBuffLineNowPtr->dataSize-ioWndBuffLineNowPtr->caretPos) * sizeof(TCHAR)) );
            if( newNextPtr != NULL )
            {
                /* ���s���O */
                newPtr->dataSize = ioWndBuffLineNowPtr->caretPos+getNewLineSize();
                newPtr->newLineCodeSize = getNewLineSize();
                newPtr->lineNum = ioWndBuffLineNowPtr->lineNum;

                memcpy( newPtr->data, ioWndBuffLineNowPtr->data, newPtr->dataSize );
                switch( ioWndBuffData.NewLineType )
                {
                case IOWND_BUFF_NEWLINE_CRLF:
                    *(newPtr->data + newPtr->dataSize-2) = '\r';
                    *(newPtr->data + newPtr->dataSize-1) = '\n';
                    break;
                case IOWND_BUFF_NEWLINE_LF  :
                    *(newPtr->data + newPtr->dataSize-1) = '\n';
                    break;
                case IOWND_BUFF_NEWLINE_CR  :
                    *(newPtr->data + newPtr->dataSize-1) = '\r';
                    break;
                case IOWND_BUFF_NEWLINE_NONE:
                default:
                    break;
                }

                newPtr->prevPtr = ioWndBuffLineNowPtr->prevPtr;
                if( newPtr->prevPtr != NULL )
                {
                    (newPtr->prevPtr)->nextPtr = newPtr;
                }
                else
                {
                    ioWndBuffListTopPtr = newPtr;
                }

                /* ���s�ȍ~ */
                newNextPtr->dataSize = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->caretPos;
                newNextPtr->newLineCodeSize = ioWndBuffLineNowPtr->newLineCodeSize;
                newNextPtr->lineNum = newPtr->lineNum+1;
                memcpy( newNextPtr->data, ioWndBuffLineNowPtr->data+ioWndBuffLineNowPtr->caretPos, newNextPtr->dataSize );

                newNextPtr->prevPtr = newPtr;
                newPtr->nextPtr = newNextPtr;

                newNextPtr->nextPtr = ioWndBuffLineNowPtr->nextPtr;
                if( newNextPtr->nextPtr != NULL )
                {
                    (newNextPtr->nextPtr)->prevPtr = newNextPtr;
                }
                else
                {
                    nop();
                }

                free( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = newNextPtr;
                ioWndBuffLineNowPtr->caretPos = 0;

                updateLineNum( ioWndBuffLineNowPtr );
            }
            else
            {
                free( newPtr );
            }
        }
        else
        {
            nop();
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �L�����N�^�Z�b�g�̔��f(Shift_JIS�̏ꍇ)
 * ����  : S_BUFF_LINE_DATA *dataPtr
 * ����  : DWORD            offset   ���f�����������̐擪����̃I�t�Z�b�g(0 origin)
 * �߂�l: CHARSET_TYPE
 ***************************************/
CHARSET_TYPE
detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset )
{
    DWORD i;
    int charType = SINGLE_CHAR;

    if( dataPtr != NULL )
    {
        for( i=0; i<(dataPtr->dataSize-dataPtr->newLineCodeSize); i++ )
        {
            if( charType == DOUBLE_CHAR_HIGH )
            {
                charType = DOUBLE_CHAR_LOW;
            }
            else
            {
                if( ( (BYTE)(*(dataPtr->data+i)) <= (BYTE)0x80) || (((BYTE)0xA0 <= (BYTE)(*(dataPtr->data+i))) && ((BYTE)(*(dataPtr->data+i)) <= (BYTE)0xDF)) )
                {
                    charType = SINGLE_CHAR;
                }
                else
                {
                    charType = DOUBLE_CHAR_HIGH;
                }
            }

            if( i==offset )
            {
                break;
            }
            else
            {
                nop();
            }
        }

        if( i==(dataPtr->dataSize-dataPtr->newLineCodeSize) )
        {
            charType = SINGLE_CHAR;
        }
        else
        {
            nop();
        }
    }
    else
    {
        nop();
    }

    return charType;
}

/********************************************************************************
 * ���e  : ���s�R�[�h�T�C�Y���擾
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
static INT
getNewLineSize( void )
{
    INT rtn = 0;

    switch( ioWndBuffData.NewLineType )
    {
    case IOWND_BUFF_NEWLINE_CRLF:
        rtn = 2;
        break;
    case IOWND_BUFF_NEWLINE_LF  :
    case IOWND_BUFF_NEWLINE_CR  :
        rtn = 1;
        break;
    case IOWND_BUFF_NEWLINE_NONE:
    default:
        break;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �s�ԍ��̍X�V
 * ����  : S_BUFF_LINE_DATA *dataPtr
 * �߂�l: �Ȃ�
 ***************************************/
static void
updateLineNum( S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr;
    DWORD i;

    if( dataPtr != NULL )
    {
        i = dataPtr->lineNum;

        for( nowPtr=dataPtr; (nowPtr != NULL); nowPtr=nowPtr->nextPtr,i++ )
        {
            nowPtr->lineNum = i;
        }
    }
    else
    {
        nop();
    }
}
