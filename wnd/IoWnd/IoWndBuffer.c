/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWndBuffer.h"
static DWORD getLineLength( TCHAR *dataPtr, DWORD maxLength );
static void ioWndBuffAddLinkedList( S_IOWND_BUFF_DATA *dataPtr );
static void ioWndBuffClearLinkedList( void );

/* �����ϐ���` */
S_IOWND_BUFF_DATA *ioWndBuffListTopPtr;

TCHAR *ioWndBuffDataPtr;
DWORD ioWndBuffDataSize;
DWORD ioWndBuffLineMax;
DWORD ioWndBuffColumnMax;

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̏�����
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffInit( void )
{
    if( ioWndBuffDataPtr != NULL )
    {
        free( ioWndBuffDataPtr );
        ioWndBuffDataPtr = NULL;
    }
    else
    {
        nop();
    }
    ioWndBuffClearLinkedList();

    ioWndBuffDataSize = 0;
    ioWndBuffLineMax = 0;
    ioWndBuffColumnMax = 0;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@(�A�����X�g)�ւ̃f�[�^�ǉ�
 * ����  : TCHAR* strPtr
 * ����  : DWORD  length
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSetLinkedList( TCHAR* strPtr, DWORD length )
{
    S_IOWND_BUFF_DATA *dataPtr;
    DWORD lineLength,lineLengthSum = 0;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffDataSize = length;

        while( lineLengthSum < length )
        {
            lineLength = getLineLength( strPtr+lineLengthSum, length - lineLengthSum );
            dataPtr = (S_IOWND_BUFF_DATA *)malloc( sizeof(S_IOWND_BUFF_DATA) + (lineLength * sizeof(TCHAR)) );

            if( dataPtr != NULL )
            {
                dataPtr->dataSize = lineLength;
                memcpy( dataPtr->data, (strPtr+lineLengthSum), lineLength );
                ioWndBuffAddLinkedList( dataPtr );
            }
            else
            {
                nop(); /* error */
            }

            ioWndBuffColumnMax = max( ioWndBuffColumnMax, lineLength );
            (ioWndBuffLineMax)++; /* ���s�̐����J�E���g */
            lineLengthSum += lineLength;
        }
    }
    else
    {
        nop(); /* error */
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�ւ̃f�[�^�ǉ�
 * ����  : TCHAR* strPtr
 * ����  : DWORD  length
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSet( TCHAR* strPtr, DWORD length )
{
    DWORD i,ColumnCnt;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffDataPtr = (TCHAR *) malloc( (length * sizeof(TCHAR)) );

        if( ioWndBuffDataPtr != NULL )
        { /* �������l���ł��� */
            ioWndBuffDataSize = length;

            for( i=0,ColumnCnt=0; i<length; i++ )
            {
                *(ioWndBuffDataPtr+i) = *(strPtr+i);
                if( *(strPtr+i) == '\n' )
                {
                    (ioWndBuffLineMax)++; /* ���s�̐����J�E���g */
                    ioWndBuffColumnMax = max( ioWndBuffColumnMax, ColumnCnt );
                    ColumnCnt=0;
                }
                else
                {
                    ColumnCnt++;
                }
            }

            if( ioWndBuffLineMax==0 )
            { /* ���s�����������ꍇ */
                ioWndBuffLineMax  = 1;
                ioWndBuffColumnMax = ioWndBuffDataSize;
            }
        }
        else
        { /* �������l���ł��Ȃ����� */
            nop(); /* error */
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
ioWndBuffClearLinkedList( void )
{
    S_IOWND_BUFF_DATA *nowPtr,*nextPtr;

    if( ioWndBuffListTopPtr == NULL )
    { /* 1���Ȃ���ĂȂ���� */
        nop();
    }
    else
    {
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
}

/********************************************************************************
 * ���e  : �A�����X�g�Ƀf�[�^��ǉ�����
 * ����  : S_IOWND_BUFF_DATA *dataPtr
 * �߂�l: �Ȃ�
 ***************************************/
static void
ioWndBuffAddLinkedList( S_IOWND_BUFF_DATA *dataPtr )
{
    S_IOWND_BUFF_DATA *nowPtr,*nextPtr;

    if( ioWndBuffListTopPtr == NULL )
    {
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
 * ���e  : ���s�܂ł̒������擾
 * ����  : TCHAR *dataPtr  �f�[�^�̐擪
 * ����  : DWORD maxLength �ő咷��
 * �߂�l: DWORD
 ***************************************/
static DWORD
getLineLength( TCHAR *dataPtr, DWORD maxLength )
{
    DWORD i;

    for( i=0; i<maxLength; i++ )
    {
        if( (i>0) &&
            (*(dataPtr+i-1) == '\r') &&
            (*(dataPtr+i)   == '\n') )
        {
            break;
        }
        else
        {
            nop();
        }
    }

    if( i>0 )
    {
        i++;
    }
    else
    {
        nop();
    }

    return i;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�|�C���^�擾
 * ����  : �Ȃ�
 * �߂�l: TCHAR *
 ***************************************/
TCHAR *
IoWndGetBuffPtr( void )
{
    return ioWndBuffDataPtr;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( void )
{
    return ioWndBuffDataSize;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő�s�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetLineMaxSize( void )
{
    return ioWndBuffLineMax;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő啶���T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetColumnMaxSize( void )
{
    return ioWndBuffColumnMax;
}

/********************************************************************************
 * ���e  : �w��s�̃f�[�^�A�h���X�擾
 * ����  : DWORD lineNum
 * �߂�l: S_IOWND_BUFF_DATA *
 ***************************************/
S_IOWND_BUFF_DATA *
IoWndBuffGetLinePtr( DWORD lineNum )
{
    S_IOWND_BUFF_DATA *nowPtr,*nextPtr,*rtnPtr = NULL;
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
