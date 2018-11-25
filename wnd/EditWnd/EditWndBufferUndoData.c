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
 * ����  : DWORD preLineNum
 * ����  : DWORD preCaretPos
 * ����  : DWORD postLineNum
 * ����  : DWORD postCaretPos
 * �߂�l: S_BUFF_UNDO_DATA *
 ***************************************/
S_BUFF_UNDO_DATA *
EditWndBufferUndoDataCreate( UNDO_TYPE undoType, TCHAR* dataPtr, DWORD length, DWORD preLineNum, DWORD preCaretPos, DWORD postLineNum, DWORD postCaretPos )
{
    S_BUFF_UNDO_DATA *newPtr = NULL;
    DWORD dataSize = (DWORD)sizeof(S_BUFF_UNDO_DATA);

    if( (undoType == UNDO_TYPE_REMOVE) || (undoType == UNDO_TYPE_SET) )
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
        TCHAR szBuf[128] = {0};
        INT i;

        memset( newPtr, 0, dataSize );
        newPtr->undoType = undoType;
        newPtr->size     = length;
        newPtr->prePos.lineNum   = preLineNum  ;
        newPtr->prePos.caretPos  = preCaretPos ;
        newPtr->postPos.lineNum  = postLineNum ;
        newPtr->postPos.caretPos = postCaretPos;

        if( (undoType == UNDO_TYPE_REMOVE) || (undoType == UNDO_TYPE_SET) )
        {
            newPtr->dataPtr = ((TCHAR *)newPtr)+sizeof(S_BUFF_UNDO_DATA);
            memcpy( newPtr->dataPtr, dataPtr, length );
        }
        else
        {
            nop();
        }

#if 0
        for( i=0; i<min(length,8); i++ )
        {
            sprintf(szBuf+i*3,"%02X ",*(dataPtr+i));
        }
        DebugWndCtrlPrintf( DEBUG_WND_CTRL_ID_03, "type=%d,size=%d,pre=(%d,%d),post=(%d,%d),data=%s",newPtr->undoType,newPtr->size,preLineNum,preCaretPos,postLineNum,postCaretPos,szBuf);
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

