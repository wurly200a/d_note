/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

typedef struct tag_iownd_buffer_data
{
    struct tag_iownd_buffer_data *prevPtr;
    struct tag_iownd_buffer_data *nextPtr;
    DWORD                         dataSize;
    TCHAR                         data[];
} S_IOWND_BUFF_DATA;

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWndBuffer.h"
static void ioWndBuffAddLinkedList( S_IOWND_BUFF_DATA *dataPtr );

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
        ioWndBuffDataSize = 0;
        ioWndBuffLineMax = 0;
        ioWndBuffColumnMax = 0;
    }
    else
    {
        nop();
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
    DWORD i,columnCnt;
    S_IOWND_BUFF_DATA *dataPtr;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffDataPtr = (TCHAR *) malloc( (length * sizeof(TCHAR)) );

        if( ioWndBuffDataPtr != NULL )
        { /* �������l���ł��� */
            ioWndBuffDataSize = length;
            for( i=0,columnCnt=0; i<length; i++ )
            {
                *(ioWndBuffDataPtr+i) = *(strPtr+i);
                if( *(strPtr+i) == '\n' )
                {
                    dataPtr = (S_IOWND_BUFF_DATA *)malloc( sizeof(S_IOWND_BUFF_DATA) + (columnCnt * sizeof(TCHAR)) );
                    if( dataPtr != NULL )
                    {
                        dataPtr->dataSize = columnCnt;
                        memcpy( dataPtr->data, (strPtr+i-columnCnt), columnCnt );
                        ioWndBuffAddLinkedList( dataPtr );
                    }
                    else
                    {
                        nop(); /* error */
                    }

                    (ioWndBuffLineMax)++; /* ���s�̐����J�E���g */
                    ioWndBuffColumnMax = max( ioWndBuffColumnMax, columnCnt );
                    columnCnt=0;
                }
                else
                {
                    columnCnt++;
                }
            }

            if( ioWndBuffLineMax==0 )
            { /* ���s�����������ꍇ */
                ioWndBuffLineMax  = 1;
                ioWndBuffColumnMax = ioWndBuffDataSize;

                dataPtr = (S_IOWND_BUFF_DATA *)malloc( sizeof(S_IOWND_BUFF_DATA) + (ioWndBuffDataSize * sizeof(TCHAR)) );
                if( dataPtr != NULL )
                {
                    ioWndBuffAddLinkedList( dataPtr );
                }
                else
                {
                    nop(); /* error */
                }
            }
            else
            {
                nop();
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
