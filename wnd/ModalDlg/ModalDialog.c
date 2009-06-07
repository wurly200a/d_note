/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "ModalDlg.h"

LRESULT CALLBACK AboutDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

/* �����ϐ���` */
static TCHAR szAboutTitleName[64];

typedef struct
{
    PTSTR   className;
    PTSTR   titleName;
    WNDPROC wndPrc   ;
    int     xOffset  ;
    int     yOffset  ;
    int     nWidth   ;
    int     nHeight  ;
} S_MODAL_DLG_INFO;

S_MODAL_DLG_INFO modalDlgInfoTbl[MODAL_DLG_ID_MAX] =
{
    /* className      , titleName             , wndPrc  */
    { TEXT("AboutDlg"), TEXT("�o�[�W�������"), AboutDlgProc, 25, 25, 487, 327 },
};

/********************************************************************************
 * ���e  : ���[�_���_�C�A���O�{�b�N�X������
 * ����  : �Ȃ�
 * �߂�l: BOOL
 ***************************************/
BOOL
ModalDlgInit( void )
{
    INT i;
    WNDCLASS wc = {0};

    for( i=0; i<MODAL_DLG_ID_MAX; i++ )
    {
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = modalDlgInfoTbl[i].wndPrc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = GetHinst();
        wc.hIcon         = NULL;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = modalDlgInfoTbl[i].className;

        switch( i )
        {
        case MODAL_DLG_ID_ABOUT:
            wsprintf( szAboutTitleName, "%s �̃o�[�W�������",GetAppName() );
            modalDlgInfoTbl[i].titleName = szAboutTitleName;
            break;
        default:
            break;
        }

        RegisterClass(&wc); /* �E�C���h�E�N���X�o�^ */
    }

    return TRUE;
}

/********************************************************************************
 * ���e  : ���[�_���_�C�A���O�{�b�N�X
 * ����  : MODAL_DLG_ID      id
 * ����  : S_MODAL_DLG_DATA *dataPtr
 * ����  : HWND              hwnd
 * ����  : int               x
 * ����  : int               y
 * �߂�l: BOOL
 ***************************************/
BOOL
ModalDlg( MODAL_DLG_ID id, S_MODAL_DLG_DATA *dataPtr, HWND hwnd, int x, int y )
{
    BOOL rtn = TRUE;
    HWND hDlg;
    MSG msg;

    if( id < MODAL_DLG_ID_MAX )
    {
        /* �_�C�A���O�{�b�N�X�쐬 */
        hDlg = CreateWindowEx( WS_EX_DLGMODALFRAME,
                               modalDlgInfoTbl[id].className,modalDlgInfoTbl[id].titleName,
                               WS_VISIBLE | (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU/*|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX */),
                               x+modalDlgInfoTbl[id].xOffset, y+modalDlgInfoTbl[id].yOffset,
                               modalDlgInfoTbl[id].nWidth   , modalDlgInfoTbl[id].nHeight  ,
                               hwnd , NULL, GetHinst(), NULL );

        EnableWindow(hwnd,FALSE); /* ���C���E�C���h�E�𖳌������ă��[�_���� */

        while( GetMessage(&msg,NULL,0,0) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        EnableWindow(hwnd,TRUE); /* ���C���E�C���h�E��L���ɂ��ă��[�_������ */
        BringWindowToTop(hwnd);
    }
    else
    {
        rtn = FALSE;
    }

    return rtn;
}

LRESULT CALLBACK
AboutDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT rtn = 0;
    HWND hBtn;

    switch( message )
    {
    case WM_CREATE:
        hBtn = CreateWindow( TEXT("Button"), TEXT("OK"), WS_CHILD|WS_VISIBLE, 382,270,90,19, hwnd, (HMENU)0, GetHinst(), NULL );
        SendMessage( hBtn, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
        SetFocus( hBtn );
        break;
    case WM_DESTROY:
        PostQuitMessage(0); /* WM_QUIT���b�Z�[�W���|�X�g���� */
        break;
    case WM_COMMAND:
        switch( LOWORD(wParam) )
        {
        case 0:
            DestroyWindow( hwnd );
            break;
        defaut:
            break;
        }
        break;
    default:
        rtn = DefWindowProc( hwnd, message, wParam, lParam );
    }

    return rtn;
}
