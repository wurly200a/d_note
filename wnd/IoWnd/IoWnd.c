/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "mbctype.h"
#include "MenuId.h"
#include "IoWndDef.h"

/* �O���֐���` */
#include "WinMain.h"
#include "StsBar.h"
#include "LinkedList.h"
#include "IoWndBuffer.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWnd.h"
LRESULT CALLBACK IoWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static IOWND_INDEX ioWndConvertMSGtoINDEX( UINT message );
static LRESULT ioOnCreate             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnPaint              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSize               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnClose              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDestroy            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCommand            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyUp              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyDown            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnChar               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnHscroll            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnVscroll            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSetFocus           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKillFocus          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseActivate      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseWheel         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseMove          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnLbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnRbuttonDown        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnLbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnRbuttonUp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnImeStartComposition( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCut                ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCopy               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnPaste              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnClear              ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnUndo               ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSetSel             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDefault            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static void ioWndRemoveData( HWND hwnd, BOOL bBackSpace );
static void updateTextMetrics( HWND hwnd );
static void getAllScrollInfo( void );
static void setAllScrollInfo( void );
static void setScrollPos( int fnBar, DWORD nPos );
static void printCaretPos( void );

/* �����ϐ���` */
static HWND hWndIo; /* �E�B���h�E�̃n���h�� */
static S_IOWND_DATA ioWndData;

/* *INDENT-OFF* */
static LRESULT (*ioWndProcTbl[IOWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    ioOnCreate             , /* WM_CREATE              */
    ioOnPaint              , /* WM_PAINT               */
    ioOnSize               , /* WM_SIZE                */
    ioOnClose              , /* WM_CLOSE               */
    ioOnDestroy            , /* WM_DESTROY             */
    ioOnCommand            , /* WM_COMMAND             */
    ioOnKeyUp              , /* WM_KEYUP               */
    ioOnKeyDown            , /* WM_KEYDOWN             */
    ioOnChar               , /* WM_CHAR                */
    ioOnHscroll            , /* WM_HSCROLL             */
    ioOnVscroll            , /* WM_VSCROLL             */
    ioOnSetFocus           , /* WM_SETFOCUS            */
    ioOnKillFocus          , /* WM_KILLFOCUS           */
    ioOnMouseActivate      , /* WM_MOUSEACTIVATE       */
    ioOnMouseWheel         , /* WM_MOUSEWHEEL          */
    ioOnMouseMove          , /* WM_MOUSEMOVE           */
    ioOnLbuttonDown        , /* WM_LBUTTONDOWN         */
    ioOnMbuttonDown        , /* WM_MBUTTONDOWN         */
    ioOnRbuttonDown        , /* WM_RBUTTONDOWN         */
    ioOnLbuttonUp          , /* WM_LBUTTONUP           */
    ioOnMbuttonUp          , /* WM_MBUTTONUP           */
    ioOnRbuttonUp          , /* WM_RBUTTONUP           */
    ioOnImeStartComposition, /* WM_IME_STARTCOMPOSITION*/
    ioOnCut                , /* WM_CUT                 */
    ioOnCopy               , /* WM_COPY                */
    ioOnPaste              , /* WM_PASTE               */
    ioOnClear              , /* WM_CLEAR               */
    ioOnUndo               , /* WM_UNDO                */
    ioOnSetSel             , /* EM_SETSEL              */
    ioOnDefault              /* default                */
};
/* *INDENT-ON* */


/********************************************************************************
 * ���e  : IO�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : HWND hWnd
 * ����  : HMENU id
 * ����  : LOGFONT *logFontPtr
 * �߂�l: HWND
 ***************************************/
HWND
IoWndCreate( HWND hWnd, HMENU id, LOGFONT *logFontPtr )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();

    ioWndData.logFontPtr = logFontPtr;

    wc.lpfnWndProc      = (WNDPROC) IoWndProc;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );
    wc.hCursor          = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground    = (HBRUSH) CreateSolidBrush( BG_COLOR_RGB );
    wc.lpszClassName    = "ioWndClass";
    wc.lpszMenuName     = NULL;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
        return (HWND)NULL;
    }

    hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW /* | WS_EX_ACCEPTFILES*/,
                             "ioWndClass", "IO Window",
                             WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             0, 0,
                             hWnd, id, hInst, NULL );

    if( hWndIo != NULL )
    {
        SetFocus(hWndIo);
    }

    return hWndIo;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃t�H���g�ύX
 * ����  : LOGFONT *logFontPtr
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndChangeFont( LOGFONT *logFontPtr )
{
    ioWndData.logFontPtr = logFontPtr;

    updateTextMetrics( hWndIo );
    SendMessage(hWndIo,(UINT)WM_SIZE,(WPARAM)0,MAKELPARAM(ioWndData.cyClient,ioWndData.cxClient));
    InvalidateRect( hWndIo, NULL, TRUE );
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃T�C�Y�ύX
 * ����  : int x
 * ����  : int y
 * ����  : int cxClient
 * ����  : int cyClient
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndSize( int x, int y, int cxClient, int cyClient )
{
    MoveWindow( hWndIo,x,y,cxClient,cyClient, TRUE);

    return TRUE;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̔j��
 * ����  : int nCmdShow
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndDestroy( void )
{
    if( hWndIo != NULL )
    {
        DestroyWindow( hWndIo );
        hWndIo = NULL;
    }
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̏�����
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndDataInit( void )
{
    IoWndBuffInit();

    setAllScrollInfo();
    InvalidateRect( hWndIo, NULL, TRUE );
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�Z�b�g
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit (TRUE:�����f�[�^���N���A,FALSE:�N���A���Ȃ�)
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    IoWndBuffDataSet( dataPtr, length, bInit );

    if( bInit )
    {
        nop();
    }
    else
    {
        SendMessage(GetParent(hWndIo), (UINT)WM_COMMAND, MAKEWPARAM(0,EN_UPDATE), (LPARAM)hWndIo);
    }

    IoWndBuffSelectOff();
    setAllScrollInfo();
    InvalidateRect( hWndIo, NULL, TRUE );
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�擾
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * ����  : IOWND_REGION region
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndDataGet( TCHAR *dataPtr, DWORD dataSize, IOWND_REGION region )
{
    return IoWndBuffDataGet( dataPtr, dataSize, region );
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�T�C�Y�擾
 * ����  : IOWND_REGION region
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetDataSize( IOWND_REGION region )
{
    return IoWndGetBuffSize( region );
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̉��s�R�[�h�Z�b�g
 * ����  : NEWLINECODE_TYPE newLineCodeType
 * �߂�l: BOOL (TRUE:�\�����������K�v)
 ***************************************/
BOOL
IoWndNewLineCodeSet( NEWLINECODE_TYPE newLineCodeType )
{
    DWORD allDataSize;
    TCHAR *dataTopPtr;
    BOOL bRtn = FALSE;

    IoWndBuffSetNewLineCode( newLineCodeType );

    allDataSize = IoWndGetBuffSize(BUFF_ALL);
    dataTopPtr  = malloc( sizeof(TCHAR) * allDataSize );
    if( dataTopPtr != NULL )
    {
        IoWndBuffDataGet( dataTopPtr, allDataSize, BUFF_ALL );
        IoWndBuffDataSet( dataTopPtr, allDataSize, TRUE );
        setAllScrollInfo();
        InvalidateRect( hWndIo, NULL, TRUE );

        free( dataTopPtr );
        bRtn = TRUE;
    }
    else
    {
        nop();
    }

    return bRtn;
}

/********************************************************************************
 * ���e  : ���C���E�B���h�E���ŏ������郁�b�Z�[�W�𔻒肷��
 * ����  : MSG *msg
 * �߂�l: BOOL
 ***************************************/
BOOL
IsIoWndMessage( MSG *msg )
{
    return FALSE;
}

/********************************************************************************
 * ���e  : �E�B���h�E�v���V�[�W���BWINDOWS����󂯎�������b�Z�[�W�̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
LRESULT CALLBACK
IoWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return ioWndProcTbl[ioWndConvertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * ���e  : �E�B���h�E�Y���b�Z�[�W����Ή�����C���f�b�N�X�ɕϊ�����
 * ����  : UINT message
 * �߂�l: MAIN_WNDPRC_INDEX
 ***************************************/
static IOWND_INDEX
ioWndConvertMSGtoINDEX( UINT message )
{
    IOWND_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE              :rtn = IOWND_ON_CREATE              ;break;
    case WM_PAINT               :rtn = IOWND_ON_PAINT               ;break;
    case WM_SIZE                :rtn = IOWND_ON_SIZE                ;break;
    case WM_CLOSE               :rtn = IOWND_ON_CLOSE               ;break;
    case WM_DESTROY             :rtn = IOWND_ON_DESTROY             ;break;
    case WM_COMMAND             :rtn = IOWND_ON_COMMAND             ;break;
    case WM_KEYUP               :rtn = IOWND_ON_KEYUP               ;break;
    case WM_KEYDOWN             :rtn = IOWND_ON_KEYDOWN             ;break;
    case WM_CHAR                :rtn = IOWND_ON_CHAR                ;break;
    case WM_HSCROLL             :rtn = IOWND_ON_HSCROLL             ;break;
    case WM_VSCROLL             :rtn = IOWND_ON_VSCROLL             ;break;
    case WM_SETFOCUS            :rtn = IOWND_ON_SETFOCUS            ;break;
    case WM_KILLFOCUS           :rtn = IOWND_ON_KILLFOCUS           ;break;
    case WM_MOUSEACTIVATE       :rtn = IOWND_ON_MOUSEACTIVATE       ;break;
    case WM_MOUSEWHEEL          :rtn = IOWND_ON_MOUSEWHEEL          ;break;
    case WM_MOUSEMOVE           :rtn = IOWND_ON_MOUSEMOVE           ;break;
    case WM_LBUTTONDOWN         :rtn = IOWND_ON_LBUTTONDOWN         ;break;
    case WM_MBUTTONDOWN         :rtn = IOWND_ON_MBUTTONDOWN         ;break;
    case WM_RBUTTONDOWN         :rtn = IOWND_ON_RBUTTONDOWN         ;break;
    case WM_LBUTTONUP           :rtn = IOWND_ON_LBUTTONUP           ;break;
    case WM_MBUTTONUP           :rtn = IOWND_ON_MBUTTONUP           ;break;
    case WM_RBUTTONUP           :rtn = IOWND_ON_RBUTTONUP           ;break;
    case WM_IME_STARTCOMPOSITION:rtn = IOWND_ON_IME_STARTCOMPOSITION;break;
    case WM_CUT                 :rtn = IOWND_ON_CUT                 ;break;
    case WM_COPY                :rtn = IOWND_ON_COPY                ;break;
    case WM_PASTE               :rtn = IOWND_ON_PASTE               ;break;
    case WM_CLEAR               :rtn = IOWND_ON_CLEAR               ;break;
    case WM_UNDO                :rtn = IOWND_ON_UNDO                ;break;
    case EM_SETSEL              :rtn = IOWND_ON_SETSEL              ;break;

    case WM_NCMOUSEMOVE         :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_SETCURSOR           :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_NCPAINT             :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_NCHITTEST           :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_ERASEBKGND          :rtn = IOWND_ON_DEFAULT             ;break;

    case WM_IME_COMPOSITION     :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_IME_NOTIFY          :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_IME_SETCONTEXT      :rtn = IOWND_ON_DEFAULT             ;break;
    case WM_IME_COMPOSITIONFULL :rtn = IOWND_ON_DEFAULT             ;break;

    default                     :rtn = IOWND_ON_DEFAULT             ;break;
    }
    /* *INDENT-ON* */

    return rtn;
}

/********************************************************************************
 * ���e  : WM_CREATE �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    int iDelta;          /* for MouseWheel */
    ULONG ulScrollLines; /* for MouseWheel */

    updateTextMetrics( hwnd );

    IoWndBuffInit();

    /* for MouseWheel */
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0, &ulScrollLines, 0);
    iDelta = HIWORD(wParam);
    if( ulScrollLines )
    {
        ioWndData.iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
    }
    else
    {
        ioWndData.iDeltaPerLine = 0;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_PAINT �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC         hdc;
    int         iHorzPos,iVertPos;
    int         iPaintBeg,iPaintEnd,y;
    int         x;
    COLORREF    bkColor,textColor;
    S_BUFF_DISP_DATA buffDispData;
    S_BUFF_LINE_DATA *lineDataPtr;

    hdc = BeginPaint( hwnd, &ps );
    SelectObject( hdc, CreateFontIndirect(ioWndData.logFontPtr) );

    getAllScrollInfo();
    iHorzPos = ioWndData.iHorzPos;
    iVertPos = ioWndData.iVertPos;

    iPaintBeg = max(0, iVertPos + ps.rcPaint.top / ioWndData.cyChar);
    iPaintEnd = min(IoWndGetLineMaxSize(),iVertPos + ps.rcPaint.bottom / ioWndData.cyChar);

    for( y=iPaintBeg,lineDataPtr=IoWndBuffGetLinePtr(y); (y<=iPaintEnd)&&(lineDataPtr != NULL); y++,lineDataPtr = (S_BUFF_LINE_DATA *)lineDataPtr->header.nextPtr )
    { /* �ĕ`��̈�̂�1�s������ */
        for( x=0; x<ioWndData.cxBuffer+1;x++ )
        {
            IoWndBuffGetDispData(lineDataPtr,x+iHorzPos,&buffDispData );

            if( buffDispData.size )
            {
                bkColor = GetBkColor(hdc);
                textColor = GetTextColor(hdc);

                if( buffDispData.type == TAB_CHAR )
                {
                    SetBkColor(hdc, TAB_BK_COLOR_RGB );
                }
                else
                {
                    SetBkColor(hdc, BK_COLOR_RGB );
                }

                if( buffDispData.bSelect )
                {
                    SetBkColor(hdc, SELECT_BK_COLOR_RGB );
                    SetTextColor(hdc, SELECT_TEXT_COLOR_RGB );
                }
                else
                {
                    SetTextColor(hdc, TEXT_COLOR_RGB );
                }

                TextOut(hdc,
                        (x*ioWndData.cxChar) - (buffDispData.offset*ioWndData.cxChar), /* x���W */
                        (y-iVertPos) * ioWndData.cyChar, /* y���W */
                        buffDispData.data,               /* ������ւ̃|�C���^ */
                        buffDispData.size                /* ������ */
                    );

                SetTextColor(hdc,textColor);
                SetBkColor(hdc,bkColor);
            }
            else
            {
                break;
            }
        }
    }

    DeleteObject( SelectObject(hdc, GetStockObject(SYSTEM_FONT)) );
    EndPaint( hwnd, &ps );

    SetCaretPos( (IoWndGetCaretXpos()-ioWndData.iHorzPos)*ioWndData.cxChar, (IoWndGetCaretYpos()-ioWndData.iVertPos)*ioWndData.cyChar);

    return 0;
}

/********************************************************************************
 * ���e  : WM_SIZE �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    ioWndData.cxClient = LOWORD( lParam );
    ioWndData.cyClient = HIWORD( lParam );

    setAllScrollInfo();

    ioWndData.cxBuffer = max( 1, ioWndData.cxClient / ioWndData.cxChar );
    ioWndData.cyBuffer = max( 1, ioWndData.cyClient / ioWndData.cyChar );

    if( hwnd == GetFocus() )
    {
        SetCaretPos( IoWndGetCaretXpos() * ioWndData.cxChar, IoWndGetCaretYpos() * ioWndData.cyChar );
        printCaretPos();
    }
    else
    {
        /* do nothing */
    }

    return 0;
}

/********************************************************************************
 * ���e  : WM_CLOSE �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyWindow( hwnd );

    return 0;
}

/********************************************************************************
 * ���e  : WM_DESTROY �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    IoWndBuffEnd();

    PostQuitMessage(0); /* WM_QUIT���b�Z�[�W���|�X�g���� */
    return 0;
}

/********************************************************************************
 * ���e  : WM_COMMAND �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    switch( LOWORD(wParam) )
    {
    case IDM_EDIT_CUT:
        rtn = ioOnCut( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_COPY:
        rtn = ioOnCopy( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_PASTE:
        rtn = ioOnPaste( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_DELETE:
        rtn = ioOnClear( hwnd, message, wParam, lParam );
        break;

    case IDM_EDIT_SELECT_ALL:
        rtn = ioOnSetSel( hwnd, message, 0, -1 );
        break;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_KEYUP �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    switch(wParam)
    {
    case VK_SHIFT:
        ioWndData.bShiftKeyOn = FALSE;
        break;
    default:
        break;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_KEYDOWN �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    BOOL bErase = TRUE;

    if( wParam == VK_SHIFT )
    {
        ioWndData.bShiftKeyOn = TRUE;
        IoWndBuffSelectOn();
    }
    else if( wParam == VK_CONTROL )
    {
        nop();
    }
    else
    {
        getAllScrollInfo();

        if( (wParam==VK_LEFT)||(wParam==VK_RIGHT)||(wParam==VK_UP)||(wParam==VK_DOWN) )
        {
            if( ioWndData.bShiftKeyOn )
            {
                IoWndBuffSelectOn();
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

        switch(wParam)
        {
        case VK_LEFT:
            bErase = FALSE;
            IoWndDecCaretXpos();
            break;
        case VK_RIGHT:
            bErase = FALSE;
            IoWndIncCaretXpos();
            break;
        case VK_UP:
            bErase = FALSE;
            IoWndDecCaretYpos();
            break;
        case VK_DOWN:
            bErase = FALSE;
            IoWndIncCaretYpos();
            break;
        case VK_DELETE:
            ioWndRemoveData( hwnd, FALSE );
            break;
        case VK_BACK:
            ioWndRemoveData( hwnd, TRUE );
            break;
        default:
            break;
        }

        if( ioWndData.bShiftKeyOn )
        {
            nop();
        }
        else
        {
            IoWndBuffSelectOff();
        }

        /* �L�����b�g���\���͈͊O�ɗL�����ꍇ�̏���(������) */
        if( IoWndGetCaretXpos() < ioWndData.iHorzPos )
        {
            setScrollPos( SB_HORZ, IoWndGetCaretXpos() );
            bErase = TRUE;
        }
        else if( (ioWndData.iHorzPos+ioWndData.cxBuffer-1) < IoWndGetCaretXpos() )
        {
            setScrollPos( SB_HORZ, (ioWndData.iHorzPos+ioWndData.cxBuffer-1) );
            bErase = TRUE;
        }
        else
        {
            nop();
        }

        /* �L�����b�g���\���͈͊O�ɗL�����ꍇ�̏���(�c����) */
        if( IoWndGetCaretYpos() < ioWndData.iVertPos )
        {
            setScrollPos( SB_VERT, IoWndGetCaretYpos() );
            bErase = TRUE;
        }
        else if( (ioWndData.iVertPos+ioWndData.cyBuffer-1) < IoWndGetCaretYpos() )
        {
            setScrollPos( SB_VERT, IoWndGetCaretYpos() - (ioWndData.cyBuffer-1) );
            bErase = TRUE;
        }
        else
        {
            nop();
        }

        printCaretPos();

        HideCaret(hwnd);
        InvalidateRect( hWndIo, NULL, bErase );
        SetCaretPos( (IoWndGetCaretXpos()-ioWndData.iHorzPos)*ioWndData.cxChar, (IoWndGetCaretYpos()-ioWndData.iVertPos)*ioWndData.cyChar);
        ShowCaret(hwnd);
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_CHAR �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    TCHAR data[2];
    int i;
    INT size;

    IoWndBuffSelectOff();

    for( i=0; i<(int) LOWORD(lParam); i++ )
    {
        size = 0;

        switch( wParam )
        {
        case '\b':  /* backspace */
            break;
        case '\x1B':/* escape */
            break;
        case '\n':  /* line feed */
            break;
        case '\r':  /* carriage return */
            size = IoWndBuffGetNewLineData(data);
            break;
        case '\t':  /* tab */
        default:
            /* �������� */
            data[0] = (TCHAR)wParam;
            size = 1;
            break ;
        }

        if( size )
        {
            IoWndBuffDataSet( data,size,FALSE );
            InvalidateRect( hWndIo, NULL, TRUE );
            SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0,EN_UPDATE), (LPARAM)hwnd);
        }
        else
        {
            nop();
        }
    }

    ioOnImeStartComposition( hwnd, message, wParam, lParam );

    SetCaretPos( (IoWndGetCaretXpos()-ioWndData.iHorzPos)*ioWndData.cxChar, (IoWndGetCaretYpos()-ioWndData.iVertPos)*ioWndData.cyChar);
    printCaretPos();
    setAllScrollInfo();

    return rtn;
}

/********************************************************************************
 * ���e  : WM_HSCROLL �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SCROLLINFO si;
    int iHorzPos;

    /* Get all the vertial scroll bar information */
    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;

    /* Save the position for comparison later on */
    GetScrollInfo( hwnd, SB_HORZ, &si );
    iHorzPos = si.nPos;

    switch(LOWORD(wParam))
    {
    case SB_LINELEFT:
        si.nPos -= 1;
        break;
    case SB_LINERIGHT:
        si.nPos += 1;
        break;
    case SB_PAGELEFT:
        si.nPos -= si.nPage;
        break;
    case SB_PAGERIGHT:
        si.nPos += si.nPage;
        break;
    case SB_THUMBPOSITION:
        si.nPos = si.nTrackPos;
        break;
    default :
        break;
    }

    /* Set the position and then retrieve it.  Due to adjustments */
    /* by Windows it may not be the same as the value set. */
    si.fMask = SIF_POS;
    SetScrollInfo( hwnd, SB_HORZ, &si, TRUE );
    GetScrollInfo( hwnd, SB_HORZ, &si );

    /* If the position has changed, scroll the window */
    if( si.nPos != iHorzPos )
    {
        ScrollWindow( hwnd, ioWndData.cxChar * (iHorzPos - si.nPos), 0, NULL, NULL );
        InvalidateRect( hWndIo, NULL, TRUE );
    }

    GetScrollInfo( hwnd, SB_HORZ, &si );
    ioWndData.iHorzPos = si.nPos;

    return rtn;
}

/********************************************************************************
 * ���e  : WM_VSCROLL ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SCROLLINFO si;
    int iVertPos;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;
    GetScrollInfo( hwnd, SB_VERT, &si ); /* �X�N���[���o�[���擾 */
    iVertPos = si.nPos;                  /* ���݂̈ʒu               */

    switch( LOWORD(wParam) )
    {
    case SB_TOP:
        si.nPos = si.nMin;
        break;
    case SB_BOTTOM:
        si.nPos = si.nMax;
        break;
    case SB_LINEUP:
        si.nPos -= 1;
        break;
    case SB_LINEDOWN:
        si.nPos += 1;
        break;
    case SB_PAGEUP:
        si.nPos -= si.nPage;
        break;
    case SB_PAGEDOWN:
        si.nPos += si.nPage;
        break;
    case SB_THUMBTRACK:
        si.nPos = si.nTrackPos;
        break;
    default:
        break;
    }
    si.fMask = SIF_POS;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE); /* �X�N���[���o�[���Z�b�g���ĕ`�� */
    GetScrollInfo(hwnd, SB_VERT, &si);       /* �X�N���[���o�[���Ď擾         */

    if( si.nPos != iVertPos )
    {
        ScrollWindow(hwnd, 0, ioWndData.cyChar * (iVertPos - si.nPos),NULL, NULL);
        UpdateWindow(hwnd);
    }

    GetScrollInfo(hwnd, SB_VERT, &si);
    ioWndData.iVertPos = si.nPos;

    return rtn;
}

/********************************************************************************
 * ���e  : WM_SETFOCUS ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnSetFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    CreateCaret( hwnd,NULL,1,ioWndData.cyChar ); /* ��=1,����=�����T�C�Y */
    SetCaretPos( IoWndGetCaretXpos() * ioWndData.cxChar, IoWndGetCaretYpos() * ioWndData.cyChar);
    ShowCaret( hwnd );

    return rtn;
}

/********************************************************************************
 * ���e  : WM_KILLFOCUS ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnKillFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    HideCaret( hwnd );
    DestroyCaret();

    return rtn;
}

/********************************************************************************
 * ���e  : WM_MOUSEACTIVATE ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnMouseActivate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SetFocus(hwnd);

    return rtn;
}

/********************************************************************************
 * ���e  : WM_MOUSEWHEEL ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnMouseWheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    if( ioWndData.iDeltaPerLine )
    {
        ioWndData.iAccumDelta += (short)HIWORD(wParam);

        while( ioWndData.iAccumDelta >= ioWndData.iDeltaPerLine )
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
            ioWndData.iAccumDelta -= ioWndData.iDeltaPerLine;
        }
        while( ioWndData.iAccumDelta <= -ioWndData.iDeltaPerLine)
        {
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
            ioWndData.iAccumDelta += ioWndData.iDeltaPerLine;
        }
    }
    else
    {
        /* do nothing */
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_MOUSEMOVE ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnMouseMove( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    SHORT x,y;

    x = max(0,(SHORT)LOWORD(lParam));
    y = max(0,(SHORT)HIWORD(lParam));

#if 0 /* �f�o�b�O */
    StsBarSetText( STS_BAR_1,"%d,%d",x,y);
#endif

    if( (wParam & MK_LBUTTON) )
    {
        IoWndSetCaretPos( ((x + (ioWndData.iHorzPos*ioWndData.cxChar))/ioWndData.cxChar), ((y + (ioWndData.iVertPos*ioWndData.cyChar))/ioWndData.cyChar) );
        SetCaretPos( (IoWndGetCaretXpos()-ioWndData.iHorzPos)*ioWndData.cxChar, (IoWndGetCaretYpos()-ioWndData.iVertPos)*ioWndData.cyChar);
        printCaretPos();
        InvalidateRect( hWndIo, NULL, FALSE );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_LBUTTONDOWN ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnLbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    int x,y;

    getAllScrollInfo();

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    IoWndSetCaretPos( ((x + (ioWndData.iHorzPos*ioWndData.cxChar))/ioWndData.cxChar), ((y + (ioWndData.iVertPos*ioWndData.cyChar))/ioWndData.cyChar) );

    SetCaretPos( (IoWndGetCaretXpos()-ioWndData.iHorzPos)*ioWndData.cxChar, (IoWndGetCaretYpos()-ioWndData.iVertPos)*ioWndData.cyChar);
    printCaretPos();

    if( wParam & MK_SHIFT )
    {
        nop();
    }
    else
    {
        SetCapture(hwnd);
        IoWndBuffSelectOff();
        IoWndBuffSelectOn();
    }
    InvalidateRect( hWndIo, NULL, TRUE );

    return rtn;
}

/********************************************************************************
 * ���e  : WM_MBUTTONDOWN ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnMbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * ���e  : WM_RBUTTONDOWN ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnRbuttonDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * ���e  : WM_LBUTTONUP ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnLbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    ReleaseCapture();

    return rtn;
}

/********************************************************************************
 * ���e  : WM_MBUTTONUP ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnMbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * ���e  : WM_RBUTTONUP ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnRbuttonUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HMENU hMenu;
    POINT point;

    point.x = LOWORD(lParam);
    point.y = HIWORD(lParam);

    ClientToScreen(hwnd,&point);

    hMenu = CreatePopupMenu();
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_UNDO       , TEXT("���ɖ߂�(&U)") );
    AppendMenu( hMenu, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_CUT        , TEXT("�؂���(&T)") );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_COPY       , TEXT("�R�s�[(&C)") );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_PASTE      , TEXT("�\��t��(&P)") );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_DELETE     , TEXT("�폜(&D)") );
    AppendMenu( hMenu, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenu, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("���ׂđI��(&A)") );

    EnableMenuItem( hMenu, IDM_EDIT_UNDO, MF_GRAYED );

    if( IoWndGetDataSize(IOWND_SELECTED) )
    {
        EnableMenuItem( hMenu, IDM_EDIT_CUT   , MF_ENABLED );
        EnableMenuItem( hMenu, IDM_EDIT_COPY  , MF_ENABLED );
        EnableMenuItem( hMenu, IDM_EDIT_DELETE, MF_ENABLED );
    }
    else
    {
        EnableMenuItem( hMenu, IDM_EDIT_CUT   , MF_GRAYED );
        EnableMenuItem( hMenu, IDM_EDIT_COPY  , MF_GRAYED );
        EnableMenuItem( hMenu, IDM_EDIT_DELETE, MF_GRAYED );
    }

    if( IsClipboardFormatAvailable(CF_TEXT) )
    {
        EnableMenuItem( hMenu, IDM_EDIT_PASTE, MF_ENABLED );
    }
    else
    {
        EnableMenuItem( hMenu, IDM_EDIT_PASTE, MF_GRAYED );
    }

    TrackPopupMenu(hMenu,TPM_RIGHTBUTTON,point.x,point.y,0,hwnd,NULL);
    DestroyMenu(hMenu);

    return rtn;
}

/********************************************************************************
 * ���e  : WM_IME_STARTCOMPOSTION ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnImeStartComposition( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HIMC hImc;
    COMPOSITIONFORM cf;

    cf.dwStyle = CFS_POINT;
    cf.ptCurrentPos.x = IoWndGetCaretXpos() * ioWndData.cxChar;
    cf.ptCurrentPos.y = IoWndGetCaretYpos() * ioWndData.cyChar;

    hImc = ImmGetContext( hwnd );
    ImmSetCompositionWindow( hImc, &cf );
    ImmSetCompositionFont( hImc,ioWndData.logFontPtr );
    ImmReleaseContext( hwnd, hImc );

    return DefWindowProc(hwnd,message, wParam, lParam);
}

/********************************************************************************
 * ���e  : WM_CUT ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnCut( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;

    dwSize = IoWndGetDataSize(IOWND_SELECTED);
    if( dwSize )
    {
        hGlobal = GlobalAlloc( GHND|GMEM_SHARE, dwSize+1 );
        pGlobal = GlobalLock( hGlobal );
        GlobalUnlock(hGlobal);
        IoWndBuffDataGet( pGlobal,dwSize,IOWND_SELECTED );
        OpenClipboard(hwnd);
        EmptyClipboard();
        SetClipboardData( CF_TEXT, hGlobal );
        CloseClipboard();

        /* CUT�̂� */
        ioWndRemoveData( hwnd,FALSE );
        InvalidateRect( hWndIo, NULL, TRUE );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_COPY ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnCopy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;

    dwSize = IoWndGetDataSize(IOWND_SELECTED);
    if( dwSize )
    {
        hGlobal = GlobalAlloc( GHND|GMEM_SHARE, dwSize+1 );
        pGlobal = GlobalLock( hGlobal );
        GlobalUnlock(hGlobal);
        IoWndBuffDataGet( pGlobal,dwSize,IOWND_SELECTED );
        OpenClipboard(hwnd);
        EmptyClipboard();
        SetClipboardData( CF_TEXT, hGlobal );
        CloseClipboard();
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_PASTE ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnPaste( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD   dwSize,dwSize2;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    PBYTE   dataPtr;

    if( IsClipboardFormatAvailable(CF_TEXT) )
    {
        OpenClipboard(hwnd);

        hGlobal = GetClipboardData(CF_TEXT);
        dwSize = GlobalSize(hGlobal) - 1;
        dataPtr = (PBYTE)malloc(dwSize);
        pGlobal = GlobalLock( hGlobal );
        strncpy( dataPtr, pGlobal, dwSize );
        GlobalUnlock(hGlobal);
        CloseClipboard();
        dwSize2 = strlen( dataPtr );
        if( dwSize2 < dwSize )
        { /* �N���b�v�{�[�h�̍ő�T�C�Y�ɒB����O��NULL������� */
            dwSize = dwSize2; /* NULL�܂ł̃f�[�^��L���Ƃ��� (�A�v���P�[�V�����ɂ���Ă� �]����NULL����ʂɓ����Ă��邱�Ƃ�����) */
        }
        else
        {
            nop();
        }
        IoWndDataSet( dataPtr,dwSize,FALSE );
        free( dataPtr );
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * ���e  : WM_CLEAR ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnClear( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    ioWndRemoveData( hwnd,FALSE );
    InvalidateRect( hWndIo, NULL, TRUE );

    return rtn;
}

/********************************************************************************
 * ���e  : WM_UNDO ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnUndo( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * ���e  : EM_SETSEL ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnSetSel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    if( (wParam == 0) && (lParam == -1) )
    { /* �h���ׂđI���h�̂ݑΉ� */
        if( IoWndBuffSelectAll() )
        {
            IoWndSetCaretPos(0,IoWndGetLineMaxSize());
            setScrollPos( SB_VERT, IoWndGetLineMaxSize() );
            InvalidateRect( hWndIo, NULL, TRUE );
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

    return rtn;
}

/********************************************************************************
 * ���e  : ���b�Z�[�W�̃f�t�H���g����
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
ioOnDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * ���e  : �f�[�^�폜
 * ����  : HWND hwnd
 * ����  : bBackSpace
 * �߂�l: �Ȃ�
 ***************************************/
static void
ioWndRemoveData( HWND hwnd, BOOL bBackSpace )
{
    IoWndBuffRemoveData( bBackSpace );
    SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0,EN_UPDATE), (LPARAM)hwnd);
}

/********************************************************************************
 * ���e  : �e�L�X�g���g���N�X�̍X�V
 * ����  : HWND hwnd
 * �߂�l: �Ȃ�
 ***************************************/
static void
updateTextMetrics( HWND hwnd )
{
    HDC        hdc;
    TEXTMETRIC tm;

    hdc = GetDC( hwnd );
    SelectObject( hdc, CreateFontIndirect(ioWndData.logFontPtr) );

    GetTextMetrics( hdc, &tm );
    ioWndData.cxChar = tm.tmAveCharWidth;
    ioWndData.cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * ioWndData.cxChar / 2;
    ioWndData.cyChar = tm.tmHeight + tm.tmExternalLeading;

    DeleteObject( SelectObject( hdc, GetStockObject(SYSTEM_FONT) ) );
    ReleaseDC( hwnd, hdc );
}

/********************************************************************************
 * ���e  : �X�N���[�����̃Z�b�g
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
static void
setAllScrollInfo( void )
{
    SCROLLINFO si;

    /* Set vertical scroll bar range and page size */
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;                                                                /* �͈͂̍ŏ��l */
    si.nMax   = max(IoWndGetLineMaxSize(),(ioWndData.cyClient / ioWndData.cyChar))-1; /* �͈͂̍ő�l */
    si.nPage  = (ioWndData.cyClient / ioWndData.cyChar); /* �y�[�W�T�C�Y */
    SetScrollInfo( hWndIo, SB_VERT, &si, TRUE );

    /* Set horizontal scroll bar range and page size*/
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = max( IoWndGetColumnMaxSize(),(ioWndData.cxClient/ioWndData.cxChar))-1;
    si.nPage  = (ioWndData.cxClient/ioWndData.cxChar);
    SetScrollInfo( hWndIo, SB_HORZ, &si, TRUE );
}

/********************************************************************************
 * ���e  : �X�N���[�����̎擾
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
static void
getAllScrollInfo( void )
{
    SCROLLINFO si;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hWndIo, SB_VERT, &si ); /* �c�X�N���[���o�[�̈ʒu���擾 */
    ioWndData.iVertPos = si.nPos;

    GetScrollInfo( hWndIo, SB_HORZ, &si);   /* ���X�N���[���o�[�̈ʒu���擾 */
    ioWndData.iHorzPos = si.nPos;
}

/********************************************************************************
 * ���e  : �X�N���[���ʒu�̃Z�b�g
 * ����  : int   fnBar �X�N���[���o�[�̃^�C�v
 * ����  : DWORD nPos  �ʒu
 * �߂�l: �Ȃ�
 ***************************************/
static void
setScrollPos( int fnBar, DWORD nPos )
{
    SCROLLINFO si;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_POS;
    GetScrollInfo( hWndIo, fnBar, &si );
    si.nPos   = nPos;
    SetScrollInfo( hWndIo, fnBar, &si, TRUE );
}

/********************************************************************************
 * ���e  : �J�[�\���ʒu���X�e�[�^�X�o�[�ɏo��
 * ����  : void
 * �߂�l: void
 ***************************************/
static void
printCaretPos( void )
{
    StsBarSetText( STS_BAR_0  ,"%d �o�C�g�A�S %d �s",IoWndGetBuffSize(BUFF_ALL),IoWndGetLineMaxSize() );
    StsBarSetText( STS_BAR_MAX,"   %d �s�A%d ��",IoWndGetCaretYpos()+1,IoWndGetCaretXpos()+1);
}
