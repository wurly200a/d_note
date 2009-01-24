/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
/* �O���ϐ���` */
extern HINSTANCE ghInst;      /* �C���X�^���X�̃n���h��     */
extern TCHAR     szAppName[]; /* �A�v���P�[�V�����̖��� */

/* �����֐���` */
#include "SomeCtrl.h"
/* �����ϐ���` */

typedef struct
{
    BOOL  exist;       /* �E�B���h�E����������Ă��邩�ǂ���         */
    HWND  hwnd;        /* �E�B���h�E�̃n���h��                       */
    PTSTR class;       /* �E�B���h�E�N���X��                         */
    PTSTR value;       /* �E�B���h�E�e�L�X�g                         */
    int   x;           /* �N���C�A���g�̈�� x���W                   */
    int   y;           /* �N���C�A���g�̈�� y���W                   */
    int   width;       /* ������                                     */
    int   height;      /* ��������                                   */
    BOOL  bSizeAdj;    /* �T�C�Y�������s����                         */
    int   widthOfst;   /* �N���C�A���g�̈�̕��T�C�Y����̃I�t�Z�b�g */
    int   heightOfst;  /* 1�������̍�������̃I�t�Z�b�g              */
    DWORD exStyle;     /* �g���X�^�C��                               */
    DWORD style;       /* �X�^�C��                                   */
} S_SOME_CTRL;

S_SOME_CTRL ctrlListTbl[SOME_CTRL_MAX] =
{
    /* exist,  hwnd, class             , value              , x , y  , w  , h  ,bSAdj ,wOfst, hOfst,exStyle,style  */
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("file")        ,  5, 15, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 10, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT)             },
};


/********************************************************************************
 * ���e   : �o�^���ꂽ�S�ẴR���g���[���𐶐�����
 * ����   : HWND hwnd�e�E�B���h�E�̃n���h��
 * �߂�l : BOOL
 ********************************************************************************/
BOOL
SomeCtrlCreate(HWND hwnd)
{
    int i,j;
    TCHAR buf[10];

    for(i=0; i<SOME_CTRL_MAX; i++)
    {
        ctrlListTbl[i].hwnd = CreateWindowEx( ctrlListTbl[i].exStyle,                  /* �g���X�^�C��       */
                                              ctrlListTbl[i].class,                    /* �N���X��           */
                                              ctrlListTbl[i].value,                    /* �E�B���h�E�e�L�X�g */
                                              ctrlListTbl[i].style,                    /* �X�^�C��           */
                                              ctrlListTbl[i].x,ctrlListTbl[i].y,       /* x���W,y���W        */
                                              ctrlListTbl[i].width,                    /* ��                 */
                                              ctrlListTbl[i].height,                   /* ����               */
                                              hwnd,(HMENU)(SOME_CTRL_ID_OFFSET+i),     /* �e�E�B���h�E,�q�E�B���h�EID */
                                              ghInst,NULL );                           /* �C���X�^���X�n���h��,�⏕���� */
        if( ctrlListTbl[i].hwnd != NULL )
        {
            SendMessage(ctrlListTbl[i].hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));

            /* �ʂɏ���������� */
            switch( i )
            {
            default:
                break;
            }

            ShowWindow(ctrlListTbl[i].hwnd, SW_SHOW);
            ctrlListTbl[i].exist = TRUE;
        }
        else
        {
            /* do nothing */
        }
    }

    return TRUE;
}


/********************************************************************************
 * ���e   : �o�^���ꂽ�S�ẴR���g���[���̃T�C�Y�𒲐�����
 * ����   : HWND hwnd�e�E�B���h�E�̃n���h��
 * �߂�l : BOOL
 ********************************************************************************/
BOOL
SomeCtrlSize(int cxClient, int cyClient)
{
    int i;

    for(i=0; i<SOME_CTRL_MAX; i++)
    {
        if( ctrlListTbl[i].bSizeAdj )
        {
            MoveWindow( ctrlListTbl[i].hwnd, ctrlListTbl[i].x,ctrlListTbl[i].y, (cxClient + ctrlListTbl[i].widthOfst),cyClient + ctrlListTbl[i].heightOfst, TRUE);
        }
        else
        {
            /* do nothing */
        }
    }

    return TRUE;
}

/********************************************************************************
 * ���e   : �R���g���[���̃E�B���h�E�n���h�����擾����
 * ����   : SOME_CTRL_ID id
 * �߂�l : HWND
 ********************************************************************************/
HWND
SomeCtrlGetHWND(SOME_CTRL_ID id)
{
    HWND rtn;

    if( ctrlListTbl[id].exist == TRUE )
    {
        rtn = ctrlListTbl[id].hwnd;
    }
    else
    {
        rtn = (HWND)0;
    }

    return rtn;
}

/********************************************************************************
 * ���e   :
 * ����   :
 * �߂�l :
 ********************************************************************************/
void
SomeCtrlEnable(SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, TRUE);
}

/********************************************************************************
 * ���e   :
 * ����   :
 * �߂�l :
 ********************************************************************************/
void
SomeCtrlDisable(SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, FALSE);
}

/********************************************************************************
 * ���e   : �E�B���h�E����e�L�X�g���擾����
 *
 * ����   : SOME_CTRL_ID id
 *        : PTSTR ptstrText
 *
 * �߂�l : BOOL
 ********************************************************************************/
BOOL
SomeCtrlGetText( SOME_CTRL_ID id, PTSTR ptstrText )
{
    BOOL rtn = FALSE;
    INT iLength;

    if( ctrlListTbl[id].exist == TRUE )
    {
        iLength = GetWindowTextLength( ctrlListTbl[id].hwnd );
        GetWindowText( ctrlListTbl[id].hwnd, ptstrText, iLength + 1 );
        rtn = TRUE;
    }
    else
    {
        /* error */
    }

    return rtn;
}
