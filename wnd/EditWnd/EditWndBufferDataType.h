#ifndef EDITWND_BUFFER_DATA_TYPE_H

typedef struct tag_buffer_line_data
{
    S_LIST_HEADER header         ;
    DWORD         lineNum        ; /* Y位置            */
    DWORD         caretDataPos   ; /* X位置            */
    DWORD         dataSize       ; /* データサイズ     */
    INT           newLineCodeSize; /* 改行コードサイズ */
    TCHAR         data[]         ;
} S_BUFF_LINE_DATA;

S_BUFF_LINE_DATA *createBuffLineData( DWORD size, INT newLineCodeSize, TCHAR *dataPtr, DWORD lineNum, DWORD caretPos );
S_BUFF_LINE_DATA *joinData( S_BUFF_LINE_DATA *data1Ptr, S_BUFF_LINE_DATA *data2Ptr );
void divideData( S_BUFF_LINE_DATA *dataPtr, S_BUFF_LINE_DATA **new1PtrPtr, S_BUFF_LINE_DATA **new2PtrPtr );
S_BUFF_LINE_DATA *shortenData( S_BUFF_LINE_DATA *dataPtr, DWORD size );
void updateLineNum( S_BUFF_LINE_DATA *dataPtr );
void addLineData( S_BUFF_LINE_DATA **topPtrPtr, S_BUFF_LINE_DATA **endPtrPtr, S_BUFF_LINE_DATA *dataPtr );
void removeLineData( S_BUFF_LINE_DATA **topPtrPtr, S_BUFF_LINE_DATA **endPtrPtr, S_BUFF_LINE_DATA *dataPtr );
void insertLineData( S_BUFF_LINE_DATA **topPtrPtr, S_BUFF_LINE_DATA **endPtrPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA **insertTopPtrPtr, S_BUFF_LINE_DATA **insertEndPtrPtr );
S_BUFF_LINE_DATA *replaceLineData( S_BUFF_LINE_DATA **topPtrPtr, S_BUFF_LINE_DATA **endPtrPtr, S_BUFF_LINE_DATA *nowPtr, S_BUFF_LINE_DATA *dataPtr );
void clearBuffLineData( S_BUFF_LINE_DATA **topPtrPtr, S_BUFF_LINE_DATA **endPtrPtr );
void destroyBuffLineData( S_BUFF_LINE_DATA *dataPtr );

#define EDITWND_BUFFER_DATA_TYPE_H
#endif /* EDITWND_BUFFER_DATA_TYPE_H */
