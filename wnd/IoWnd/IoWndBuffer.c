/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWndBuffer.h"

typedef struct tagS_LIST
{
    struct tagS_LIST *prevPtr;
    struct tagS_LIST *nextPtr;
} S_LIST_HEADER;

typedef struct tag_buffer_line_data
{
    S_LIST_HEADER header         ;
    DWORD         lineNum        ; /* Y�ʒu            */
    DWORD         caretDataPos   ; /* X�ʒu            */
    DWORD         dataSize       ; /* �f�[�^�T�C�Y     */
    INT           newLineCodeSize; /* ���s�R�[�h�T�C�Y */
    TCHAR         data[]         ;
} S_BUFF_LINE_DATA;

typedef struct tagS_IOWND_BUFF_LOCAL
{
    struct
    {
        S_BUFF_LINE_DATA *topPtr       ; /* �擪�̃f�[�^                           */
        S_BUFF_LINE_DATA *endPtr       ; /* �Ō�̃f�[�^                           */
        S_BUFF_LINE_DATA *nowPtr       ; /* �L�����b�g������f�[�^                 */
        S_BUFF_LINE_DATA *selectPtr    ; /* �I��͈͂̐擪�̃f�[�^                 */
        DWORD            selectCaretPos; /* �I��͈͂̐擪�̃f�[�^�̃L�����b�g�ʒu */
    } lineData;
    UINT  NewLineType;
    DWORD xCaret     ;
    DWORD yCaret     ;
    INT   tabSize    ;
} S_IOWND_BUFF_LOCAL;
typedef S_IOWND_BUFF_LOCAL * H_IOWND_BUFF_LOCAL;

static S_BUFF_LINE_DATA *createBuffLineData( DWORD size, INT newLineCodeSize, TCHAR *dataPtr, DWORD lineNum, DWORD caretPos );
static void destroyBuffLineData( S_BUFF_LINE_DATA *dataPtr );
static S_BUFF_LINE_DATA *getBuffLine( H_IOWND_BUFF_LOCAL h, TCHAR *dataPtr, DWORD maxLength );
static S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
static void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1Ptr, S_BUFF_LINE_DATA **new2Ptr );
static S_BUFF_LINE_DATA *shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size );
static CHARSET_TYPE detectCharSet( S_BUFF_LINE_DATA *dataPtr, DWORD offset );
static DWORD getCaretDispXpos( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dataPos );
static BOOL getDispCharData( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr );
static void updateLineNum( S_BUFF_LINE_DATA *dataPtr );

static void addLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void removeLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *dataPtr );
static void insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **insertTopPtr, S_BUFF_LINE_DATA **insertEndPtr );
static S_BUFF_LINE_DATA *replaceLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
static void clearBuffLineData( H_IOWND_BUFF_LOCAL h );

static void addLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr );
static S_LIST_HEADER *replaceLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr );
static S_LIST_HEADER *removeLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr );
static void insertLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER **insertTopPtr, S_LIST_HEADER **insertEndPtr );

/* �����ϐ���` */

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃I�u�W�F�N�g����
 * ����  : �Ȃ�
 * �߂�l: H_IOWND_BUFF
 ***************************************/
H_IOWND_BUFF
IoWndBuffCreate( void )
{
    H_IOWND_BUFF_LOCAL h;

    h = (H_IOWND_BUFF_LOCAL)malloc( sizeof(S_IOWND_BUFF_LOCAL) );
    if( h != NULL )
    {
        h->lineData.topPtr         = NULL;
        h->lineData.endPtr         = NULL;
        h->lineData.nowPtr         = NULL;
        h->lineData.selectPtr      = NULL;
        h->lineData.selectCaretPos = 0;
        h->NewLineType = IOWND_BUFF_NEWLINE_CRLF;
        h->xCaret      = 0;
        h->yCaret      = 0;
        h->tabSize     = 8;
    }
    else
    {
        nop();
    }

    return (H_IOWND_BUFF)h;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃I�u�W�F�N�g�j��
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffDestroy( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h != NULL) )
    {
        free( h );
    }
    else
    {
        nop();
    }

    return TRUE;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̏�����
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffInit( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;
    S_BUFF_LINE_DATA *lineDataPtr;

    clearBuffLineData(h);

    /* ��f�[�^��ǉ� */
    lineDataPtr = createBuffLineData( 0, 0, NULL, 0, 0 );
    if( lineDataPtr != NULL )
    {
        addLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),lineDataPtr );
        (h->lineData.nowPtr) = lineDataPtr;
    }
    else
    {
        nop();
    }

    (h->lineData.selectPtr) = NULL;
    (h->lineData.selectCaretPos) = 0;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̏I��
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffEnd( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    clearBuffLineData(h);
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃f�[�^�Z�b�g
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit  (TRUE:�����f�[�^���N���A,FALSE:�N���A���Ȃ�)
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffDataSet( H_IOWND_BUFF hIoWndBuff, TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    S_BUFF_LINE_DATA *lineDataPtr;
    S_BUFF_LINE_DATA *tempTopPtr = NULL;
    S_BUFF_LINE_DATA *tempEndPtr = NULL;
    S_BUFF_LINE_DATA *newPtr,*targetPtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    DWORD lineLengthSum = 0;
    DWORD caretPos = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( bInit )
    {
        IoWndBuffInit(hIoWndBuff);
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
            lineDataPtr = (S_BUFF_LINE_DATA *)getBuffLine( h, dataPtr+lineLengthSum, length - lineLengthSum );

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

        if( (h->lineData.nowPtr)->caretDataPos == 0 )
        { /* �s�̐擪�ɑ}�� */
            insertLineData(&(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),&tempTopPtr,&tempEndPtr);

            if( tempEndPtr->newLineCodeSize == 0 )
            { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                newPtr = joinData(tempEndPtr,(h->lineData.nowPtr)); /* �ǉ��f�[�^�̍ŏI�s�ƌ��݂̍s�ƌ��������V���ȃf�[�^���� */
                if( newPtr != NULL )
                {
                    targetPtr = (h->lineData.nowPtr);
                    (h->lineData.nowPtr) = replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr ); /* ���݂̍s��u������ */
                    destroyBuffLineData( targetPtr );                                                       /* ���݂̍s�̃f�[�^�����                   */

                    caretPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;           /* �L�����b�g�ʒu�͒ǉ��f�[�^�̍ŏI�s�̍Ō� */
                    removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempEndPtr); /* �ŏI�s���͕̂s�v�Ȃ̂ŘA�����X�g����O�� */
                    destroyBuffLineData( tempEndPtr );

                    (h->lineData.nowPtr) = newPtr;                                            /* �����������̂����ݍs�Ƃ��� */
                    (h->lineData.nowPtr)->caretDataPos = caretPos;                                /* �L�����b�g�ʒu�͌����ʒu     */
                }
                else
                {
                    nop();
                }
            }
            else
            { /* �}���f�[�^�̍Ō�ɉ��s�������� */
                (h->lineData.nowPtr)->caretDataPos = 0;
            }
        }
        else
        { /* �s�̓r���ɑ}�� */
            /* ���݂̍s���L�����b�g�ʒu�ŕ���(��������) */
            divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr );
            if( dividePrePtr != NULL )
            {
                if( dividePostPtr != NULL )
                {
                    replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),dividePostPtr ); /* �܂� �Â��f�[�^�����s�ȍ~�̃f�[�^�ɒu������ */
                    insertLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePostPtr,&dividePrePtr,&dividePrePtr ); /* ���̑O�ɉ��s�O�̃f�[�^��}�� */

                    destroyBuffLineData( (h->lineData.nowPtr) );
                    (h->lineData.nowPtr) = dividePostPtr;
                    (h->lineData.nowPtr)->caretDataPos = 0;
                    /* ���݂̍s���L�����b�g�ʒu�ŕ���(�����܂�) */

                    if( tempTopPtr == tempEndPtr )
                    { /* 1�s�}���̏ꍇ */
                        insertLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),&tempEndPtr,&tempEndPtr );

                        /* ���������O���f�[�^�ƁA�}���f�[�^�̐擪������(��������) */
                        newPtr = joinData( dividePrePtr, tempTopPtr );
                        if( newPtr != NULL )
                        {
                            targetPtr = tempTopPtr;
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempTopPtr,newPtr );
                            destroyBuffLineData( targetPtr );
                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePrePtr);
                            destroyBuffLineData( dividePrePtr );
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
                                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePostPtr,newPtr );
                                destroyBuffLineData( dividePostPtr );
                                (h->lineData.nowPtr) = newPtr;
                                removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempEndPtr);
                                destroyBuffLineData( tempEndPtr );
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
                        insertLineData(&(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),&tempTopPtr,&tempEndPtr);

                        if( tempEndPtr->newLineCodeSize == 0 )
                        { /* �}���f�[�^�̍Ō�ɉ��s���Ȃ���� */
                            newPtr = joinData(tempEndPtr,dividePostPtr); /* �ǉ��f�[�^�̍ŏI�s�Ɖ��s�ȍ~�̍s�ƌ��� */
                            if( newPtr != NULL )
                            {
                                newPtr->caretDataPos = tempEndPtr->dataSize - tempEndPtr->newLineCodeSize;
                                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePostPtr,newPtr );
                                destroyBuffLineData( dividePostPtr );
                                (h->lineData.nowPtr) = newPtr;
                                removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempEndPtr);
                                destroyBuffLineData( tempEndPtr );
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
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),tempTopPtr,newPtr );
                            destroyBuffLineData( targetPtr );
                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),dividePrePtr);
                            destroyBuffLineData( dividePrePtr );
                        }
                        else
                        {
                            nop();
                        }
                    }
                }
                else
                {
                    destroyBuffLineData( dividePrePtr );
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
        if( (h->lineData.topPtr) != NULL )
        {
            (h->lineData.nowPtr) = (h->lineData.topPtr);
            (h->lineData.nowPtr)->caretDataPos = 0;
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
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffDataGet( H_IOWND_BUFF hIoWndBuff, TCHAR *dataPtr, DWORD dataSize, IOWND_BUFF_REGION region )
{
    BOOL rtn = FALSE;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;
    DWORD size;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( region == BUFF_ALL )
    {
        if( (h->lineData.topPtr) == NULL )
        {
            nop();
        }
        else
        {
            if( dataPtr != NULL )
            {
                for( nowPtr=(h->lineData.topPtr); nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
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
            if( (h->lineData.selectPtr) != NULL )
            { /* �I���J�n�ʒu�L�� */
                if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
                { /* �������ɑI�� */
                    for( nowPtr = (h->lineData.selectPtr),prevPtr = NULL; prevPtr != (h->lineData.nowPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == (h->lineData.selectPtr) )
                        {
                            size = (nowPtr->dataSize - (h->lineData.selectCaretPos));
                            memcpy( dataPtr, (nowPtr->data + (h->lineData.selectCaretPos)),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == (h->lineData.nowPtr) )
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
                else if( ((h->lineData.selectPtr)->lineNum) > ((h->lineData.nowPtr)->lineNum) )
                { /* �������ɑI�� */
                    for( nowPtr = (h->lineData.nowPtr),prevPtr = NULL; prevPtr != (h->lineData.selectPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                    {
                        if( nowPtr == (h->lineData.nowPtr) )
                        {
                            size = (nowPtr->dataSize - nowPtr->caretDataPos);
                            memcpy( dataPtr, (nowPtr->data + nowPtr->caretDataPos),size );
                            dataPtr += size;
                        }
                        else if( nowPtr == (h->lineData.selectPtr) )
                        {
                            size = (h->lineData.selectCaretPos);
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
                    if( (h->lineData.nowPtr) == (h->lineData.selectPtr) )
                    {
                        if( (h->lineData.nowPtr)->caretDataPos < (h->lineData.selectCaretPos) )
                        {
                            size = ((h->lineData.selectCaretPos) - (h->lineData.nowPtr)->caretDataPos);
                            memcpy( dataPtr, ((h->lineData.nowPtr)->data + (h->lineData.nowPtr)->caretDataPos),size );
                            dataPtr += size;
                        }
                        else if( (h->lineData.nowPtr)->caretDataPos > (h->lineData.selectCaretPos) )
                        {
                            size = ((h->lineData.nowPtr)->caretDataPos - (h->lineData.selectCaretPos));
                            memcpy( dataPtr, ((h->lineData.nowPtr)->data + (h->lineData.selectCaretPos)),size );
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
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : IOWND_BUFF_REGION region
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndBuffGetDataSize( H_IOWND_BUFF hIoWndBuff, IOWND_BUFF_REGION region )
{
    DWORD dataSize = 0;
    S_BUFF_LINE_DATA *nowPtr,*prevPtr;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( region == BUFF_ALL )
    {
        for( nowPtr = (h->lineData.topPtr); nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
        {
            dataSize += nowPtr->dataSize;
        }
    }
    else
    {
        if( (h->lineData.selectPtr) != NULL )
        { /* �I���J�n�ʒu�L�� */
            if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
            { /* �������ɑI�� */
                for( nowPtr = (h->lineData.selectPtr),prevPtr = NULL; prevPtr != (h->lineData.nowPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == (h->lineData.selectPtr) )
                    {
                        dataSize += (nowPtr->dataSize - (h->lineData.selectCaretPos));
                    }
                    else if( nowPtr == (h->lineData.nowPtr) )
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
            else if( ((h->lineData.selectPtr)->lineNum) > ((h->lineData.nowPtr)->lineNum) )
            { /* �������ɑI�� */
                for( nowPtr = (h->lineData.nowPtr),prevPtr = NULL; prevPtr != (h->lineData.selectPtr); nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
                {
                    if( nowPtr == (h->lineData.nowPtr) )
                    {
                        dataSize += (nowPtr->dataSize - nowPtr->caretDataPos);
                    }
                    else if( nowPtr == (h->lineData.selectPtr) )
                    {
                        dataSize += (h->lineData.selectCaretPos);
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
                if( (h->lineData.nowPtr) == (h->lineData.selectPtr) )
                {
                    if( (h->lineData.nowPtr)->caretDataPos < (h->lineData.selectCaretPos) )
                    {
                        dataSize += ((h->lineData.selectCaretPos) - (h->lineData.nowPtr)->caretDataPos);
                    }
                    else if( (h->lineData.nowPtr)->caretDataPos > (h->lineData.selectCaretPos) )
                    {
                        dataSize += ((h->lineData.nowPtr)->caretDataPos - (h->lineData.selectCaretPos));
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
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndBuffGetLineMaxSize( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    return (h->lineData.endPtr)->lineNum;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̍ő啶���T�C�Y�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndBuffGetColumnMaxSize( H_IOWND_BUFF hIoWndBuff)
{
    DWORD columnMaxSize = 0;
    S_BUFF_LINE_DATA *nowPtr;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    for( nowPtr = (h->lineData.topPtr); nowPtr != NULL; nowPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr )
    {
        columnMaxSize = max( columnMaxSize, (nowPtr->dataSize - nowPtr->newLineCodeSize) );
    }

    return columnMaxSize;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndBuffGetCaretXpos( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    return getCaretDispXpos(h,(h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos);
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndBuffGetCaretYpos( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    return (h->lineData.nowPtr)->lineNum;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�g�ʒu�Z�b�g
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : DWORD xPos
 * ����  : DWORD lineNum
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSetCaretPos( H_IOWND_BUFF hIoWndBuff, DWORD xPos, DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr = NULL;
    S_BUFF_DISP_DATA dispData;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    nowPtr = (S_BUFF_LINE_DATA *)IoWndBuffGetLinePtr( hIoWndBuff, lineNum );

    if( nowPtr != NULL )
    {
        (h->lineData.nowPtr) = nowPtr;

        getDispCharData( h,(h->lineData.nowPtr), xPos, &dispData );
        (h->lineData.nowPtr)->caretDataPos = dispData.caretPos;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffIncCaretXpos( H_IOWND_BUFF hIoWndBuff )
{
    int moveAmount = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.nowPtr) != NULL )
    {
        if( detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos) == DOUBLE_CHAR_HIGH )
        { /* ���̕����ňړ��ʂ𔻒f */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( ((h->lineData.nowPtr)->caretDataPos + moveAmount) > ((h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize) )
        { /* �L�����b�g�͂��łɍs�̍ŉE�[ */
            if( ((h->lineData.nowPtr)->header.nextPtr != NULL) )
            { /* ���s�L�� */
                (h->lineData.nowPtr) = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.nextPtr;
                (h->lineData.nowPtr)->caretDataPos = 0;
            }
            else
            { /* ���s���� */
                nop();
            }
        }
        else
        { /* �܂��E�ֈړ��\ */
            (h->lineData.nowPtr)->caretDataPos += moveAmount;
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gX�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffDecCaretXpos( H_IOWND_BUFF hIoWndBuff )
{
    int moveAmount = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.nowPtr) != NULL )
    {
        if( ((h->lineData.nowPtr)->caretDataPos>0) &&
            (detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos-1) == DOUBLE_CHAR_LOW) )
        { /* �O�̕����ňړ��ʂ𔻒f */
            moveAmount = 2;
        }
        else
        {
            moveAmount = 1;
        }

        if( (h->lineData.nowPtr)->caretDataPos == 0 )
        { /* �L�����b�g�͂��łɍs�̍ō��[ */
            if( ((h->lineData.nowPtr)->header.prevPtr != NULL) )
            { /* �O�s�L�� */
                (h->lineData.nowPtr) = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.prevPtr;
                (h->lineData.nowPtr)->caretDataPos = (h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize;
            }
            else
            { /* �O�s���� */
                nop();
            }
        }
        else
        { /* �܂����ֈړ��\ */
            (h->lineData.nowPtr)->caretDataPos = max( (h->lineData.nowPtr)->caretDataPos - moveAmount, 0 );
        }
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffIncCaretYpos( H_IOWND_BUFF hIoWndBuff )
{
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *nextPtr;
    S_BUFF_DISP_DATA dispData;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    nextPtr = (S_BUFF_LINE_DATA *)((h->lineData.nowPtr)->header.nextPtr);

    if( ((h->lineData.nowPtr) != NULL) && (nextPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(h,(h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos);
        getDispCharData( h, nextPtr, preDispPos, &dispData );
        nextPtr->caretDataPos = dispData.caretPos;
        (h->lineData.nowPtr) = nextPtr;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃L�����b�gY�ʒu���Z
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffDecCaretYpos( H_IOWND_BUFF hIoWndBuff )
{
    DWORD preDispPos = 0;
    S_BUFF_LINE_DATA *prevPtr;
    S_BUFF_DISP_DATA dispData;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    prevPtr = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.prevPtr;

    if( ((h->lineData.nowPtr) != NULL) && (prevPtr != NULL) )
    {
        preDispPos = getCaretDispXpos(h,(h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos);
        getDispCharData( h, prevPtr, preDispPos, &dispData );
        prevPtr->caretDataPos = dispData.caretPos;
        (h->lineData.nowPtr) = prevPtr;
    }
    else
    {
        nop();
    }
}

/********************************************************************************
 * ���e  : �w��s�f�[�^�̃n���h���擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : DWORD lineNum
 * �߂�l: H_IOWND_BUFF_LINE
 ***************************************/
H_IOWND_BUFF_LINE
IoWndBuffGetLinePtr( H_IOWND_BUFF hIoWndBuff, DWORD lineNum )
{
    S_BUFF_LINE_DATA *nowPtr,*nextPtr,*rtnPtr = NULL;
    DWORD i;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.topPtr) == NULL )
    {
        nop();
    }
    else
    {
        for( i=0,nowPtr = nextPtr = (h->lineData.topPtr); (nowPtr != NULL) && (i<=lineNum); nowPtr=nextPtr,i++ )
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

    return (H_IOWND_BUFF_LINE)rtnPtr;
}

/********************************************************************************
 * ���e  : ���̍s�f�[�^�̃n���h���擾
 * ����  : H_IOWND_BUFF_LINE hLineData
 * �߂�l: H_IOWND_BUFF_LINE
 ***************************************/
H_IOWND_BUFF_LINE
IoWndBuffGetLineNextPtr( H_IOWND_BUFF_LINE hLineData )
{
    return (H_IOWND_BUFF_LINE)(((S_BUFF_LINE_DATA *)hLineData)->header.nextPtr);
}

/********************************************************************************
 * ���e  : �w��s�A�w���̃f�[�^���擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : H_IOWND_BUFF_LINE hLineData
 * ����  : DWORD             dispPos     �\���ʒu
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffGetDispData( H_IOWND_BUFF hIoWndBuff, H_IOWND_BUFF_LINE hLineData, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    getDispCharData( h, (S_BUFF_LINE_DATA *)hLineData, dispPos, dataPtr );

    return TRUE;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̉��s�R�[�h�Z�b�g
 * ����  : UINT newLineType
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSetNewLineCode( H_IOWND_BUFF hIoWndBuff, UINT newLineType )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    h->NewLineType = newLineType;
}

/********************************************************************************
 * ���e  : �f�[�^�폜
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : BOOL bBackSpace
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffRemoveData( H_IOWND_BUFF hIoWndBuff, BOOL bBackSpace )
{
    DWORD removeSize = 0,saveCaretPos;
    S_BUFF_LINE_DATA *newPtr = NULL,*prevPtr = NULL,*nextPtr = NULL,*nowPtr = NULL,*savePtr;
    S_BUFF_LINE_DATA *dividePrePtr,*dividePostPtr;
    BOOL bValid = FALSE;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.nowPtr) != NULL )
    {
        if( (h->lineData.selectPtr) != NULL )
        { /* �I���J�n�ʒu�L�� */
            if( ((h->lineData.selectPtr)->lineNum) == ((h->lineData.nowPtr)->lineNum) )
            { /* ����s���őI�� */
                if( (h->lineData.nowPtr) == (h->lineData.selectPtr) )
                {
                    if( (h->lineData.nowPtr)->caretDataPos < (h->lineData.selectCaretPos) )
                    {
                        removeSize += ((h->lineData.selectCaretPos) - (h->lineData.nowPtr)->caretDataPos);
                        saveCaretPos = (h->lineData.nowPtr)->caretDataPos;
                        (h->lineData.nowPtr)->caretDataPos = (h->lineData.selectCaretPos);
                        (h->lineData.selectCaretPos) = saveCaretPos;
                    }
                    else if( (h->lineData.nowPtr)->caretDataPos > (h->lineData.selectCaretPos) )
                    {
                        removeSize += ((h->lineData.nowPtr)->caretDataPos - (h->lineData.selectCaretPos));
                    }
                    else
                    {
                        nop();
                    }

                    divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
                    newPtr = shortenData( dividePrePtr, removeSize );                 /* ������̑O���f�[�^�̖������珊��ʍ폜 */
                    destroyBuffLineData( dividePrePtr );                                 /* �O���f�[�^�� */
                    dividePrePtr = newPtr;                                            /* ��������     */

                    newPtr = joinData( dividePrePtr, dividePostPtr );                 /* �Č��� */
                    newPtr->lineNum = (h->lineData.nowPtr)->lineNum;                   /* �s�ԍ��͓��� */
                    newPtr->caretDataPos = (h->lineData.nowPtr)->caretDataPos - removeSize;    /* �L�����b�g�ʒu�͍폜�ʕ��O���ɂȂ� */
                    destroyBuffLineData( dividePrePtr );                                 /* ��ƃf�[�^���폜 */
                    destroyBuffLineData( dividePostPtr );                                /* ��ƃf�[�^���폜 */

                    /* �Â��s�f�[�^�ƒu�������� */
                    replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr );
                    destroyBuffLineData( (h->lineData.nowPtr) );
                    (h->lineData.nowPtr) = newPtr;
                }
                else
                {
                    nop();
                }
            }
            else
            {
                if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
                { /* �������ɑI�� */
                    nop();
                }
                else
                { /* �������ɑI�� */
                    savePtr = (h->lineData.selectPtr);
                    saveCaretPos = (h->lineData.selectCaretPos);
                    (h->lineData.selectPtr) = (h->lineData.nowPtr);
                    (h->lineData.selectCaretPos) = (h->lineData.nowPtr)->caretDataPos;
                    (h->lineData.nowPtr) = savePtr;
                    (h->lineData.nowPtr)->caretDataPos = saveCaretPos;
                }

                /* �擪����Ō�܂őI�����ꂽ�s���폜 */
                for( nowPtr = (S_BUFF_LINE_DATA *)(h->lineData.selectPtr)->header.nextPtr; nowPtr != (h->lineData.nowPtr); nowPtr = nextPtr )
                {
                    nextPtr = (S_BUFF_LINE_DATA *)nowPtr->header.nextPtr;
                    removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),nowPtr);
                    destroyBuffLineData( nowPtr );
                }
                /* �擪�s�̂����A�I���ʒu����O���c�� */
                if( (h->lineData.selectCaretPos) == 0 )
                { /* �I���ʒu���擪��������A1�s�ہX�폜 */
                    removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.selectPtr));
                }
                else
                {
                    (h->lineData.selectPtr)->caretDataPos = (h->lineData.selectCaretPos);
                    divideData( (h->lineData.selectPtr), &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
                    replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.selectPtr),dividePrePtr );
                    destroyBuffLineData( (h->lineData.selectPtr) );
                }

                /* �ŏI�s�̂����A�I���ʒu�������c�� */
                divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),dividePostPtr );
                destroyBuffLineData( (h->lineData.nowPtr) );
                (h->lineData.nowPtr) = dividePostPtr;
                (h->lineData.nowPtr)->caretDataPos = 0;
                updateLineNum( (h->lineData.nowPtr) );
            }

            (h->lineData.selectPtr) = NULL;
            (h->lineData.selectCaretPos) = 0;
        }
        else
        { /* �I�𖳂� */
            if( bBackSpace )
            { /* BS�L�[ */
                if( (h->lineData.nowPtr)->caretDataPos > 0  )
                {
                    bValid = TRUE;

                    if( detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos-1) == DOUBLE_CHAR_LOW )
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
                    if( (h->lineData.nowPtr)->header.prevPtr != NULL )
                    {
                        prevPtr = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.prevPtr;
                        newPtr = joinData( prevPtr,(h->lineData.nowPtr) ); /* �O�s�Ɩ{�s�����������V�����s�f�[�^�𐶐� */

                        if( newPtr != NULL )
                        {
                            /* �s�ԍ��A�L�����b�g�ʒu��O�s�f�[�^�ɂ���Č��܂� */
                            newPtr->lineNum  = prevPtr->lineNum;
                            newPtr->caretDataPos = prevPtr->dataSize - prevPtr->newLineCodeSize;

                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr) ); /* �{�s�͍폜 */
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),prevPtr,newPtr );    /* �O�s�͐V�����s�f�[�^�ɒu�������� */

                            destroyBuffLineData( (h->lineData.nowPtr) );
                            destroyBuffLineData( prevPtr );
                            (h->lineData.nowPtr) = newPtr;

                            updateLineNum( (h->lineData.nowPtr) );
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
                if( (h->lineData.nowPtr)->caretDataPos != ((h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize) )
                {
                    bValid = TRUE;

                    if( detectCharSet((h->lineData.nowPtr),(h->lineData.nowPtr)->caretDataPos) == DOUBLE_CHAR_HIGH )
                    { /* �L�����b�g�ʒu�̕����ō폜�ʂ𔻒f */
                        removeSize = 2;
                    }
                    else
                    {
                        removeSize = 1;
                    }
                    (h->lineData.nowPtr)->caretDataPos += removeSize;
                }
                else
                { /* �L�����b�g���s�̍ŏI�ʒu�B�܂�A���s�Ƃ̌����B*/
                    if( (h->lineData.nowPtr)->header.nextPtr != NULL )
                    {
                        nextPtr = (S_BUFF_LINE_DATA *)(h->lineData.nowPtr)->header.nextPtr;
                        newPtr = joinData( (h->lineData.nowPtr),nextPtr ); /* �{�s�Ǝ��s�����������V�����s�f�[�^�𐶐� */

                        if( newPtr != NULL )
                        {
                            /* �s�ԍ��A�L�����b�g�ʒu�͖{�s�f�[�^�ɂ���Č��܂� */
                            newPtr->lineNum  = (h->lineData.nowPtr)->lineNum;
                            newPtr->caretDataPos = (h->lineData.nowPtr)->caretDataPos;

                            removeLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),nextPtr );         /* ���s�͍폜 */
                            replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr ); /* �{�s�͐V�����s�f�[�^�ɒu�������� */

                            destroyBuffLineData( (h->lineData.nowPtr) );
                            destroyBuffLineData( nextPtr );
                            (h->lineData.nowPtr) = newPtr;

                            updateLineNum( (h->lineData.nowPtr) );
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
                divideData( (h->lineData.nowPtr), &dividePrePtr, &dividePostPtr ); /* �L�����b�g�ʒu�ŕ��� */
                newPtr = shortenData( dividePrePtr, removeSize );                 /* ������̑O���f�[�^�̖������珊��ʍ폜 */
                destroyBuffLineData( dividePrePtr );                                 /* �O���f�[�^�� */
                dividePrePtr = newPtr;                                            /* ��������     */

                newPtr = joinData( dividePrePtr, dividePostPtr );                 /* �Č��� */
                newPtr->lineNum = (h->lineData.nowPtr)->lineNum;                   /* �s�ԍ��͓��� */
                newPtr->caretDataPos = (h->lineData.nowPtr)->caretDataPos - removeSize;    /* �L�����b�g�ʒu�͍폜�ʕ��O���ɂȂ� */
                destroyBuffLineData( dividePrePtr );                                 /* ��ƃf�[�^���폜 */
                destroyBuffLineData( dividePostPtr );                                /* ��ƃf�[�^���폜 */

                /* �Â��s�f�[�^�ƒu�������� */
                replaceLineData( &(h->lineData.topPtr),&(h->lineData.endPtr),(h->lineData.nowPtr),newPtr );
                destroyBuffLineData( (h->lineData.nowPtr) );
                (h->lineData.nowPtr) = newPtr;
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
 * ���e  : ���s�f�[�^�擾
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : PTSTR dataPtr
 * �߂�l: INT
 ***************************************/
INT
IoWndBuffGetNewLineData( H_IOWND_BUFF hIoWndBuff, PTSTR dataPtr )
{
    INT size = 0;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    switch( h->NewLineType )
    {
    case IOWND_BUFF_NEWLINE_CRLF:
        *dataPtr     = '\r';
        *(dataPtr+1) = '\n';
        size = 2;
        break;
    case IOWND_BUFF_NEWLINE_LF  :
        *dataPtr = '\n';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_CR  :
        *dataPtr = '\r';
        size = 1;
        break;
    case IOWND_BUFF_NEWLINE_NONE:
    default:
        break;
    }

    return size;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̃^�u�T�C�Y�Z�b�g
 * ����  : H_IOWND_BUFF hIoWndBuff
 * ����  : INT tabSize
 * �߂�l: BOOL (TRUE:�ύX���ꂽ)
 ***************************************/
BOOL
IoWndBuffSetTabSize( H_IOWND_BUFF hIoWndBuff, INT tabSize )
{
    BOOL bRtn = FALSE;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( tabSize != h->tabSize )
    {
        h->tabSize = tabSize;
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
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSelectOn( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.selectPtr) )
    { /* ���ɑI���ς݂̏ꍇ */
        nop(); /* �ύX���Ȃ� */
    }
    else
    {
        (h->lineData.selectPtr) = (h->lineData.nowPtr);
        (h->lineData.selectCaretPos) = (h->lineData.nowPtr)->caretDataPos;
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�͈̔͑I��OFF
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndBuffSelectOff( H_IOWND_BUFF hIoWndBuff )
{
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    (h->lineData.selectPtr) = NULL;
    (h->lineData.selectCaretPos) = 0;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�o�b�t�@�̑S�͈͑I��
 * ����  : H_IOWND_BUFF hIoWndBuff
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndBuffSelectAll( H_IOWND_BUFF hIoWndBuff )
{
    BOOL bRtn = FALSE;
    H_IOWND_BUFF_LOCAL h = (H_IOWND_BUFF_LOCAL)hIoWndBuff;

    if( (h->lineData.topPtr) != NULL )
    {
        if( ((h->lineData.selectPtr) == (h->lineData.topPtr)) &&
            ((h->lineData.selectCaretPos) == 0) &&
            ((h->lineData.nowPtr) == (h->lineData.endPtr)) &&
            ((h->lineData.nowPtr)->caretDataPos == ((h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize) ) )
        {
            nop();
        }
        else
        {
            (h->lineData.selectPtr) = (h->lineData.topPtr);
            (h->lineData.selectCaretPos) = 0;
            (h->lineData.nowPtr) = (h->lineData.endPtr);
            (h->lineData.nowPtr)->caretDataPos = (h->lineData.nowPtr)->dataSize - (h->lineData.nowPtr)->newLineCodeSize;
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

    newPtr = (S_BUFF_LINE_DATA *)malloc( sizeof(S_BUFF_LINE_DATA) + (size * sizeof(TCHAR)) + 1 );
    if( newPtr != NULL )
    {
        memset( newPtr, sizeof(S_BUFF_LINE_DATA), 0 );
        newPtr->lineNum         = lineNum;
        newPtr->caretDataPos    = caretPos;
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
        newPtr->data[size] = '\0';
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
 * ����  : H_IOWND_BUFF_LOCAL h
 * ����  : TCHAR *dataPtr  �f�[�^�̐擪
 * ����  : DWORD maxLength �ő咷��
 * �߂�l: S_BUFF_LINE_DATA *
 ***************************************/
static S_BUFF_LINE_DATA *
getBuffLine( H_IOWND_BUFF_LOCAL h, TCHAR *dataPtr, DWORD maxLength )
{
    S_BUFF_LINE_DATA *lineDataPtr = NULL;
    DWORD i;
    INT   newLineCodeSize = 0;

    for( i=0; i<maxLength; i++ )
    {
        if( h->NewLineType == IOWND_BUFF_NEWLINE_CRLF )
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
        else if( h->NewLineType == IOWND_BUFF_NEWLINE_LF )
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
        else if( h->NewLineType == IOWND_BUFF_NEWLINE_CR )
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
 * ����  : H_IOWND_BUFF_LOCAL h
 * ����  : S_BUFF_LINE_DATA *linePtr   �s�f�[�^
 * ����  : DWORD             dataPos   ��ʒu(�f�[�^���)
 * �߂�l: DWORD
 ***************************************/
static DWORD
getCaretDispXpos( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dataPos )
{
    DWORD i,j=0;
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

                    tab_offset = h->tabSize - (j % h->tabSize);
                    j+=tab_offset;
                }
                else if( ( (BYTE)(*(linePtr->data+i)) <= (BYTE)0x80) ||
                         (((BYTE)0xA0 <= (BYTE)(*(linePtr->data+i))) && ((BYTE)(*(linePtr->data+i)) <= (BYTE)0xDF)) ||
                         ((BYTE)0xF0 <= (BYTE)(*(linePtr->data+i))) )
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
 * ����  : H_IOWND_BUFF_LOCAL h
 * ����  : S_BUFF_LINE_DATA *linePtr   �s�f�[�^
 * ����  : DWORD             dispPos   ��ʒu(�\�����)
 * ����  : S_BUFF_DISP_DATA *dataPtr
 * �߂�l: BOOL
 ***************************************/
static BOOL
getDispCharData( H_IOWND_BUFF_LOCAL h, S_BUFF_LINE_DATA *linePtr, DWORD dispPos, S_BUFF_DISP_DATA *dataPtr )
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

                    tab_offset = h->tabSize - (j % h->tabSize);

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
                         ((BYTE)0xF0 <= (BYTE)(*(linePtr->data+dataPos))) )
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

            if( (h->lineData.selectPtr) != NULL )
            { /* �I���J�n�ʒu�L�� */
                if( ((h->lineData.selectPtr)->lineNum) < ((h->lineData.nowPtr)->lineNum) )
                { /* �������ɑI�� */
                    if( ((h->lineData.selectPtr)->lineNum < linePtr->lineNum) && (linePtr->lineNum < (h->lineData.nowPtr)->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.selectPtr)->lineNum == linePtr->lineNum) && ((h->lineData.selectCaretPos) <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.nowPtr)->lineNum == linePtr->lineNum) && (dataPos < linePtr->caretDataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else
                    {
                        nop();
                    }
                }
                else if( ((h->lineData.selectPtr)->lineNum) > ((h->lineData.nowPtr)->lineNum) )
                { /* �������ɑI�� */
                    if( ((h->lineData.nowPtr)->lineNum < linePtr->lineNum) && (linePtr->lineNum < (h->lineData.selectPtr)->lineNum) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.nowPtr)->lineNum == linePtr->lineNum) && (linePtr->caretDataPos <= dataPos) )
                    {
                        dataPtr->bSelect = TRUE;
                    }
                    else if( ((h->lineData.selectPtr)->lineNum == linePtr->lineNum) && (dataPos < (h->lineData.selectCaretPos)) )
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
                    if( (h->lineData.nowPtr) == linePtr )
                    {
                        if( linePtr->caretDataPos < (h->lineData.selectCaretPos) )
                        {
                            if( (linePtr->caretDataPos <= dataPos) && (dataPos < (h->lineData.selectCaretPos)) )
                            {
                                dataPtr->bSelect = TRUE;
                            }
                            else
                            {
                                nop();
                            }
                        }
                        else if( linePtr->caretDataPos > (h->lineData.selectCaretPos) )
                        {
                            if( ((h->lineData.selectCaretPos) <= dataPos) && (dataPos < linePtr->caretDataPos) )
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

    addLinkedList( (S_LIST_HEADER **)topPtr, (S_LIST_HEADER **)endPtr, (S_LIST_HEADER *)dataPtr );
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

    removeNextPtr = (S_BUFF_LINE_DATA *)removeLinkedList( (S_LIST_HEADER **)topPtr,(S_LIST_HEADER **)endPtr,(S_LIST_HEADER *)dataPtr );
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
 * ���e  : �s�f�[�^�̑}��
 * ����  : S_BUFF_LINE_DATA **topPtr       �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA **topPtr       �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_BUFF_LINE_DATA *nowPtr        �}���ʒu
 * ����  : S_BUFF_LINE_DATA **insertTopPtr
 * ����  : S_BUFF_LINE_DATA **insertEndPtr
 * �߂�l: void
 ***************************************/
static void
insertLineData( S_BUFF_LINE_DATA **topPtr, S_BUFF_LINE_DATA **endPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **insertTopPtr, S_BUFF_LINE_DATA **insertEndPtr )
{
    if( (nowPtr != NULL) && (insertTopPtr != NULL) )
    {
        (*insertTopPtr)->lineNum = nowPtr->lineNum;

        insertLinkedList((S_LIST_HEADER **)topPtr,(S_LIST_HEADER **)endPtr,(S_LIST_HEADER *)nowPtr,(S_LIST_HEADER **)insertTopPtr,(S_LIST_HEADER **)insertEndPtr);
        updateLineNum( (*insertTopPtr) );
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
        dataPtr = (S_BUFF_LINE_DATA *)replaceLinkedList((S_LIST_HEADER **)topPtr,(S_LIST_HEADER **)endPtr,(S_LIST_HEADER *)nowPtr,(S_LIST_HEADER *)dataPtr);
    }
    else
    {
        nop();
    }

    return dataPtr;
}

/********************************************************************************
 * ���e  : �o�b�t�@�f�[�^�̃N���A
 * ����  : H_IOWND_BUFF_LOCAL h
 * �߂�l: �Ȃ�
 ***************************************/
static void
clearBuffLineData( H_IOWND_BUFF_LOCAL h )
{
    S_BUFF_LINE_DATA *lineDataPtr,*nextPtr;

    lineDataPtr = h->lineData.topPtr;
    while( lineDataPtr != NULL )
    {
        nextPtr = (S_BUFF_LINE_DATA *)removeLinkedList((S_LIST_HEADER **)&(h->lineData.topPtr),(S_LIST_HEADER **)&(h->lineData.endPtr),(S_LIST_HEADER *)lineDataPtr);
        destroyBuffLineData(lineDataPtr);
        lineDataPtr = nextPtr;
    }
}

/********************************************************************************
 * ���e  : �A�����X�g�Ƀf�[�^��ǉ�����
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER *dataPtr �Ȃ��f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
static void
addLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
{
    if( *topPtr == NULL )
    {
        dataPtr->prevPtr = NULL;
        dataPtr->nextPtr = NULL;
        *topPtr = dataPtr;
        *endPtr = dataPtr;
    }
    else
    {
        if( *endPtr != NULL )
        {
            dataPtr->prevPtr   = *endPtr;
            dataPtr->nextPtr   = NULL;
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
 * ���e  : �A�����X�g�̃f�[�^��u��������
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER *nowPtr  �u�������Ώۂ̃f�[�^
 * ����  : S_LIST_HEADER *dataPtr �u��������f�[�^
 * �߂�l: �u���������f�[�^
 ***************************************/
static S_LIST_HEADER *
replaceLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
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
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_LIST_HEADER **topPtr �ŏI�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_LIST_HEADER *dataPtr �폜����f�[�^
 * �߂�l: S_LIST_HEADER *        �폜�������̃f�[�^
 ***************************************/
static S_LIST_HEADER *
removeLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
{
    S_LIST_HEADER *nextPtr = NULL;

    if( (topPtr != NULL) && (endPtr != NULL) && (dataPtr != NULL) )
    {
        nextPtr = dataPtr->nextPtr; /* �߂�l */

        if( dataPtr->prevPtr != NULL )
        { /* �O�f�[�^�L�� */
            (dataPtr->prevPtr)->nextPtr = dataPtr->nextPtr; /* ���f�[�^���A�O�f�[�^�̎��f�[�^�Ƃ��� */
            if( (dataPtr->prevPtr)->nextPtr != NULL )
            { /* ���f�[�^���������ꍇ */
                ((dataPtr->prevPtr)->nextPtr)->prevPtr = dataPtr->prevPtr; /* ���f�[�^�̑O�f�[�^�́A�O�f�[�^ */
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
                (*topPtr)->prevPtr = NULL;
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
 * ���e  : �A�����X�g�ւ̑}��
 * ����  : S_LIST_HEADER **topPtr       �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER **topPtr       �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER *nowPtr        �}���ʒu
 * ����  : S_LIST_HEADER **insertTopPtr �}������A�����X�g�̐擪
 * ����  : S_LIST_HEADER **insertEndPtr �}������A�����X�g�̍Ō�
 * �߂�l: void
 ***************************************/
static void
insertLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER **insertTopPtr, S_LIST_HEADER **insertEndPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (insertTopPtr != NULL) && (insertEndPtr != NULL) )
    {
        (*insertTopPtr)->prevPtr = nowPtr->prevPtr;

        if( (*insertTopPtr)->prevPtr != NULL )
        {
            ((*insertTopPtr)->prevPtr)->nextPtr = (*insertTopPtr);
        }
        else
        {
            (*topPtr) = (*insertTopPtr);
        }

        (*insertEndPtr)->nextPtr = nowPtr;
        nowPtr->prevPtr = (*insertEndPtr);
    }
    else
    {
        nop();
    }
}
