///YS HTTP�������̣߳�����������£�
///YS 1����ץ����������������ݰ�
///YS 2���жϰ����Ƿ���HTTP��Ϣ
///YS 3���еĻ���ȡ��HOST����REFERER�ֶΣ�ȡ���Ľ������"www.baidu.com"��û�еĻ��ͷ���
///YS 4����ȡ��URL����ȡ�����ݰ����IP����ϳ�IP+URL����ʽ
///YS 5�������IP+URL��һ�¹�ϣ������ڰ�����������˺������ﶼû���ҵ��Ļ��ͽ��������������
///YS 6���ȵ��û���ϣ������IP��Ӧ���û����磬1001���ó�1001+URL
///YS 7��������µ�ֵȥ���������������ҵ���д�����ػ�������û�ҵ��Ļ�������ȫ�ֺ�����
///YS 8��ֱ����URL����ȫ�ֺ��������ң��ҵ���д�����ػ�����
///YS 9����û�ҵ��ͷ��иð�
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcap/pcap.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <libnet.h>
#include "packet_info.h"
#include "ringbuff.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <glib.h>
#include "config.h"
#include <pthread.h>
extern GHashTable *http_hash_white_list;
extern GHashTable *http_hash_person_list;
extern GHashTable *http_hash_group_list;
extern GHashTable *http_hash_whole_list;
extern GHashTable *http_hash_ALL_w_list;
extern GHashTable *http_hash_ALL_b_list;
extern GHashTable *user_hash_list;
extern void dbug(char *fmt,...);
extern CircularBuffer cb_http_a;
extern pthread_mutex_t *mutex_for_cb_http_a;
extern pthread_cond_t *notFull_http_a, *notEmpty_http_a;

extern CircularBuffer cb_http_b;  ///YS ringbuff b ���ڱ�������http��
extern pthread_mutex_t *mutex_for_cb_http_b;///YS ����ͬ��http�����߳�������߳�
extern pthread_cond_t *notFull_http_b, *notEmpty_http_b;///YS ����ͬ��http�����߳�������߳�

extern GThreadPool *pool_tcp_process ;
extern GThreadPool *pool_tcp_rst;
extern GError *error;
extern int process_qq(struct packet_info * packet_tmp); ///YS �Ƿ���QQЭ��


/***************************
*http���ݰ������߳�
****************************/
void process_http( )
{
    struct ip * ip_header;
    struct packet_info packet_tmp;

    char *p="0";
    char *p2="0";
    char gid[8];
    int len_end,g_push_i=1;
    char str_cmp[http_process_str_len];
    char http_get_tmp[http_process_str_len];
    memset(str_cmp,0,http_process_str_len);
    memset(http_get_tmp,0,http_process_str_len);



    ///YS �߳̿�ʼλ��
    pthread_mutex_lock(mutex_for_cb_http_a);///YS �����

    if(cbIsEmpty(&cb_http_a))///YS ����ǿյĻ����͵ȴ���ֱ���ź���notEmpty_http_a�Ĳ���
    {
        pthread_cond_wait(notEmpty_http_a,mutex_for_cb_http_a);
    }
    else
    {
        cbRead(&cb_http_a, &packet_tmp);///YS �ӻ����������һ�����ݰ���д�뵽packet temp��
        pthread_mutex_unlock(mutex_for_cb_http_a);
        pthread_cond_signal(notFull_http_a);
    }

		///YS  ��ǰ���̴߳���ʱ���������� ���ע�͵��Ĵ��룬���ڸ����̳߳ؾͲ�����
   /* if(!memmem( packet_tmp.raw_packet,packet_tmp.cap_size,"HTTP/1.1",8))
    {
        pthread_mutex_lock(mutex_for_cb_tcp_a);
        if(cbIsFull(&cb_tcp_a))
        {
            dbug("tcp_cap>>>>>>>>>here will fork the new thread to process the  packet  ");//for test
            pthread_cond_wait(notFull_tcp_a, mutex_for_cb_tcp_a);
        }
        else
        {
            cbWrite(&cb_tcp_a, &packet_tmp);
            g_thread_pool_push(pool_tcp_process, &g_push_i,&error);
        }
        pthread_mutex_unlock(mutex_for_cb_tcp_a);
        pthread_cond_signal(notEmpty_tcp_a);
    }*/

    ip_header = (struct ip *)(packet_tmp.raw_packet + sizeof(struct ether_header));///YS ipͷ��Ϣ��ȡ
    strcpy(packet_tmp.src_ip,inet_ntoa(ip_header->ip_src));///YS ��ȡIP_SRC
    strcpy(packet_tmp.dst_ip,inet_ntoa(ip_header->ip_dst));///YS ��ȡIP_DST

    process_qq(&packet_tmp);///YS ����һ���Ƿ�ΪQQ�İ�����ΪQQ����80�˿�

    if(g_hash_table_lookup(http_hash_ALL_w_list, packet_tmp.src_ip))
    {
        dbug("imp person  all white  list "); ///YS �鵽�ڰ�����������HTTP ALL������˿��������Ƶ�ʹ��80�˿ڣ�
        return;
    }

    packet_tmp.packet_size=ntohs(ip_header->ip_len)-40;///YS ���ݰ�����Ч����


    p=(char *)memmem( packet_tmp.raw_packet,packet_tmp.cap_size,"Host: ",6);///YS �����Ƿ���HOST�������
    if(p)///YS �еĻ���ȡ��������ۺ�����ݣ�ֱ��\R����������ͨ��Ϊ��www.baidu.com
    {
        p2=strchr(p,'\r');
        if(p2)
        {
            len_end=p2-p;
            if(len_end>sizeof(http_get_tmp))///YS �Ƿ����ݳ��ȱ�BUFF�ĳ���Ҫ��
            {
                snprintf(http_get_tmp,sizeof(http_get_tmp),"%s",p+6);
            }
            else
            {
                snprintf(http_get_tmp,len_end-5,"%s",p+6);
            }
        }
    }
    if(!http_get_tmp[0]) return ;///YS �������Ϊ�յĻ��ͷ���

    strcpy(str_cmp,http_get_tmp);
    strcat(str_cmp,packet_tmp.src_ip);///YS ��ȡIP���Խ��к��������˲���
    packet_tmp.block_flag=http_flag;///YS ��ϵı�־��HTTP�����ֿ�HTTP������TCP
    if(g_hash_table_lookup(http_hash_white_list, str_cmp))
    {
        dbug("imp white  list %s",str_cmp); ///YS �����������ˣ�������
        return ;
    }
    else
    {

        if(g_hash_table_lookup(http_hash_person_list, str_cmp))
        {
            dbug("imp person  list %s",str_cmp);///YS �����������ˣ�

            pthread_mutex_lock(mutex_for_cb_http_b);
            if(cbIsFull(&cb_http_b))
            {
                dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
            }
            else
            {
                cbWrite(&cb_http_b, &packet_tmp);///YS д�����ػ�����
                g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);///YS ֪ͨ�̳߳أ������ݰ�Ҫ����
            }
            pthread_mutex_unlock(mutex_for_cb_http_b);
            pthread_cond_signal(notEmpty_http_b);
            return;
        }
        else
        {
            memset(str_cmp,0,http_process_str_len);
            strcpy(str_cmp,http_get_tmp);///YS ������˵İ׺�������û���ҵ��������½���������������ַ�����ƥ��
            if(g_hash_table_lookup(user_hash_list,packet_tmp.src_ip))///YS ����IP��Ӧ���û���
                strcpy(gid,g_hash_table_lookup(user_hash_list,packet_tmp.src_ip));
            strcat(str_cmp,gid);///YS �����ַ�������URL+1001
            if(g_hash_table_lookup(http_hash_group_list, str_cmp))///YS ������������Ĳ���
            {
                dbug("impgroup list %s",str_cmp);   ///YS ���������飩
                pthread_mutex_lock(mutex_for_cb_http_b);
                if(cbIsFull(&cb_http_b))
                {
                    dbug("http_process  cb >>>>>>>>>>here will fork the new thread ");
                    pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                }
                else
                {
                    cbWrite(&cb_http_b, &packet_tmp);///YS д�����ػ�����
                    g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);
                }
                pthread_mutex_unlock(mutex_for_cb_http_b);
                pthread_cond_signal(notEmpty_http_b);
                return;
            }
            else///YS �������û�ҵ�����ȫ�ֵ�
            {
                if(g_hash_table_lookup(http_hash_whole_list, http_get_tmp))
                {
                    dbug("impwhole list %s",http_get_tmp); ///YS ��������ȫ�֣�
                    pthread_mutex_lock(mutex_for_cb_http_b);
                    if(cbIsFull(&cb_http_b))
                    {
                        dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");//for test
                        pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                    }
                    else
                    {
                        cbWrite(&cb_http_b, &packet_tmp);///YS д�����ػ�����
                        g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);

                    }
                    pthread_mutex_unlock(mutex_for_cb_http_b);
                    pthread_cond_signal(notEmpty_http_b);
                    return;
                }
                else///YS �����Ƿ���http  all���Ƿ�ȫ����������
                {
                    if(g_hash_table_lookup(http_hash_ALL_b_list, packet_tmp.src_ip))///YS ��IP�������ALL BAN
                    {
                        dbug("imp person  all bla  list "); ///YS ������ALL�����ˣ�
                        pthread_mutex_lock(mutex_for_cb_http_b);
                        if(cbIsFull(&cb_http_b))
                        {
                            dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                            pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                        }
                        else
                        {
                            cbWrite(&cb_http_b, &packet_tmp);///YS д�����ػ�����
                            g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);
                        }
                        pthread_mutex_unlock(mutex_for_cb_http_b);
                        pthread_cond_signal(notEmpty_http_b);
                        return;
                    }
                }
            }
        }
    }
}

