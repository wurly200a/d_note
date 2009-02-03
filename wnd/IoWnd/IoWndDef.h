#ifndef IOWND_DEF_H

typedef enum
{
    IOWND_ON_CREATE       , /* WM_CREATE        */
    IOWND_ON_PAINT        , /* WM_PAINT         */
    IOWND_ON_SIZE         , /* WM_SIZE          */
    IOWND_ON_CLOSE        , /* WM_CLOSE         */
    IOWND_ON_DESTROY      , /* WM_DESTROY       */
    IOWND_ON_COMMAND      , /* WM_COMMAND       */
    IOWND_ON_KEYUP        , /* WM_KEYUP         */
    IOWND_ON_KEYDOWN      , /* WM_KEYDOWN       */
    IOWND_ON_CHAR         , /* WM_CHAR          */
    IOWND_ON_HSCROLL      , /* WM_HSCROLL       */
    IOWND_ON_VSCROLL      , /* WM_VSCROLL       */
    IOWND_ON_SETFOCUS     , /* WM_SETFOCUS      */
    IOWND_ON_KILLFOCUS    , /* WM_KILLFOCUS     */
    IOWND_ON_MOUSEACTIVATE, /* WM_MOUSEACTIVATE */
    IOWND_ON_MOUSEWHEEL   , /* WM_MOUSEWHEEL    */
    IOWND_ON_DEFAULT      , /* default          */
    IOWND_MAX
} IOWND_INDEX;

typedef struct
{
    int    cxChar;    /* �����T�C�Y(X)             */
    int    cyChar;    /* �����T�C�Y(Y)             */
    int    cxClient;  /* �N���C�A���g�̈�T�C�Y(X) */
    int    cyClient;  /* �N���C�A���g�̈�T�C�Y(Y) */
    int    xCaret;    /* �J�[�\���ʒu(X)           */
    int    yCaret;    /* �J�[�\���ʒu(Y)           */
    int    cxBuffer;  /* �\���\�ȕ�����(X)       */
    int    cyBuffer;  /* �\���\�ȕ�����(Y)       */
    DWORD  dwCharSet;
    TCHAR* bufferPtr; /* �\�����镶���̃o�b�t�@    */
    DWORD  bufferSize;/* �\�����镶���̃o�b�t�@�T�C�Y*/

    int    cxCaps;    /*  */
    int    iMaxWidth; /* �ő啶����(X)             */
    int    iAccumDelta;
    int    iDeltaPerLine;
} S_IOWND_DATA;

#define IOWND_DEF_H
#endif /* IOWND_DEF_H */
