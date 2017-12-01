/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"

/* �ʃC���N���[�h�t�@�C�� */
#include "DebugWnd.h"
#include "LinkedList.h"
#include "EditWndBufferUndoDataType.h"

/* �O���֐���` */

/* �O���ϐ���` */

/* �����֐���` */

/********************************************************************************
 * ���e  : �A���h�D�f�[�^��ǉ�����
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �擪�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �ŏI�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA *dataPtr �Ȃ��f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
void
EditWndBufferUndoDataAddLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr )
{
    AddLinkedList( (S_LIST_HEADER **)topPtrPtr, (S_LIST_HEADER **)endPtrPtr, (S_LIST_HEADER *)dataPtr );
}

/********************************************************************************
 * ���e  : �A���h�D�f�[�^���폜����
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �擪�f�[�^���Ȃ��ł���|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA **endPtrPtr �ŏI�f�[�^���Ȃ��ł���|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA *dataPtr    �폜����f�[�^
 * �߂�l: S_BUFF_UNDO_DATA *           �폜�������̃f�[�^
 ***************************************/
void
EditWndBufferUndoDataRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr )
{
    S_BUFF_UNDO_DATA *removeNextPtr;

    removeNextPtr = (S_BUFF_UNDO_DATA *)RemoveLinkedList( (S_LIST_HEADER **)topPtrPtr,(S_LIST_HEADER **)endPtrPtr,(S_LIST_HEADER *)dataPtr );
}

/********************************************************************************
 * ���e  : �A���h�D�f�[�^�̃N���A
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �擪�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA **endPtrPtr �ŏI�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * �߂�l: �Ȃ�
 ***************************************/
void
EditWndBufferUndoDataAllRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr )
{
    S_BUFF_UNDO_DATA *lineDataPtr,*nextPtr;

    lineDataPtr = *topPtrPtr;
    while( lineDataPtr != NULL )
    {
        nextPtr = (S_BUFF_UNDO_DATA *)RemoveLinkedList((S_LIST_HEADER **)topPtrPtr,(S_LIST_HEADER **)endPtrPtr,(S_LIST_HEADER *)lineDataPtr);
        EditWndBufferUndoDataDestroy(lineDataPtr);
        lineDataPtr = nextPtr;
    }
}

/********************************************************************************
 * ���e  : �A���h�D�f�[�^�̐���
 * ����  : UNDO_TYPE undoType
 * ����  : TCHAR* dataPtr
 * ����  : DWORD length
 * ����  : DWORD lineNum
 * ����  : DWORD caretPos
 * �߂�l: S_BUFF_UNDO_DATA *
 ***************************************/
S_BUFF_UNDO_DATA *
EditWndBufferUndoDataCreate( UNDO_TYPE undoType, TCHAR* dataPtr, DWORD length, DWORD lineNum, DWORD caretPos )
{
    S_BUFF_UNDO_DATA *newPtr = NULL;
    DWORD dataSize = (DWORD)sizeof(S_BUFF_UNDO_DATA);

    if( undoType == UNDO_TYPE_REMOVE )
    {
        dataSize += length;
    }
    else
    {
        nop();
    }
    newPtr = (S_BUFF_UNDO_DATA *)malloc( dataSize );

    if( newPtr != NULL )
    {
        TCHAR cForLog = ' ';

        memset( newPtr, 0, dataSize );
        newPtr->undoType = undoType;
        newPtr->size     = length;
        newPtr->lineNum  = lineNum;
        newPtr->caretPos = caretPos;

        if( undoType == UNDO_TYPE_REMOVE )
        {
            newPtr->dataPtr = ((TCHAR *)newPtr)+sizeof(S_BUFF_UNDO_DATA);
            memcpy( newPtr->dataPtr, dataPtr, length );

            cForLog = *(newPtr->dataPtr);
        }
        else
        {
            nop();
        }
#if 0
        DebugWndPrintf("EditWndBufferUndoDataCreate,%d,%c...,%08lX,%08lX,%08lX\r\n",newPtr->undoType,cForLog,newPtr->size,newPtr->lineNum,newPtr->caretPos);
#endif
    }
    else
    {
        nop();
    }

    return newPtr;
}

/********************************************************************************
 * ���e  : �A���h�D�f�[�^�̉��
 * ����  : S_BUFF_UNDO_DATA *
 * �߂�l: �Ȃ�
 ***************************************/
void
EditWndBufferUndoDataDestroy( S_BUFF_UNDO_DATA *dataPtr )
{
    free( dataPtr );
}

