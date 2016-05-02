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
#include <time.h>
#include <pthread.h>
extern GHashTable *tcp_hash_white_list;
extern GHashTable *tcp_hash_person_list;
extern GHashTable *tcp_hash_group_list;
extern GHashTable *tcp_hash_whole_list;
extern GHashTable *user_hash_list;
extern int public_fun_put_Sip_into_cache(struct packet_info * packet_tmp);
extern void dbug(char *fmt,...);
extern GHashTable *tcp_port_list;
extern int process_qq(struct packet_info * packet_tmp);
extern CircularBuffer cb_tcp_a;
extern pthread_mutex_t *mutex_for_cb_tcp_a;
extern pthread_cond_t *notFull_tcp_a, *notEmpty_tcp_a;

extern CircularBuffer cb_tcp_b;  ///YS ringbuff b ���ڱ�������tcp��
extern pthread_mutex_t *mutex_for_cb_tcp_b;///YS ����ͬ��tcp�����߳�������߳�
extern pthread_cond_t *notFull_tcp_b, *notEmpty_tcp_b;///YS ����ͬ��tcp�����߳�������߳�
extern struct pcre_info s_pcre[pcre_num];

time_t timep;
extern int sx;
extern pthread_mutex_t *tcp_hash_mutex;
extern GThreadPool *pool_tcp_rst;
extern GError *error;
extern char sub_net[24];


/*************************
ƥ��TCP������Ч�������ݵ�������
����1��struct packet_info * packet_tmp�����������Զ������ݰ�
*************************/
int tcp_packet_feature(struct packet_info * packet_tmp)
{

    int i=0,rcCM=0;
    int ovector[OVECCOUNT]= {0};

    if(packet_tmp->behavior_type.already_process==3)
    {
        public_fun_put_Sip_into_cache(packet_tmp);
    }
    else
    {

        while(s_pcre[i].p_pcre)
        {
            ///YS ƥ��pcre����õ�ģʽ���ɹ�����������ʧ�ܷ��ظ�����54��ֱ������TCP��ͷ��Ҫ��Ҫ��Ч����
            rcCM = pcre_exec(s_pcre[i].p_pcre, NULL, (const char *)packet_tmp->raw_packet+54, packet_tmp->cap_size-54, 0, 0, ovector, OVECCOUNT);

            if (rcCM==PCRE_ERROR_NOMATCH )
            {
                // dbug(" No match ...%s",s_pcre[i].feature);
                i++;
                continue;
            }
            if(rcCM>=0)///YS �ɹ��ҵ�������
            {
                strcpy(packet_tmp->behavior_type.detail,s_pcre[i].name);
                //put_into_cache_server_ip(packet_tmp->dst_ip);
                public_fun_put_Sip_into_cache(packet_tmp); ///YS �ҵ������ֺ����ǰѸð�����ķ�����IP����ǵ����ݿ���

                return 1;
            }

            i++;
        }
    }


    return 0;
}


/************************
*tcp���ݰ������̣߳�����������£���ȡ�����ݰ����SRC_IP,DST_IP��Ȼ���һ���Ƿ��ڰ����������˺����������ڵĻ���ִ����Ӧ���к�����
*����ںڰ�������û�ҵ���������Ӧ��IP���û���GID+IP�����������������ҵ����Ǿ����أ��������
*������������û�ҵ�������ȫ�ֵģ�ȫ�ֵ�ֻҪIP�Ϳ�����
*�����û�ҵ����Ϳ�һ��������������֣�����������ҵ��ˣ��Ͱѷ�������IP����������Ȼ�������Ӧ������
*************************/

void process_tcp()
{
    struct tcphdr *tcp_header=NULL;
    struct ip * ip_header=NULL;
    struct packet_info packet_tmp;
    memset(&packet_tmp,0,sizeof(struct packet_info));
    int port_flag=1,g_push_i=1;

    char gid[8]= {0};
    char port[8]= {0};
    char str_cmp[tcp_process_strtmp_len]= {0};
    char tcp_get_tmp[tcp_process_strtmp_len]= {0};

    time(&timep);

    pthread_mutex_lock(mutex_for_cb_tcp_a);
    if(cbIsEmpty(&cb_tcp_a))///YS ����ǿյĻ����͵ȴ���ֱ���ź���notEmpty_http_a�Ĳ���
    {
        pthread_cond_wait(notEmpty_tcp_a,mutex_for_cb_tcp_a);
    }
    else
    {
        cbRead(&cb_tcp_a, &packet_tmp);///YS �ӻ����ж���������Ӧ�����ݣ����浽packet temp
        pthread_mutex_unlock(mutex_for_cb_tcp_a);
        pthread_cond_signal(notFull_tcp_a);
    }





    ip_header = (struct ip *)(packet_tmp.raw_packet + sizeof(struct ether_header));
    strcpy(packet_tmp.src_ip,inet_ntoa(ip_header->ip_src));
    strcpy(packet_tmp.dst_ip,inet_ntoa(ip_header->ip_dst));

    process_qq(& packet_tmp);///YS ����QQ����ΪQQ��ʱ����443�˿�ͨ��
    if(strstr(packet_tmp.dst_ip,sub_net))
    {
        return ;
    }


    tcp_header= (struct tcphdr *)(packet_tmp.raw_packet+ sizeof(struct ether_header) + sizeof(struct ip));
    packet_tmp.src_port=ntohs(tcp_header->th_sport);
    snprintf(port,sizeof(port),"%d",packet_tmp.src_port);

    if(g_hash_table_lookup(tcp_port_list,port))    ///YS ��һ��������ݰ���Ķ˿��ǲ��������ǵĶ˿ڹ�ϣ������ڣ����磬23TELNET��һ���
    {
        strcat(str_cmp,packet_tmp.dst_ip);///YS �ҵ��˿ڵĻ����Ǿ�֤��������ǲ��ù̶��˿�ͨ�ŵģ�prot_flag=2
        strcat(str_cmp,port);///YS �����Ļ�Ҫ���ϣ���KEY����IP+PROT
        port_flag=2;
    }
    else
    {
        strcat(str_cmp,packet_tmp.src_ip);
        strcat(str_cmp,packet_tmp.dst_ip);///YS ����û���ҵ��̶��˿ڣ�Ҫ���ϣ���KEY�ǣ�serverip+useripys
        strcpy(tcp_get_tmp,packet_tmp.dst_ip);
    }


    if(g_hash_table_lookup(tcp_hash_white_list, str_cmp))
    {
        dbug("in while list !!!!!!");    ///YS ������IP+IP
        dbug(str_cmp);
        return;
    }
    else
    {
        if(g_hash_table_lookup(tcp_hash_person_list, str_cmp))
        {
            dbug("in person list!!!! %s",str_cmp);
            g_hash_table_insert(tcp_hash_person_list, g_strdup(str_cmp), (gpointer)timep);
            pthread_mutex_lock(mutex_for_cb_http_b);
            if(cbIsFull(&cb_http_b))
            {
                dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
            }
            else
            {
                cbWrite(&cb_http_b, &packet_tmp);///YS �ڵ�����������ݰ�д�����ػ���
                g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);///YS ֪ͨ�����̳߳�

            }
            pthread_mutex_unlock(mutex_for_cb_http_b);
            pthread_cond_signal(notEmpty_http_b);
            return;
        }
        else
        {
            memset(str_cmp,0,tcp_process_strtmp_len);
            if(g_hash_table_lookup(user_hash_list,packet_tmp.src_ip))
            {
                strcpy(gid,g_hash_table_lookup(user_hash_list,packet_tmp.src_ip));
                strcpy(str_cmp,gid);
                strcpy(packet_tmp.gid,gid);
                if(port_flag==2)///YS ����ǹ̶��˿����صģ�������ϳ��µ�KEY����ȥ���
                {
                    strcat(str_cmp,port);
                }
                else
                {
                    snprintf(str_cmp,sizeof(str_cmp),"%s%s",gid,tcp_get_tmp);
                }
            }
            if(g_hash_table_lookup(tcp_hash_group_list, str_cmp))///YS str_cmpҪô��IP+GID��Ҫô��Port+GID�����ǹ̶��˿ڵģ�
            {

                dbug("in group list !!!!!! %d",timep);
                g_hash_table_insert(tcp_hash_group_list, g_strdup(str_cmp),(gpointer) timep);
                pthread_mutex_lock(mutex_for_cb_http_b);
                if(cbIsFull(&cb_http_b))
                {
                    dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                    pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                }
                else
                {
                    cbWrite(&cb_http_b, &packet_tmp);
                    g_thread_pool_push(pool_tcp_rst,&g_push_i,&error);

                }
                pthread_mutex_unlock(mutex_for_cb_http_b);
                pthread_cond_signal(notEmpty_http_b);
                return;
            }
            else
            {
                memset(str_cmp,0,tcp_process_strtmp_len);
                if(port_flag==2)///YS  ͬ�ϣ�����ǹ̶��˿ڵ�
                {
                    ///YS ֱ��ʹ��PORTȥ���
                    snprintf(str_cmp,sizeof(str_cmp),"%s",port);
                }
                else
                {
                    ///YS ֱ��ʹ��IPȥ���
                    snprintf(str_cmp,sizeof(str_cmp),"%s",tcp_get_tmp);
                }
                if(g_hash_table_lookup(tcp_hash_whole_list, str_cmp))///YS ����ҵ��ˣ�����ȫ�ֺ�������
                {
                    g_hash_table_insert(tcp_hash_whole_list, g_strdup(str_cmp),(gpointer) timep);
                    pthread_mutex_lock(mutex_for_cb_http_b);
                    if(cbIsFull(&cb_http_b))
                    {
                        dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                        pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                    }
                    else
                    {
                        cbWrite(&cb_http_b, &packet_tmp);///YS д�뵽���ػ�����
                        g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);

                    }
                    pthread_mutex_unlock(mutex_for_cb_http_b);
                    pthread_cond_signal(notEmpty_http_b);
                    return;
                }
            }
        }
        if(sx==1)
        {
//            exit(0);
        }
        ///YS �����û���ҵ��Ļ�������������ĳ��ȡ�64���Ǿ�ʹ�������ֽ��в���
        if(packet_tmp.cap_size>64) tcp_packet_feature(&packet_tmp);
    }
}
