/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"
#include "LinkedList.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWndBuffer.h"
static S_BUFF_LINE_DATA *createBuffLineData( DWORD size, INT newLineCodeSize, TCHAR *dataPtr, DWORD lineNum, DWORD caretPos );
static void destroyBuffLineData( S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *getBuffLine( TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
static void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr );
static S_BUFF_LINE_DATA *shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size );
static CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );
static DWORD getCaretDispXpos( S_BUFF_LINE_DATA *linePtr, DWORD dataPos );
static BOOL getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );
static INT getNewLineSize( void );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

static void addLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void removeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void mergeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr );
static S_BUFF_LINE_DATA *replaceLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static void insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );

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

    ClearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);

    /* ��f�[�^��ǉ� */
    lineDataPtr = createBuffLineData( 0, 0, NULL, 0, 0 );
    if( lineDataPtr != NULL )
    {
        addLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,lineDataPtr );
        ioWndBuffLineNowPtr = lineDataPtr;
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
    ClearLinkedList(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr);
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
                addLineData( &tempTopPtr,&tempEndPtr,lineDataPtr );
                lineLengthSum += lineDataPtr->dataSize;
            }
            else
            {
                break;
            }
        }
        /* ���s�ŕ��������f�[�^�����A�����X�g(tempTopPtr�`tempEndPtr)�ɓo�^(�����܂�) */

        if( ioWndBuffLineNowPtr->caretDataPos == 0 )
        { /* �s�̐擪�ɑ}�� */
            mergeLineData(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

            if( tempEndPtr->newLineCodeSize == 0 )
            { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                newPtr = joinData(tempEndPtr,ioWndBuffLineNowPtr); /* �ǉ��f�[�^�̍ŏI�s�ƌ��݂̍s�ƌ��������V���ȃf�[�^���� */
                if( newPtr != NULL )
                {
                    targetPtr = ioWndBuffLineNowPtr;
                    ioWndBuffLineNowPtr = replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr ); /* ���݂̍s��u������ */
                    free( targetPtr );                                                       /* ���݂̍s�̃f�[�^�����                   */

                    caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;           /* �L�����b�g�ʒu�͒ǉ��f�[�^�̍ŏI�s�̍Ō� */
                    removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr); /* �ŏI�s���͕̂s�v�Ȃ̂ŘA�����X�g����O�� */
                    free( tempEndPtr );

                    ioWndBuffLineNowPtr = newPtr;                                            /* �����������̂����ݍs�Ƃ��� */
                    ioWndBuffLineNowPtr->caretDataPos = caretPos;                                /* �L�����b�g�ʒu�͌����ʒu     */
                }
                else
                {
                    nop();
                }
            }
            else
            { /* �}���f�[�^�̍Ō�ɉ��s�������� */
                ioWndBuffLineNowPtr->caretDataPos = 0;
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
                    replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,dividePostPtr ); /* �܂� �Â��f�[�^�����s�ȍ~�̃f�[�^�ɒu������ */
                    insertLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,dividePrePtr ); /* ���̑O�ɉ��s�O�̃f�[�^��}�� */

                    free( ioWndBuffLineNowPtr );
                    ioWndBuffLineNowPtr = dividePostPtr;
                    ioWndBuffLineNowPtr->caretDataPos = 0;
                    /* ���݂̍s���L�����b�g�ʒu�ŕ���(�����܂�) */

                    if( tempTopPtr == tempEndPtr )
                    { /* 1�s�}���̏ꍇ */
                        insertLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,tempEndPtr );

                        /* ���������O���f�[�^�ƁA�}���f�[�^�̐擪������(��������) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
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
                                newPtr->caretDataPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
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
                        mergeLineData(&ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,&tempTopPtr,&tempEndPtr);

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* �ǉ��f�[�^�̍ŏI�s�Ɖ��s�ȍ~�̍s�ƌ��� */
                            if( newPtr != NULL )
                            {
                                newPtr->caretDataPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePostPtr,newPtr );
                                free( dividePostPtr );
                                ioWndBuffLineNowPtr = newPtr;
                                removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempEndPtr);
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
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,tempTopPtr,newPtr );
                            free( targetPtr );
                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,dividePrePtr);
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
            ioWndBuffLineNowPtr->caretDataPos = 0;
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
                for( nowPtr=ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
                    for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineSelectPtr )
                        {
                            size = (nowPtr->dataSize - selectCaretPos);
                            memcpy( dataPtr, (nowPtr->data + selectCaretPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = nowPtr->caretDataPos;
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
                    for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == ioWndBuffLineNowPtr )
                        {
                            size = (nowPtr->dataSize - nowPtr->caretDataPos);
                            memcpy( dataPtr, (nowPtr->data + nowPtr->caretDataPos),size );
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
                        if( ioWndBuffLineNowPtr->caretDataPos < selectCaretPos )
                        {
                            size = (selectCaretPos - ioWndBuffLineNowPtr->caretDataPos);
                            memcpy( dataPtr, (ioWndBuffLineNowPtr->data + ioWndBuffLineNowPtr->caretDataPos),size );
                            dataPtr += size;
                        }
                        else if( ioWndBuffLineNowPtr->caretDataPos > selectCaretPos )
                        {
                            size = (ioWndBuffLineNowPtr->caretDataPos - selectCaretPos);
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
        for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
                for( nowPtr = ioWndBuffLineSelectPtr,prevPtr = NULL; prevPtr != ioWndBuffLineNowPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == ioWndBuffLineSelectPtr )
                    {
                        dataSize += (nowPtr->dataSize - selectCaretPos);
                    }
                    else if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += nowPtr->caretDataPos;
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
                for( nowPtr = ioWndBuffLineNowPtr,prevPtr = NULL; prevPtr != ioWndBuffLineSelectPtr; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == ioWndBuffLineNowPtr )
                    {
                        dataSize += (nowPtr->dataSize - nowPtr->caretDataPos);
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
                    if( ioWndBuffLineNowPtr->caretDataPos < selectCaretPos )
                    {
                        dataSize += (selectCaretPos - ioWndBuffLineNowPtr->caretDataPos);
                    }
                    else if( ioWndBuffLineNowPtr->caretDataPos > selectCaretPos )
                    {
                        dataSize += (ioWndBuffLineNowPtr->caretDataPos - selectCaretPos);
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

    for( nowPtr = ioWndBuffListTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
    return getCaretDispXpos(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos);
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
    S_BUFF_DISP_DATA dispData;

    nowPtr = IoWndBuffGetLinePtr( lineNum );

    if( nowPtr != NULL )
    {
        ioWndBuffLineNowPtr = nowPtr;

        getDispCharData( ioWndBuffLineNowPtr, xPos, &dispData );
        ioWndBuffLineNowPtr->caretDataPos = dispData.caretPos;
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
        if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos) == DOUBLE_CHAR_HIGH )
        { /* ���̕����ňړ��ʂ𔻒f */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( (ioWndBuffLineNowPtr->caretDataPos + moveAmount) > (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
        { /* �L�����b�g�͂��łɍs�̍ŉE�[ */
            if( (ioWndBuffLineNowPtr->header.nextPtr != NULL) )
            { /* ���s�L�� */
                ioWndBuffLineNowPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.nextPtr;
                ioWndBuffLineNowPtr->caretDataPos = 0;
            }
            else
            { /* ���s���� */
                nop();
            }
        }
        else
        { /* �܂��E�ֈړ��\ */
            ioWndBuffLineNowPtr->caretDataPos += moveAmount;
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
        if( (ioWndBuffLineNowPtr->caretDataPos>0) &&
            (detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos-1) == DOUBLE_CHAR_LOW) )
        { /* �O�̕����ňړ��ʂ𔻒f */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( ioWndBuffLineNowPtr->caretDataPos == 0 )
        { /* �L�����b�g�͂��łɍs�̍ō��[ */
            if( (ioWndBuffLineNowPtr->header.prevPtr != NULL) )
            { /* �O�s�L�� */
                ioWndBuffLineNowPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.prevPtr;
                ioWndBuffLineNowPtr->caretDataPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
            }
            else
            { /* �O�s���� */
                nop();
            }
        }
        else
        { /* �܂����ֈړ��\ */
            ioWndBuffLineNowPtr->caretDataPos = max( ioWndBuffLineNowPtr->caretDataPos - moveAmount, 0 );
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
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *nextPtr;
    S_BUFF_DISP_DATA dispData;

    nextPtr = (S_BUFF_LINE_DATA *)(ioWndBuffLineNowPtr->header.nextPtr);

    if( (ioWndBuffLineNowPtr != NULL) && (nextPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos);
        getDispCharData( nextPtr, preDispPos, &dispData );
        nextPtr->caretDataPos = dispData.caretPos;
        ioWndBuffLineNowPtr = nextPtr;
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
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *prevPtr;
    S_BUFF_DISP_DATA dispData;

    prevPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.prevPtr;

    if( (ioWndBuffLineNowPtr != NULL) && (prevPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos);
        getDispCharData( prevPtr, preDispPos, &dispData );
        prevPtr->caretDataPos = dispData.caretPos;
        ioWndBuffLineNowPtr = prevPtr;
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
            nextPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr;

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
 * ����  : S_BUFF_LINE_DATA *lineDataPtr
 * ����  : DWORD             dispPos     �\���ʒu
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffGetDispData( S_BUFF_LINE_DATA *lineDataPtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    getDispCharData( lineDataPtr, dispPos, dataPtr );

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
            for( nowPtr=ioWndBuffListTopPtr,dataPtr=dataTopPtr; nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
    DWORD removeSize = 0,saveCaretPos;
    S_BUFF_LINE_DATA *newPtr = NULL,*prevPtr = NULL,*nextPtr = NULL,*nowPtr = NULL,*savePtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    BOOL bValid = FALSE;

    if( ioWndBuffLineNowPtr != NULL )
    {
        if( ioWndBuffLineSelectPtr != NULL )
        { /* �I���J�n�ʒu�L�� */
            if( (ioWndBuffLineSelectPtr->lineNum) == (ioWndBuffLineNowPtr->lineNum) )
            { /* ����s���őI�� */
                if( ioWndBuffLineNowPtr == ioWndBuffLineSelectPtr )
                {
                    if( ioWndBuffLineNowPtr->caretDataPos < selectCaretPos )
                    {
                        removeSize += (selectCaretPos - ioWndBuffLineNowPtr->caretDataPos);
                        saveCaretPos = ioWndBuffLineNowPtr->caretDataPos;
                        ioWndBuffLineNowPtr->caretDataPos = selectCaretPos;
                        selectCaretPos = saveCaretPos;
                    }
                    else if( ioWndBuffLineNowPtr->caretDataPos > selectCaretPos )
                    {
                        removeSize += (ioWndBuffLineNowPtr->caretDataPos - selectCaretPos);
                    }
                    else
                    {
                        nop();
                    }

                    divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
                    newPtr = shortenData( dividePrePtr, removeSize );                 /* ������̑O���f�[�^�̖������珊��ʍ폜 */
                    destroyBuffLineData( dividePrePtr );                                 /* �O���f�[�^�� */
                    dividePrePtr = newPtr;                                            /* ��������     */

                    newPtr = joinData( dividePrePtr, dividePostPtr );                 /* �Č��� */
                    newPtr->lineNum = ioWndBuffLineNowPtr->lineNum;                   /* �s�ԍ��͓��� */
                    newPtr->caretDataPos = ioWndBuffLineNowPtr->caretDataPos - removeSize;    /* �L�����b�g�ʒu�͍폜�ʕ��O���ɂȂ� */
                    destroyBuffLineData( dividePrePtr );                                 /* ��ƃf�[�^���폜 */
                    destroyBuffLineData( dividePostPtr );                                /* ��ƃf�[�^���폜 */

                    /* �Â��s�f�[�^�ƒu�������� */
                    replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
                    destroyBuffLineData( ioWndBuffLineNowPtr );
                    ioWndBuffLineNowPtr = newPtr;
                }
                else
                {
                    nop();
                }
            }
            else
            {
                if( (ioWndBuffLineSelectPtr->lineNum) < (ioWndBuffLineNowPtr->lineNum) )
                { /* �������ɑI�� */
                    nop();
                }
                else
                { /* �������ɑI�� */
                    savePtr = ioWndBuffLineSelectPtr;
                    saveCaretPos = selectCaretPos;
                    ioWndBuffLineSelectPtr = ioWndBuffLineNowPtr;
                    selectCaretPos = ioWndBuffLineNowPtr->caretDataPos;
                    ioWndBuffLineNowPtr = savePtr;
                    ioWndBuffLineNowPtr->caretDataPos = saveCaretPos;
                }

                /* �擪����Ō�܂őI�����ꂽ�s���폜 */
                for( nowPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineSelectPtr->header.nextPtr; nowPtr != ioWndBuffLineNowPtr; nowPtr = nextPtr )
                {
                    nextPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr;
                    removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,nowPtr);
                    destroyBuffLineData( nowPtr );
                }
                /* �擪�s�̂����A�I���ʒu����O���c�� */
                if( selectCaretPos == 0 )
                { /* �I���ʒu���擪��������A1�s�ہX�폜 */
                    removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineSelectPtr);
                }
                else
                {
                    ioWndBuffLineSelectPtr->caretDataPos = selectCaretPos;
                    divideData( ioWndBuffLineSelectPtr, &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
                    replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineSelectPtr,dividePrePtr );
                    destroyBuffLineData( ioWndBuffLineSelectPtr );
                }

                /* �ŏI�s�̂����A�I���ʒu�������c�� */
                divideData( ioWndBuffLineNowPtr, &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,dividePostPtr );
                destroyBuffLineData( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = dividePostPtr;
                ioWndBuffLineNowPtr->caretDataPos = 0;
                updateLineNum( ioWndBuffLineNowPtr );
            }

            ioWndBuffLineSelectPtr = NULL;
            selectCaretPos = 0;
        }
        else
        { /* �I�𖳂� */
            if( bBackSpace )
            { /* BS�L�[ */
                if( ioWndBuffLineNowPtr->caretDataPos > 0  )
                {
                    bValid = TRUE;

                    if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos-1) == DOUBLE_CHAR_LOW )
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
                    if( ioWndBuffLineNowPtr->header.prevPtr != NULL )
                    {
                        prevPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.prevPtr;
                        newPtr = joinData( prevPtr,ioWndBuffLineNowPtr ); /* �O�s�Ɩ{�s�����������V�����s�f�[�^�𐶐� */

                        if( newPtr != NULL )
                        {
                            /* �s�ԍ��A�L�����b�g�ʒu��O�s�f�[�^�ɂ���Č��܂� */
                            newPtr->lineNum  = prevPtr->lineNum;
                            newPtr->caretDataPos = prevPtr->dataSize - prevPtr->newLineCodeSize;

                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr ); /* �{�s�͍폜 */
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,prevPtr,newPtr );    /* �O�s�͐V�����s�f�[�^�ɒu�������� */

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
                if( ioWndBuffLineNowPtr->caretDataPos != (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) )
                {
                    bValid = TRUE;

                    if( detectCharSet(ioWndBuffLineNowPtr,ioWndBuffLineNowPtr->caretDataPos) == DOUBLE_CHAR_HIGH )
                    { /* �L�����b�g�ʒu�̕����ō폜�ʂ𔻒f */
                        removeSize = 2;
                    }
                    else
                    {
                        removeSize = 1;
                    }
                    ioWndBuffLineNowPtr->caretDataPos += removeSize;
                }
                else
                { /* �L�����b�g���s�̍ŏI�ʒu�B�܂�A���s�Ƃ̌����B*/
                    if( ioWndBuffLineNowPtr->header.nextPtr != NULL )
                    {
                        nextPtr = (S_BUFF_LINE_DATA *)ioWndBuffLineNowPtr->header.nextPtr;
                        newPtr = joinData( ioWndBuffLineNowPtr,nextPtr ); /* �{�s�Ǝ��s�����������V�����s�f�[�^�𐶐� */

                        if( newPtr != NULL )
                        {
                            /* �s�ԍ��A�L�����b�g�ʒu�͖{�s�f�[�^�ɂ���Č��܂� */
                            newPtr->lineNum  = ioWndBuffLineNowPtr->lineNum;
                            newPtr->caretDataPos = ioWndBuffLineNowPtr->caretDataPos;

                            removeLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,nextPtr );         /* ���s�͍폜 */
                            replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr ); /* �{�s�͐V�����s�f�[�^�ɒu�������� */

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
                destroyBuffLineData( dividePrePtr );                                 /* �O���f�[�^�� */
                dividePrePtr = newPtr;                                            /* ��������     */

                newPtr = joinData( dividePrePtr, dividePostPtr );                 /* �Č��� */
                newPtr->lineNum = ioWndBuffLineNowPtr->lineNum;                   /* �s�ԍ��͓��� */
                newPtr->caretDataPos = ioWndBuffLineNowPtr->caretDataPos - removeSize;    /* �L�����b�g�ʒu�͍폜�ʕ��O���ɂȂ� */
                destroyBuffLineData( dividePrePtr );                                 /* ��ƃf�[�^���폜 */
                destroyBuffLineData( dividePostPtr );                                /* ��ƃf�[�^���폜 */

                /* �Â��s�f�[�^�ƒu�������� */
                replaceLineData( &ioWndBuffListTopPtr,&ioWndBuffListEndPtr,ioWndBuffLineNowPtr,newPtr );
                destroyBuffLineData( ioWndBuffLineNowPtr );
                ioWndBuffLineNowPtr = newPtr;
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
        selectCaretPos = ioWndBuffLineNowPtr->caretDataPos;
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
            (ioWndBuffLineNowPtr->caretDataPos == (ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize) ) )
        {
            nop();
        }
        else
        {
            ioWndBuffLineSelectPtr = ioWndBuffListTopPtr;
            selectCaretPos = 0;
            ioWndBuffLineNowPtr = ioWndBuffListEndPtr;
            ioWndBuffLineNowPtr->caretDataPos = ioWndBuffLineNowPtr->dataSize - ioWndBuffLineNowPtr->newLineCodeSize;
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
 * ���e  : �s�f�[�^�̐���
 * ����  : DWORD size          �f�[�^�T�C�Y(���s�R�[�h�܂�)
 * ����  : INT newLineCodeSize ���s�R�[�h�T�C�Y
 * ����  : TCHAR *dataPtr      �f�[�^(�|�C���^)
 * ����  : DWORD lineNum       �s�ԍ�
 * ����  : DWORD caretPos      �L�����b�g�ʒu
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
createBuffLineData( DWORD size, INT newLineCodeSize, TCHAR *dataPtr, DWORD lineNum, DWORD caretPos )
{
    S_BUFF_LINE_DATA *newPtr = NULL;

    newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (size * sizeof(TCHAR)) );
    if( newPtr != NULL )
    {
        memset( newPtr, sizeof(S_BUFF_LINE_DATA), 0 );
        newPtr->lineNum         = lineNum;
        newPtr->caretDataPos        = caretPos;
        newPtr->dataSize        = size;
        newPtr->newLineCodeSize = newLineCodeSize;

        if( dataPtr != NULL )
        {
            memcpy( newPtr->data, dataPtr, size );
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
 * ���e  : �s�f�[�^�̉��
 * ����  : S_BUFF_LINE_DATA *
 * �߂�l: �Ȃ�
 ***************************************/
static void
destroyBuffLineData( S_BUFF_LINE_DATA *dataPtr )
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

    lineDataPtr = createBuffLineData( i, newLineCodeSize, dataPtr, 0, 0 );

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
        newPtr = createBuffLineData( data1Ptr->dataSize-data1Ptr->newLineCodeSize+data2Ptr->dataSize, data2Ptr->newLineCodeSize, data1Ptr->data, data1Ptr->lineNum, 0 );
        if( newPtr != NULL )
        {
            memcpy( newPtr->data + data1Ptr->dataSize - data1Ptr->newLineCodeSize, data2Ptr->data, data2Ptr->dataSize );
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
        *new1Ptr = createBuffLineData( dataPtr->caretDataPos                  , 0                       , dataPtr->data                  , dataPtr->lineNum  , dataPtr->caretDataPos ); /* ���s���O */
        *new2Ptr = createBuffLineData( dataPtr->dataSize-dataPtr->caretDataPos, dataPtr->newLineCodeSize, dataPtr->data+dataPtr->caretDataPos, dataPtr->lineNum+1, 0                 ); /* ���s���� */
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
        newPtr = createBuffLineData( dataPtr->dataSize-size, dataPtr->newLineCodeSize, dataPtr->data, dataPtr->lineNum, dataPtr->caretDataPos );
        if( newPtr != NULL )
        {
            /* ���s�R�[�h�̃R�s�[ */
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
 * ���e  : �L�����b�gX�\���ʒu�擾
 * ����  : S_BUFF_LINE_DATA *linePtr   �s�f�[�^
 * ����  : DWORD             dataPos   ��ʒu(�f�[�^���)
 * �߂�l: DWORD
 ***************************************/
static DWORD
getCaretDispXpos( S_BUFF_LINE_DATA *linePtr, DWORD dataPos )
{
    DWORD i,j=0,k;
    DWORD literalMaxSize;
    int tab_offset;
    CHARSET_TYPE charType;

    if( linePtr != NULL )
    {
        charType = SINGLE_CHAR;
        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( i=0,j=0; (i<dataPos)&&(i<literalMaxSize); i++ )
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
 * ���e  : �w��ʒu�̕\���f�[�^���擾����
 * ����  : S_BUFF_LINE_DATA *linePtr   �s�f�[�^
 * ����  : DWORD             dispPos   ��ʒu(�\�����)
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
static BOOL
getDispCharData( S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    DWORD dataPos,j,k;
    DWORD literalMaxSize;
    int tab_offset;

    if( (linePtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->size    = 0;
        dataPtr->type    = SINGLE_CHAR;
        dataPtr->bSelect = FALSE;
        dataPtr->dataPos = 0;
        dataPtr->caretPos= 0;

        literalMaxSize = (linePtr->dataSize-linePtr->newLineCodeSize);

        for( dataPos=0,j=0; dataPos<literalMaxSize; dataPos++ )
        { /* 1�s���̃f�[�^��1���������� */
            if( dataPtr->type == DOUBLE_CHAR_HIGH )
            { /* �O������2byte�����̏��byte�������� */
                dataPtr->type = DOUBLE_CHAR_LOW;
                *(dataPtr->data+1) = *(linePtr->data+dataPos);
            }
            else
            { /* �O������2byte�����̏��byte�ȊO */
                if( (*(linePtr->data+dataPos)) == '\0' )
                {
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = ' ';
                }
                else if( (*(linePtr->data+dataPos)) == '\t' )
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
                else if( ( (BYTE)(*(linePtr->data+dataPos)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+dataPos))) && ((BYTE)(*(linePtr->data+dataPos)) <= (BYTE)0xDF)) ||
                         (((BYTE)0xF0 <= (BYTE)(*(linePtr->data+dataPos))) && ((BYTE)(*(linePtr->data+dataPos)) <= (BYTE)0xFF)) )
                { /* �������̕�����1byte���� */
                    dataPtr->type = SINGLE_CHAR;
                    *(dataPtr->data) = *(linePtr->data+dataPos);
                }
                else
                { /* �������̕�����2byte�����̏��byte */
                    dataPtr->type = DOUBLE_CHAR_HIGH;
                    *(dataPtr->data)   = *(linePtr->data+dataPos);
                    *(dataPtr->data+1) = *(linePtr->data+dataPos+1);
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

        if( dataPos == literalMaxSize )
        {
            dataPtr->type = END_CHAR;
        }
        else
        {
            nop();
        }

        dataPtr->dataPos = dataPos;

        if( dataPos >= literalMaxSize )
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
            default:
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
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (selectCaretPos <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (dataPos < linePtr->caretDataPos) )
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
                    else if( (ioWndBuffLineNowPtr->lineNum == linePtr->lineNum) && (linePtr->caretDataPos <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( (ioWndBuffLineSelectPtr->lineNum == linePtr->lineNum) && (dataPos < selectCaretPos) )
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
                        if( linePtr->caretDataPos < selectCaretPos )
                        {
                            if( (linePtr->caretDataPos <= dataPos) && (dataPos < selectCaretPos) )
                            {
                                dataPtr->bSelect = TRUE;
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else if( linePtr->caretDataPos > selectCaretPos )
                        {
                            if( (selectCaretPos <= dataPos) && (dataPos < linePtr->caretDataPos) )
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

        if( dataPtr->type == DOUBLE_CHAR_LOW )
        {
            dataPtr->caretPos = dataPtr->dataPos + 1;
        }
        else if( (dataPtr->type == TAB_CHAR) && (dataPtr->offset != 0)  )
        {
            dataPtr->caretPos = dataPtr->dataPos + 1;
        }
        else
        {
            dataPtr->caretPos = dataPtr->dataPos;
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

        for( nowPtr=dataPtr; (nowPtr != NULL); nowPtr=(S_BUFF_LINE_DATA *)nowPtr->header.nextPtr,i++ )
        {
            nowPtr->lineNum = i;
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �s�f�[�^��ǉ�����
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER *dataPtr �Ȃ��f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
static void
addLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( *topPtr == NULL )
    {
        dataPtr->lineNum = 0;
    }
    else
    {
        if( *endPtr != NULL )
        {
            dataPtr->lineNum = (*endPtr)->lineNum+1;
        }
        else
        {
            nop(); /* �ُ� */
        }
    }

    AddLinkedList( topPtr, endPtr, dataPtr );
}

/********************************************************************************
 * ���e  : �s�f�[�^���폜����
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_BUFF_LINE_DATA *dataPtr �폜����f�[�^
 * �߂�l: S_BUFF_LINE_DATA *        �폜�������̃f�[�^
 ***************************************/
static void
removeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr )
{
    S_BUFF_LINE_DATA *removeNextPtr;

    removeNextPtr = RemoveLinkedList( topPtr,endPtr,dataPtr );
    if( removeNextPtr != NULL )
    {
        if( removeNextPtr->header.prevPtr != NULL )
        {
            updateLineNum( (S_BUFF_LINE_DATA *)removeNextPtr->header.prevPtr );
        }
        else
        {
            removeNextPtr->lineNum = 0;
            updateLineNum( removeNextPtr );
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �s�f�[�^�̌���
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *nowPtr  �}���ʒu
 * ����  : S_BUFF_LINE_DATA **mergeTopPtr
 * ����  : S_BUFF_LINE_DATA **mergeEndPtr
 * �߂�l: void
 ***************************************/
void
mergeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **mergeTopPtr, S_BUFF_LINE_DATA **mergeEndPtr )
{
    if( (nowPtr != NULL) && (mergeTopPtr != NULL) )
    {
        if( nowPtr->header.prevPtr != NULL )
        { /* �O�s������� */
            (*mergeTopPtr)->lineNum = ((S_BUFF_LINE_DATA *)(nowPtr->header.prevPtr))->lineNum+1;
        }
        else
        {
            (*mergeTopPtr)->lineNum = 0;
        }

        MergeLinkedList(topPtr,endPtr,nowPtr,mergeTopPtr,mergeEndPtr);
        updateLineNum( (*mergeTopPtr) );
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �s�f�[�^��u��������
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *nowPtr  �u�������Ώۂ̃f�[�^
 * ����  : S_BUFF_LINE_DATA *dataPtr �u��������f�[�^
 * �߂�l: �u���������f�[�^
 ***************************************/
static S_BUFF_LINE_DATA *
replaceLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->lineNum = nowPtr->lineNum;
        dataPtr = ReplaceLinkedList(topPtr,endPtr,nowPtr,dataPtr);
    }
    else
    {
        nop();
    }

    return dataPtr;
}

/********************************************************************************
 * ���e  : �s�f�[�^��}������
 * ����  : S_BUFF_LINE_DATA **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *nowPtr  �}���ʒu
 * ����  : S_BUFF_LINE_DATA *dataPtr �}������f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
static void
insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->lineNum = nowPtr->lineNum;

        InsertLinkedList(topPtr,endPtr,nowPtr,dataPtr);

        (nowPtr->lineNum)++;
        updateLineNum( nowPtr );
    }
    else
    {
        nop();
    }
}
