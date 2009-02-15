/* ���ʃC���N���[�h�t�@�C�� */
#include "common.h"
/* �ʃC���N���[�h�t�@�C�� */

/* �O���֐���` */
/* �O���ϐ���` */
/* �����֐���` */
#include "File.h"
/* �����ϐ���` */
static TCHAR nullStr = '\0';

typedef struct
{
    BOOL          init;       /* �������ς��ǂ���         */
    DWORD         size;       /* �t�@�C���T�C�Y           */
    PTSTR         pFileName;  /* �t�@�C����(�t���p�X)     */
    PTSTR         pTitleName; /* �t�@�C�����̂�           */
    PTSTR         pDirPath;   /* �f�B���N�g���p�X         */
    OPENFILENAME* pOfn;
    PTSTR         pFilter;
    PTSTR         pExt;
    PBYTE         pByte;      /* �t�@�C����ǂݍ��񂾗̈� */
} S_FILE_LIST;

static TCHAR szFileName [FILE_ID_MAX][1024];
static TCHAR szTitleName[FILE_ID_MAX][1024];
static TCHAR szDirPath  [FILE_ID_MAX][1024];
static OPENFILENAME ofns[FILE_ID_MAX];

static S_FILE_LIST fileList[FILE_ID_MAX] =
{  /* exist, size, file, title, dir ,  ofn  ,filter                            , ext        ,pByte */
    { FALSE,    0, NULL, NULL , NULL,  NULL ,TEXT("txt files (*.txt)\0*.txt\0"), TEXT("txt"),NULL  },
};

/********************************************************************************
 * ���e  : ���W���[��������
 * ����  : HWND �E�B���h�E�n���h��
 * �߂�l: �Ȃ�
 ***************************************/
void
FileInitialize( HWND hwnd )
{
    INT i;

    for( i=0; i<FILE_ID_MAX; i++ )
    {
        fileList[i].init = TRUE;

        fileList[i].pFileName  = szFileName[i];
        fileList[i].pTitleName = szTitleName[i];
        fileList[i].pDirPath   = szDirPath[i];
        fileList[i].pOfn       = &(ofns[i]);

        fileList[i].pOfn->lStructSize       = sizeof(OPENFILENAME);
        fileList[i].pOfn->hwndOwner         = hwnd;
        fileList[i].pOfn->hInstance         = NULL;
        fileList[i].pOfn->lpstrFilter       = fileList[i].pFilter;
        fileList[i].pOfn->lpstrCustomFilter = NULL;
        fileList[i].pOfn->nMaxCustFilter    = 0;
        fileList[i].pOfn->nFilterIndex      = 0;
        fileList[i].pOfn->lpstrFile         = NULL;
        fileList[i].pOfn->nMaxFile          = MAX_PATH;
        fileList[i].pOfn->lpstrFileTitle    = NULL;
        fileList[i].pOfn->nMaxFileTitle     = MAX_PATH;
        fileList[i].pOfn->lpstrInitialDir   = NULL;
        fileList[i].pOfn->lpstrTitle        = NULL;
        fileList[i].pOfn->Flags             = 0;
        fileList[i].pOfn->nFileOffset       = 0;
        fileList[i].pOfn->nFileExtension    = 0;
/*      fileList[i].pOfn->lpstrDefExt       = TEXT ("txt") ; */
        fileList[i].pOfn->lpstrDefExt       = fileList[i].pExt;
        fileList[i].pOfn->lCustData         = 0L;
        fileList[i].pOfn->lpfnHook          = NULL;
        fileList[i].pOfn->lpTemplateName    = NULL;
    }
}


/********************************************************************************
 * ���e  : ���W���[���I��
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void
FileEnd( void )
{
    INT i;

    for( i=0; i<FILE_ID_MAX; i++ )
    {
        if( fileList[i].init == TRUE )
        {
            if( fileList[i].pByte != NULL )
            {
                free( fileList[i].pByte );
                fileList[i].pByte = NULL;
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* do nothing */
        }
    }
}

/********************************************************************************
 * ���e  : �R�����_�C�A���O�ɂ��A�t�@�C���̃t���p�X�A���O���擾����
 * ����  : HWND    hwnd
 * ����  : FILE_ID id
 * �߂�l: BOOL
 **********************************************/
BOOL
FileOpenDlg( HWND hwnd, FILE_ID id )
{
    BOOL rtn = FALSE;

    if( (id < FILE_ID_MAX) && (fileList[id].init == TRUE) )
    {
        fileList[id].pOfn->hwndOwner      = hwnd;
        fileList[id].pOfn->lpstrFile      = fileList[id].pFileName;
        fileList[id].pOfn->lpstrFileTitle = fileList[id].pTitleName;
        fileList[id].pOfn->Flags          = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

        rtn = GetOpenFileName( fileList[id].pOfn );

        if( rtn == TRUE )
        {
            lstrcpy( (PTSTR)fileList[id].pDirPath , (PTSTR) fileList[id].pFileName);
            *(fileList[id].pDirPath + fileList[id].pOfn->nFileOffset) = '\0';
        }
        else
        {
            *(fileList[id].pDirPath) = '\0';
        }
    }
    else
    {
        /* error */
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �t�@�C������(�t���p�X)���擾����
 * ����  : FILE_ID id
 * �߂�l: PTSTR
 ***************************************/
PTSTR
FileGetName( FILE_ID id )
{
    PTSTR rtn;

    if( (id < FILE_ID_MAX) && (fileList[id].init == TRUE) )
    {
        rtn = fileList[id].pFileName;
    }
    else
    {
        rtn = &nullStr;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �t�@�C�����̂��擾����
 * ����  : FILE_ID id
 * �߂�l: PTSTR
 ***************************************/
PTSTR
FileGetTitleName( FILE_ID id )
{
    PTSTR rtn;

    if( (id < FILE_ID_MAX) && (fileList[id].init == TRUE) )
    {
        rtn = fileList[id].pTitleName;
    }
    else
    {
        rtn = &nullStr;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �f�B���N�g���p�X���擾����
 * ����  : FILE_ID id
 * �߂�l: PTSTR
 ***************************************/
PTSTR
FileGetDir( FILE_ID id )
{
    PTSTR rtn;

    if( (id < FILE_ID_MAX) && (fileList[id].init == TRUE) )
    {
        rtn = fileList[id].pDirPath;
    }
    else
    {
        rtn = &nullStr;
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �t�@�C�����̂��Z�b�g����
 * ����  : FILE_ID id
 * ����  : PTSTR   ptstrFileName
 * ����  : BOOL    bCat (TRUE:�A������,FALSE:�㏑��)
 * �߂�l: PTSTR
 ***************************************/
BOOL
FileSetName( FILE_ID id, PTSTR ptstrFileName, BOOL bCat )
{
    BOOL rtn = FALSE;

    if( (id < FILE_ID_MAX) && (fileList[id].init == TRUE) )
    {
        if( bCat )
        { /* �A������ */
            lstrcat( fileList[id].pFileName, ptstrFileName );
        }
        else
        {
            lstrcpy( fileList[id].pFileName, ptstrFileName );
        }

        if( ((strrchr(fileList[id].pFileName,'\\')) != NULL) ) /* �t�@�C������ �����t���Ă��邱�Ƃ��m�F */
        {
            lstrcpy( (PTSTR)fileList[id].pDirPath , (PTSTR) fileList[id].pFileName);
            *( (strrchr(fileList[id].pDirPath,'\\')) + 1 ) = '\0';

            lstrcpy( (PTSTR)fileList[id].pTitleName , (PTSTR)(strrchr(fileList[id].pFileName,'\\'))+1 );
        }
        else
        {
            /* null */
        }

        rtn = TRUE;
    }
    else
    {
        /* error */
    }

    return rtn;
}

/********************************************************************************
 * ���e  : �t�@�C������1byte���f�[�^��ǂݍ��ށB�̈��malloc�Ŏ擾����
 * ����  : FILE_ID id    �t�@�C��ID
 *       : LPDWORD pSize �f�[�^�T�C�Y���i�[����ϐ��̃|�C���^
 * �߂�l: PBYTE         �ǂݍ��񂾃f�[�^�̐擪�|�C���^
 ***************************************/
PBYTE
FileReadByte( FILE_ID id, LPDWORD pSize )
{
    HANDLE hFile;
    DWORD  dwBytesRead;
    PBYTE  rtn;

    if( (id < FILE_ID_MAX) && (fileList[id].init == TRUE) )
    {
        if( fileList[id].pByte != NULL )
        {
            /* ���ɓ����Ă����������Ă��� */
            free( fileList[id].pByte );
            fileList[id].pByte = NULL;
        }
        else
        {
            /* do nothing */
        }

        hFile = CreateFile( fileList[id].pFileName, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, 0, NULL );
        if( hFile == INVALID_HANDLE_VALUE )
        {
            rtn = NULL; /* error */
        }
        else
        {
            fileList[id].size  = GetFileSize( hFile, NULL );
            fileList[id].pByte = malloc( fileList[id].size + 2 );
            ReadFile( hFile, fileList[id].pByte, fileList[id].size, &dwBytesRead, NULL );
            CloseHandle( hFile );
            *(fileList[id].pByte + fileList[id].size)     = '\0'; /* �ی� */
            *(fileList[id].pByte + fileList[id].size + 1) = '\0'; /* �ی� */
            *pSize = fileList[id].size;
            rtn = fileList[id].pByte;
        }
    }
    else
    {
        /* error */
    }

    return rtn;
}


/********************************************************************************
 * ���e  : �t�@�C���̒������擾����(Max: 4GByte)
 * ����  : PTSTR pstrFileName �t�@�C���̃t���p�X
 * �߂�l: DWORD
 ***************************************/
DWORD
FileGetLength( PTSTR pstrFileName )
{
    HANDLE hFile;
    DWORD  dwFileLength;

    hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, 0, NULL);
    if( hFile == INVALID_HANDLE_VALUE )
    {
        dwFileLength = 0;
    }
    else
    {
        dwFileLength = GetFileSize(hFile, NULL); /* �t�@�C���̒����擾 */
    }

    return dwFileLength;
}
