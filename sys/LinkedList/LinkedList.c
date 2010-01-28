/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "LinkedList.h"

/********************************************************************************
 * ���e  : �A�����X�g�̃f�[�^���N���A
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��ł���|�C���^
 * ����  : S_LIST_HEADER **endPtr �ŏI�f�[�^���Ȃ��ł���|�C���^
 * �߂�l: �Ȃ�
 ***************************************/
void
ClearLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr )
{
    S_LIST_HEADER *nowPtr,*nextPtr;

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
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER *dataPtr �Ȃ��f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
void
AddLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
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
 * ���e  : �A�����X�g�Ƀf�[�^��}������
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER *nowPtr  �}���ʒu
 * ����  : S_LIST_HEADER *dataPtr �}������f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
void
InsertLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (dataPtr != NULL) )
    {
        dataPtr->prevPtr = nowPtr->prevPtr;
        dataPtr->nextPtr = nowPtr;

        if( dataPtr->prevPtr != NULL )
        { /* �O�f�[�^�L�� */
            (dataPtr->prevPtr)->nextPtr = dataPtr;
        }
        else
        { /* �O�f�[�^���� */
            (*topPtr) = dataPtr;
        }

        nowPtr->prevPtr = dataPtr;
    }
    else
    {
        nop();
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
S_LIST_HEADER *
ReplaceLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER *dataPtr )
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
S_LIST_HEADER *
RemoveLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *dataPtr )
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
 * ���e  : �A�����X�g�̌���
 * ����  : S_LIST_HEADER **topPtr �擪�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER **topPtr �ŏI�f�[�^���Ȃ��|�C���^
 * ����  : S_LIST_HEADER *nowPtr  �}���ʒu
 * ����  : S_LIST_HEADER **mergeTopPtr
 * ����  : S_LIST_HEADER **mergeEndPtr
 * �߂�l: void
 ***************************************/
void
MergeLinkedList( S_LIST_HEADER **topPtr, S_LIST_HEADER **endPtr, S_LIST_HEADER *nowPtr, S_LIST_HEADER **mergeTopPtr, S_LIST_HEADER **mergeEndPtr )
{
    if( (topPtr != NULL) && (endPtr != NULL) && (nowPtr != NULL) && (mergeTopPtr != NULL) && (mergeEndPtr != NULL) )
    {
        (*mergeTopPtr)->prevPtr = nowPtr->prevPtr;

        if( (*mergeTopPtr)->prevPtr != NULL )
        {
            ((*mergeTopPtr)->prevPtr)->nextPtr = (*mergeTopPtr);
        }
        else
        {
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
    }
    else
    {
        nop();
    }
}
