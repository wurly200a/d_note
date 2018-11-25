/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
/* �O���ϐ���` */

/* �����֐���` */
#include "DebugWndSomeCtrl.h"
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

static S_SOME_CTRL ctrlListTbl[DEBUG_WND_SOME_CTRL_MAX] =
{
    /* exist,  hwnd, class            , value               , x , y , w  , h  ,bSAdj ,wOfst, hOfst,exStyle       ,style  */
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("PARAM1")      ,  5, 15, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 10, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("PARAM2")      ,  5, 40, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 35, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("PARAM3")      ,  5, 65, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 60, 0  , 0  ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },

    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("4")           ,  5, 90, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            , 70, 85, 55 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("5")           ,130, 90, 25 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,160, 85, 80 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("6")           ,245, 90, 25 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,275, 85, 80 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("7")           ,360, 90, 25 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,390, 85, 80 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("8")           ,480, 90, 30 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,515, 85, 40 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },
    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("9")           ,565, 90, 40 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("edit")    , TEXT("")            ,610, 85, 40 , 20 ,FALSE ,0    , 0    ,(MY_EX_STYLE),(WS_CHILD|WS_BORDER|ES_LEFT|ES_READONLY) },

    {FALSE  ,(HWND)0, TEXT("static")  , TEXT("combo")       ,  5,115, 60 , 15 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_RIGHT)                      },
    {FALSE  ,(HWND)0, TEXT("combobox"), TEXT("")            , 70,110, 60 , 300,FALSE ,0    , 0    ,0            ,(WS_CHILD|CBS_SORT|CBS_DROPDOWNLIST)},
    {FALSE  ,(HWND)0, PROGRESS_CLASS  , TEXT("")            , 70,160, 0  , 15 ,TRUE  ,-120 , 0    ,(MY_EX_STYLE),(WS_CHILD|ES_LEFT)                       },
    {FALSE  ,(HWND)0, TEXT("button")  , TEXT("Exec")        , 70,135, 100, 20 ,FALSE ,0    , 0    ,0            ,(WS_CHILD|ES_LEFT)                       },
};


/********************************************************************************
 * ���e   : �o�^���ꂽ�S�ẴR���g���[���𐶐�����
 * ����   : HINSTANCE hInst
 * ����   : HWND hwnd�e�E�B���h�E�̃n���h��
 * �߂�l : BOOL
 ********************************************************************************/
BOOL
DebugWndSomeCtrlCreate(HINSTANCE hInst,HWND hwnd)
{
    int i;

    for(i=0; i<DEBUG_WND_SOME_CTRL_MAX; i++)
    {
        ctrlListTbl[i].hwnd = CreateWindowEx( ctrlListTbl[i].exStyle,                  /* �g���X�^�C��       */
                                              ctrlListTbl[i].class,                    /* �N���X��           */
                                              ctrlListTbl[i].value,                    /* �E�B���h�E�e�L�X�g */
                                              ctrlListTbl[i].style,                    /* �X�^�C��           */
                                              ctrlListTbl[i].x,ctrlListTbl[i].y,       /* x���W,y���W        */
                                              ctrlListTbl[i].width,                    /* ��                 */
                                              ctrlListTbl[i].height,                   /* ����               */
                                              hwnd,(HMENU)(DEBUG_WND_SOME_CTRL_ID_OFFSET+i), /* �e�E�B���h�E,�q�E�B���h�EID */
                                              hInst,NULL );                            /* �C���X�^���X�n���h��,�⏕���� */
        if( ctrlListTbl[i].hwnd != NULL )
        {
            SendMessage(ctrlListTbl[i].hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));

            /* �ʂɏ���������� */
            switch( i )
            {
            case DEBUG_WND_SOME_CTRL_COMBOBOX1:
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("2"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("3"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("4"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("5"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("6"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("7"));
                SendMessage(ctrlListTbl[i].hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("8"));
                SendMessage(ctrlListTbl[i].hwnd, CB_SELECTSTRING, 0, (LPARAM)TEXT("1"));
                break;
            case DEBUG_WND_SOME_CTRL_PROGRESS_BAR:
#if 0
                SendMessage(ctrlListTbl[i].hwnd, PBM_SETRANGE,(WPARAM)0 ,MAKELPARAM(0, 100) ); /* range 0-100 */
                SendMessage(ctrlListTbl[i].hwnd, PBM_SETPOS  ,(WPARAM)50,0                  ); /* set 50      */
                SendMessage(ctrlListTbl[i].hwnd, PBM_SETSTEP ,(WPARAM)1 ,0                  ); /* 1/1step     */
                SendMessage(ctrlListTbl[i].hwnd, PBM_STEPIT  ,0         ,0                  ); /* 1step add   */
#endif
                break;
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
DebugWndSomeCtrlSize(int cxClient, int cyChar)
{
    int i;

    for(i=0; i<DEBUG_WND_SOME_CTRL_MAX; i++)
    {
        if( ctrlListTbl[i].bSizeAdj )
        {
            MoveWindow( ctrlListTbl[i].hwnd, ctrlListTbl[i].x,ctrlListTbl[i].y, (cxClient + ctrlListTbl[i].widthOfst),cyChar + ctrlListTbl[i].heightOfst, TRUE);
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
 * ����   : DEBUG_WND_SOME_CTRL_ID id
 * �߂�l : HWND
 ********************************************************************************/
HWND
DebugWndSomeCtrlGetHWND(DEBUG_WND_SOME_CTRL_ID id)
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
DebugWndSomeCtrlEnable(DEBUG_WND_SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, TRUE);
}

/********************************************************************************
 * ���e   :
 * ����   :
 * �߂�l :
 ********************************************************************************/
void
DebugWndSomeCtrlDisable(DEBUG_WND_SOME_CTRL_ID id)
{
    EnableWindow( ctrlListTbl[id].hwnd, FALSE);
}

/********************************************************************************
 * ���e   : �E�B���h�E����e�L�X�g���擾����
 *
 * ����   : DEBUG_WND_SOME_CTRL_ID id
 *        : PTSTR ptstrText
 *
 * �߂�l : BOOL
 ********************************************************************************/
BOOL
DebugWndSomeCtrlGetText( DEBUG_WND_SOME_CTRL_ID id, PTSTR ptstrText )
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
