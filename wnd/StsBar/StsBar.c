/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */

/* �����֐���` */
#include "StsBar.h"

/* �����ϐ���` */
static HWND hwndSbar;
static int sbarColWidth[STS_BAR_MAX+1] = { 100, 200, 300, 400, -1 };

/********************************************************************************
 * ���e  : �X�e�[�^�X�o�[����
 * ����  : HWND hwnd �e�E�B���h�E�̃n���h��
 * �߂�l: HWND
 ***************************************/
HWND
StsBarCreate( HWND hwnd )
{
    hwndSbar = CreateWindowEx(0,
                              STATUSCLASSNAME, NULL,
                              WS_CHILD | SBS_SIZEGRIP | WS_CLIPSIBLINGS | SBT_NOBORDERS,
                              0, 0, 0, 0,
                              hwnd, (HMENU)1500, GetHinst(), NULL);
    if( hwndSbar != NULL )
    {
        SendMessage(hwndSbar, SB_SETPARTS, sizeof(sbarColWidth)/sizeof(int), (LPARAM)sbarColWidth);
        ShowWindow(hwndSbar, SW_SHOW);
    }
    else
    {
        /* do nothing */
    }

    return hwndSbar;
}

/********************************************************************************
 * ���e  : �X�e�[�^�X�o�[�̃T�C�Y����
 * ����  : int cxClient
 * ����  : int cyClient
 * �߂�l: void
 ***************************************/
void
StsBarSize( int cxClient,int cyClient )
{
    RECT  RectSbar;

    SendMessage(hwndSbar, WM_SIZE, SIZE_RESTORED, MAKELPARAM(cxClient, cyClient));
    GetClientRect(hwndSbar, &RectSbar);
}

/********************************************************************************
 * ���e  : �X�e�[�^�X�o�[�ւ̕�����Z�b�g
 * ����  : STS_BAR_ID id
 * ����  : PTSTR ptstrFormat, ...
 * �߂�l: �Ȃ�
 ***************************************/
void
StsBarSetText( STS_BAR_ID id, PTSTR ptstrFormat, ... )
{
    TCHAR szBuf[50];
    RECT  RectSbar;
    va_list vaArgs;

    va_start(vaArgs, ptstrFormat);
    if( wvsprintf(szBuf, ptstrFormat, vaArgs) != EOF )
    {
        SendMessage(hwndSbar, SB_SETTEXT, id, (LPARAM)szBuf);
        GetClientRect(hwndSbar, &RectSbar);
    }
    else
    {
        /* do nothing */
    }
    va_end(vaArgs);

}
