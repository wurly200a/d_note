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
static S_BUFF_LINE_DATA *mallocBuffLineData( DWORD size );
static void freeBuffLineData( S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
static void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr );
static S_BUFF_LINE_DATA *shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size );

static CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );
static BOOL getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );
static INT getNewLineSize( void );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

/* �����ϐ���` */
S_BUFF_LINE_DATA *ioWndBuffListTopPtr;    /* �擪�̃f�[�^                           */
S_BUFF_LINE_DATA *ioWndBuffListEndPtr;    /* �Ō�̃f�[�^                           */
S_BUFF_LINE_DATA *ioWndBuffLineNowPtr;    /* �L�����b�g������f�[�^                 */
S_BUFF_LINE_DATA *ioWndBuffLineSelectPtr; /* �I��͈͂̐擪�̃f�[�^                 */
DWORD selectCaretPos;                     /* �I��͈͂̐擪�̃f�[�^�̃L�����b�g�ʒu */

typedef struct
{
    UINT  NewLineType;
    DWORD xCaret;
    DWORD yCaret;
    INT   tabSize;
} S_IOWND_BUFF_DATA;

S_IOWND_BUFF_DATA ioWndBuffData = {IOWND_BUFF_NEWLINE_CRLF,0,0,8};

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

    ioWndBuffLineSelectPtr = NULL;
    selectCaretPos = 0;
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
 * ����  : BOOL   bInit  (TRUE:�����f�[�^���N���A,FALSE:�N���A���Ȃ�)
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

        /* ���s�ŕ��������f�[�^�����A�����X�g(tempTopPtr�`tempEndPtr)�ɓo�^(��������) */
        while( lineLengthSum < length )
        {
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( dataPtr+lineLengthSum, length - lineLengthSum );

            if( lineDataPtr != NULL )
            {
                addLinkedList( &tempTopPtr,&tempEndPtr,lineDataPtr );
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
        }
        /* ���s�ŕ��������f�[�^�����A�����X�g(tempTopPtr�`tempEndPtr)�ɓo�^(�����܂�) */

        if( ioWndBuffLineNowPtr->caretPos == 0 )
        { /* �s�̐擪�ɑ}�� */
            mergeLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

            if( tempEndPtr->newLineCodeSize == 0 )
            { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                newPtr = joinData(tempEndPtr,ioWndBuffLineNowPtr); /* �ǉ��f�[�^�̍ŏI�s�ƌ��݂̍s�ƌ��� */
                if( newPtr != NULL )
                {
                    targetPtr = ioWndBuffLineNowPtr;
                    ioWndBuffLineNowPtr = replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr ); /* ���݂̍s��u������ */
                    free( targetPtr );                                                       /* ���݂̍s�̃f�[�^�����                   */
                    caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;           /* �L�����b�g�ʒu�͒ǉ��f�[�^�̍ŏI�s�̍Ō� */
                    removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr); /* �ŏI�s���͕̂s�v�Ȃ̂ŘA�����X�g����O�� */
                    free( tempEndPtr );                                                      /* �f�[�^���̂����                         */

                    ioWndBuffLineNowPtr = newPtr;                                            /* �����������̂����ݍs�Ƃ��� */
                    ioWndBuffLineNowPtr->caretPos = caretPos;                                /* �L�����b�g�ʒu�͌����ʒu     */
                }
                else
                {
                    nop();
                }
            }
            else
            { /* �}���f�[�^�̍Ō�ɉ��s�������� */
                ioWndBuffLineNowPtr->caretPos = 0;
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

                    if( tempTopPtr == tempEndPtr )
                    { /* 1�s�}���̏ꍇ */
                        insertLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,tempEndPtr );

                        /* ���������O���f�[�^�ƁA�}���f�[�^�̐擪������(��������) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
                            free( dividePrePtr );
                        }
                        else
                        {
                            nop();
                        }
                        tempTopPtr = tempEndPtr = newPtr;

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* �ǉ��f�[�^�̍ŏI�s�Ɖ��s�ȍ~�̍s�ƌ��� */
                            if( newPtr != NULL )
                            {
                                newPtr->caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
                                free( tempEndPtr );
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else
                        { /* �}���f�[�^�̍Ō�ɉ��s�L�� */
                            nop();
                        }
                    }
                    else
                    { /* �����s�}���̏ꍇ */
                        /* ���������ʒu�Ƀf�[�^��}��(��������) */
                        mergeLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* �ǉ��f�[�^�̍ŏI�s�Ɖ��s�ȍ~�̍s�ƌ��� */
                            if( newPtr != NULL )
                            {
                                newPtr->caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
                                free( tempEndPtr );
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else
                        { /* �}���f�[�^�̍Ō�ɉ��s�L�� */
                            nop();
                        }

                        /* ���������O���f�[�^�ƁA�}���f�[�^�̐擪������(��������) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
                            free( dividePrePtr );
                        }
                        else
                        {
                            nop();
                        }
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
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffDataGet( TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region )
{
    BOOL rtn = FALSE;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;
    DWORD size;

    if( region == BUFF_ALL )
    {
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
    }
    else
    {
        if( dataPtr != NULL )
        {
            if( ioWndBuffLineSelectPtr != NULL )
            { /* �I���J�n�ʒu�L�� */
                if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
                { /* �������ɑI�� */
                    for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = nowPtr->nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineSelectPtr )
                        {
                            size = (nowPtr->dataSize - selectCaretPos);
                            memcpy( dataPtr, (nowPtr->data + selectCaretPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = nowPtr->caretPos;
                            memcpy( dataPtr,nowPtr->data,size );
                            dataPtr += size;
                        }
                        else
                        {
                            size = nowPtr->dataSize;
                            memcpy( dataPtr, nowPtr->data,size );
                            dataPtr += size;
                        }
                        prevPtr = nowPtr;
                    }
                }
                else if( (ioWndBuffLineSelectPtr->lineNum) > (ioWndBuffLineNowPtr->lineNum) )
                { /* �������ɑI�� */
                    for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = nowPtr->nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = (nowPtr->dataSize - nowPtr->caretPos);
                            memcpy( dataPtr, (nowPtr->data + nowPtr->caretPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == ioWndBuffLineSelectPtr )
                        {
                            size = selectCaretPos;
                            memcpy( dataPtr,nowPtr->data,size );
                            dataPtr += size;
                        }
                        else
                        {
                            size = nowPtr->dataSize;
                            memcpy( dataPtr, nowPtr->data,size );
                            dataPtr += size;
                        }
                        prevPtr = nowPtr;
                    }
                }
                else
                { /* ����s���őI�� */
                    if( ioWndBuffLineNowPtr == ioWndBuffLineSelectPtr )
                    {
                        if( ioWndBuffLineNowPtr->caretPos < selectCaretPos )
                        {
                            size = (selectCaretPos - ioWndBuffLineNowPtr->caretPos);
                            memcpy( dataPtr, (ioWndBuffLineNowPtr->data + ioWndBuffLineNowPtr->caretPos),size );
                            dataPtr += size;
                        }
                        else if( ioWndBuffLineNowPtr->caretPos > selectCaretPos )
                        {
                            size = (ioWndBuffLineNowPtr->caretPos - selectCaretPos);
                            memcpy( dataPtr, (ioWndBuffLineNowPtr->data + selectCaretPos),size );
                            dataPtr += size;
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
        else
        {
            nop();
        }
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
 * ���e  : �s�f�[�^�̐���
 * ����  : DWORD size
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
mallocBuffLineData( DWORD size )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (size * sizeof(TCHAR)) );
    if( newPtr != NULL )
    {
        memset( newPtr, sizeof(S_BUFF_LINE_DATA), 0 );
        newPtr->dataSize = size;
    }
    else
    {
        nop();
    }

    return newPtr;
}

/********************************************************************************
 * ���e  : �s�f�[�^�̉��
 * ����  : S_BUFF_LINE_DATA *
 * �߂�l: �Ȃ�
 ***************************************/
static void
freeBuffLineData( S_BUFF_LINE_DATA *dataPtr )
{
    free( dataPtr );
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
            newPtr->lineNum = 0;

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
        *new1Ptr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (dataPtr->caretPos) * sizeof(TCHAR));

        if( *new1Ptr != NULL )
        {
            *new2Ptr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + ((dataPtr->dataSize-dataPtr->caretPos) * sizeof(TCHAR)) );
            if( *new2Ptr != NULL )
            {
                /* ���s���O�̃f�[�^���� */
                (*new1Ptr)->dataSize = dataPtr->caretPos;
                (*new1Ptr)->newLineCodeSize = 0;
                (*new1Ptr)->lineNum = dataPtr->lineNum;
                (*new1Ptr)->caretPos = dataPtr->caretPos;

                memcpy( (*new1Ptr)->data, dataPtr->data, (*new1Ptr)->dataSize );

                /* ���s�ȍ~�̃f�[�^���� */
                (*new2Ptr)->dataSize = dataPtr->dataSize - dataPtr->caretPos;
                (*new2Ptr)->newLineCodeSize = dataPtr->newLineCodeSize;
                (*new2Ptr)->lineNum = (*new1Ptr)->lineNum+1;
                (*new2Ptr)->caretPos = 0;
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
 * ���e  : �s�f�[�^��Z������
 * ����  : S_BUFF_LINE_DATA *dataPtr
 * ����  : DWORD size
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    if( (dataPtr != NULL) &&
        (dataPtr->dataSize >= size) )
    {
        newPtr = mallocBuffLineData( dataPtr->dataSize-size );
        if( newPtr != NULL )
        {
            memcpy( newPtr->data, dataPtr->data, newPtr->dataSize - dataPtr->newLineCodeSize );
            newPtr->newLineCodeSize = dataPtr->newLineCodeSize;
            memcpy( newPtr->data + newPtr->dataSize - newPtr->newLineCodeSize, dataPtr->data + dataPtr->dataSize - dataPtr->newLineCodeSize, dataPtr->newLineCodeSize );
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
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�T�C�Y�擾
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetBuffSize( IOWND_BUFF_REGION region )
{
    DWORD dataSize = 0;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;

    if( region == BUFF_ALL )
    {
        for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = nowPtr->nextPtr )
        {
            dataSize += nowPtr->dataSize;
        }
    }
    else
    {
        if( ioWndBuffLineSelectPtr != NULL )
        { /* �I���J�n�ʒu�L�� */
            if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
            { /* �������ɑI�� */
                for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = nowPtr->nextPtr )
                {
                    if( nowPtr == ioWndBuffLineSelectPtr )
                    {
                        dataSize += (nowPtr->dataSize - selectCaretPos);
                    }
                    else if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += nowPtr->caretPos;
                    }
                    else
                    {
                        dataSize += nowPtr->dataSize;
                    }
                    prevPtr = nowPtr;
                }
            }
            else if( (ioWndBuffLineSelectPtr->lineNum) > (ioWndBuffLineNowPtr->lineNum) )
            { /* �������ɑI�� */
                for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = nowPtr->nextPtr )
                {
                    if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += (nowPtr->dataSize - nowPtr->caretPos);
                    }
                    else if( nowPtr == ioWndBuffLineSelectPtr )
                    {
                        dataSize += selectCaretPos;
                    }
                    else
                    {
                        dataSize += nowPtr->dataSize;
                    }
                    prevPtr = nowPtr;
                }
            }
            else
            { /* ����s���őI�� */
                if( ioWndBuffLineNowPtr == ioWndBuffLineSelectPtr )
                {
                    if( ioWndBuffLineNowPtr->caretPos < selectCaretPos )
                    {
                        dataSize += (selectCaretPos - ioWndBuffLineNowPtr->caretPos);
                    }
                    else if( ioWndBuffLineNowPtr->caretPos > selectCaretPos )
                    {
                        dataSize += (ioWndBuffLineNowPtr->caretPos - selectCaretPos);
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
    DWORD i,j=0,k;
    DWORD literalMaxSize;
    int tab_offset;
    S_BUFF_LINE_DATA *linePtr;
    DWORD dispPos;
    CHARSET_TYPE charType;

    if( ioWndBuffLineNowPtr != NULL )
    {
        linePtr = ioWndBuffLineNowPtr;
        dispPos = linePtr->caretPos;
        charType = SINGLE_CHAR;
        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( i=0,j=0; (i<dispPos)&&(i<literalMaxSize); i++ )
        { /* 1�s���̃f�[�^��1���������� */
            if( charType == DOUBLE_CHAR_HIGH )
            { /* �O������2byte�����̏��byte�������� */
                charType = DOUBLE_CHAR_LOW;
                j++;
            }
            else
            { /* �O������2byte�����̏��byte�ȊO */
                if( (*(linePtr->data+i)) == '\0' )
                {
                    charType = SINGLE_CHAR;
                    j++;
                }
                else if( (*(linePtr->data+i)) == '\t' )
                { /* �������̕�����TAB */
                    charType = TAB_CHAR;

                    tab_offset = ioWndBuffData.tabSize - (j % ioWndBuffData.tabSize);
                    j+=tab_offset;
                }
                else if( ( (BYTE)(*(linePtr->data+i)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xDF)) ||
                         (((BYTE)0xF0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xFF)) )
                { /* �������̕�����1byte���� */
                    charType = SINGLE_CHAR;
                    j++;
                }
                else
                { /* �������̕�����2byte�����̏��byte */
                    charType = DOUBLE_CHAR_HIGH;
                    j++;
                }
            }
        }
    }
    else
    {
        nop();
    }

    return j;
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
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffGetDispData( DWORD lineNum, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr;
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
                getDispCharData( nowPtr, dispPos, dataPtr );
                break;
            }
            else
            {
                nop();
            }
        }
    }

    return TRUE;
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
        allDataSize = IoWndGetBuffSize(BUFF_ALL);
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
 * ���e  : �f�[�^�폜
 * ����  : BOOL bBackSpace
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffRemoveData( BOOL bBackSpace )
{
    int removeSize = 0;
    S_BUFF_LINE_DATA *newPtr = NULL,*prevPtr = NULL,*nextPtr = NULL;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    BOOL bValid = FALSE;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( bBackSpace )
        { /* BS�L�[ */
            if( ioWndBuffLineNowPtr->caretPos > 0  )
            {
                bValid = TRUE;

                if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos-1) == DOUBLE_CHAR_LOW )
                { /* ���̕����ō폜�ʂ𔻒f */
                    removeSize = 2;
                }
                else
                {
                    removeSize = 1;
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
        { /* DEL�L�[ */
            if( ioWndBuffLineNowPtr->caretPos != (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
            {
                bValid = TRUE;

                if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretPos) == DOUBLE_CHAR_HIGH )
                { /* �L�����b�g�ʒu�̕����ō폜�ʂ𔻒f */
                    removeSize = 2;
                }
                else
                {
                    removeSize = 1;
                }
                ioWndBuffLineNowPtr->caretPos += removeSize;
            }
            else
            { /* �L�����b�g���s�̍ŏI�ʒu�B�܂�A���s�Ƃ̌����B*/
                if( ioWndBuffLineNowPtr->nextPtr != NULL )
                {
                    nextPtr = ioWndBuffLineNowPtr->nextPtr;
                    newPtr = joinData( ioWndBuffLineNowPtr,nextPtr ); /* �{�s�Ǝ��s�����������V�����s�f�[�^�𐶐� */

                    if( newPtr != NULL )
                    {
                        /* �s�ԍ��A�L�����b�g�ʒu�͖{�s�f�[�^�ɂ���Č��܂� */
                        newPtr->lineNum  = ioWndBuffLineNowPtr->lineNum;
                        newPtr->caretPos = ioWndBuffLineNowPtr->caretPos;

                        removeLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,nextPtr );         /* ���s�͍폜 */
                        replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr ); /* �{�s�͐V�����s�f�[�^�ɒu�������� */

                        free( ioWndBuffLineNowPtr );
                        free( nextPtr );
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

        if( bValid )
        { /* �폜�L�� */
            divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
            newPtr = shortenData( dividePrePtr, removeSize );                 /* ������̑O���f�[�^�̖������珊��ʍ폜 */
            freeBuffLineData( dividePrePtr );                                 /* �O���f�[�^�� */
            dividePrePtr = newPtr;                                            /* ��������     */

            newPtr = joinData( dividePrePtr, dividePostPtr );                 /* �Č��� */
            newPtr->lineNum = ioWndBuffLineNowPtr->lineNum;                   /* �s�ԍ��͓��� */
            newPtr->caretPos = ioWndBuffLineNowPtr->caretPos - removeSize;    /* �L�����b�g�ʒu�͍폜�ʕ��O���ɂȂ� */
            freeBuffLineData( dividePrePtr );                                 /* ��ƃf�[�^���폜 */
            freeBuffLineData( dividePostPtr );                                /* ��ƃf�[�^���폜 */

            /* �Â��s�f�[�^�ƒu�������� */
            replaceLinkedList( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
            freeBuffLineData( ioWndBuffLineNowPtr );
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
 * ���e  : ���s�ǉ�
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffAddNewLine( void )
{
    TCHAR data[2];
    INT size = 0;

    switch( ioWndBuffData.NewLineType )
    {
    case IOWND_BUFF_NEWLINE_CRLF:
        data[0] = '\r';
        data[1] = '\n';
        size = 2;
        break;
    case IOWND_BUFF_NEWLINE_LF  :
        data[0] = '\n';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_CR  :
        data[0] = '\r';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_NONE:
    default:
        break;
    }

    if( size )
    {
        IoWndBuffDataSet( data, size, FALSE );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃^�u�T�C�Y�Z�b�g
 * ����  : INT tabSize
 * �߂�l: BOOL (TRUE:�ύX���ꂽ)
 ***************************************/
BOOL
IoWndBuffSetTabSize( INT tabSize )
{
    BOOL bRtn = FALSE;

    if( tabSize != ioWndBuffData.tabSize )
    {
        ioWndBuffData.tabSize = tabSize;
        bRtn = TRUE;
    }
    else
    {
        nop();
    }

    return bRtn;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�͈̔͑I��ON
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSelectOn( void )
{
    if( ioWndBuffLineSelectPtr )
    { /* ���ɑI���ς݂̏ꍇ */
        nop(); /* �ύX���Ȃ� */
    }
    else
    {
        ioWndBuffLineSelectPtr = ioWndBuffLineNowPtr;
        selectCaretPos = ioWndBuffLineNowPtr->caretPos;
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�͈̔͑I��OFF
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSelectOff( void )
{
    ioWndBuffLineSelectPtr = NULL;
    selectCaretPos = 0;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̑S�͈͑I��
 * ����  : �Ȃ�
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffSelectAll( void )
{
    BOOL bRtn = FALSE;

    if( ioWndBuffListTopPtr != NULL )
    {
        if( (ioWndBuffLineSelectPtr == ioWndBuffListTopPtr) &&
            (selectCaretPos == 0) &&
            (ioWndBuffLineNowPtr == ioWndBuffListEndPtr) &&
            (ioWndBuffLineNowPtr->caretPos == (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) ) )
        {
            nop();
        }
        else
        {
            ioWndBuffLineSelectPtr = ioWndBuffListTopPtr;
            selectCaretPos = 0;
            ioWndBuffLineNowPtr = ioWndBuffListEndPtr;
            ioWndBuffLineNowPtr->caretPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
            bRtn = TRUE;
        }
    }
    else
    {
        nop();
    }

    return bRtn;
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

/********************************************************************************
 * ���e  : �w��ʒu�̕\���f�[�^���擾����
 * ����  : S_BUFF_LINE_DATA *linePtr   �s�f�[�^
 * ����  : DWORD             dispPos   ��ʒu(�\�����)
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
static BOOL
getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    DWORD i,j,k;
    DWORD literalMaxSize;
    int tab_offset;

    if( (linePtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->size    = 0;
        dataPtr->type    = SINGLE_CHAR;
        dataPtr->bSelect = FALSE;

        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( i=0,j=0; i<literalMaxSize; i++ )
        { /* 1�s���̃f�[�^��1���������� */
            if( dataPtr->type == DOUBLE_CHAR_HIGH )
            { /* �O������2byte�����̏��byte�������� */
                dataPtr->type = DOUBLE_CHAR_LOW;
                *(dataPtr->data+1) = *(linePtr->data+i);
            }
            else
            { /* �O������2byte�����̏��byte�ȊO */
                if( (*(linePtr->data+i)) == '\0' )
                {
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = ' ';
                }
                else if( (*(linePtr->data+i)) == '\t' )
                { /* �������̕�����TAB */
                    dataPtr->type = TAB_CHAR;

                    tab_offset = ioWndBuffData.tabSize - (j % ioWndBuffData.tabSize);

                    if( dispPos < (j+tab_offset) )
                    {
                        for(k=0;k<tab_offset;k++)
                        {
                            *(dataPtr->data+k) = ' ';
                        }
                        dataPtr->size = tab_offset;
                        dataPtr->offset = dispPos - j;
                        break;
                    }
                    else
                    {
                        j+=tab_offset;
                        continue;
                    }
                }
                else if( ( (BYTE)(*(linePtr->data+i)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xDF)) ||
                         (((BYTE)0xF0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xFF)) )
                { /* �������̕�����1byte���� */
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = *(linePtr->data+i);
                }
                else
                { /* �������̕�����2byte�����̏��byte */
                    dataPtr->type = DOUBLE_CHAR_HIGH;
                    *(dataPtr->data)   = *(linePtr->data+i);
                    *(dataPtr->data+1) = *(linePtr->data+i+1);
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
            switch( dataPtr->type )
            {
            case SINGLE_CHAR:
                dataPtr->size = 1;
                dataPtr->offset = 0;
                break;
            case DOUBLE_CHAR_HIGH:
                dataPtr->size = 2;
                dataPtr->offset = 0;
                break;
            case DOUBLE_CHAR_LOW:
                dataPtr->size = 2;
                dataPtr->offset = 1;
                break;
            defalut:
                break;
            }

            if( ioWndBuffLineSelectPtr != NULL )
            { /* �I���J�n�ʒu�L�� */
                if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
                { /* �������ɑI�� */
                    if( (ioWndBuffLineSelectPtr->lineNum < linePtr->lineNum) && (linePtr->lineNum < ioWndBuffLineNowPtr->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (selectCaretPos <= i) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (i < linePtr->caretPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else
                    {
                        nop();
                    }
                }
                else if( (ioWndBuffLineSelectPtr->lineNum) > (ioWndBuffLineNowPtr->lineNum) )
                { /* �������ɑI�� */
                    if( (ioWndBuffLineNowPtr->lineNum < linePtr->lineNum) && (linePtr->lineNum < ioWndBuffLineSelectPtr->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (linePtr->caretPos <= i) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (i < selectCaretPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else
                    {
                        nop();
                    }
                }
                else
                { /* ����s���őI�� */
                    if( ioWndBuffLineNowPtr == linePtr )
                    {
                        if( linePtr->caretPos < selectCaretPos )
                        {
                            if( (linePtr->caretPos <= i) && (i < selectCaretPos) )
                            {
                                dataPtr->bSelect = TRUE;
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else if( linePtr->caretPos > selectCaretPos )
                        {
                            if( (selectCaretPos <= i) && (i < linePtr->caretPos) )
                            {
                                dataPtr->bSelect = TRUE;
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
                        nop();
                    }
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

    return TRUE;
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

