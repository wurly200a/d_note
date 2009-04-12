/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWndBuffer.h"
static void clearLinkedList ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr );
static void addLinkedList   ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void insertLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *replaceLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *removeLinkedList ( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void mergeLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
static void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr );

enum
{
    SINGLE_CHAR,
    DOUBLE_CHAR_HIGH,
    DOUBLE_CHAR_LOW,
    TAB_CHAR,
};
typedef int CHARSET_TYPE;
static CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );
static int getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, TCHAR *dataPtr, int *offsetPtr );
static INT getNewLineSize( void );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

/* �����ϐ���` */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;
S_BUFF_LINE_DATA *ioWndBuffListEndPtr;
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
    S_BUFF_LINE_DATA *lineDataPtr;

    clearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);

    /* �f�[�^0�A���s�R�[�h0�̃f�[�^��ǉ� */
    lineDataPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) );
    if( lineDataPtr != NULL )
    {
        lineDataPtr->dataSize = 0;
        lineDataPtr->newLineCodeSize = 0;
        lineDataPtr->lineNum = 0;
        addLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,lineDataPtr );
    }
    else
    {
        nop();
    }

    /* �J�[�\���ʒu��擪�ɂ��� */
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
 * ���e  : IO�E�B���h�E�o�b�t�@�̏I��
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffEnd( void )
{
    clearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�Z�b�g
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    S_BUFF_LINE_DATA *lineDataPtr;
    S_BUFF_LINE_DATA *tempTopPtr = NULL;
    S_BUFF_LINE_DATA *tempEndPtr = NULL;
    S_BUFF_LINE_DATA *newPtr,*targetPtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    DWORD lineLengthSum = 0;
    DWORD addLineNum = 0;
    DWORD caretPos = 0;

    if( bInit )
    {
        IoWndBuffInit();
    }
    else
    {
        nop();
    }

    if( (dataPtr != NULL) && (length > 0) )
    { /* �f�[�^�L��̏ꍇ */
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                addLinkedList( &tempTopPtr,&tempEndPtr,lineDataPtr );
                addLineNum++;
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
        }
        /* �����܂ŁA�f�[�^�����A�����X�g(tempTopPtr�`tempEndPtr)�ɓo�^ */

        if( ioWndBuffLineNowPtr->caretPos == 0 )
        { /* �s�̐擪�ɑ}�� */
            mergeLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

            if( tempEndPtr->newLineCodeSize == 0 )
            { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                newPtr = joinData(tempEndPtr,ioWndBuffLineNowPtr);
                if( newPtr != NULL )
                {
                    targetPtr = ioWndBuffLineNowPtr;
                    ioWndBuffLineNowPtr = replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
                    free( targetPtr );
                    targetPtr = tempEndPtr;
                    caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                    removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
                    free( tempEndPtr );

                    ioWndBuffLineNowPtr = newPtr;
                    ioWndBuffLineNowPtr->caretPos = caretPos;
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
        { /* �s�̓r���ɑ}�� */
            /* ���݂̍s���L�����b�g�ʒu�ŕ���(��������) */
            divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr );
            if( dividePrePtr != NULL )
            {
                if( dividePostPtr != NULL )
                {
                    replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,dividePostPtr ); /* �܂� �Â��f�[�^�����s�ȍ~�̃f�[�^�ɒu������ */
                    insertLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,dividePrePtr ); /* ���̑O�ɉ��s�O�̃f�[�^��}�� */
                    free( ioWndBuffLineNowPtr );
                    ioWndBuffLineNowPtr = dividePostPtr;
                    ioWndBuffLineNowPtr->caretPos = 0;
                    /* ���݂̍s���L�����b�g�ʒu�ŕ���(�����܂�) */

                    /* ���������ʒu�Ƀf�[�^��}��(��������) */
                    mergeLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

                    if( tempEndPtr->newLineCodeSize == 0 )
                    { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                        newPtr = joinData(tempEndPtr,dividePostPtr);
                        if( newPtr != NULL )
                        {
                            newPtr->caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                            free( dividePostPtr );
                            ioWndBuffLineNowPtr = newPtr;
                            removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
                            free( tempEndPtr );

                            tempEndPtr = newPtr;
                            if( addLineNum == 1 )
                            {
                                tempTopPtr = tempEndPtr;
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
                        tempEndPtr->caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                    }

                    /* ���������O���f�[�^�ƁA�}���f�[�^�̐擪������(��������) */
                    newPtr = joinData( dividePrePtr, tempTopPtr );
                    if( newPtr != NULL )
                    {
                        newPtr->caretPos = tempEndPtr->caretPos + (dividePrePtr->dataSize - dividePrePtr->newLineCodeSize);
                        targetPtr = tempTopPtr;
                        replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                        free( targetPtr );
                        removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
                        free( dividePrePtr );

                        tempTopPtr = newPtr;
                        if( addLineNum == 1 )
                        {
                            tempEndPtr = tempTopPtr;
                        }
                        else
                        {
                            nop();
                        }
                        /* ���������O���f�[�^�ƁA�}���f�[�^�̐擪������(�����܂�) */
                        ioWndBuffLineNowPtr = tempEndPtr;
                    }
                    else
                    {
                        nop();
                    }
                }
                else
                {
                    free( dividePrePtr );
                }
            }
            else
            {
                nop();
            }
        }
    }
    else
    { /* �f�[�^�����̏ꍇ */
        nop();
    }

    if( bInit )
    {
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
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�擾
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffDataGet( TCHAR *dataPtr, DWORD dataSize )
{
    BOOL rtn = FALSE;
    S_BUFF_LINE_DATA *nowPtr;

    if( ioWndBuffListTopPtr == NULL )
    {
        nop();
    }
    else
    {
        if( dataPtr != NULL )
        {
            for( nowPtr=ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
            {
                memcpy( dataPtr, nowPtr->data, nowPtr->dataSize );
                dataPtr += nowPtr->dataSize;
            }
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
 * ���e  : �A�����X�g�̃f�[�^���N���A
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_BUFF_LINE_DATA **endPtr �ŏI�f�[�^���Ȃ��ł���|�C���^
 * �߂�l: �Ȃ�
 ***************************************/
static void
clearLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;

    for( nowPtr = nextPtr = *topPtr; nowPtr != NULL; nowPtr = nextPtr )
    {
        nextPtr = nowPtr->nextPtr;
        free( nowPtr );

        if( nextPtr == NULL )
        { /* ���ɂȂ���Ă���f�[�^���� */
            *topPtr = NULL;
            *endPtr = NULL;
        }
        else
        { /* ���ɂȂ���Ă���f�[�^�L�� */
            nop();
        }
    }
}

/********************************************************************************
 * ���e  : �A�����X�g�Ƀf�[�^��ǉ�����
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *dataPtr �Ȃ��f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
static void
addLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( *topPtr == NULL )
    {
        dataPtr->prevPtr = NULL;
        dataPtr->nextPtr = NULL;
        dataPtr->lineNum = 0;
        *topPtr = dataPtr;
        *endPtr = dataPtr;
    }
    else
    {
        if( *endPtr != NULL )
        {
            dataPtr->prevPtr   = *endPtr;
            dataPtr->nextPtr   = NULL;
            dataPtr->lineNum   = (*endPtr)->lineNum+1;
            (*endPtr)->nextPtr = dataPtr;
            *endPtr            = dataPtr;
        }
        else
        {
            nop(); /* �ُ� */
        }
    }
}

/********************************************************************************
 * ���e  : �A�����X�g�Ƀf�[�^��}������
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *nowPtr  �}���ʒu
 * ����  : S_BUFF_LINE_DATA *dataPtr �}������f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
static void
insertLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->prevPtr = nowPtr->prevPtr;
        dataPtr->nextPtr = nowPtr;
        dataPtr->lineNum = nowPtr->lineNum;

        if( dataPtr->prevPtr != NULL )
        { /* �O�f�[�^�L�� */
            (dataPtr->prevPtr)->nextPtr = dataPtr;
        }
        else
        { /* �O�f�[�^���� */
            (*topPtr) = dataPtr;
        }

        nowPtr->prevPtr = dataPtr;
        (nowPtr->lineNum)++;

        updateLineNum( nowPtr );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �A�����X�g�̃f�[�^��u��������
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *nowPtr  �u�������Ώۂ̃f�[�^
 * ����  : S_BUFF_LINE_DATA *dataPtr �u��������f�[�^
 * �߂�l: �u���������f�[�^
 ***************************************/
static S_BUFF_LINE_DATA *
replaceLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->lineNum = nowPtr->lineNum;

        dataPtr->prevPtr = nowPtr->prevPtr;
        dataPtr->nextPtr = nowPtr->nextPtr;

        if( dataPtr->prevPtr != NULL )
        {
            (dataPtr->prevPtr)->nextPtr = dataPtr;
        }
        else
        {
            *(topPtr) = dataPtr;
        }
        if( dataPtr->nextPtr != NULL )
        {
            (dataPtr->nextPtr)->prevPtr = dataPtr;
        }
        else
        {
            *(endPtr) = dataPtr;
        }
    }
    else
    {
        nop();
    }

    return dataPtr;
}

/********************************************************************************
 * ���e  : �A�����X�g����f�[�^���폜����
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_BUFF_LINE_DATA *dataPtr �폜����f�[�^
 * �߂�l: S_BUFF_LINE_DATA *        �폜�������̃f�[�^
 ***************************************/
static S_BUFF_LINE_DATA *
removeLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nextPtr = NULL;

    if( (topPtr != NULL) && (endPtr != NULL) && (dataPtr != NULL) )
    {
        nextPtr = dataPtr->nextPtr; /* �߂�l */

        if( dataPtr->prevPtr != NULL )
        { /* �O�f�[�^�L�� */
            (dataPtr->prevPtr)->nextPtr = dataPtr->nextPtr; /* ���f�[�^���A�O�f�[�^�̎��f�[�^�Ƃ��� */
            if( (dataPtr->prevPtr)->nextPtr != NULL )
            { /* ���f�[�^���������ꍇ */
                ((dataPtr->prevPtr)->nextPtr)->lineNum = (dataPtr->prevPtr)->lineNum+1;
                ((dataPtr->prevPtr)->nextPtr)->prevPtr = dataPtr->prevPtr; /* ���f�[�^�̑O�f�[�^�́A�O�f�[�^ */
                updateLineNum( (dataPtr->prevPtr)->nextPtr );
            }
            else
            { /* ���f�[�^���� */
                (*endPtr) = dataPtr->prevPtr; /* �O�f�[�^���Ō�̃f�[�^�ƂȂ� */
            }
        }
        else
        { /* �O�f�[�^���� */
            *topPtr = dataPtr->nextPtr;
            if( *topPtr != NULL )
            { /* ���f�[�^���������ꍇ */
                (*topPtr)->lineNum = 0;
                (*topPtr)->prevPtr = NULL;
                updateLineNum( (*topPtr) );
            }
            else
            { /* ���f�[�^���� */
                (*endPtr) = NULL; /* �f�[�^�͑S�������Ȃ��� */
            }
        }
    }
    else
    {
        nop();
    }

    return nextPtr;
}

/********************************************************************************
 * ���e  : �A�����X�g�̌���
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *nowPtr  �}���ʒu
 * ����  : S_BUFF_LINE_DATA **mergeTopPtr
 * ����  : S_BUFF_LINE_DATA **mergeEndPtr
 * �߂�l: void
 ***************************************/
static void
mergeLinkedList( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (mergeTopPtr != NULL) && (mergeEndPtr != NULL) )
    {
        (*mergeTopPtr)->prevPtr = nowPtr->prevPtr;

        if( (*mergeTopPtr)->prevPtr != NULL )
        {
            (*mergeTopPtr)->lineNum = ((*mergeTopPtr)->prevPtr)->lineNum+1;
            ((*mergeTopPtr)->prevPtr)->nextPtr = (*mergeTopPtr);
        }
        else
        {
            (*mergeTopPtr)->lineNum = 0;
            (*topPtr) = (*mergeTopPtr);
        }

        (*mergeEndPtr)->nextPtr = nowPtr;
        if( (*mergeEndPtr)->nextPtr != NULL )
        {
            ((*mergeEndPtr)->nextPtr)->prevPtr = (*mergeEndPtr);
        }
        else
        {
            nop(); /* �L�蓾�Ȃ� */
        }

        updateLineNum( (*mergeTopPtr) );
    }
    else
    {
        nop();
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
 * ���e  : �s�f�[�^�̌���
 * ����  : S_BUFF_LINE_DATA *data1Ptr (���s�R�[�h�͍폜�����)
 * ����  : S_BUFF_LINE_DATA *data2Ptr
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( (data1Ptr != NULL) && (data2Ptr != NULL) )
    {
        newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((data1Ptr->dataSize-data1Ptr->newLineCodeSize+data2Ptr->dataSize) * sizeof(TCHAR)) );
        if( newPtr != NULL )
        {
            newPtr->dataSize = data1Ptr->dataSize - data1Ptr->newLineCodeSize + data2Ptr->dataSize;
            newPtr->newLineCodeSize = data2Ptr->newLineCodeSize;

            memcpy( newPtr->data, data1Ptr->data, data1Ptr->dataSize - data1Ptr->newLineCodeSize );
            memcpy( newPtr->data + data1Ptr->dataSize - data1Ptr->newLineCodeSize, data2Ptr->data, data2Ptr->dataSize );

            newPtr->prevPtr = NULL;
            newPtr->nextPtr = NULL;
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

    return newPtr;
}

/********************************************************************************
 * ���e  : �s�f�[�^�̕���
 * ����  : S_BUFF_LINE_DATA *dataPtr  ��������f�[�^
 * ����  : S_BUFF_LINE_DATA **new1Ptr ������̃f�[�^1 (�������m�ۂ���)
 * ����  : S_BUFF_LINE_DATA **new2Ptr ������̃f�[�^2 (�������m�ۂ���)
 * �߂�l: �Ȃ�
 ***************************************/
static void
divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( (dataPtr != NULL) && (new1Ptr != NULL) && (new2Ptr != NULL) )
    {
        *new1Ptr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (dataPtr->caretPos+getNewLineSize()) * sizeof(TCHAR));

        if( *new1Ptr != NULL )
        {
            *new2Ptr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((dataPtr->dataSize-dataPtr->caretPos) * sizeof(TCHAR)) );
            if( *new2Ptr != NULL )
            {
                /* ���s���O�̃f�[�^���� */
                (*new1Ptr)->dataSize = dataPtr->caretPos+getNewLineSize();
                (*new1Ptr)->newLineCodeSize = getNewLineSize();
                (*new1Ptr)->lineNum = dataPtr->lineNum;

                memcpy( (*new1Ptr)->data, dataPtr->data, (*new1Ptr)->dataSize );
                switch( ioWndBuffData.NewLineType )
                {
                case IOWND_BUFF_NEWLINE_CRLF:
                    *((*new1Ptr)->data + (*new1Ptr)->dataSize-2) = '\r';
                    *((*new1Ptr)->data + (*new1Ptr)->dataSize-1) = '\n';
                    break;
                case IOWND_BUFF_NEWLINE_LF  :
                    *((*new1Ptr)->data + (*new1Ptr)->dataSize-1) = '\n';
                    break;
                case IOWND_BUFF_NEWLINE_CR  :
                    *((*new1Ptr)->data + (*new1Ptr)->dataSize-1) = '\r';
                    break;
                case IOWND_BUFF_NEWLINE_NONE:
                default:
                    break;
                }

                /* ���s�ȍ~�̃f�[�^���� */
                (*new2Ptr)->dataSize = dataPtr->dataSize - dataPtr->caretPos;
                (*new2Ptr)->newLineCodeSize = dataPtr->newLineCodeSize;
                (*new2Ptr)->lineNum = (*new1Ptr)->lineNum+1;
                memcpy( (*new2Ptr)->data, dataPtr->data+dataPtr->caretPos, (*new2Ptr)->dataSize );
            }
            else
            {
                free( *new1Ptr );
                *new1Ptr = NULL;
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
    return ioWndBuffListEndPtr->lineNum;
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
 * ���e  : �w��s�A�w���̃f�[�^���擾
 * ����  : DWORD  lineNum   �s
 * ����  : DWORD  dispPos   �\���ʒu
 * ����  : TCHAR *dataPtr   �f�[�^�i�[�̈�
 * ����  : INT   *offsetPtr �f�[�^�ʒu�i�[�̈�
 * �߂�l: INT              �i�[�����f�[�^�̃T�C�Y
 ***************************************/
INT
IoWndBuffGetDispData( DWORD lineNum, DWORD dispPos, TCHAR *dataPtr, INT *offsetPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;
    DWORD i;
    INT iSize = 0;

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
                iSize = getDispCharData( nowPtr, dispPos, dataPtr, offsetPtr );
                break;
            }
            else
            {
                nop();
            }
        }
    }

    return iSize;
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
            IoWndBuffDataSet( dataTopPtr, allDataSize, TRUE );
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
        /* 1�����ǉ������V�����s�f�[�^�𐶐� */
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

            /* �Â��s�f�[�^�ƒu�������� */
            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
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
            /* 1�����폜�����V�����s�f�[�^�𐶐� */
            if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos-1) == DOUBLE_CHAR_LOW )
            { /* ���̕����ō폜�ʂ𔻒f */
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

                /* �Â��s�f�[�^�ƒu�������� */
                replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
                free( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = newPtr;
            }
            else
            {
                nop();
            }
        }
        else
        { /* �L�����b�g���s�̐擪�ʒu�B�܂�A�O�s�Ƃ̌����B*/
            if( ioWndBuffLineNowPtr->prevPtr != NULL )
            {
                prevPtr = ioWndBuffLineNowPtr->prevPtr;
                newPtr = joinData( prevPtr,ioWndBuffLineNowPtr ); /* �O�s�Ɩ{�s�����������V�����s�f�[�^�𐶐� */

                if( newPtr != NULL )
                {
                    /* �s�ԍ��A�L�����b�g�ʒu��O�s�f�[�^�ɂ���Č��܂� */
                    newPtr->lineNum  = prevPtr->lineNum;
                    newPtr->caretPos = prevPtr->dataSize - prevPtr->newLineCodeSize;

                    removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr ); /* �{�s�͍폜 */
                    replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,prevPtr,newPtr );    /* �O�s�͐V�����s�f�[�^�ɒu�������� */

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
    S_BUFF_LINE_DATA *newPtr,*newNextPtr,*addPtr;

    if( ioWndBuffLineNowPtr != NULL )
    {
        divideData( ioWndBuffLineNowPtr, &newPtr, &newNextPtr );
        if( newPtr != NULL )
        {
            if( newNextPtr != NULL )
            {
                addPtr = replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newNextPtr ); /* �܂� �Â��f�[�^�����s�ȍ~�̃f�[�^�ɒu������ */
                insertLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,addPtr,newPtr ); /* ���̑O�ɉ��s�O�̃f�[�^��}�� */
                free( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = addPtr;
                ioWndBuffLineNowPtr->caretPos = 0;
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
static CHARSET_TYPE
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

#define TAB_SIZE 8 /* �b�� */
/********************************************************************************
 * ���e  : �w��ʒu�̕\���f�[�^���擾����
 * ����  : S_BUFF_LINE_DATA *linePtr   �s�f�[�^
 * ����  : DWORD             dispPos   ��ʒu(�\�����)
 * ����  : TCHAR            *dataPtr   �f�[�^�i�[�̈�
 * ����  : int              *offsetPtr �f�[�^�ʒu�i�[�̈�
 * �߂�l: int                         �i�[�����f�[�^�̃T�C�Y
 ***************************************/
static int
getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, TCHAR *dataPtr, int *offsetPtr )
{
    DWORD i,j,k;
    int charType = SINGLE_CHAR;
    int size = 0;
    DWORD literalMaxSize;

    if( linePtr != NULL )
    {
        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( i=0,j=0; i<literalMaxSize; i++ )
        { /* 1�s���̃f�[�^��1���������� */
            if( charType == DOUBLE_CHAR_HIGH )
            { /* �O������2byte�����̏��byte�������� */
                charType = DOUBLE_CHAR_LOW;
                *(dataPtr+1) = *(linePtr->data+i);
            }
            else
            { /* �O������2byte�����̏��byte�ȊO */
                if( (*(linePtr->data+i)) == '\0' )
                {
                    charType = SINGLE_CHAR;
                    *dataPtr = ' ';
                }
                else if( (*(linePtr->data+i)) == '\t' )
                { /* �������̕�����TAB */
                    charType = TAB_CHAR;

                    if( dispPos < (j+TAB_SIZE) )
                    {
                        for(k=0;k<TAB_SIZE;k++)
                        {
                            *(dataPtr+k) = ' ';
                        }
                        size = TAB_SIZE;
                        *offsetPtr = dispPos - j;
                        break;
                    }
                    else
                    {
                        j+=TAB_SIZE;
                        continue;
                    }
                }
                else if( ( (BYTE)(*(linePtr->data+i)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xDF)) ||
                         (((BYTE)0xF0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xFF)) )
                { /* �������̕�����1byte���� */
                    charType = SINGLE_CHAR;
                    *dataPtr = *(linePtr->data+i);
                }
                else
                { /* �������̕�����2byte�����̏��byte */
                    charType = DOUBLE_CHAR_HIGH;
                    *dataPtr     = *(linePtr->data+i);
                    *(dataPtr+1) = *(linePtr->data+i+1);
                }
            }

            if( j == dispPos )
            { /* �������̕������w�肳�ꂽ�\���ʒu */
                break;
            }
            else
            {
                nop();
            }

            j++;
        }

        if( i >= literalMaxSize )
        {
            nop();
        }
        else
        {
            switch( charType )
            {
            case SINGLE_CHAR:
                size = 1;
                *offsetPtr = 0;
                break;
            case DOUBLE_CHAR_HIGH:
                size = 2;
                *offsetPtr = 0;
                break;
            case DOUBLE_CHAR_LOW:
                size = 2;
                *offsetPtr = 1;
                break;
            defalut:
                break;
            }
        }
    }
    else
    {
        nop();
    }

    return size;
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
