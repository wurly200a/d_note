#ifndef SOME_CTRL_H

typedef enum
{
    SOME_CTRL_FILENAME_STATIC,
    SOME_CTRL_FILENAME,
    SOME_CTRL_FILEOPEN_BUTTON,
    SOME_CTRL_MAX
}SOME_CTRL_ID;

#define SOME_CTRL_ID_OFFSET 10
#define MY_EX_STYLE (WS_EX_OVERLAPPEDWINDOW)

/********************************************************************************
 * $BFbMF(B  : $BEPO?$5$l$?A4$F$N%3%s%H%m!<%k$r@8@.$9$k(B
 * $B0z?t(B  : HWND hwnd $B?F%&%#%s%I%&$N%O%s%I%i(B
 * $BLa$jCM(B: BOOL
 ***************************************/
BOOL SomeCtrlCreate( HWND hwnd );

/********************************************************************************
 * $BFbMF(B  : $BEPO?$5$l$?A4$F$N%3%s%H%m!<%k$N%5%$%:$rD4@0$9$k(B
 * $B0z?t(B  : int cxClient
 * $B0z?t(B  : int cyClient
 * $BLa$jCM(B: BOOL
 ***************************************/
BOOL SomeCtrlSize( int cxClient,int cyClient );

/********************************************************************************
 * $BFbMF(B  : $B%3%s%H%m!<%k$N%&%#%s%I%&%O%s%I%i$r<hF@$9$k(B
 * $B0z?t(B  : SOME_CTRL_ID id
 * $BLa$jCM(B: HWND
 ***************************************/
HWND SomeCtrlGetHWND( SOME_CTRL_ID id );

/********************************************************************************
 * $BFbMF(B  : $B%3%s%H%m!<%k$rM-8z$K$9$k(B
 * $B0z?t(B  : SOME_CTRL_ID id
 * $BLa$jCM(B: $B$J$7(B
 ***************************************/
void SomeCtrlEnable( SOME_CTRL_ID id );

/********************************************************************************
 * $BFbMF(B  : $B%3%s%H%m!<%k$rL58z$K$9$k(B
 * $B0z?t(B  : SOME_CTRL_ID id
 * $BLa$jCM(B: $B$J$7(B
 ***************************************/
void SomeCtrlDisable( SOME_CTRL_ID id );

/********************************************************************************
 * $BFbMF(B  : $B%3%s%H%m!<%k$+$i%F%-%9%H$r<hF@$9$k(B
 * $B0z?t(B  : SOME_CTRL_ID id
 * $B0z?t(B  : PTSTR ptstrText
 * $BLa$jCM(B: BOOL
 ***************************************/
BOOL SomeCtrlGetText( SOME_CTRL_ID id, PTSTR ptstrText );

#define SOME_CTRL_H
#endif /* SOME_CTRL_H */
