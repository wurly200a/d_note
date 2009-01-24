#ifndef SOME_CTRL_H

typedef enum
{
    SOME_CTRL_FILENAME_STATIC,
    SOME_CTRL_FILENAME,
    SOME_CTRL_MAX
}SOME_CTRL_ID;

#define SOME_CTRL_ID_OFFSET 10
#define MY_EX_STYLE (WS_EX_OVERLAPPEDWINDOW)

BOOL SomeCtrlCreate(HWND hwnd);
HWND SomeCtrlGetHWND(SOME_CTRL_ID id);
void SomeCtrlEnable(SOME_CTRL_ID id);
void SomeCtrlDisable(SOME_CTRL_ID id);
BOOL SomeCtrlGetText( SOME_CTRL_ID id, PTSTR ptstrText );

#define SOME_CTRL_H
#endif /* SOME_CTRL_H */
