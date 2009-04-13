/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "mbctype.h"
#include "IoWndDef.h"

/* �O���֐���` */
#include "WinMain.h"
#include "StsBar.h"
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
static LRESULT ioOnDefault            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

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
    ioOnDefault              /* default                */
};
/* *INDENT-ON* */


/********************************************************************************
 * ���e  : IO�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : HWND hWnd
 * ����  : LOGFONT *logFontPtr
 * �߂�l: HWND
 ***************************************/
HWND
IoWndCreate( HWND hWnd, LOGFONT *logFontPtr )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();

    ioWndData.logFontPtr = logFontPtr;

    wc.lpfnWndProc      = (WNDPROC) IoWndProc;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );
    wc.hCursor          = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground    = (HBRUSH) GetStockObject(WHITE_BRUSH);
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
                                 hWnd, NULL, hInst, NULL );

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
void IoWndChangeFont( LOGFONT *logFontPtr )
{
    ioWndData.logFontPtr = logFontPtr;

    updateTextMetrics( hWndIo );
    SendMessage(hWndIo,WM_SIZE,0,MAKELONG(ioWndData.cxClient,ioWndData.cyClient));
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
    IoWndInvalidateRect();
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�Z�b�g
 * ����  : TCHAR* dataPtr
 * ����  : DWORD  length
 * ����  : BOOL   bInit
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndDataSet( TCHAR* dataPtr, DWORD length, BOOL bInit )
{
    IoWndBuffDataSet( dataPtr, length, bInit );

    setAllScrollInfo();
    IoWndInvalidateRect();
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�擾
 * ����  : TCHAR *dataPtr
 * ����  : DWORD dataSize
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndDataGet( TCHAR *dataPtr, DWORD dataSize )
{
    return IoWndBuffDataGet( dataPtr, dataSize );
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃f�[�^�T�C�Y�擾
 * ����  : �Ȃ�
 * �߂�l: DWORD
 ***************************************/
DWORD
IoWndGetDataSize( void )
{
    return IoWndGetBuffSize();
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̋�`������
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndInvalidateRect( void )
{
    InvalidateRect( hWndIo, NULL, TRUE );
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̉��s�R�[�h�Z�b�g
 * ����  : NEWLINECODE_TYPE newLineCodeType
 * �߂�l: BOOL (TRUE:�f�[�^���ύX���ꂽ)
 ***************************************/
BOOL
IoWndNewLineCodeSet( NEWLINECODE_TYPE newLineCodeType )
{
    return IoWndBuffSetNewLineCode( newLineCodeType );
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

    hdc = BeginPaint( hwnd, &ps );
    SelectObject( hdc, CreateFontIndirect(ioWndData.logFontPtr) );

    getAllScrollInfo();
    iHorzPos = ioWndData.iHorzPos;
    iVertPos = ioWndData.iVertPos;

    iPaintBeg = max(0, iVertPos + ps.rcPaint.top / ioWndData.cyChar);
    iPaintEnd = min(IoWndGetLineMaxSize(),iVertPos + ps.rcPaint.bottom / ioWndData.cyChar);

    for( y=iPaintBeg; y<=iPaintEnd; y++ )
    { /* �ĕ`��̈�̂�1�s������ */
        for( x=0; x<ioWndData.cxBuffer+1; )
        {
            IoWndBuffGetDispData(y,x+iHorzPos,&buffDispData );

            if( buffDispData.size )
            {
                bkColor = GetBkColor(hdc);
                textColor = GetTextColor(hdc);

                if( buffDispData.type == TAB_CHAR )
                {
#if 1
                    SetBkColor(hdc,RGB(0xE0,0xFF,0xFF));
#else
                    SetBkColor(hdc,RGB(0,13,0x7F));
                    SetTextColor(hdc,RGB(0xFF,0xFF,0xFF));
#endif
                }
                else
                {
                    nop();
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
            x += buffDispData.size;
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

    getAllScrollInfo();

    switch(wParam)
    {
    case VK_LEFT:
        IoWndDecCaretXpos();
        break;
    case VK_RIGHT:
        IoWndIncCaretXpos();
        break;
    case VK_UP:
        IoWndDecCaretYpos();
        break;
    case VK_DOWN:
        IoWndIncCaretYpos();
        break;
    case VK_SHIFT:
        break;
    default:
        break;
    }

    /* �L�����b�g���\���͈͊O�ɗL�����ꍇ�̏���(������) */
    if( IoWndGetCaretXpos() < ioWndData.iHorzPos )
    {
        setScrollPos( SB_HORZ, IoWndGetCaretXpos() );
    }
    else if( (ioWndData.iHorzPos+ioWndData.cxBuffer-1) < IoWndGetCaretXpos() )
    {
        setScrollPos( SB_HORZ, (ioWndData.iHorzPos+ioWndData.cxBuffer-1) );
    }
    else
    {
        nop();
    }

    /* �L�����b�g���\���͈͊O�ɗL�����ꍇ�̏���(�c����) */
    if( IoWndGetCaretYpos() < ioWndData.iVertPos )
    {
        setScrollPos( SB_VERT, IoWndGetCaretYpos() );
    }
    else if( (ioWndData.iVertPos+ioWndData.cyBuffer-1) < IoWndGetCaretYpos() )
    {
        setScrollPos( SB_VERT, IoWndGetCaretYpos() - (ioWndData.cyBuffer-1) );
    }
    else
    {
        nop();
    }

    SetCaretPos( (IoWndGetCaretXpos()-ioWndData.iHorzPos)*ioWndData.cxChar, (IoWndGetCaretYpos()-ioWndData.iVertPos)*ioWndData.cyChar);
    printCaretPos();

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
    int i;

    for( i=0; i<(int) LOWORD(lParam); i++ )
    {
        switch( wParam )
        {
        case '\b':  /* backspace */
            IoWndBuffRemoveData();
            IoWndInvalidateRect();
            break;
        case '\x1B':/* escape */
            break;
        case '\n':  /* line feed */
            break;
        case '\r':  /* carriage return */
            IoWndBuffAddNewLine();
            IoWndInvalidateRect();
            break;
        case '\t':  /* tab */
        default:
            /* �������� */
            IoWndBuffAddData( wParam );
            IoWndInvalidateRect();
            break ;
        }
    }
    SetCaretPos( (IoWndGetCaretXpos()-ioWndData.iHorzPos)*ioWndData.cxChar, (IoWndGetCaretYpos()-ioWndData.iVertPos)*ioWndData.cyChar);

    ioOnImeStartComposition( hwnd, message, wParam, lParam );

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
    LRESULT rtn = 0;
    HIMC hImc;
    COMPOSITIONFORM cf;

    cf.dwStyle = CFS_POINT;
    cf.ptCurrentPos.x = IoWndGetCaretXpos() * ioWndData.cxChar;
    cf.ptCurrentPos.y = IoWndGetCaretYpos() * ioWndData.cyChar;

    hImc = ImmGetContext( hwnd );
    ImmSetCompositionWindow( hImc, &cf );
    ImmSetCompositionFont( hImc,ioWndData.logFontPtr );
    ImmReleaseContext( hwnd, hImc );

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
    IoWndInvalidateRect();
}

/********************************************************************************
 * ���e  : �J�[�\���ʒu���X�e�[�^�X�o�[�ɏo��
 * ����  : void
 * �߂�l: void
 ***************************************/
static void
printCaretPos( void )
{
    StsBarSetText( STS_BAR_0  ,"%d �o�C�g�A�S %d �s",IoWndGetBuffSize(),IoWndGetLineMaxSize() );
    StsBarSetText( STS_BAR_MAX,"   %d �s�A%d ��",IoWndGetCaretYpos()+1,IoWndGetCaretXpos()+1);
}
