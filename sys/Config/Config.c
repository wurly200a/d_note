/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
#include "WinMain.h"

/* �O���ϐ���` */
/* �����֐���` */
#include "Config.h"
/* �����ϐ���` */
static BOOL bInitConfig;
static TCHAR szIniFileName[64];

typedef struct
{
    PTSTR         pKeyName;    /* �L�[�� */
    PTSTR         pInitValue;  /* �����l */
} S_CONFIG_INFO;

static S_CONFIG_INFO configInfoTbl[CONFIG_ID_MAX] =
{
    { TEXT("iWindowPosX" ),TEXT("0x00000000") },
    { TEXT("iWindowPosY" ),TEXT("0x00000000") },
    { TEXT("iWindowPosDX"),TEXT("0x000001F4") }, /* 500 */
    { TEXT("iWindowPosDY"),TEXT("0x0000012C") }, /* 300 */
};

/********************************************************************************
 * ���e  : �ݒ�Ǘ����W���[���̏�����
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
ConfigInit( void )
{
    bInitConfig = TRUE;
    wsprintf( szIniFileName, ".\\%s.ini",GetAppName() );
}

/********************************************************************************
 * ���e  : DWORD�ݒ�l��ۑ�����
 * ����  : CONFIG_ID id
 * ����  : DWORD     data
 * �߂�l: �Ȃ�
 ***************************************/
void
ConfigSaveDword( CONFIG_ID id, DWORD data )
{
    TCHAR szDword[11];

    if( bInitConfig )
    {
        if( id < CONFIG_ID_MAX )
        {
            wsprintf( szDword, "0x%08lX", data );
            WritePrivateProfileString( GetAppName(), configInfoTbl[id].pKeyName, szDword, szIniFileName );
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
}

/********************************************************************************
 * ���e  : DWORD�ݒ�l��ǂݍ���
 * ����  : CONFIG_ID id
 * �߂�l: �Ȃ�
 ***************************************/
INT
ConfigLoadDword( CONFIG_ID id )
{
    TCHAR szDword[11];
    DWORD rtn = (DWORD)0;

    if( bInitConfig )
    {
        if( id < CONFIG_ID_MAX )
        {
            GetPrivateProfileString( GetAppName(), configInfoTbl[id].pKeyName, configInfoTbl[id].pInitValue, szDword, 11, szIniFileName );
            rtn = strtol( szDword+2,NULL,16 );
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

    return (DWORD)rtn;
}
