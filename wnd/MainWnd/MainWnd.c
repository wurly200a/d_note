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
#include "Config.h"
#include "DateTime.h"
#include "ModalDlg.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "MainWnd.h"
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static MAINWND_INDEX convertMSGtoINDEX( UINT message );
static LRESULT onCreate          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onPaint           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSize            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMove            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onClose           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDestroy         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onCommand         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyUp           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKeyDown         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onChar            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onHscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onVscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onMouseWheel      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onSetFocus        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onKillFocus       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDropFiles       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onInitMenuPopup   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

void doCaption( HWND hwnd, TCHAR* szTitleName );

/* �����ϐ���` */
static HWND hwndMain; /* ���C���E�B���h�E�̃n���h�� */
static S_MAINWND_DATA mainWndData;

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[MAINWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate          , /* WM_CREATE           */
    onPaint           , /* WM_PAINT            */
    onSize            , /* WM_SIZE             */
    onMove            , /* WM_MOVE             */
    onWindowPosChanged, /* WM_WINDOWPOSCHANGED */
    onClose           , /* WM_CLOSE            */
    onDestroy         , /* WM_DESTROY          */
    onCommand         , /* WM_COMMAND          */
    onKeyUp           , /* WM_KEYUP            */
    onKeyDown         , /* WM_KEYDOWN          */
    onChar            , /* WM_CHAR             */
    onHscroll         , /* WM_HSCROLL          */
    onVscroll         , /* WM_VSCROLL          */
    onMouseWheel      , /* WM_MOUSEWHEEL       */
    onSetFocus        , /* WM_SETFOCUS         */
    onKillFocus       , /* WM_KILLFOCUS        */
    onDropFiles       , /* WM_DROPFILES        */
    onInitMenuPopup   , /* WM_INITMENUPOPUP    */
    onApp             , /* WM_APP              */
    onDefault           /* default             */
};
/* *INDENT-ON* */

/********************************************************************************
 * ���e  : ���C���E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : int nCmdShow
 * ����  : HACCEL *hAccelPtr
 * �߂�l: HWND
 ***************************************/
HWND
MainWndCreate( int nCmdShow, HACCEL *hAccelPtr )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = GetAppName();
    HMENU hMenu = NULL;

    hwndMain = NULL;

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
    { /* �E�B���h�E�N���X�̓o�^���s */
        MessageBox( NULL, TEXT("This program requires Windows 2000!"), pAppName, MB_ICONERROR );
    }
    else
    {
        /* �ݒ�̏����� */
        ConfigInit();

        /* ���j���[�̐��� */
        hMenu = MenuCreate();

        /* �A�N�Z�����[�^�̐��� */
        if( hAccelPtr != NULL )
        {
            *(hAccelPtr) = AccelCreate();
        }
        else
        {
            nop();
        }

        /* ���C���E�B���h�E���쐬 */
        InitCommonControls(); /* commctrl.h�̃C���N���[�h�Acomctl32.lib�̃v���W�F�N�g�ւ̎Q�����K�v */
        hwndMain = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                                   pAppName, pAppName,
                                   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_X) , ConfigLoadDword(CONFIG_ID_WINDOW_POS_Y) ,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_DX), ConfigLoadDword(CONFIG_ID_WINDOW_POS_DY),
                                   NULL, hMenu, hInst, NULL );

        if( hwndMain == NULL )
        { /* �E�B���h�E�̐������s */
            nop();
        }
        else
        {
            ShowWindow( hwndMain, nCmdShow ); /* ->WM_SIZE   */ /* �E�C���h�E��\�� */
            UpdateWindow( hwndMain );         /* ->WM_PAINT  */
        }
    }

    return hwndMain;
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
    case WM_CREATE          :rtn = MAINWND_ON_CREATE          ;break;
    case WM_PAINT           :rtn = MAINWND_ON_PAINT           ;break;
    case WM_SIZE            :rtn = MAINWND_ON_SIZE            ;break;
    case WM_MOVE            :rtn = MAINWND_ON_MOVE            ;break;
    case WM_WINDOWPOSCHANGED:rtn = MAINWND_ON_WINDOWPOSCHANGED;break;
    case WM_CLOSE           :rtn = MAINWND_ON_CLOSE           ;break;
    case WM_DESTROY         :rtn = MAINWND_ON_DESTROY         ;break;
    case WM_COMMAND         :rtn = MAINWND_ON_COMMAND         ;break;
    case WM_APP             :rtn = MAINWND_ON_APP             ;break;
    case WM_KEYUP           :rtn = MAINWND_ON_KEYUP           ;break;
    case WM_KEYDOWN         :rtn = MAINWND_ON_KEYDOWN         ;break;
    case WM_CHAR            :rtn = MAINWND_ON_CHAR            ;break;
    case WM_HSCROLL         :rtn = MAINWND_ON_HSCROLL         ;break;
    case WM_VSCROLL         :rtn = MAINWND_ON_VSCROLL         ;break;
    case WM_MOUSEWHEEL      :rtn = MAINWND_ON_MOUSEWHEEL      ;break;
    case WM_SETFOCUS        :rtn = MAINWND_ON_SETFOCUS        ;break;
    case WM_KILLFOCUS       :rtn = MAINWND_ON_KILLFOCUS       ;break;
    case WM_DROPFILES       :rtn = MAINWND_ON_DROPFILES       ;break;
    case WM_INITMENUPOPUP   :rtn = MAINWND_ON_INITMENUPOPUP   ;break;
    default                 :rtn = MAINWND_ON_DEFAULT         ;break;
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

    ModalDlgInit();
    FileInitialize( hwnd ); /* �t�@�C��������     */
    FontInit();

    mainWndData.hWndIo = IoWndCreate( hwnd, FontGetLogFont(FONT_ID_IO) );
#if 0
    SomeCtrlCreate( hwnd ); /* �R���g���[���𐶐� */
#endif
    StsBarCreate( hwnd, TRUE ); /* �X�e�[�^�X�o�[�����A�f�t�H���g�\�� */

    MenuCheckItem( IDM_VIEW_STS_BAR );
    MenuCheckItem( IDM_EXTEND_NEWLINE_CRLF );

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
 * ���e  : WM_MOVE �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onMove( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    mainWndData.xPos = LOWORD( lParam );
    mainWndData.yPos = HIWORD( lParam );

    return 0;
}

/********************************************************************************
 * ���e  : WM_WINDOWPOSCHANGED �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    mainWndData.xWindowPos = ((WINDOWPOS *)lParam)->x ;
    mainWndData.yWindowPos = ((WINDOWPOS *)lParam)->y ;
    mainWndData.cxWindow   = ((WINDOWPOS *)lParam)->cx;
    mainWndData.cyWindow   = ((WINDOWPOS *)lParam)->cy;

    return DefWindowProc( hwnd, message, wParam, lParam );
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
    ConfigSaveDword( CONFIG_ID_WINDOW_POS_X , mainWndData.xWindowPos );
    ConfigSaveDword( CONFIG_ID_WINDOW_POS_Y , mainWndData.yWindowPos );
    ConfigSaveDword( CONFIG_ID_WINDOW_POS_DX, mainWndData.cxWindow   );
    ConfigSaveDword( CONFIG_ID_WINDOW_POS_DY, mainWndData.cyWindow   );

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
    DestroyAcceleratorTable( mainWndData.hAccel );

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
    DWORD dwSize,dwSize2;
    PBYTE dataPtr;
    HGLOBAL hGlobal;
    PTSTR   pGlobal;
    S_MODAL_DLG_DATA modalDlgData;

    switch( LOWORD(wParam) )
    {
    case IDM_FILE_NEW:
        IoWndDataInit();
        doCaption( hwnd, "" );
        break;
    case IDM_FILE_OPEN:
        if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
        {
            dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
            IoWndDataSet( dataPtr,dwSize,TRUE );
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
            IoWndDataSet( dataPtr,dwSize,TRUE );
        }
        else
        {
            /* �L�����Z�����ꂽ�B���̓G���[ */
        }
        break;
#endif

    case IDM_FILE_SAVE:
        dwSize = IoWndGetDataSize(IOWND_ALL);
        dataPtr = malloc( dwSize * sizeof(TCHAR) );
        if( dataPtr != NULL )
        {
            IoWndDataGet( dataPtr,dwSize,IOWND_ALL );
            if( (FileWrite( FILE_ID_BIN, dataPtr, dwSize )) == FILE_NAME_NOT_SET )
            {
                if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                {
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN) );
                    FileWrite( FILE_ID_BIN, dataPtr, dwSize );
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
            free( dataPtr );
        }
        else
        {
            nop();
        }
        break;
    case IDM_FILE_SAVE_AS:
        dwSize = IoWndGetDataSize(IOWND_ALL);
        dataPtr = malloc( dwSize * sizeof(TCHAR) );
        if( dataPtr != NULL )
        {
            IoWndDataGet( dataPtr,dwSize,IOWND_ALL );
            if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
            {
                doCaption( hwnd, FileGetTitleName(FILE_ID_BIN) );
                FileWrite( FILE_ID_BIN, dataPtr, dwSize );
            }
            else
            {
                nop();
            }
            free( dataPtr );
        }
        else
        {
            nop();
        }
        break;

    case IDM_EDIT_CUT:
    case IDM_EDIT_COPY:
        dwSize = IoWndGetDataSize(IOWND_SELECTED);
        if( dwSize )
        {
            hGlobal = GlobalAlloc( GHND|GMEM_SHARE, dwSize+1 );
            pGlobal = GlobalLock( hGlobal );
            GlobalUnlock(hGlobal);
            IoWndDataGet( pGlobal,dwSize,IOWND_SELECTED );
            OpenClipboard(hwnd);
            EmptyClipboard();
            SetClipboardData( CF_TEXT, hGlobal );
            CloseClipboard();

            if( LOWORD(wParam) == IDM_EDIT_CUT )
            {
                IoWndSelectDataRemove();
                IoWndInvalidateRect(TRUE);
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
        break;

    case IDM_EDIT_PASTE:
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
        break;

    case IDM_EDIT_DELETE:
        IoWndSelectDataRemove();
        IoWndInvalidateRect(TRUE);
        break;

    case IDM_EDIT_SELECT_ALL:
        if( IoWndSelectAll() )
        {
            IoWndInvalidateRect(TRUE);
        }
        else
        {
            nop();
        }
        break;

    case IDM_EDIT_DATETIME:
        pGlobal = DateTimeGetString();
        IoWndDataSet( pGlobal, strlen(pGlobal), FALSE );
        break;

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
        if( MenuInqItemChecked(IDM_VIEW_STS_BAR) )
        {
            MenuUnCheckItem( IDM_VIEW_STS_BAR );
            StsBarShowWindow( FALSE );
        }
        else
        {
            MenuCheckItem( IDM_VIEW_STS_BAR );
            StsBarShowWindow( TRUE );
        }
        SendMessage(hwnd,WM_SIZE,0,MAKELONG(mainWndData.cxClient,mainWndData.cyClient));
        break;

    case IDM_EXTEND_NEWLINE_CRLF:
        if( IoWndNewLineCodeSet(NEWLINECODE_CRLF) )
        {
            IoWndInvalidateRect(TRUE);
        }
        else
        {
            nop();
        }
        MenuCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
        MenuUnCheckItem( IDM_EXTEND_NEWLINE_LF   );
        MenuUnCheckItem( IDM_EXTEND_NEWLINE_CR   );
        break;

    case IDM_EXTEND_NEWLINE_LF  :
        if( IoWndNewLineCodeSet(NEWLINECODE_LF) )
        {
            IoWndInvalidateRect(TRUE);
        }
        else
        {
            nop();
        }
        MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
        MenuCheckItem    ( IDM_EXTEND_NEWLINE_LF   );
        MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CR   );
        break;

    case IDM_EXTEND_NEWLINE_CR  :
        if( IoWndNewLineCodeSet(NEWLINECODE_CR) )
        {
            IoWndInvalidateRect(TRUE);
        }
        else
        {
            nop();
        }
        MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
        MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_LF   );
        MenuCheckItem    ( IDM_EXTEND_NEWLINE_CR   );
        break;

    case IDM_FILE_EXIT:
        SendMessage( hwnd, WM_CLOSE, 0, 0 );
        break;

    case IDM_HELP_ABOUT:
        ModalDlg( MODAL_DLG_ID_ABOUT, &modalDlgData, hwnd, mainWndData.xPos, mainWndData.yPos );
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
    IoWndDataSet( dataPtr,dwSize,TRUE );
    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN) );

    return rtn;
}

/********************************************************************************
 * ���e  : WM_INITMENUPOPUP ����������
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onInitMenuPopup( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    if( LOWORD(lParam) == 1 )
    { /* �u�ҏW�v */

        if( IoWndGetDataSize(IOWND_SELECTED) )
        {
            MenuEnableItem( IDM_EDIT_CUT    );
            MenuEnableItem( IDM_EDIT_COPY   );
            MenuEnableItem( IDM_EDIT_DELETE );
        }
        else
        {
            MenuUnEnableItem( IDM_EDIT_CUT    );
            MenuUnEnableItem( IDM_EDIT_COPY   );
            MenuUnEnableItem( IDM_EDIT_DELETE );
        }

        if( IsClipboardFormatAvailable(CF_TEXT) )
        {
            MenuEnableItem( IDM_EDIT_PASTE );
        }
        else
        {
            MenuUnEnableItem( IDM_EDIT_PASTE );
        }
    }
    else
    {
        nop();
    }

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
