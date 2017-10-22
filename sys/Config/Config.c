/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */

/* �O���ϐ���` */
/* �����֐���` */
#include "Config.h"
/* �����ϐ���` */

typedef struct
{
    HINSTANCE hInstance;
    PTSTR     szAppName;
    BOOL      bInitConfig;
    TCHAR     szIniFileName[512];
} S_CONFIG__DATA;

static S_CONFIG__DATA configData;

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
 * ����  : HINSTANCE hInst
 * ����  : PTSTR szAppName
 * �߂�l: �Ȃ�
 ***************************************/
void
ConfigInit( HINSTANCE hInst, PTSTR szAppName )
{
    DWORD length;

    configData.bInitConfig = TRUE;
    configData.hInstance   = hInst;
    configData.szAppName   = szAppName;

    length = GetModuleFileName(NULL,configData.szIniFileName,512);
    configData.szIniFileName[length-1] = 'i';
    configData.szIniFileName[length-2] = 'n';
    configData.szIniFileName[length-3] = 'i';
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

    if( configData.bInitConfig )
    {
        if( id < CONFIG_ID_MAX )
        {
            wsprintf( szDword, "0x%08lX", data );
            WritePrivateProfileString( configData.szAppName, configInfoTbl[id].pKeyName, szDword, configData.szIniFileName );
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

    if( configData.bInitConfig )
    {
        if( id < CONFIG_ID_MAX )
        {
            GetPrivateProfileString( configData.szAppName, configInfoTbl[id].pKeyName, configInfoTbl[id].pInitValue, szDword, 11, configData.szIniFileName );
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
