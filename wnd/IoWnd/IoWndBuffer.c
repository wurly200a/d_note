/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWndBuffer.h"

/* �����ϐ���` */
typedef struct tag_iownd_buffer_data
{
    struct tag_iownd_buffer_data *prevPtr;
    struct tag_iownd_buffer_data *nextPtr;
    DWORD                         dataSize;
    TCHAR                        *dataPtr;
} S_IOWND_BUFF_DATA;

S_IOWND_BUFF_DATA ioWndBuffQueTop;
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
    if( ioWndBuffQueTop.dataPtr != NULL )
    {
        free( ioWndBuffQueTop.dataPtr );
        ioWndBuffQueTop.dataPtr  = NULL;
        ioWndBuffQueTop.dataSize = 0;
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
    DWORD i,j;

    IoWndBuffInit();

    if( (strPtr != NULL) && (length > 0) )
    {
        ioWndBuffQueTop.dataPtr = (TCHAR *) malloc( (length * sizeof(TCHAR)) );

        if( ioWndBuffQueTop.dataPtr != NULL )
        {
            ioWndBuffQueTop.dataSize = length;

            for( i=0,j=0; i<length; i++ )
            {
                *(ioWndBuffQueTop.dataPtr+i) = *(strPtr+i);
                if( *(strPtr+i) == '\n' )
                {
                    (ioWndBuffLineMax)++; /* ���s�̐����J�E���g */
                    ioWndBuffColumnMax = max( ioWndBuffColumnMax, j );
                    j=0;
                }
                else
                {
                    j++;
                }
            }

            if( ioWndBuffLineMax==0 )
            { /* ���s�����������ꍇ */
                ioWndBuffLineMax  = 1;
                ioWndBuffColumnMax = ioWndBuffQueTop.dataSize;
            }
        }
        else
        {
            nop(); /* error */
        }
    }
    else
    {
        nop(); /* error */
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
    return ioWndBuffQueTop.dataPtr;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( void )
{
    return ioWndBuffQueTop.dataSize;
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
