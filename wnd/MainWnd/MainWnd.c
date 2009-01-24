/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "MainWndDef.h"

/* �O���֐���` */
#include "SomeCtrl.h"

/* �O���ϐ���` */
extern HINSTANCE ghInst;      /* �C���X�^���X�̃n���h��     */
extern TCHAR     szAppName[]; /* �A�v���P�[�V�����̖��� */

/* �����֐���` */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static WNDPROC_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyUp  ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onChar   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

/* �����ϐ���` */
static HWND hwndMain; /* ���C���E�B���h�E�̃n���h�� */
static HWND hwndSbar;
static S_MAIN_STATUS mainSts;

static int sbarColWidth[5] = { 100, 200, 300, 400, -1 };

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[WNDPROC_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate , /* WM_CREATE  */
    onPaint  , /* WM_PAINT   */
    onSize   , /* WM_SIZE    */
    onClose  , /* WM_CLOSE   */
    onDestroy, /* WM_DESTROY */
    onCommand, /* WM_COMMAND */
    onKeyUp  , /* WM_KEYUP   */
    onKeyDown, /* WM_KEYDOWN */
    onChar   , /* WM_CHAR    */
    onHscroll, /* WM_HSCROLL */
    onVscroll, /* WM_VSCROLL */
    onApp    , /* WM_APP     */
    onDefault  /* default    */
};
/* *INDENT-ON* */


/********************************************************************************
 * �֐��� : MainWndCreate()
 * ����   : int nCmdShow
 * �߂�l : BOOL
 * ���e   : ���C���E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 ********************************************************************************/
BOOL
MainWndCreate( int nCmdShow )
{
    WNDCLASS wc = {0};

    /* ���C���E�B���h�E�N���X */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) WndProc;
    wc.cbClsExtra       = 0; /* �N���X�\���ׂ̂̈̒ǉ��̈��\�񂷂� */
    wc.cbWndExtra       = 0; /* �E�B���h�E�\���ׂ̂̈̒ǉ��̈��\�񂷂� */
    wc.hInstance        = ghInst;
    wc.hIcon            = LoadIcon( ghInst, szAppName );          /* �A�C�R�� */
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1); /* �w�i */
    wc.lpszMenuName     = szAppName;
    wc.lpszClassName    = szAppName;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), szAppName, MB_ICONERROR );
        return FALSE;
    }

    /* ���C���E�B���h�E���쐬 */
    InitCommonControls(); /* commctrl.h�̃C���N���[�h�Acomctl32.lib�̃v���W�F�N�g�ւ̎Q�����K�v */
    hwndMain = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                               szAppName, szAppName,
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               WND_WIDTH    , WND_HEIGHT,
                               NULL, NULL, ghInst, NULL);

    if( hwndMain == NULL )
    {
        return FALSE;
    }

    ShowWindow( hwndMain, nCmdShow ); /* ->WM_SIZE   */ /* �E�C���h�E��\�� */
    UpdateWindow( hwndMain );         /* ->WM_PAINT  */

    return TRUE;
}

/********************************************************************************
 * �֐��� : IsMainWndMessage()
 * ����   : MSG *msg
 * �߂�l : BOOL
 * ���e   : ���C���E�B���h�E���ŏ������郁�b�Z�[�W�𔻒肷��
 ********************************************************************************/
BOOL
IsMainWndMessage( MSG *msg )
{
    return FALSE;
}

/********************************************************************************
 * �֐��� : WndProc()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : �E�B���h�E�v���V�[�W���BWINDOWS����󂯎�������b�Z�[�W����������
 ********************************************************************************/
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return wndProcTbl[convertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * �֐��� : ConvertMSGtoINDEX()
 * ����   : UINT message
 * �߂�l : MAIN_WNDPRC_INDEX
 * ���e   : �E�B���h�E�Y���b�Z�[�W����Ή�����C���f�b�N�X�ɕϊ�����
 ********************************************************************************/
static WNDPROC_INDEX convertMSGtoINDEX( UINT message )
{
    WNDPROC_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE :rtn = WNDPROC_ON_CREATE ;break;
    case WM_PAINT  :rtn = WNDPROC_ON_PAINT  ;break;
    case WM_SIZE   :rtn = WNDPROC_ON_SIZE   ;break;
    case WM_CLOSE  :rtn = WNDPROC_ON_CLOSE  ;break;
    case WM_DESTROY:rtn = WNDPROC_ON_DESTROY;break;
    case WM_COMMAND:rtn = WNDPROC_ON_COMMAND;break;
    case WM_APP    :rtn = WNDPROC_ON_APP    ;break;
    case WM_KEYUP  :rtn = WNDPROC_ON_KEYUP  ;break;
    case WM_KEYDOWN:rtn = WNDPROC_ON_KEYDOWN;break;
    case WM_CHAR   :rtn = WNDPROC_ON_CHAR   ;break;
    case WM_HSCROLL:rtn = WNDPROC_ON_HSCROLL;break;
    case WM_VSCROLL:rtn = WNDPROC_ON_VSCROLL;break;
    default        :rtn = WNDPROC_ON_DEFAULT;break;
    }
    /* *INDENT-ON* */

    return rtn;
}

/********************************************************************************
 * �֐��� : onCreate()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_CREATE ����������
 ********************************************************************************/
static LRESULT onCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    HFONT hFont,hOldFont;

    hdc = GetDC( hwnd );
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    mainSts.cxChar = tm.tmAveCharWidth;
    mainSts.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

#if 0
    IoWndCreate( hwnd );
    mainSts.execute = 0;
#endif

    /* �R���g���[�����܂Ƃ߂Đ��� */
    SomeCtrlCreate( hwnd );

    return rtn;
}

/********************************************************************************
 * �֐��� : onPaint()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_PAINT ����������
 ********************************************************************************/
static LRESULT onPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
  �֐��� : onSize()
  ����   : �E�B���h�E�n���h�� HWND hwnd
           ���b�Z�[�W         UINT message
           �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
           �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
  �߂�l : LRESULT
  ���e   : WM_SIZE ����������
********************************************************************************/
static LRESULT onSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    mainSts.cxClient = LOWORD( lParam );
    mainSts.cyClient = HIWORD( lParam );

    /* �R���g���[�����܂Ƃ߂Ē��� */
    SomeCtrlSize( mainSts.cxClient, mainSts.cyChar );

#if 0
    IoWndSize( mainSts.cxClient, mainSts.cyClient );
#endif
    return 0;
}

/********************************************************************************
 * �֐��� : onClose()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_CLOSE ����������
 ********************************************************************************/
static LRESULT onClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyWindow( hwnd );

    return 0;
}

/********************************************************************************
 * �֐��� : onDestroy()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_DESTROY ����������
 ********************************************************************************/
static LRESULT onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#if 0
    IoWndDestroy();
#endif

    PostQuitMessage(0); /* WM_QUIT���b�Z�[�W���|�X�g���� */
    return 0;
}

/********************************************************************************
 * �֐��� : onCommand()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_COMMAND ����������
 ********************************************************************************/
static LRESULT onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
#if 0
    static TCHAR szComPort[50];
    static S_COMM_PARAM commParam;
    TCHAR szBuf[100];

    switch( LOWORD(wParam) )
    {
    case (WND_SOME_CTRL_COMPORT_BUTTON+WND_SOME_CTRL_ID_OFFSET):
        if( mainSts.execute == 0 )
        {
            /* COM�|�[�g���擾 */
            SomeCtrlGetText( WND_SOME_CTRL_COMPORT_COMBOBOX, szComPort );
            commParam.portStr = &szComPort;
            if( COMMopen( &commParam ) )
            {
                mainSts.execute = 1;

                SomeCtrlDisable( WND_SOME_CTRL_COMPORT_COMBOBOX );
                SetWindowText( SomeCtrlGetHWND(WND_SOME_CTRL_COMPORT_BUTTON), TEXT("DISCONNECT") );
            }
            else
            {
                chMB("Cannot Open.");
            }
        }
        else
        {
            COMMclose();
            mainSts.execute = 0;

            SomeCtrlEnable( WND_SOME_CTRL_COMPORT_COMBOBOX );
            SetWindowText( SomeCtrlGetHWND(WND_SOME_CTRL_COMPORT_BUTTON), TEXT("CONNECT") );
        }
        break;

    case (WND_SOME_CTRL_PROPERTY_BUTTON+WND_SOME_CTRL_ID_OFFSET):
        /* COM�|�[�g���擾 */
        SomeCtrlGetText( WND_SOME_CTRL_COMPORT_COMBOBOX, szComPort );
        commParam.portStr = &szComPort;
        COMMproperty( hwnd, &commParam );
        break;

#if 0
    case IDM_APP_EXIT:
        SendMessage( hwnd, WM_CLOSE, 0, 0 );
        break;
    case IDM_APP_ABOUT:
        DialogBox( ghInst, TEXT("ABOUT_BOX"), hwnd, AboutDlgProc );
        /* �_�C�A���O�{�b�N�X���N���[�Y�����܂Ő����Ԃ��Ȃ� */
        break;
#endif
    default:
        break;
    }
#endif

    return rtn;
}


/********************************************************************************
 * �֐��� : onKeyUp()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_KEYUP ����������
 ********************************************************************************/
static LRESULT onKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * �֐��� : onKeyDown()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_KEYDOWN ����������
 ********************************************************************************/
static LRESULT onKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * �֐��� : onChar()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_CHAR ����������
 ********************************************************************************/
static LRESULT onChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * �֐��� : onHscroll()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_HSCROLL ����������
 ********************************************************************************/
static LRESULT onHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * �֐��� : onVscroll()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_VSCROLL ����������
 ********************************************************************************/
static LRESULT onVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}


/********************************************************************************
 * �֐��� : onApp()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : WM_APP ����������
 ********************************************************************************/
static LRESULT onApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    switch( LOWORD(wParam) )
    {
    default:
        break;
    }

    return rtn;
}

/********************************************************************************
 * �֐��� : onDefault()
 * ����   : �E�B���h�E�n���h�� HWND hwnd
 *          ���b�Z�[�W         UINT message
 *          �p�����[�^1        WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 *          �p�����[�^2        LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l : LRESULT
 * ���e   : ���b�Z�[�W�̃f�t�H���g����
 ********************************************************************************/
static LRESULT onDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}
