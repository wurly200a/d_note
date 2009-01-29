/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "IoWndDef.h"

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "IoWnd.h"
LRESULT CALLBACK IoWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static IOWND_INDEX ioWndConvertMSGtoINDEX( UINT message );
static void ioWndBufferOut( int x, int y, TCHAR c );
static LRESULT ioOnCreate       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnPaint        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSize         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnClose        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDestroy      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnCommand      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyUp        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKeyDown      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnChar         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnHscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnVscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnSetFocus     ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnKillFocus    ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnMouseActivate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT ioOnDefault      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

/* �����ϐ���` */
static HWND hWndIo; /* �E�B���h�E�̃n���h�� */
static S_IOWND_DATA ioWndData;

/* *INDENT-OFF* */
static LRESULT (*ioWndProcTbl[IOWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    ioOnCreate       , /* WM_CREATE    */
    ioOnPaint        , /* WM_PAINT     */
    ioOnSize         , /* WM_SIZE      */
    ioOnClose        , /* WM_CLOSE     */
    ioOnDestroy      , /* WM_DESTROY   */
    ioOnCommand      , /* WM_COMMAND   */
    ioOnKeyUp        , /* WM_KEYUP     */
    ioOnKeyDown      , /* WM_KEYDOWN   */
    ioOnChar         , /* WM_CHAR      */
    ioOnHscroll      , /* WM_HSCROLL   */
    ioOnVscroll      , /* WM_VSCROLL   */
    ioOnSetFocus     , /* WM_SETFOCUS  */
    ioOnKillFocus    , /* WM_KILLFOCUS */
    ioOnMouseActivate, /* WM_KILLFOCUS */
    ioOnDefault        /* default      */
};
/* *INDENT-ON* */


/********************************************************************************
 * ���e  : IO�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : int nCmdShow
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndCreate( HWND hWnd )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();

    wc.lpfnWndProc      = (WNDPROC) IoWndProc;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wc.lpszClassName    = "ioWndClass";
    wc.lpszMenuName     = NULL;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
        return FALSE;
    }

    hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW | WS_EX_ACCEPTFILES,
                                 "ioWndClass", "IO Window",
                                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 0, 0,
                                 hWnd, NULL, hInst, NULL );

    if( hWndIo == NULL )
    {
        return FALSE;
    }

    SetFocus(hWndIo);

    return TRUE;
}

/********************************************************************************
 * ���e  : IO�E�B���h�E�̃T�C�Y�ύX
 * ����  : int cxClient
 * ����  : int cyClient
 * �߂�l: BOOL
 ***************************************/
BOOL
IoWndSize( int cxClient, int cyClient )
{
    MoveWindow( hWndIo,0,70,cxClient,cyClient-70, TRUE);
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
 * ���e  : ������o��
 * ����  : TCHAR* strPtr
 * ����  : int length
 * �߂�l: �Ȃ�
 ***************************************/
void
IoWndPrint( TCHAR* strPtr, int length )
{
    int i;

    for( i=0; i<length; i++ )
    {
        ioWndBufferOut( ioWndData.xCaret, ioWndData.yCaret, *strPtr );

        /* �L�����b�g�̈ʒu��i�߂� */
        if( ++ioWndData.xCaret == ioWndData.cxBuffer )
        {
            ioWndData.xCaret = 0 ;

            if( ++ioWndData.yCaret == ioWndData.cyBuffer )
            {
                ioWndData.yCaret = 0;
            }
            else
            {
                /* do nothing */
            }

        }
        else
        {
            /* do nothing */
        }

        strPtr++;
    }

    SendMessage( hWndIo, WM_PAINT, 0, 0 );
    InvalidateRect( hWndIo, NULL, TRUE );
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
    case WM_CREATE       :rtn = IOWND_ON_CREATE       ;break;
    case WM_PAINT        :rtn = IOWND_ON_PAINT        ;break;
    case WM_SIZE         :rtn = IOWND_ON_SIZE         ;break;
    case WM_CLOSE        :rtn = IOWND_ON_CLOSE        ;break;
    case WM_DESTROY      :rtn = IOWND_ON_DESTROY      ;break;
    case WM_COMMAND      :rtn = IOWND_ON_COMMAND      ;break;
    case WM_KEYUP        :rtn = IOWND_ON_KEYUP        ;break;
    case WM_KEYDOWN      :rtn = IOWND_ON_KEYDOWN      ;break;
    case WM_CHAR         :rtn = IOWND_ON_CHAR         ;break;
    case WM_HSCROLL      :rtn = IOWND_ON_HSCROLL      ;break;
    case WM_VSCROLL      :rtn = IOWND_ON_VSCROLL      ;break;
    case WM_SETFOCUS     :rtn = IOWND_ON_SETFOCUS     ;break;
    case WM_KILLFOCUS    :rtn = IOWND_ON_KILLFOCUS    ;break;
    case WM_MOUSEACTIVATE:rtn = IOWND_ON_MOUSEACTIVATE;break;
    default              :rtn = IOWND_ON_DEFAULT      ;break;
    }
    /* *INDENT-ON* */

    return rtn;
}

/********************************************************************************
 * ���e  : �\���o�b�t�@�ɏo�͂���
 * ����  : int   x �\���o�b�t�@�̏o�͈ʒu(X)
 * ����  : int   y �\���o�b�t�@�̏o�͈ʒu(Y)
 * ����  : TCHAR c �o�͂��镶����
 * �߂�l: �Ȃ�
 ***************************************/
static void
ioWndBufferOut( int x, int y, TCHAR c )
{
    if( ioWndData.bufferPtr != NULL )
    {
        *(ioWndData.bufferPtr + y * ioWndData.cxBuffer + x) = c;
    }
    else
    {
        /* do nothing */
    }
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
    HDC        hdc;
    TEXTMETRIC tm;
    LRESULT rtn = 0;

    ioWndData.bufferPtr = NULL;
    ioWndData.dwCharSet = DEFAULT_CHARSET;

    hdc = GetDC( hwnd );
    SelectObject( hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, ioWndData.dwCharSet, 0, 0, 0, FIXED_PITCH, NULL) );

    GetTextMetrics( hdc, &tm );
    ioWndData.cxChar = tm.tmAveCharWidth;
    ioWndData.cyChar = tm.tmHeight;

    DeleteObject( SelectObject( hdc, GetStockObject(SYSTEM_FONT)) );
    ReleaseDC( hwnd, hdc );

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
    PAINTSTRUCT  ps;
    HDC          hdc;
    int y;

    /* �\���o�b�t�@��\�������� */
    hdc = BeginPaint( hwnd, &ps ) ;
    SelectObject( hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, ioWndData.dwCharSet, 0, 0, 0, FIXED_PITCH, NULL) );
    for( y = 0; y < ioWndData.cyBuffer; y++ )
    {
        TextOut( hdc, 0, y * ioWndData.cyChar, (ioWndData.bufferPtr + y * ioWndData.cxBuffer + 0), ioWndData.cxBuffer );
    }
    DeleteObject( SelectObject(hdc, GetStockObject(SYSTEM_FONT)) );
    EndPaint( hwnd, &ps ) ;

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
    int x,y;

    ioWndData.cxClient = LOWORD( lParam );
    ioWndData.cyClient = HIWORD( lParam );

    ioWndData.cxBuffer = max( 1, ioWndData.cxClient / ioWndData.cxChar );
    ioWndData.cyBuffer = max( 1, ioWndData.cyClient / ioWndData.cyChar );

    if( ioWndData.bufferPtr != NULL )
    {
        free( ioWndData.bufferPtr );
    }
    else
    {
        /* do nothing */
    }

    ioWndData.bufferPtr = (TCHAR *) malloc( ioWndData.cxBuffer * ioWndData.cxBuffer * sizeof(TCHAR) );

    for( y=0; y<ioWndData.cyBuffer; y++ )
    {
        for( x=0; x<ioWndData.cxBuffer; x++ )
        {
            ioWndBufferOut( x,y,' ' );
        }
    }

    ioWndData.xCaret = 0;
    ioWndData.yCaret = 0;

    if( hwnd == GetFocus() )
    {
        SetCaretPos( ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar );
    }
    else
    {
        /* do nothing */
    }

#if 0 /* �K�v�H*/
    InvalidateRect( hwnd, NULL, TRUE );
#endif

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
    if( ioWndData.bufferPtr != NULL )
    {
        free( ioWndData.bufferPtr );
    }
    else
    {
        /* do nothing */
    }

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

    switch (wParam)
    {
    case VK_LEFT:
        ioWndData.xCaret = max( ioWndData.xCaret - 1, 0 );
        break;
    case VK_RIGHT:
        ioWndData.xCaret = min( ioWndData.xCaret + 1, ioWndData.cxBuffer - 1 );
        break ;
    case VK_UP:
        ioWndData.yCaret = max( ioWndData.yCaret - 1, 0 );
        break;
    case VK_DOWN:
        ioWndData.yCaret = min( ioWndData.yCaret + 1, ioWndData.cyBuffer - 1 );
        break;
    default:
        break;
    }

    SetCaretPos( ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar) ;

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
    PAINTSTRUCT  ps;
    HDC          hdc;
    int i;

    for( i=0; i<(int) LOWORD(lParam); i++ )
    {
        switch( wParam )
        {
        case '\b':  /* backspace */
        case '\t':  /* tab */
        case '\n':  /* line feed */
        case '\r':  /* carriage return */
        case '\x1B':/* escape */
            break;
        default:    /* character codes */
            /* ��������o�b�t�@�ɒǉ� */
            ioWndBufferOut( ioWndData.xCaret, ioWndData.yCaret, (TCHAR)wParam );

            HideCaret( hwnd );

            /* �ǉ����ꂽ��������o�� */
            hdc = GetDC( hwnd );
            SelectObject( hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, ioWndData.dwCharSet, 0, 0, 0, FIXED_PITCH, NULL) );
            TextOut( hdc, ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar, (ioWndData.bufferPtr + ioWndData.yCaret * ioWndData.cxBuffer + ioWndData.xCaret), 1 );
            DeleteObject( SelectObject(hdc, GetStockObject(SYSTEM_FONT)) );
            ReleaseDC(hwnd, hdc);

            ShowCaret(hwnd) ;

            /* �L�����b�g�̈ʒu��i�߂� */
            if( ++ioWndData.xCaret == ioWndData.cxBuffer )
            {
                ioWndData.xCaret = 0 ;

                if( ++ioWndData.yCaret == ioWndData.cyBuffer )
                {
                    ioWndData.yCaret = 0;
                }
                else
                {
                    /* do nothing */
                }

            }
            else
            {
                /* do nothing */
            }
            break ;
        }
    }
    SetCaretPos( ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar );

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
static LRESULT ioOnHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

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

    CreateCaret( hwnd,NULL,ioWndData.cxChar,ioWndData.cyChar );
    SetCaretPos( ioWndData.xCaret * ioWndData.cxChar, ioWndData.yCaret * ioWndData.cyChar) ;
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
