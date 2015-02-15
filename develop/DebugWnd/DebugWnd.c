/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */
#include "DebugWndDef.h"
#include "DebugMenuId.h"
#include "DebugWndMenu.h"

/* �O���֐���` */
#include "WinMain.h"
#include "EditWnd.h"
#include "SomeCtrl.h"
#include "File.h"
#include "StsBar.h"
#include "Font.h"
#include "Config.h"
#include "DateTime.h"
#include "ModalDlg.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "DebugWnd.h"
LRESULT CALLBACK DebugWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static DEBUGWND_INDEX debugConvertMSGtoINDEX( UINT message );
static LRESULT debugOnCreate          ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnPaint           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnSize            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnMove            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnClose           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnDestroy         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnCommand         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnKeyUp           ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnKeyDown         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnChar            ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnHscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnVscroll         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnMouseWheel      ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnSetFocus        ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnKillFocus       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnDropFiles       ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnInitMenuPopup   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnFindMsgString   ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnApp             ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static LRESULT debugOnDefault         ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

static PTSTR getModuleString( void );
void debugDoCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave );
short debugAskAboutSave( HWND hwnd, TCHAR * szTitleName );

/* �����ϐ���` */
static HWND hDebugWnd; /* �f�o�b�O�E�B���h�E�̃n���h�� */
static S_DEBUGWND_DATA debugWndData;
static TCHAR szCmdLineLocal[1024];

/* *INDENT-OFF* */
static LRESULT (*wndProcTbl[DEBUGWND_MAX])( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) =
{
    debugOnCreate          , /* WM_CREATE                     */
    debugOnPaint           , /* WM_PAINT                      */
    debugOnSize            , /* WM_SIZE                       */
    debugOnMove            , /* WM_MOVE                       */
    debugOnWindowPosChanged, /* WM_WINDOWPOSCHANGED           */
    debugOnClose           , /* WM_CLOSE                      */
    debugOnDestroy         , /* WM_DESTROY                    */
    debugOnCommand         , /* WM_COMMAND                    */
    debugOnKeyUp           , /* WM_KEYUP                      */
    debugOnKeyDown         , /* WM_KEYDOWN                    */
    debugOnChar            , /* WM_CHAR                       */
    debugOnHscroll         , /* WM_HSCROLL                    */
    debugOnVscroll         , /* WM_VSCROLL                    */
    debugOnMouseWheel      , /* WM_MOUSEWHEEL                 */
    debugOnSetFocus        , /* WM_SETFOCUS                   */
    debugOnKillFocus       , /* WM_KILLFOCUS                  */
    debugOnDropFiles       , /* WM_DROPFILES                  */
    debugOnInitMenuPopup   , /* WM_INITMENUPOPUP              */
    debugOnFindMsgString   , /* FINDMSGSTRING�̓o�^���b�Z�[�W */
    debugOnApp             , /* WM_APP                        */
    debugOnDefault           /* default                       */
};
/* *INDENT-ON* */

static TCHAR szModuleName[] = TEXT("Debug"); /* �A�v���P�[�V�����̖��� */

#define USE_EDITCONTROL

/********************************************************************************
 * ���e  : �f�o�b�O�E�B���h�E�N���X�̓o�^�A�E�B���h�E�̐���
 * ����  : LPSTR szCmdLine
 * ����  : int nCmdShow
 * ����  : HACCEL *hAccelPtr
 * �߂�l: HWND
 ***************************************/
HWND
DebugWndCreate( LPSTR szCmdLine, int nCmdShow, HACCEL *hAccelPtr )
{
    WNDCLASS wc = {0};
    HINSTANCE hInst = GetHinst();
    PTSTR pAppName = getModuleString();
    HMENU hMenu = NULL;

    hDebugWnd = NULL;

    /* �f�o�b�O�E�B���h�E�N���X */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) DebugWndProc;
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
        hMenu = DebugMenuCreate();

        /* �A�N�Z�����[�^�̐��� */
        if( hAccelPtr != NULL )
        {
            *(hAccelPtr) = DebugAccelCreate();
        }
        else
        {
            nop();
        }

        strncpy(szCmdLineLocal,szCmdLine,1024);

        /* �f�o�b�O�E�B���h�E���쐬 */
        InitCommonControls(); /* commctrl.h�̃C���N���[�h�Acomctl32.lib�̃v���W�F�N�g�ւ̎Q�����K�v */
        hDebugWnd = CreateWindowEx( /* WS_EX_OVERLAPPEDWINDOW | */ WS_EX_ACCEPTFILES,
                                   pAppName, pAppName,
                                   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /* | WS_VSCROLL | WS_HSCROLL*/,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_X) , ConfigLoadDword(CONFIG_ID_WINDOW_POS_Y) ,
                                   ConfigLoadDword(CONFIG_ID_WINDOW_POS_DX), ConfigLoadDword(CONFIG_ID_WINDOW_POS_DY),
                                   NULL, hMenu, hInst, NULL );

        if( hDebugWnd == NULL )
        { /* �E�B���h�E�̐������s */
            nop();
        }
        else
        {
            ShowWindow( hDebugWnd, nCmdShow ); /* ->WM_SIZE   */ /* �E�C���h�E��\�� */
            UpdateWindow( hDebugWnd );         /* ->WM_PAINT  */
        }
    }

    return hDebugWnd;
}

/********************************************************************************
 * ���e  : �f�o�b�O�E�B���h�E���ŏ������郁�b�Z�[�W�𔻒肷��
 * ����  : MSG *msgPtr
 * �߂�l: BOOL
 ***************************************/
BOOL
IsDebugWndMessage( MSG *msgPtr )
{
    BOOL bRtn = FALSE;

    if( (debugWndData.hDlgModeless) &&                      /* ���[�h���X�_�C�A���O�{�b�N�X�\�����ŁA */
        IsDialogMessage(debugWndData.hDlgModeless,msgPtr) ) /* �{���b�Z�[�W�����[�h���X�_�C�A���O�{�b�N�X�̃E�B���h�E�v���V�[�W���ŏ������ꂽ */
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
DebugWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return wndProcTbl[debugConvertMSGtoINDEX(message)]( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * �֐��� : ConvertMSGtoINDEX()
 * ����   : UINT message
 * �߂�l : WNDPRC_INDEX
 * ���e   : �E�B���h�E�Y���b�Z�[�W����Ή�����C���f�b�N�X�ɕϊ�����
 ***************************************/
static DEBUGWND_INDEX
debugConvertMSGtoINDEX( UINT message )
{
    DEBUGWND_INDEX rtn;

    /* *INDENT-OFF* */
    switch( message )
    {
    case WM_CREATE          :rtn = DEBUGWND_ON_CREATE          ;break;
    case WM_PAINT           :rtn = DEBUGWND_ON_PAINT           ;break;
    case WM_SIZE            :rtn = DEBUGWND_ON_SIZE            ;break;
    case WM_MOVE            :rtn = DEBUGWND_ON_MOVE            ;break;
    case WM_WINDOWPOSCHANGED:rtn = DEBUGWND_ON_WINDOWPOSCHANGED;break;
    case WM_CLOSE           :rtn = DEBUGWND_ON_CLOSE           ;break;
    case WM_DESTROY         :rtn = DEBUGWND_ON_DESTROY         ;break;
    case WM_COMMAND         :rtn = DEBUGWND_ON_COMMAND         ;break;
    case WM_APP             :rtn = DEBUGWND_ON_APP             ;break;
    case WM_KEYUP           :rtn = DEBUGWND_ON_KEYUP           ;break;
    case WM_KEYDOWN         :rtn = DEBUGWND_ON_KEYDOWN         ;break;
    case WM_CHAR            :rtn = DEBUGWND_ON_CHAR            ;break;
    case WM_HSCROLL         :rtn = DEBUGWND_ON_HSCROLL         ;break;
    case WM_VSCROLL         :rtn = DEBUGWND_ON_VSCROLL         ;break;
    case WM_MOUSEWHEEL      :rtn = DEBUGWND_ON_MOUSEWHEEL      ;break;
    case WM_SETFOCUS        :rtn = DEBUGWND_ON_SETFOCUS        ;break;
    case WM_KILLFOCUS       :rtn = DEBUGWND_ON_KILLFOCUS       ;break;
    case WM_DROPFILES       :rtn = DEBUGWND_ON_DROPFILES       ;break;
    case WM_INITMENUPOPUP   :rtn = DEBUGWND_ON_INITMENUPOPUP   ;break;
    default                 :
        if( message == debugWndData.messageFindReplace )
        {
            rtn = DEBUGWND_ON_FINDMSGSTRING;
        }
        else
        {
            rtn = DEBUGWND_ON_DEFAULT;
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
debugOnCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HDC        hdc;
    TEXTMETRIC tm;
    HFONT hFont,hOldFont;

    memset( &debugWndData, 0, sizeof(debugWndData) );

    hdc = GetDC( hwnd );
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = SelectObject(hdc, hFont);
    GetTextMetrics( hdc, &tm );
    debugWndData.cxChar = tm.tmAveCharWidth;
    debugWndData.cyChar = tm.tmHeight + (tm.tmHeight/2) + (GetSystemMetrics(SM_CYEDGE) * 2);

    debugWndData.messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC( hwnd,hdc );

    ModalDlgInit();
    FileInitialize( hwnd ); /* �t�@�C��������     */
    FontInit();

#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
    EditWndRegisterClass( GetHinst() );
    debugWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT("teddedit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL ,
                                         CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
                                         hwnd, (HMENU)0, GetHinst(), NULL );
#else                   /* [�G�f�B�b�g�R���g���[���g�p] or  �ʏ�  */
    debugWndData.hWndIo = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW,
                                         TEXT ("edit"), NULL,
                                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                         ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                         0, 0, 0, 0,
                                         hwnd, (HMENU)0, GetHinst(), NULL) ;
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
    debugWndData.hFontIo = NULL;

#if 0
    SomeCtrlCreate( hwnd ); /* �R���g���[���𐶐� */
#endif
    StsBarCreate( hwnd, TRUE ); /* �X�e�[�^�X�o�[�����A�f�t�H���g�\�� */

    DebugMenuCheckItem( IDM_DEBUG_VIEW_STS_BAR );
    DebugMenuCheckItem( IDM_DEBUG_EXTEND_NEWLINE_CRLF );

    if( (szCmdLineLocal[0] != '\0') &&
        (FileSetName(FILE_ID_BIN,szCmdLineLocal,FALSE)) )
    {
        DWORD dwSize;
        PBYTE dataPtr;

        dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
        EditWndDataSet( debugWndData.hWndIo,dataPtr,dwSize,TRUE );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
        debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
    }
    else
    {
        debugDoCaption( hwnd, "" , FALSE );
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
debugOnPaint( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnSize( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LONG topSizeSum=0,bottomSizeSum=0;

    debugWndData.cxClient = LOWORD( lParam );
    debugWndData.cyClient = HIWORD( lParam );

#if 0
    topSizeSum += SomeCtrlSize( debugWndData.cxClient, debugWndData.cyChar ); /* �R���g���[��   */
#endif
    bottomSizeSum += StsBarSize( debugWndData.cxClient, debugWndData.cyChar ); /* �X�e�[�^�X�o�[ */

    MoveWindow( debugWndData.hWndIo, 0, topSizeSum, debugWndData.cxClient, debugWndData.cyClient - topSizeSum - bottomSizeSum, TRUE) ;

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
debugOnMove( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    debugWndData.xPos = LOWORD( lParam );
    debugWndData.yPos = HIWORD( lParam );

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
debugOnWindowPosChanged( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    debugWndData.xWindowPos = ((WINDOWPOS *)lParam)->x ;
    debugWndData.yWindowPos = ((WINDOWPOS *)lParam)->y ;
    debugWndData.cxWindow   = ((WINDOWPOS *)lParam)->cx;
    debugWndData.cyWindow   = ((WINDOWPOS *)lParam)->cy;

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
debugOnClose( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_X , debugWndData.xWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_Y , debugWndData.yWindowPos );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DX, debugWndData.cxWindow   );
        ConfigSaveDword( CONFIG_ID_WINDOW_POS_DY, debugWndData.cyWindow   );

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
debugOnDestroy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DestroyAcceleratorTable( debugWndData.hAccel );

    DestroyWindow( debugWndData.hWndIo );
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
debugOnCommand( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    DWORD dwSize;
    PBYTE dataPtr;
    PTSTR strPtr;
    S_MODAL_DLG_DATA modalDlgData;
    static FINDREPLACE fr;
    static TCHAR strFind[80],strRep[80],strMsg[1024];
    HFONT hFontOld;

    if( (HWND)lParam == debugWndData.hWndIo )
    {
        switch( HIWORD(wParam) )
        {
        case EN_UPDATE:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            StsBarSetText( STS_BAR_0  ,"%d �o�C�g�A�S %d �s",EditWndGetDataSize(debugWndData.hWndIo, EDITWND_ALL),(DWORD)SendMessage(debugWndData.hWndIo,EM_GETLINECOUNT,0,0) );
            StsBarSetText( STS_BAR_MAX,"   %d �s�A%d ��",(DWORD)(SendMessage(debugWndData.hWndIo,EM_LINEFROMCHAR,-1,0))+1,EditWndGetCaretXpos(debugWndData.hWndIo)+1);
#else                   /* [�G�f�B�b�g�R���g���[���g�p] or  �ʏ�  */
            StsBarSetText( STS_BAR_0  ,"%d �o�C�g�A�S %d �s",GetWindowTextLength(debugWndData.hWndIo),(DWORD)SendMessage(debugWndData.hWndIo,EM_GETLINECOUNT,0,0) );
            StsBarSetText( STS_BAR_MAX,"   %d �s�A%d ��",(DWORD)(SendMessage(debugWndData.hWndIo,EM_LINEFROMCHAR,-1,0))+1,0);
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
            break;
        case EN_CHANGE:
            if( debugWndData.bNeedSave )
            {
                nop();
            }
            else
            {
                debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),TRUE );
                debugWndData.bNeedSave = TRUE;
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
        case IDM_DEBUG_FILE_NEW:
            if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                debugWndData.bNeedSave = FALSE;
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
                EditWndDataInit(debugWndData.hWndIo);
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
                debugDoCaption( hwnd, "", FALSE);
            }
            break;
        case IDM_DEBUG_FILE_OPEN:
            if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
            {
                nop();
            }
            else
            {
                if( FileOpenDlg( hwnd,FILE_ID_BIN ) )
                {
                    debugWndData.bNeedSave = FALSE;
                    dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
                    EditWndDataSet( debugWndData.hWndIo, dataPtr,dwSize,TRUE );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
                    debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN), FALSE );
                }
                else
                {
                    /* �L�����Z�����ꂽ�B���̓G���[ */
                }
            }
            break;
        case IDM_DEBUG_FILE_SAVE:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            dwSize = EditWndGetDataSize(debugWndData.hWndIo,EDITWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                EditWndDataGet( debugWndData.hWndIo, dataPtr,dwSize,EDITWND_ALL );
                if( (FileWrite( FILE_ID_BIN, dataPtr, dwSize )) == FILE_NAME_NOT_SET )
                {
                    if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                    {
                        debugWndData.bNeedSave = FALSE;
                        debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
                    debugWndData.bNeedSave = FALSE;
                    debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
        case IDM_DEBUG_FILE_SAVE_AS:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            dwSize = EditWndGetDataSize(debugWndData.hWndIo,EDITWND_ALL);
            dataPtr = malloc( dwSize * sizeof(TCHAR) );
            if( dataPtr != NULL )
            {
                EditWndDataGet( debugWndData.hWndIo, dataPtr,dwSize,EDITWND_ALL );
                if( FileSaveDlg( hwnd,FILE_ID_BIN ) )
                {
                    debugWndData.bNeedSave = FALSE;
                    debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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

        case IDM_DEBUG_EDIT_UNDO:
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            SendMessage( debugWndData.hWndIo, WM_UNDO, 0, 0 );
#else                   /* [�G�f�B�b�g�R���g���[���g�p] or  �ʏ�  */
            SendMessage( debugWndData.hWndIo, EM_UNDO, 0, 0 );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
            break;

        case IDM_DEBUG_EDIT_CUT:
            SendMessage( debugWndData.hWndIo, WM_CUT, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_COPY:
            SendMessage( debugWndData.hWndIo, WM_COPY, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_PASTE:
            SendMessage( debugWndData.hWndIo, WM_PASTE, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_DELETE:
            SendMessage( debugWndData.hWndIo, WM_CLEAR, 0, 0 );
            break;

        case IDM_DEBUG_EDIT_FIND:
            fr.lStructSize   = sizeof (FINDREPLACE);
            fr.hwndOwner     = hwnd;
            fr.Flags         = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat = strFind;
            fr.wFindWhatLen  = 80;
            debugWndData.hDlgModeless = FindText(&fr);
            break;

        case IDM_DEBUG_EDIT_REPLACE:
            fr.lStructSize      = sizeof(FINDREPLACE);
            fr.hwndOwner        = hwnd;
            fr.Flags            = FR_MATCHCASE|FR_HIDEWHOLEWORD;
            fr.lpstrFindWhat    = strFind;
            fr.lpstrReplaceWith = strRep;
            fr.wReplaceWithLen  = fr.wFindWhatLen = 80;
            debugWndData.hDlgModeless = ReplaceText(&fr);
            break;

        case IDM_DEBUG_EDIT_FIND_NEXT:
            break;

        case IDM_DEBUG_EDIT_SELECT_ALL:
            SendMessage( debugWndData.hWndIo, EM_SETSEL, 0, -1 );
            break;

        case IDM_DEBUG_EDIT_DATETIME:
            strPtr = DateTimeGetString();
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
            EditWndDataSet( debugWndData.hWndIo, strPtr, strlen(strPtr), FALSE );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
            break;

        case IDM_DEBUG_FORMAT_FONT:
            if( FontChooseFont( hwnd, FONT_ID_IO ) )
            {
                hFontOld = debugWndData.hFontIo;
                debugWndData.hFontIo = CreateFontIndirect( FontGetLogFont(FONT_ID_IO) );
                SendMessage( debugWndData.hWndIo, WM_SETFONT, (WPARAM)debugWndData.hFontIo, (LPARAM)TRUE );

                if( hFontOld != NULL )
                {
                    DeleteObject(hFontOld);
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

        case IDM_DEBUG_VIEW_STS_BAR:
            if( DebugMenuInqItemChecked(IDM_DEBUG_VIEW_STS_BAR) )
            {
                DebugMenuUnCheckItem( IDM_DEBUG_VIEW_STS_BAR );
                StsBarShowWindow( FALSE );
            }
            else
            {
                DebugMenuCheckItem( IDM_DEBUG_VIEW_STS_BAR );
                StsBarShowWindow( TRUE );
            }
            SendMessage(hwnd,WM_SIZE,0,MAKELONG(debugWndData.cxClient,debugWndData.cyClient));
            break;

#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
        case IDM_DEBUG_EXTEND_NEWLINE_CRLF:
            EditWndNewLineCodeSet(debugWndData.hWndIo,NEWLINECODE_CRLF);
            DebugMenuCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CRLF );
            DebugMenuUnCheckItem( IDM_DEBUG_EXTEND_NEWLINE_LF   );
            DebugMenuUnCheckItem( IDM_DEBUG_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR+LF");
            break;

        case IDM_DEBUG_EXTEND_NEWLINE_LF  :
            EditWndNewLineCodeSet(debugWndData.hWndIo,NEWLINECODE_LF);
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CRLF );
            DebugMenuCheckItem    ( IDM_DEBUG_EXTEND_NEWLINE_LF   );
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"LF");
            break;

        case IDM_DEBUG_EXTEND_NEWLINE_CR  :
            EditWndNewLineCodeSet(debugWndData.hWndIo,NEWLINECODE_CR);
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_CRLF );
            DebugMenuUnCheckItem  ( IDM_DEBUG_EXTEND_NEWLINE_LF   );
            DebugMenuCheckItem    ( IDM_DEBUG_EXTEND_NEWLINE_CR   );
            StsBarSetText( STS_BAR_1,"CR");
            break;
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */

        case IDM_DEBUG_FILE_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;

        case IDM_DEBUG_HELP_ABOUT:
            ModalDlg( MODAL_DLG_ID_ABOUT, &modalDlgData, hwnd, debugWndData.xPos, debugWndData.yPos );
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
debugOnKeyUp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnKeyDown( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnChar( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnHscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnVscroll( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnMouseWheel( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(debugWndData.hWndIo,message,wParam,lParam);

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
debugOnSetFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SetFocus(debugWndData.hWndIo);

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
debugOnKillFocus( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    SendMessage(debugWndData.hWndIo,message,wParam,lParam);

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
debugOnDropFiles( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    static TCHAR szFileName[1024];
    DWORD dwSize;
    PBYTE dataPtr;

    if( debugWndData.bNeedSave && ((debugAskAboutSave( hwnd, FileGetTitleName(FILE_ID_BIN))) == IDCANCEL) )
    {
        nop();
    }
    else
    {
        debugWndData.bNeedSave = FALSE;
        DragQueryFile( (HDROP)wParam, (UINT)0, (LPSTR)szFileName, (UINT)sizeof(szFileName) );

        FileSetName( FILE_ID_BIN, szFileName, FALSE );
        dataPtr = FileReadByte(FILE_ID_BIN,&dwSize);
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
        EditWndDataSet( debugWndData.hWndIo,dataPtr,dwSize,TRUE );
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */
        debugDoCaption( hwnd, FileGetTitleName(FILE_ID_BIN),FALSE );
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
debugOnInitMenuPopup( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;

    if( LOWORD(lParam) == 1 )
    { /* �u�ҏW�v */
#ifndef USE_EDITCONTROL /*  �G�f�B�b�g�R���g���[���g�p  or [�ʏ�] */
        if( EditWndGetDataSize(debugWndData.hWndIo,EDITWND_SELECTED) )
        {
            DebugMenuEnableItem( IDM_DEBUG_EDIT_CUT    );
            DebugMenuEnableItem( IDM_DEBUG_EDIT_COPY   );
            DebugMenuEnableItem( IDM_DEBUG_EDIT_DELETE );
        }
        else
        {
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_CUT    );
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_COPY   );
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_DELETE );
        }
#endif                  /*  �G�f�B�b�g�R���g���[���g�p  or  �ʏ�  */

        if( IsClipboardFormatAvailable(CF_TEXT) )
        {
            DebugMenuEnableItem( IDM_DEBUG_EDIT_PASTE );
        }
        else
        {
            DebugMenuUnEnableItem( IDM_DEBUG_EDIT_PASTE );
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
debugOnFindMsgString( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    LPFINDREPLACE pfr;

    pfr = (LPFINDREPLACE)lParam;

    if( pfr->Flags & FR_DIALOGTERM )
    {
        debugWndData.hDlgModeless = NULL;
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
debugOnApp( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
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
debugOnDefault( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hwnd, message, wParam, lParam );
}

/********************************************************************************
 * ���e   : ���W���[���̖��̕�����̎擾
 * ����   : �Ȃ�
 * �߂�l : PTSTR
 ***************************************/
static PTSTR
getModuleString( void )
{
    return szModuleName;
}

/********************************************************************************
 * ���e  :
 * ����  : HWND hwnd
 * ����  : TCHAR* szTitleName
 * ����  : BOOL bNeedSave
 * �߂�l: �Ȃ�
 ***************************************/
void
debugDoCaption( HWND hwnd, TCHAR* szTitleName, BOOL bNeedSave )
{
     TCHAR szCaption[64 + MAX_PATH];

     if( bNeedSave )
     {
         wsprintf( szCaption, TEXT ("*%s - %s"), (szTitleName[0] ? szTitleName : TEXT("����")),getModuleString() );
     }
     else
     {
         wsprintf( szCaption, TEXT ("%s - %s"), (szTitleName[0] ? szTitleName : TEXT("����")),getModuleString() );
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
debugAskAboutSave( HWND hwnd, TCHAR * szTitleName )
{
    TCHAR szBuffer[64 + MAX_PATH];
    int   iReturn;

    wsprintf(szBuffer, TEXT("�t�@�C�� %s �̓��e�͕ύX����Ă��܂��B\n\n�ύX��ۑ����܂���?"), szTitleName[0] ? szTitleName : TEXT("����") );

    iReturn = MessageBox( hwnd,szBuffer,getModuleString(),MB_YESNOCANCEL|MB_ICONEXCLAMATION );

    if( iReturn == IDYES )
    {
        if( !SendMessage( hwnd,WM_COMMAND,IDM_DEBUG_FILE_SAVE,0) )
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
