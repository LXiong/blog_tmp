///YS  cache ��ʽ���������ԡ����д���������ģ����ڵ������Ǹĳ���
///YS  �ԡ�������ʵ�ʱ�䡱���㣬Խ�Ǿ������ʵ�IP���ͷŵ��ڴ�Ļ��������������ʵģ��ڱ��浽���ݿ�󣬻ᱻ��������
///YS  ����ע���У����ԡ�������ʵ�ʱ�䡱��������

#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include "public_function.h"
#include <glib.h>
#include "config.h"
#include <stdio.h>
#include "packet_info.h"
#include <time.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include "openssl/md5.h"

MYSQL cache_mysql;
MYSQL_RES *cache_res;
MYSQL_ROW cache_row;
char key_p[32];///YS ��ʱ��ȫ�ֱ��������ڱ��滺����KEY��ʵ����IP
long int value_p;///YS ��ʱ��ȫ�ֱ��������ڱ��滺����VALUE��ʵ���Ǹ���ʱ��
long int sum_temp=0;///YS ��ʱ��ȫ�ֱ��������ڼ�¼������������ж�������¼
char CACHE_SELECT_QUERY[cache_sql_str_len];///YS ��ʱ��ȫ�ֱ��������ڱ���Ҫִ�е�SQL���

extern GHashTable *tcp_hash_white_list;
extern GHashTable *tcp_hash_person_list;
extern GHashTable *tcp_hash_group_list;
extern GHashTable *tcp_hash_whole_list;
extern int put_server_ip_into_db();
extern int cache_to_db(char * file_name);
extern char *get_cache_num (char *filename,long *len,char **buffer);
extern void dbug(char *fmt,...);
extern int sql_start(MYSQL *mysql);
extern int cache_count_time ;
extern long packet_num;
extern int get_file_name_temp(int a,int b);
extern int cache_num_limit ;
char filepath[512];
/****************************
�ú����ǰ�ÿһ����¼��д�뵽���ݿ�
����1��char *table_name�����Ǳ���
����2��char * key�����ǹ�ϣ���KEY�������������IP
����3��long int value���ǹ�ϣ���VALUE���ڴ����������д���
*****************************/
int input_into_mysql( char *table_name,char * key,long int value)
{
    memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
    snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"select valuetime from `%s` where keyip = '%s';",table_name,key);

    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }

    cache_res=mysql_store_result(&cache_mysql);

    if(mysql_num_rows(cache_res))///YS ��INSERT������UPDATE
    {
        ///ys  update
        memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
        //sprintf(CACHE_SELECT_QUERY,"1111");
        snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"UPDATE `%s` SET `valuetime`='%ld' WHERE `keyip`='%s';",table_name,value,key);

    }
    else
    {
        ///ys insert
        memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
        snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"INSERT INTO `%s` (`keyip`, `valuetime`) VALUES ('%s', '%ld');",table_name,key,value);
    }

    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }

    mysql_free_result(cache_res);

return 0;
}


/****************************
�ú����ǰ�ÿһ����¼��д�뵽���ݿ⣬����һ�������ݿ�Ĺ��ܽ�������Ϳ��Դﵽɸѡ������IP
����1��gpointer key�����ǹ�ϣ���KEY�������������IP
����2��gpointer value���ǹ�ϣ���VALUE���ڴ����������д���
����3��gpointer user_data�����û������ݣ��ڴ������ǹ�ϣ���Ӧ������
*****************************/
int get_hit_count_persent(gpointer key, gpointer value ,gpointer user_data)
{
    sum_temp++;///YS ������Ӧ����Ŀ����++�������ж��Ƿ�����������õĻ�����С��ֵ
    memset(key_p,0,32);
    strcpy(key_p,(char *)key);
    value_p=0;
    value_p=((long int ) value);///YS ��ȡ���д���
    switch(*(int*)user_data)
    {
    case person_list :
    {
        //dbug("impputinto mysql ~~~~~~~~~~~~~~~~ person_list_cache");
        input_into_mysql("tcp_cache_list_person",key_p,value_p);///YS ��KEY,VALUE�������ݿ�
        break;
    }
    case group_list :
    {
        //dbug("impputinto mysql ~~~~~~~~~~~~~~~~group_list_cache");
        input_into_mysql("tcp_cache_list_group",key_p,value_p);///YS ��KEY,VALUE�������ݿ�
        break;
    }
    case whole_list :
    {
        //dbug("impputinto mysql ~~~~~~~~~~~~~~~~whole_list_cache");
        input_into_mysql("tcp_cache_list_whole",key_p,value_p);///YS ��KEY,VALUE�������ݿ�
        break;
    }
    }

return 0;
}

/****************************
����������������͵�ɾ��
����1��GHashTable* hash������Ҫ����Ĺ�ϣ��
����2��char *table_name������ÿ����ϣ����Ӧ�ı������ñ����ڱ������������KEY->VALUE
****************************/
int cache_del(GHashTable* hash,char *table_name)
{
    memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);///YS ������ǵ���ʱBUFF����BUFF�����ڱ���SQL���
    snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"select count(valuetime) from `%s` ;",table_name);///YS ��һ�����ǵı����ж�������¼
    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))///YS MYSQLAPI��ִ�����ǵ�SQL
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }
    cache_res=mysql_store_result(&cache_mysql);///YS MYSQLAPI����ȡ���
    cache_row=mysql_fetch_row(cache_res);

    /*???????????????????�������3�仰����ûʲô��???????????????????*/

    ///ys��ѯ��cache_num_limit���������������Щ����ǰ�ʱ��Ĵ�С����DESC�����
    memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
    snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"SELECT keyip FROM `%s` order by `valuetime` desc LIMIT %d,%s;",table_name,cache_num_limit,cache_row[0]);

    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }

    cache_res=mysql_store_result(&cache_mysql);
    while((cache_row=mysql_fetch_row(cache_res))) ///YS һ����ȥɾ��
    {
        g_hash_table_remove(hash,cache_row[0]);///YS ����Щʱ��ȽϾɵ���Ŀ�ӹ�ϣ����ɾ��

        memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
        snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"delete  FROM `%s` where `keyip` = '%s';",table_name,cache_row[0]);///YS ���Ҵ����ݿ����ɾ��

        if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
        {
            fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
            //exit(1);
        }

    }
    mysql_free_result(cache_res);

return 0;
}
/***************************
������ϣ��������Ұ�������������ʵ�IP�ó���
����1��GHashTable* hash����Ҫ����Ĺ�ϣ��
����2��int  p_hash_sum_Type���ǹ�ϣ���Ӧ������
****************************/
void cache_count_hash(GHashTable* hash,int  p_hash_sum_Type)
{
    dbug("cache running =======================");

    sum_temp=0;///YS �ñ��������ϣ�����ж�������¼
    ///YS gilbAPI��������ϣ��ÿ��KEY,VALUE���ú�����get_hit_count_persent"���д���
    ///YS �ڴ�����ɺ����ǵ����ݿ��Ӧ�ı���ᱣ�������еĻ��������ݣ�Ȼ�������������ݿ�Ĺ������������
    g_hash_table_foreach(hash, (GHFunc)get_hit_count_persent,&p_hash_sum_Type);

   ///YS �������ļ�¼�������������޶���������Ҳ���ǹ�ϣ�����ˣ�
   ///YS ���Ǿ�ȥ����Ӧ�����ݿ�ı����������򣬵ó��Ľ���ٷŻ����ǵĹ�ϣ����
    if(sum_temp>cache_num_limit)
    {
        dbug("the cache is full!!!");
        switch(p_hash_sum_Type)
        {
        case person_list :
        {
            cache_del(hash,"tcp_cache_list_person");///YS �����
            break;
        }
        case group_list :
        {
            cache_del(hash,"tcp_cache_list_group");
            break;
        }
        case whole_list :
        {
            cache_del(hash,"tcp_cache_list_whole");
            break;
        }
        }


    }
    dbug("cache end ============================");
}

/***************************
��ʱʱ�䵽�ˣ��ͻ�ִ�иú������ú���������Ҫ�����TCP���صģ�����HTTP���صģ�
��ΪTCP��ʶ��ÿ�ζ�Ҫ���������ֵķ���������̫����ֱ�Ӱ��Ǹ�����IP��¼������
��YYЭ���IP��XX.XX.XX.X���Ժ󿴵����XXIP���Ϳ���ֱ�ӵ�����YY�ˣ�
��ȻXX->IP��������̫׼ȷ����Ϊ���IP�п�����������Ӧ�ã��������Խ��ܡ�
*///////////////////////////
void sigFunc()
{
    packet_num=packet_num+cache_count_time;
    if(packet_num>60*60*3||packet_num==cache_count_time)
    {
        report_error();
        packet_num++;
    }
//        get_arp_user_list_from_db(&cache_mysql);
//  cache_count_hash(tcp_hash_white_list,white_list);
    cache_count_hash(tcp_hash_person_list,person_list);///YS �������˺�����
    cache_count_hash(tcp_hash_group_list,group_list);///YS �����������
    cache_count_hash(tcp_hash_whole_list,whole_list);///YS ����ȫ������
	put_server_ip_into_db();///YS �������ˣ��Ͱ�ÿ���������ľ������ʵķ�����IP���浽���ݿ�ı���Ժ���������ʱҲ����������ЩIP
}


/****************************
��ʱ����,�����Ǹ����ҵĻ����(��Ҫ��HASH��),��Ĵ�С���������ݿ�������,ͬʱ���µ�ʱ��Ҳ���������ݿ�������
*****************************/
int cache_count(void)
{
    sql_start(&cache_mysql);
    struct itimerval tv, otv;
    signal(SIGALRM, sigFunc); ///ys ʱ�䵽�˺�ִ�еĺ�����sigFunc

    tv.it_value.tv_sec = 4;///ys ��������4����ܵ�һ��
    tv.it_value.tv_usec = 0;

    tv.it_interval.tv_sec = cache_count_time; ///ys��һ��,����,�Ժ�ÿ��cache_count_time������һ�Σ���ֵ�����ݿ���
    tv.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &tv, &otv) != 0) ///YS ϵͳ���ã����ö�ʱ
        printf("setitimer err %d\n", errno);


return 0;
}



