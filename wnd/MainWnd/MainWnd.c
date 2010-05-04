/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "MainWndDef.h"
#include "MenuId.h"
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
static LRESULT onFindMsgString   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onApp             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT onDefault         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

void doCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave );
short AskAboutSave( HWND hwnd, TCHAR * szTitleName );

/* �����ϐ���` */
static HWND hwndMain; /* ���C���E�B���h�E�̃n���h�� */
static S_MAINWND_DATA mainWndData;

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[MAINWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    onCreate          , /* WM_CREATE                     */
    onPaint           , /* WM_PAINT                      */
    onSize            , /* WM_SIZE                       */
    onMove            , /* WM_MOVE                       */
    onWindowPosChanged, /* WM_WINDOWPOSCHANGED           */
    onClose           , /* WM_CLOSE                      */
    onDestroy         , /* WM_DESTROY                    */
    onCommand         , /* WM_COMMAND                    */
    onKeyUp           , /* WM_KEYUP                      */
    onKeyDown         , /* WM_KEYDOWN                    */
    onChar            , /* WM_CHAR                       */
    onHscroll         , /* WM_HSCROLL                    */
    onVscroll         , /* WM_VSCROLL                    */
    onMouseWheel      , /* WM_MOUSEWHEEL                 */
    onSetFocus        , /* WM_SETFOCUS                   */
    onKillFocus       , /* WM_KILLFOCUS                  */
    onDropFiles       , /* WM_DROPFILES                  */
    onInitMenuPopup   , /* WM_INITMENUPOPUP              */
    onFindMsgString   , /* FINDMSGSTRING�̓o�^���b�Z�[�W */
    onApp             , /* WM_APP                        */
    onDefault           /* default                       */
};
/* *INDENT-ON* */

#if 0
#define USE_EDITCONTROL
#endif

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
 * ����  : MSG *msgPtr
 * �߂�l: BOOL
 ***************************************/
BOOL
IsMainWndMessage( MSG *msgPtr )
{
    BOOL bRtn = FALSE;

    if( (mainWndData.hDlgModeless) &&                      /* ���[�h���X�_�C�A���O�{�b�N�X�\�����ŁA */
        IsDialogMessage(mainWndData.hDlgModeless,msgPtr) ) /* �{���b�Z�[�W�����[�h���X�_�C�A���O�{�b�N�X�̃E�B���h�E�v���V�[�W���ŏ������ꂽ */
    {
        bRtn = TRUE;
    }
    else
    {
        nop();
    }

    return bRtn;
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
    default                 :
        if( message == mainWndData.messageFindReplace )
        {
            rtn = MAINWND_ON_FINDMSGSTRING;
        }
        else
        {
            rtn = MAINWND_ON_DEFAULT;
        }
        break;
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

    mainWndData.messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

    ModalDlgInit();
    FileInitialize( hwnd ); /* �t�@�C��������     */
    FontInit();

#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
    mainWndData.hWndIo = IoWndCreate( GetHinst(), hwnd );
#else                   /* [�G�f�B�b�g�R���g���[���g�p] or  �ʏ�  */
    mainWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT ("edit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                         WS_BORDER | ES_LEFT | ES_MULTILINE |
                                         ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                         0, 0, 0, 0,
                                         hwnd, (HMENU)0, GetHinst(), NULL) ;
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */

#if 0
    SomeCtrlCreate( hwnd ); /* �R���g���[���𐶐� */
#endif
    StsBarCreate( hwnd, TRUE ); /* �X�e�[�^�X�o�[�����A�f�t�H���g�\�� */

    MenuCheckItem( IDM_VIEW_STS_BAR );
    MenuCheckItem( IDM_EXTEND_NEWLINE_CRLF );

    doCaption( hwnd, "" , FALSE );

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

    MoveWindow( mainWndData.hWndIo, 0, topSizeSum, mainWndData.cxClient, mainWndData.cyClient - topSizeSum - bottomSizeSum, TRUE) ;

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
    if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_X , mainWndData.xWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_Y , mainWndData.yWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DX, mainWndData.cxWindow   );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DY, mainWndData.cyWindow   );

        DestroyWindow( hwnd );
    }

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

    DestroyWindow( mainWndData.hWndIo );
    FileEnd();

    PostQuitMessage(0); /* WM_QUIT���b�Z�[�W���|�X�g���� */
    return 0;
}

/********************************************************************************
 * ���e  : WM_COMMAND �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam �ʏ��LOWORD���ʒm�R�[�h�B(�R���g���[������̒ʒm����HIWORD���ʒm�R�[�h�ALOWORD���R���g���[��ID)
 * ����  : LPARAM lParam �ʏ��NULL�B(�R���g���[������̒ʒm���̓E�B���h�E�n���h��)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD dwSize;
    PBYTE dataPtr;
    PTSTR strPtr;
    S_MODAL_DLG_DATA modalDlgData;
    static FINDREPLACE fr;
    static TCHAR strFind[80],strRep[80],strMsg[1024];

    if( (HWND)lParam == mainWndData.hWndIo )
    {
        switch( HIWORD(wParam) )
        {
        case EN_UPDATE:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            StsBarSetText( STS_BAR_0  ,"%d �o�C�g�A�S %d �s",IoWndGetDataSize(mainWndData.hWndIo, IOWND_ALL),IoWndGetLineMaxSize(mainWndData.hWndIo) );
            StsBarSetText( STS_BAR_MAX,"   %d �s�A%d ��",IoWndGetCaretYpos(mainWndData.hWndIo)+1,IoWndGetCaretXpos(mainWndData.hWndIo)+1);
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
            break;
        case EN_CHANGE:
            if( mainWndData.bNeedSave )
            {
                nop();
            }
            else
            {
                doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),TRUE );
                mainWndData.bNeedSave = TRUE;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch( LOWORD(wParam) )
        {
        case IDM_FILE_NEW:
            if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                mainWndData.bNeedSave = FALSE;
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
                IoWndDataInit(mainWndData.hWndIo);
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
                doCaption( hwnd, "", FALSE);
            }
            break;
        case IDM_FILE_OPEN:
            if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
                {
                    mainWndData.bNeedSave = FALSE;
                    dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
                    IoWndDataSet( mainWndData.hWndIo, dataPtr,dwSize,TRUE );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN), FALSE );
                }
                else
                {
                    /* �L�����Z�����ꂽ�B���̓G���[ */
                }
            }
            break;
        case IDM_FILE_SAVE:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            dwSize = IoWndGetDataSize(mainWndData.hWndIo,IOWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                IoWndDataGet( mainWndData.hWndIo, dataPtr,dwSize,IOWND_ALL );
                if( (FileWrite( FILE_ID_BIN, dataPtr, dwSize )) == FILE_NAME_NOT_SET )
                {
                    if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                    {
                        mainWndData.bNeedSave = FALSE;
                        doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                        FileWrite( FILE_ID_BIN, dataPtr, dwSize );
                        rtn = 1;
                    }
                    else
                    { /* �L�����Z�� */
                        nop();
                    }
                }
                else
                {
                    mainWndData.bNeedSave = FALSE;
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
                    FileWrite( FILE_ID_BIN, dataPtr, dwSize );
                    rtn = 1;
                }
                free( dataPtr );
            }
            else
            {
                nop();
            }
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
            break;
        case IDM_FILE_SAVE_AS:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            dwSize = IoWndGetDataSize(mainWndData.hWndIo,IOWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                IoWndDataGet( mainWndData.hWndIo, dataPtr,dwSize,IOWND_ALL );
                if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                {
                    mainWndData.bNeedSave = FALSE;
                    doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
            break;

        case IDM_EDIT_CUT:
            SendMessage( mainWndData.hWndIo, WM_CUT, 0, 0 );
            break;

        case IDM_EDIT_COPY:
            SendMessage( mainWndData.hWndIo, WM_COPY, 0, 0 );
            break;

        case IDM_EDIT_PASTE:
            SendMessage( mainWndData.hWndIo, WM_PASTE, 0, 0 );
            break;

        case IDM_EDIT_DELETE:
            SendMessage( mainWndData.hWndIo, WM_CLEAR, 0, 0 );
            break;

        case IDM_EDIT_FIND:
            fr.lStructSize   = sizeof (FINDREPLACE);
            fr.hwndOwner     = hwnd;
            fr.Flags         = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat = strFind;
            fr.wFindWhatLen  = 80;
            mainWndData.hDlgModeless = FindText(&fr);
            break;

        case IDM_EDIT_REPLACE:
            fr.lStructSize      = sizeof(FINDREPLACE);
            fr.hwndOwner        = hwnd;
            fr.Flags            = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat    = strFind;
            fr.lpstrReplaceWith = strRep;
            fr.wReplaceWithLen  = fr.wFindWhatLen = 80;
            mainWndData.hDlgModeless = ReplaceText(&fr);
            break;

        case IDM_EDIT_FIND_NEXT:
            break;

        case IDM_EDIT_SELECT_ALL:
            SendMessage( mainWndData.hWndIo, EM_SETSEL, 0, -1 );
            break;

        case IDM_EDIT_DATETIME:
            strPtr = DateTimeGetString();
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            IoWndDataSet( mainWndData.hWndIo, strPtr, strlen(strPtr), FALSE );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
            break;

        case IDM_FORMAT_FONT:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            if( FontChooseFont( hwnd, FONT_ID_IO ) )
            {
                mainWndData.hFontIo = CreateFontIndirect( FontGetLogFont(FONT_ID_IO) );
                IoWndChangeFont( mainWndData.hWndIo, mainWndData.hFontIo );
            }
            else
            {
                nop();
            }
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
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

#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
        case IDM_EXTEND_NEWLINE_CRLF:
            IoWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_CRLF);
            MenuCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuUnCheckItem( IDM_EXTEND_NEWLINE_LF   );
            MenuUnCheckItem( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR+LF");
            break;

        case IDM_EXTEND_NEWLINE_LF  :
            IoWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_LF);
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuCheckItem    ( IDM_EXTEND_NEWLINE_LF   );
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"LF");
            break;

        case IDM_EXTEND_NEWLINE_CR  :
            IoWndNewLineCodeSet(mainWndData.hWndIo,NEWLINECODE_CR);
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_CRLF );
            MenuUnCheckItem  ( IDM_EXTEND_NEWLINE_LF   );
            MenuCheckItem    ( IDM_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR");
            break;
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */

        case IDM_FILE_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;

        case IDM_HELP_ABOUT:
            ModalDlg( MODAL_DLG_ID_ABOUT, &modalDlgData, hwnd, mainWndData.xPos, mainWndData.yPos );
            break;

        default:
            break;
        }
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

    if( mainWndData.bNeedSave && ((AskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        mainWndData.bNeedSave = FALSE;
        DragQueryFile( (HDROP)wParam, (UINT)0, (LPSTR)szFileName, (UINT)sizeof(szFileName) );

        FileSetName( FILE_ID_BIN, szFileName, FALSE );
        dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
        IoWndDataSet( mainWndData.hWndIo,dataPtr,dwSize,TRUE );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
        doCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
    }

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
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
        if( IoWndGetDataSize(mainWndData.hWndIo,IOWND_SELECTED) )
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
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */

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
 * ���e  : FINDMSGSTRING�̓o�^���b�Z�[�W �̏���
 * ����  : HWND hwnd
 * ����  : UINT message
 * ����  : WPARAM wParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * ����  : LPARAM lParam (���e�̓��b�Z�[�W�̎�ނɂ��قȂ�)
 * �߂�l: LRESULT
 ***************************************/
static LRESULT
onFindMsgString( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    LPFINDREPLACE pfr;

    pfr = (LPFINDREPLACE)lParam;

    if( pfr->Flags & FR_DIALOGTERM )
    {
        mainWndData.hDlgModeless = NULL;
    }
    else
    {
        nop();
    }

#if 0
    if( (pfr->Flags & FR_FINDNEXT) && !PopFindFindText(hwndEdit,&iOffset,pfr) )
    {
        OkMessage(hwnd, TEXT ("Text not found!"),TEXT ("\0")) ;
    }
    else
    {
        nop();
    }

    if( ((pfr->Flags & FR_REPLACE) || (pfr->Flags & FR_REPLACEALL)) && (!PopFindReplaceText(hwndEdit,&iOffset,pfr)) )
    {
        OkMessage (hwnd, TEXT ("Text not found!"),TEXT ("\0")) ;
    }
    else
    {
        nop();
    }

    if( pfr->Flags & FR_REPLACEALL )
    {
        while( PopFindReplaceText(hwndEdit, &iOffset, pfr) );
    }
    else
    {
        nop();
    }
#endif

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
 * ����  : BOOL bNeedSave
 * �߂�l: �Ȃ�
 ***************************************/
void
doCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave )
{
     TCHAR szCaption[64 + MAX_PATH];

     if( bNeedSave )
     {
         wsprintf( szCaption, TEXT ("*%s - %s"), (szTitleName[0] ? szTitleName : TEXT("����")),GetAppName() );
     }
     else
     {
         wsprintf( szCaption, TEXT ("%s - %s"), (szTitleName[0] ? szTitleName : TEXT("����")),GetAppName() );
     }

     SetWindowText( hwnd, szCaption );
}

/********************************************************************************
 * ���e  :
 * ����  : HWND hwnd
 * ����  : TCHAR* szTitleName
 * �߂�l: �Ȃ�
 ***************************************/
short
AskAboutSave( HWND hwnd, TCHAR * szTitleName )
{
    TCHAR szBuffer[64 + MAX_PATH];
    int   iReturn;

    wsprintf(szBuffer, TEXT("�t�@�C�� %s �̓��e�͕ύX����Ă��܂��B\n\n�ύX��ۑ����܂���?"), szTitleName[0] ? szTitleName : TEXT("����") );

    iReturn = MessageBox( hwnd,szBuffer,GetAppName(),MB_YESNOCANCEL|MB_ICONEXCLAMATION );

    if( iReturn == IDYES )
    {
        if( !SendMessage( hwnd,WM_COMMAND,IDM_FILE_SAVE,0) )
        {
            iReturn = IDCANCEL;
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

    return iReturn;
}
