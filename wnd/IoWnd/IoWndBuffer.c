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

/* �����ϐ���` */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;

typedef struct
{
    UINT  NewLineType;
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

    IoWndBuffInit();

    if( (dataPtr != NULL) && (length > 0) )
    {
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                addLinkedList( lineDataPtr );
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
        }

        if( (lineDataPtr != NULL) &&
            (lineDataPtr->newLineCodeSize) )
        { /* �Ō�̍s�ɉ��s�R�[�h���������ꍇ�A�f�[�^0�A���s�R�[�h0�̃f�[�^��ǉ� */
            lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
            if( lineDataPtr != NULL )
            {
                lineDataPtr->dataSize = 0;
                lineDataPtr->newLineCodeSize = 0;
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
    {
        nop(); /* error */
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
