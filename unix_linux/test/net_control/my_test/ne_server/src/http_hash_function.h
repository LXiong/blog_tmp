#include <glib.h>
#ifndef HTTP_HASH_FUNCTION_H
#define HTTP_HASH_FUNCTION_H
/***************************
*��ѯhttp��ϣ��
*����1��GHashTable* hash��,hash�ǹ�ϣ��
*����2"int hash_type"�ǹ�ϣ���ͣ�1��2��3��4�ֱ��Ӧwhite   person   group whole
****************************/
extern void http_hase_table_find(GHashTable* hash,int hash_type);

#endif
