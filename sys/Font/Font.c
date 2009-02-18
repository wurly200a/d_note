/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

typedef struct
{
    LOGFONT logfont;
} S_FONT_DATA;

/* �O���֐���` */

/* �O���ϐ���` */

/* �����֐���` */
#include "Font.h"

/* �����ϐ���` */
static S_FONT_DATA fontData[FONT_ID_MAX];

/********************************************************************************
 * ���e  : �t�H���g������
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
FontInit( void )
{
    INT i;

    for( i=0; i<FONT_ID_MAX; i++ )
    {
        GetObject( GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), (PTSTR)&(fontData[i].logfont) );
    }
}

/********************************************************************************
 * ���e  : �t�H���g�I��
 * ����  : HWND hwnd
 * ����  : FONT_ID id
 * �߂�l: BOOL (FALSE:�L�����Z�����ꂽ)
 ***************************************/
BOOL
FontChooseFont( HWND hwnd, FONT_ID id )
{
    BOOL rtn = FALSE;
    CHOOSEFONT cf;

    if( id < FONT_ID_MAX )
    {
        cf.lStructSize    = sizeof(CHOOSEFONT);
        cf.hwndOwner      = hwnd;
        cf.hDC            = NULL;
        cf.lpLogFont      = &(fontData[id].logfont);
        cf.iPointSize     = 0;
        cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS /*| CF_EFFECTS */;
        cf.rgbColors      = 0;
        cf.lCustData      = 0;
        cf.lpfnHook       = NULL;
        cf.lpTemplateName = NULL;
        cf.hInstance      = NULL;
        cf.lpszStyle      = NULL;
        cf.nFontType      = 0;
        cf.nSizeMin       = 0;
        cf.nSizeMax       = 0;

        rtn = ChooseFont(&cf);
    }
    else
    {
        nop();
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �_���t�H���g�擾
 * ����  : FONT_ID id
 * �߂�l: LOGFONT *
 ***************************************/
LOGFONT *
FontGetLogFont( FONT_ID id )
{
    HFONT rtn = NULL;

    if( id < FONT_ID_MAX )
    {
        rtn = &(fontData[id].logfont);
    }
    else
    {
        nop();
    }

    return rtn;
}
