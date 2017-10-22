/* 共通インクルードファイル */
#include "common.h"
/* 個別インクルードファイル */

/* 外部関数定義 */

/* 外部変数定義 */
/* 内部関数定義 */
#include "Config.h"
/* 内部変数定義 */

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
    PTSTR         pKeyName;    /* キー名 */
    PTSTR         pInitValue;  /* 初期値 */
} S_CONFIG_INFO;

static S_CONFIG_INFO configInfoTbl[CONFIG_ID_MAX] =
{
    { TEXT("iWindowPosX" ),TEXT("0x00000000") },
    { TEXT("iWindowPosY" ),TEXT("0x00000000") },
    { TEXT("iWindowPosDX"),TEXT("0x000001F4") }, /* 500 */
    { TEXT("iWindowPosDY"),TEXT("0x0000012C") }, /* 300 */
};

/********************************************************************************
 * 内容  : 設定管理モジュールの初期化
 * 引数  : HINSTANCE hInst
 * 引数  : PTSTR szAppName
 * 戻り値: なし
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
 * 内容  : DWORD設定値を保存する
 * 引数  : CONFIG_ID id
 * 引数  : DWORD     data
 * 戻り値: なし
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
 * 内容  : DWORD設定値を読み込む
 * 引数  : CONFIG_ID id
 * 戻り値: なし
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
