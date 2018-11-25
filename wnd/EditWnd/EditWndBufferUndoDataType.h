#ifndef EDITWND_BUFFER_UNDO_DATA_TYPE_H

typedef enum
{
    UNDO_TYPE_NONE  ,
    UNDO_TYPE_SET   ,
    UNDO_TYPE_REMOVE,
    UNDO_TYPE_JOIN  ,
} UNDO_TYPE;

typedef struct S_BUFF_UNDO_POSITION
{
    DWORD lineNum ;
    DWORD caretPos;
} S_BUFF_UNDO_POSITION;

typedef struct tagS_BUFF_UNDO_DATA
{
    S_LIST_HEADER header         ;
    UNDO_TYPE     undoType       ;
    TCHAR         *dataPtr       ;
    DWORD         size           ;
    S_BUFF_UNDO_POSITION prePos  ;
    S_BUFF_UNDO_POSITION postPos ;
} S_BUFF_UNDO_DATA;

/********************************************************************************
 * ���e  : �A���h�D�f�[�^��ǉ�����
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �擪�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �ŏI�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA *dataPtr �Ȃ��f�[�^
 * �߂�l: �Ȃ�
 ***************************************/
void EditWndBufferUndoDataAddLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr );

/********************************************************************************
 * ���e  : �A���h�D�f�[�^���폜����
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �擪�f�[�^���Ȃ��ł���|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA **endPtrPtr �ŏI�f�[�^���Ȃ��ł���|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA *dataPtr    �폜����f�[�^
 * �߂�l: S_BUFF_UNDO_DATA *           �폜�������̃f�[�^
 ***************************************/
void EditWndBufferUndoDataRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr, S_BUFF_UNDO_DATA *dataPtr );

/********************************************************************************
 * ���e  : �A���h�D�f�[�^�̃N���A
 * ����  : S_BUFF_UNDO_DATA **topPtrPtr �擪�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * ����  : S_BUFF_UNDO_DATA **endPtrPtr �ŏI�f�[�^���Ȃ��|�C���^(�̃|�C���^)
 * �߂�l: �Ȃ�
 ***************************************/
void EditWndBufferUndoDataAllRemoveLinkedList( S_BUFF_UNDO_DATA **topPtrPtr, S_BUFF_UNDO_DATA **endPtrPtr );

/********************************************************************************
 * ���e  : �A���h�D�f�[�^�̐���
 * ����  : UNDO_TYPE undoType
 * ����  : TCHAR* dataPtr
 * ����  : DWORD length
 * ����  : DWORD lineNum
 * ����  : DWORD caretPos
 * �߂�l: S_BUFF_UNDO_DATA *
 ***************************************/
S_BUFF_UNDO_DATA *EditWndBufferUndoDataCreate( UNDO_TYPE undoType, TCHAR* dataPtr, DWORD length, DWORD lineNum, DWORD caretPos );

/********************************************************************************
 * ���e  : �A���h�D�f�[�^�̉��
 * ����  : S_BUFF_UNDO_DATA *
 * �߂�l: �Ȃ�
 ***************************************/
void EditWndBufferUndoDataDestroy( S_BUFF_UNDO_DATA *dataPtr );

#define EDITWND_BUFFER_UNDO_DATA_TYPE_H
#endif /* EDITWND_BUFFER_UNDO_DATA_TYPE_H */
