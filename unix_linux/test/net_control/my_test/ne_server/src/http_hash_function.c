///YS һЩ����HTTP��ϣ��ĺ���
#include <glib.h>
#include "config.h"
#include <stdio.h>
#include <stdlib.h>


GHashTable *http_hash_white_list;
GHashTable *http_hash_person_list;
GHashTable *http_hash_group_list;
GHashTable *http_hash_whole_list;
GHashTable *http_hash_ALL_w_list;
GHashTable *http_hash_ALL_b_list;
extern void dbug(char *fmt,...);


void iterator(gpointer key, gpointer value ,gpointer user_data)
{
    printf("key is %s  %s\n",(char *)key ,(char *)value);
}

void iterator_tcp(gpointer key, gpointer value ,gpointer user_data)
{
    //g_printf(user_data, key, value);
    printf("key is %s\n",(char *)key);
}

/***************************
*��ѯhttp��ϣ��
*����1��GHashTable* hash��,hash�ǹ�ϣ��
*����2"int hash_type"�ǹ�ϣ���ͣ�1��2��3��4�ֱ��Ӧwhite   person   group whole
****************************/
void http_hase_table_find(GHashTable* hash,int hash_type)
{
    dbug("######################## %d",hash_type);

    g_hash_table_foreach(hash, (GHFunc)iterator, "The hash of %s is %s  \n");
    //  else g_hash_table_foreach(hash, (GHFunc)iterator_tcp, 8);
    /*
        gboolean result = g_hash_table_lookup_extended(hash, "192.168.1.22", (gpointer*)key_ptr, (gpointer*)value_ptr);
     if (result)
      {
      g_printf("Found that the %s >>>>>>>>> %s", capital, state);
     }
    */
    dbug("######################## %d",hash_type);
}


/*************************
*����http��ϣ��
*
*************************/
void http_hase_table_create()
{
    http_hash_white_list =  g_hash_table_new(g_str_hash, g_str_equal);
    http_hash_person_list = g_hash_table_new(g_str_hash, g_str_equal);
    http_hash_group_list = g_hash_table_new(g_str_hash, g_str_equal);
    http_hash_whole_list = g_hash_table_new(g_str_hash, g_str_equal);
    http_hash_ALL_w_list= g_hash_table_new(g_str_hash, g_str_equal);
     http_hash_ALL_b_list= g_hash_table_new(g_str_hash, g_str_equal);
}

/*
*����http��ϣ��
*������website_str��Ҫ�������ַ
*http_hash_type�ǹ�ϣ���ͣ�1��2��3��4----white   person   group whole
*/
/*void http_hase_table_insert(char *  website_str,char * ip,int http_type)
{

    switch(http_type)
    {
    case white_list:
        g_hash_table_insert(http_hash_white_list, website_str, ip);
        break;
    case person_list:
        dbug("333333333333333333");
        g_hash_table_insert(http_hash_person_list, website_str, ip);
        break;
    case group_list:
        g_hash_table_insert(http_hash_group_list, website_str, ip);
        break;
    case whole_list:
        g_hash_table_insert(http_hash_whole_list, website_str, ip);
        break;
    default :
        break;
    }

}*/
