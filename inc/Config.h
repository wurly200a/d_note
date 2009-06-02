#ifndef CONFIG_H

/* �Ǘ�����ݒ��ID */
typedef enum
{
    CONFIG_ID_WINDOW_POS_X,
    CONFIG_ID_WINDOW_POS_Y,
    CONFIG_ID_WINDOW_POS_DX,
    CONFIG_ID_WINDOW_POS_DY,
    CONFIG_ID_MAX
}CONFIG_ID;

/********************************************************************************
 * ���e  : �ݒ�Ǘ����W���[���̏�����
 * ����  : �Ȃ�
 * �߂�l: �Ȃ�
 ***************************************/
void ConfigInit( void );

/********************************************************************************
 * ���e  : DWORD�ݒ�l��ۑ�����
 * ����  : CONFIG_ID id
 * ����  : DWORD     data
 * �߂�l: �Ȃ�
 ***************************************/
void ConfigSaveDword( CONFIG_ID id, DWORD data );

/********************************************************************************
 * ���e  : DWORD�ݒ�l��ǂݍ���
 * ����  : CONFIG_ID id
 * �߂�l: �Ȃ�
 ***************************************/
INT ConfigLoadDword( CONFIG_ID id );

#define CONFIG_H
#endif /* CONFIG_H */
