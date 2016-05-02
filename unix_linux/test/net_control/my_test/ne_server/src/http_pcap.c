///ys HTTP ץ����غ���
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
#include <pthread.h>
#include "public_function.h"
#include "config.h"
#include "packet_info.h"
#include "ringbuff.h"

extern CircularBuffer cb_http_a;  ///YS ringbuff a   ���ڱ��沶��HTTP�����ݰ�
extern pthread_mutex_t *mutex_for_cb_http_a;///YS ����http�������߳���http�����߳�֮��Ļ���

extern void dbug(char *fmt,...);
extern pthread_mutex_t *mutex_for_sock;///YS����http�������߳���tcp�����߳�֮�以��
extern pthread_cond_t *notFull_http_a, *notEmpty_http_a;///YS ����ͬ��http�������߳���http�����̵߳��ź���
extern char my_device[64];
extern GThreadPool *pool_http_process ;
extern GError *error;
extern int newsockfd;///YS ��Ч��TCP���Ӿ��
extern char ex_ip[24];
extern char ex_ip1[24];
extern char ex_ip2[24];
extern char dst_db_ip[32];

/************************
*�Ѳ����http���ݰ����浽ringbuff  a
*����1��u_char *arg�� �ǻص��������������û��Զ������
*����2��const struct pcap_pkthdr* pkthdr���� pkthdr�����ݰ��Ĳ���ʱ������Ĵ�С�����񳤶�
*����3��packet�� �������ݰ���ԭʼ����
************************/
void http_putin_buff(u_char *arg, const struct pcap_pkthdr* pkthdr,const u_char * packet) ///YS callback funtion
{
    int g_push_i=1;///YS g_push_iֻ��Ϊ�������ȥ����Ϊ��NULL��������
    struct packet_info my_packet;///YS ���Ƕ�������ݰ��ṹ��
    //struct sock_buff my_sock_buff;///YS ���Ƕ������SOCKET�����ݰ��Ľṹ��
    //memset(&my_sock_buff,0,sizeof(struct sock_buff));
    my_packet.cap_size=pkthdr->caplen; ///YS ץ���ĳ���

    pthread_mutex_lock(mutex_for_cb_http_a);
    if(cbIsFull(&cb_http_a))///YS �������������
    {
        dbug("http_pcap>>>>>>>>>>here will fork the new thread to process the  packet");///YS �̳߳�����̲߳�����ʱ��Ӧ��Ҫ�½��̣߳�������Ժ��Ż�
        pthread_cond_wait(notFull_http_a, mutex_for_cb_http_a);///YS �߳̽���˯�ߣ������ͷ���������notFull_http_a�źų���ʱ���Ŵ�˯��״̬�ѹ���
        memcpy(my_packet.raw_packet,packet,pkthdr->caplen);
        cbWrite(&cb_http_a, &my_packet);///YS д�뻺����
        g_thread_pool_push(pool_http_process, &g_push_i,&error);///YS ֪ͨ�̳߳أ�����������

        ///YS ����Ӧ��Ҫ��HTTP�İ���ȥ����Ǳߵģ�����TCP�����Ǳ��Ѿ������ˣ����ԾͲ�����
    	/*my_sock_buff.len=pkthdr->caplen;
        memcpy(my_sock_buff.raw_packet,packet,pkthdr->caplen);
        if (newsockfd >0)
        {
            //if (write(newsockfd,(void*)&my_sock_buff,sizeof(struct sock_buff))< 0)
            if (write(newsockfd,(void*)&my_sock_buff,1544)< 0)
            {
                close(newsockfd);
                newsockfd=-1;
            }
        }*/
    }
    else
    {
        memcpy(my_packet.raw_packet,packet,pkthdr->caplen);
        cbWrite(&cb_http_a, &my_packet);
        g_thread_pool_push(pool_http_process, &g_push_i,&error);
     		///YS ����Ӧ��Ҫ��HTTP�İ���ȥ����Ǳߵģ�����TCP�����Ǳ��Ѿ������ˣ����ԾͲ�����
     		/* my_sock_buff.len=pkthdr->caplen;
        memcpy(my_sock_buff.raw_packet,packet,pkthdr->caplen);
        if (newsockfd >0)
        {

             n=write(newsockfd,(void*)&my_sock_buff,1544);
            if (n<1)
            {
                close(newsockfd);
                newsockfd=-1;
            }

           //  pthread_mutex_unlock(mutex_for_sock);
        }*/
    }
    pthread_mutex_unlock(mutex_for_cb_http_a);
    pthread_cond_signal(notEmpty_http_a);


    return;
}


/*************************
*�����http���ݰ��߳�
**************************/
void  http_pcap()
{
    int count =0;
    pcap_t *descr = NULL;
    struct bpf_program fp;
    char errbuf[1024];
    char http_bfp[2048];
    memset(http_bfp,sizeof(http_bfp),2048);
    ///YS HTTP���ص�����
    snprintf(http_bfp,sizeof(http_bfp),"tcp port 80 and (not host %s) and (tcp[13] != 0x19) \
             and (not host %s ) and (not host %s ) and (not host %s )",dst_db_ip,ex_ip,ex_ip1,ex_ip2);
    memset(errbuf,0,1024);
    descr = pcap_open_live(my_device,MAXBYTES2CAPTURE,1,10,errbuf);///YS LIBPCAPAPI
		//if(public_fun_bpf_config(descr,"tcp port 80 and (not src host 192.168.1.222 or not dst host 192.168.1.222)",fp,0)==-1)//tcp port 80
    if(public_fun_bpf_config(descr,http_bfp,fp,0)==-1)///YS �������ǵ���������
    {
        dbug("120\n");
//        exit(0);
    }

    pcap_loop(descr,-1,http_putin_buff,(u_char *)&count);//����2�����ٸ������ֹͣ����-1��û������
    pcap_close(descr);
}

