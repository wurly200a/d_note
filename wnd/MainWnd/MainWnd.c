/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "MainWndDef.h"
#include "MainWndMenu.h"

/* �O���֐���` */
#include "WinMain.h"
#include "IoWnd.h"
#include "SomeCtrl.h"
#include "File.h"
#include "StsBar.h"
#include "Font.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static MAINWND_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onPaint        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyUp        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyDown      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onChar         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onHscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onVscroll      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMouseWheel   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSetFocus     ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKillFocus    ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDropFiles    ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

void doCaption( HWND hwnd, TCHAR* szTitleName );

/* �����ϐ���` */
static HWND hwndMain; /* ���C���E�B���h�E�̃n���h�� */
static S_MAINWND_DATA mainWndData;

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[MAINWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate       , /* WM_CREATE           */
    onPaint        , /* WM_PAINT            */
    onSize         , /* WM_SIZE             */
    onClose        , /* WM_CLOSE            */
    onDestroy      , /* WM_DESTROY          */
    onCommand      , /* WM_COMMAND          */
    onKeyUp        , /* WM_KEYUP            */
    onKeyDown      , /* WM_KEYDOWN          */
    onChar         , /* WM_CHAR             */
    onHscroll      , /* WM_HSCROLL          */
    onVscroll      , /* WM_VSCROLL          */
    onMouseWheel   , /* WM_MOUSEWHEEL       */
    onSetFocus     , /* WM_SETFOCUS         */
    onKillFocus    , /* WM_KILLFOCUS        */
    onDropFiles    , /* WM_DROPFILES        */
    onApp          , /* WM_APP              */
    onDefault        /* default             */
};
/* *INDENT-ON* */

/********************************************************************************
 * ���e  : ���C���E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : int nCmdShow
 * �߂�l: BOOL
 ***************************************/
BOOL
MainWndCreate( int nCmdShow )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();
    HMENU hMenuPopup;

    /* ���C���E�B���h�E�N���X */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) WndProc;
    wc.cbClsExtra       = 0; /* �N���X�\���ׂ̂̈̒ǉ��̈��\�񂷂� */
    wc.cbWndExtra       = 0; /* �E�B���h�E�\���ׂ̂̈̒ǉ��̈��\�񂷂� */
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, pAppName );          /* �A�C�R�� */
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1); /* �w�i */
    wc.lpszMenuName     = pAppName;
    wc.lpszClassName    = pAppName;

    if( !RegisterClass(&wc) )
    {
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
        return FALSE;
    }

    mainWndData.hMenu = CreateMenu();

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_NEW       , TEXT("�V�K(&N)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_OPEN      , TEXT("�J��(&O)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE      , TEXT("�㏑���ۑ�(&S)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_SAVE_AS   , TEXT("���O��t���ĕۑ�(&A)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                  , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PAGE      , TEXT("�y�[�W�ݒ�(&U)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_PRINT     , TEXT("���(&P)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                  , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_FILE_EXIT      , TEXT("�I��(&X)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�t�@�C��(&F)") );

    EnableMenuItem( mainWndData.hMenu, IDM_FILE_SAVE      , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_FILE_SAVE_AS   , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_FILE_PAGE      , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_FILE_PRINT     , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_UNDO       , TEXT("���ɖ߂�(&U)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_CUT        , TEXT("�؂���(&T)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_COPY       , TEXT("�R�s�[(&C)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_PASTE      , TEXT("�\��t��(&P)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DELETE     , TEXT("�폜(&L)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND       , TEXT("����(&F)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_FIND_NEXT  , TEXT("��������(&N)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_REPLACE    , TEXT("�u��(&R)...") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_GOTO_LINE  , TEXT("�s�ֈړ�(&G)...") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0                   , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_SELECT_ALL , TEXT("�S�đI��(&A)") );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_EDIT_DATETIME   , TEXT("���t�Ǝ���(&D)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�ҏW(&E)") );

    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_UNDO       , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_CUT        , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_COPY       , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_PASTE      , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_DELETE     , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_FIND       , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_FIND_NEXT  , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_REPLACE    , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_GOTO_LINE  , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_SELECT_ALL , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_EDIT_DATETIME   , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_WRAP , TEXT("�E�[�Ő܂�Ԃ�(&W)") );
    AppendMenu( hMenuPopup, MF_STRING, IDM_FORMAT_FONT , TEXT("�t�H���g(&F)...") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("����(&O)") );

    EnableMenuItem( mainWndData.hMenu, IDM_FORMAT_WRAP , MF_GRAYED );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING, IDM_VIEW_STS_BAR, TEXT("�X�e�[�^�X �o�[(&S)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�\��(&V)") );

    hMenuPopup = CreateMenu();
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_HELP  , TEXT("�g�s�b�N�̌���(&H)") );
    AppendMenu( hMenuPopup, MF_SEPARATOR, 0              , NULL );
    AppendMenu( hMenuPopup, MF_STRING   , IDM_HELP_ABOUT , TEXT("�o�[�W�������(&A)") );
    AppendMenu( mainWndData.hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("�w���v(&H)") );

    EnableMenuItem( mainWndData.hMenu, IDM_HELP_HELP  , MF_GRAYED );
    EnableMenuItem( mainWndData.hMenu, IDM_HELP_ABOUT , MF_GRAYED );

    /* ���C���E�B���h�E���쐬 */
    InitCommonControls(); /* commctrl.h�̃C���N���[�h�Acomctl32.lib�̃v���W�F�N�g�ւ̎Q�����K�v */
    hwndMain = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                               pAppName, pAppName,
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               WND_WIDTH    , WND_HEIGHT,
                               NULL, mainWndData.hMenu, hInst, NULL);

    if( hwndMain == NULL )
    {
        return FALSE;
    }

    ShowWindow( hwndMain, nCmdShow ); /* ->WM_SIZE   */ /* �E�C���h�E��\�� */
    UpdateWindow( hwndMain );         /* ->WM_PAINT  */

    return TRUE;
}

/********************************************************************************
 * ���e  : ���C���E�B���h�E���ŏ������郁�b�Z�[�W�𔻒肷��
 * ����  : MSG *msg
 * �߂�l: BOOL
 ***************************************/
BOOL
IsMainWndMessage( MSG *msg )
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
WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return wndProcTbl[convertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * �֐��� : ConvertMSGtoINDEX()
 * ����   : UINT message
 * �߂�l : WNDPRC_INDEX
 * ���e   : �E�B���h�E�Y���b�Z�[�W����Ή�����C���f�b�N�X�ɕϊ�����
 ***************************************/
static MAINWND_INDEX
convertMSGtoINDEX( UINT message )
{
    MAINWND_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE       :rtn = MAINWND_ON_CREATE       ;break;
    case WM_PAINT        :rtn = MAINWND_ON_PAINT        ;break;
    case WM_SIZE         :rtn = MAINWND_ON_SIZE         ;break;
    case WM_CLOSE        :rtn = MAINWND_ON_CLOSE        ;break;
    case WM_DESTROY      :rtn = MAINWND_ON_DESTROY      ;break;
    case WM_COMMAND      :rtn = MAINWND_ON_COMMAND      ;break;
    case WM_APP          :rtn = MAINWND_ON_APP          ;break;
    case WM_KEYUP        :rtn = MAINWND_ON_KEYUP        ;break;
    case WM_KEYDOWN      :rtn = MAINWND_ON_KEYDOWN      ;break;
    case WM_CHAR         :rtn = MAINWND_ON_CHAR         ;break;
    case WM_HSCROLL      :rtn = MAINWND_ON_HSCROLL      ;break;
    case WM_VSCROLL      :rtn = MAINWND_ON_VSCROLL      ;break;
    case WM_MOUSEWHEEL   :rtn = MAINWND_ON_MOUSEWHEEL   ;break;
    case WM_SETFOCUS     :rtn = MAINWND_ON_SETFOCUS     ;break;
    case WM_KILLFOCUS    :rtn = MAINWND_ON_KILLFOCUS    ;break;
    case WM_DROPFILES    :rtn = MAINWND_ON_DROPFILES    ;break;
    default              :rtn = MAINWND_ON_DEFAULT      ;break;
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
onCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    HFONT hFont,hOldFont;

    hdc = GetDC( hwnd );
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    mainWndData.cxChar = tm.tmAveCharWidth;
    mainWndData.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );


    FileInitialize( hwnd ); /* �t�@�C��������     */
    FontInit();

    mainWndData.hWndIo = IoWndCreate( hwnd, FontGetLogFont(FONT_ID_IO) );
#if 0
    SomeCtrlCreate( hwnd ); /* �R���g���[���𐶐� */
#endif
    StsBarCreate( hwnd, TRUE ); /* �X�e�[�^�X�o�[�����A�f�t�H���g�\�� */
    CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_CHECKED );

    doCaption( hwnd, "" );

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
onPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
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
onSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LONG topSizeSum=0,bottomSizeSum=0;

    mainWndData.cxClient = LOWORD( lParam );
    mainWndData.cyClient = HIWORD( lParam );

#if 0
    topSizeSum += SomeCtrlSize( mainWndData.cxClient, mainWndData.cyChar ); /* �R���g���[��   */
#endif
    bottomSizeSum += StsBarSize( mainWndData.cxClient, mainWndData.cyChar ); /* �X�e�[�^�X�o�[ */
    IoWndSize( 0, topSizeSum, mainWndData.cxClient, mainWndData.cyClient - topSizeSum - bottomSizeSum );

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
onClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
onDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    IoWndDestroy();
    FileEnd();

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
onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD dwSize;
    PBYTE dataPtr;
    INT iMenuFlags;
    MENUITEMINFO menuItemInfo;

    switch( LOWORD(wParam) )
    {
    case IDM_FILE_NEW:
        IoWndPrint( NULL, 0 );
        doCaption( hwnd, "" );
        break;
    case IDM_FILE_OPEN:
        if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
        {
            dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
            IoWndPrint( dataPtr,dwSize );
            doCaption( hwnd, FileGetTitleName(FILE_ID_BIN) );
        }
        else
        {
            /* �L�����Z�����ꂽ�B���̓G���[ */
        }
        break;
#if 0
    case (SOME_CTRL_FILEOPEN_BUTTON+SOME_CTRL_ID_OFFSET):
        if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
        {
            SetWindowText( SomeCtrlGetHWND(SOME_CTRL_FILENAME), FileGetName(FILE_ID_BIN) );
            dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
            IoWndPrint( dataPtr,dwSize );
        }
        else
        {
            /* �L�����Z�����ꂽ�B���̓G���[ */
        }
        break;
#endif

    case IDM_FORMAT_FONT:
        if( FontChooseFont( hwnd, FONT_ID_IO ) )
        {
            IoWndChangeFont( FontGetLogFont(FONT_ID_IO) );
        }
        else
        {
            nop();
        }
        break;

    case IDM_VIEW_STS_BAR:
#if 1
        iMenuFlags = GetMenuState( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_BYCOMMAND );
        if( iMenuFlags & MF_CHECKED )
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_UNCHECKED );
            StsBarShowWindow( FALSE );
        }
        else
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_CHECKED );
            StsBarShowWindow( TRUE );
        }
#else
        menuItemInfo.cbSize = sizeof(MENUITEMINFO);
        menuItemInfo.fMask  = MIIM_STATE;
        GetMenuItemInfo( mainWndData.hMenu, IDM_VIEW_STS_BAR, FALSE, &menuItemInfo );
        if( menuItemInfo.fState & MFS_CHECKED )
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_UNCHECKED );
            StsBarShowWindow( FALSE );
        }
        else
        {
            CheckMenuItem( mainWndData.hMenu, IDM_VIEW_STS_BAR, MF_CHECKED );
            StsBarShowWindow( TRUE );
        }
#endif
        SendMessage(hwnd,WM_SIZE,0,MAKELONG(mainWndData.cxClient,mainWndData.cyClient));
        break;

    case IDM_FILE_EXIT:
        SendMessage( hwnd, WM_CLOSE, 0, 0 );
        break;
    default:
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
onKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
onKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

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
onChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

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
onHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    return rtn;
}

/********************************************************************************
 * ���e  : WM_VSCROLL �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

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
onMouseWheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(mainWndData.hWndIo,message,wParam,lParam);

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
onSetFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SetFocus(mainWndData.hWndIo);

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
onKillFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(mainWndData.hWndIo,message,wParam,lParam);

    return rtn;
}

/********************************************************************************
 * ���e  : WM_DROPFILES ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onDropFiles( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    static TCHAR szFileName[1024];
    DWORD dwSize;
    PBYTE dataPtr;

    DragQueryFile( wParam, 0, szFileName, sizeof(szFileName) );

    FileSetName( FILE_ID_BIN, szFileName, FALSE );
    dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
    IoWndPrint( dataPtr,dwSize );
    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN) );

    return rtn;
}

/********************************************************************************
 * ���e  : WM_APP �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
 * ���e  : ���b�Z�[�W�̃f�t�H���g����
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * ���e  :
 * ����  : HWND hwnd
 * ����  : TCHAR* szTitleName
 * �߂�l: �Ȃ�
 ***************************************/
void
doCaption( HWND hwnd, TCHAR* szTitleName )
{
     TCHAR szCaption[64 + MAX_PATH];

     wsprintf( szCaption, TEXT ("%s - %s"), (szTitleName[0] ? szTitleName : TEXT("����")),GetAppName() );

     SetWindowText( hwnd, szCaption );
}
