///YS �û��Ĳ����ĺ�������Ҫ���û���Ӧ����ID�Ĳ���
#include "public_function.h"
#include <glib.h>

GHashTable *user_hash_list;

extern void dbug(char *fmt,...);

/*****************************
�ú������ڴ�ӡ��ϣ������û�����û�IP
����hash_type��ֻ���ڴ�ӡ�����ûʵ������
*****************************/
void user_hase_table_find(GHashTable* hash,int hash_type)
{

    dbug("######################## %d",hash_type);
    g_hash_table_foreach(hash, (GHFunc)public_fun_user_iterator, "The hash of %s        is          %s \n");

    dbug("######################## %d",hash_type);
}

/*****************************
�ú������ڴ����û���ϣ��
*****************************/
void user_hase_table_create()
{
    user_hash_list =  g_hash_table_new(g_str_hash, g_str_equal);

}

/*****************************
�ú������ڲ����û���Ϣ����ϣ���ú�����ʱû��ʹ��
����1��IP�������û���IP
����2��GID�������û�����ID
*****************************/
void user_hase_table_insert(char *  ip,char * gid)
{
    g_hash_table_insert(user_hash_list, ip, gid);

}
