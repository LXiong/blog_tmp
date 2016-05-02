///ys ֻҪ���������ص����ݰ���UDP�Ҷ˿ڴ���3K�ģ��Ǿ�ARP����ARP��ּ�2����
#include <libnet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <mysql/mysql.h>
#include "packet_info.h"
#define UPDATE_FLOW_MAC "UPDATE `flow_statistics` set `Mac`='%x:%x:%x:%x:%x:%x' where `User_IP`='%s';"


u_char mac[6]; ///ys ����û����
char mac_2_ip[32];///ys MAC ��Ӧ ��IP
char db_ip[24];///ys ����ƭ��Ŀ��IP
int ip1,ip2,ip3,ip4;///ys ��IP1.IP2.IP3.IP4��������ʽ����IP�ַ���ÿ���ֶε�ֵ
struct arp_user user_list[255];///ys ��ҪARP���û�
extern char my_device[64];///ys ��������
//extern u_int8_t arp_mac[6];
extern  unsigned char arp_mac2[6];///ys ������mac
unsigned int arp_mac_wrong[6]= {0x11,0x11,0x11,0x11,0x11,0x11}; ///ys ������mac
extern char arp_source[32];///ys ������IP
char ex_ip[24];///ys VIPIP1�����ᱻ���ص�IP����VIPIP
char ex_ip1[24];///ys VIPIP2
char ex_ip2[24];///ys VIPIP3
extern char  net_addr [8];///YS ����
extern char server_ip[32];
extern char localip[32];
extern MYSQL mysql;
/**********************
�ú������ڰ�����ARP��Ŀ��IP���뵽���ǵ�APR_USRR_LIST���棬����1"const u_char *my_packet",������ͨ��LIBPCAP��ץ����ԭʼ�İ�
**********************/
int get_arp_user_list_from_db(MYSQL *sock)
{
    #define usre_flow_list "select User_IP,Mac from flow_statistics where Block=1"
    MYSQL_ROW row;
    MYSQL_RES *res;
    if(mysql_query(sock,usre_flow_list))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))///YS �����ݿ���ҵ��Ľ��
    {
        printf("aaaaa  >>>  %s   %s \n",row[0],row[1]);
        sscanf(row[0],"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS ��ȡ����MAC
        //if(strlen(user_list[ip4].ip)<3||user_list[ip4].count>33 && user_list[ip4].flag==0)
        if(user_list[ip4].flag==0)///YS �����û����ARP�����IP�Ļ�,�Ͱ����IP���뵽���ǵ�ARP_USR_LIST��,���ҷ��ʹ���Ϊ1,�Ѿ�����ı�־Ϊ1
        {
            printf("insert arp_usr_list   %s    %d\n",row[0],ip4);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            user_list[ip4].from=4;
            sscanf(row[1],"%x:%x:%x:%x:%x:%x",
                   & user_list[ip4].mac[0],& user_list[ip4].mac[1],& user_list[ip4].mac[2],& user_list[ip4].mac[3]
                   ,& user_list[ip4].mac[4],& user_list[ip4].mac[5]);
        }
        sprintf(user_list[ip4].ip,"%s",row[0]);
    }
    mysql_free_result(res);
    return 0;
}




/**********************
�ú������ڰ�����ARP��Ŀ��IP���뵽���ǵ�APR_USRR_LIST���棬����1"const u_char *my_packet",������ͨ��LIBPCAP��ץ����ԭʼ�İ�
**********************/
int creat_arp_user_list(struct arp_user *arp_user_tmp)
{
    int j;
    char mac_sql[1024]={'\0'};
    sscanf(arp_user_tmp->ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS ��ȡ����MAC
    if(user_list[ip4].count==0)///YS �����û����ARP�����IP�Ļ�,�Ͱ����IP���뵽���ǵ�ARP_USR_LIST��,���ҷ��ʹ���Ϊ1,�Ѿ�����ı�־Ϊ1
    {
        printf("insert arp_usr_list   %s  \n",arp_user_tmp->ip);
        user_list[ip4].count=1;
        user_list[ip4].flag=1;

        sprintf(user_list[ip4].ip,"%s",arp_user_tmp->ip);
        for(j=0; j<6; j++)
        {
            user_list[ip4].mac[j]=arp_user_tmp->mac[j];
            printf("%x", user_list[ip4].mac[j]);
        }
        snprintf(mac_sql,sizeof(mac_sql),UPDATE_FLOW_MAC,arp_user_tmp->mac[0],arp_user_tmp->mac[1],arp_user_tmp->mac[2],arp_user_tmp->mac[3],arp_user_tmp->mac[4],arp_user_tmp->mac[5],arp_user_tmp->ip);
        sql_put_mac_to_flow_table(mac_sql,&mysql);
        printf("\n%s\n",mac_sql);
    }
    else
    {
        printf("count !=0  %d\n",user_list[ip4].count);
    }
    return 0;
}

/*******************
arp��ƭ�����ͺ���,��apr_user_list����IPȻ��ȥ��ƭ�����˺�ͼ�1��ֱ������33�������ٿ����Է���û�м��������߶˿ڵ�UDP�����еĻ�������
***********************/
void arp_spoofind( )
{

	return 1;
    libnet_t *handle=NULL;        /* Libnet��� */
    int packet_size;
    u_int32_t dst_ip, src_ip;                /* ��·���Ŀ��IP��ԴIP */
    char error[LIBNET_ERRBUF_SIZE];        /* ������Ϣ */
    libnet_ptag_t arp_proto_tag, eth_proto_tag;



    int i=0;
    while(1)
    {
        ///YS ÿ5���ӷ�һ��ARP
        sleep(5);

        for(i=0; i<255; i++)
        {
            if(user_list[i].flag!=1)
            {
                continue;
            }

            printf("pass  arp>>>  %s   %d\n",user_list[i].ip,user_list[i].count);
            /* ��Ŀ��IP��ַ�ַ���ת���������� */
            dst_ip = libnet_name2addr4(handle, user_list[i].ip, LIBNET_RESOLVE);
            /* ��ԴIP��ַ�ַ���ת���������� */
            src_ip = libnet_name2addr4(handle, arp_source, LIBNET_RESOLVE);

            if ( dst_ip == -1 || src_ip == -1 )
            {
                printf("ip address convert error\n");
                continue;
            }

            /* ��ʼ��Libnet,ע���һ��������TCP��ʼ����ͬ */
            if ( (handle = libnet_init(LIBNET_LINK_ADV, my_device, error)) == NULL )
            {
                printf("158   %s\n",my_device);
                continue;
            }

            if(user_list[i].from==4)
            {

                /* ����arpЭ��� */
                arp_proto_tag = libnet_build_arp(
                                    ARPHRD_ETHER,        /* Ӳ������,1��ʾ��̫��Ӳ����ַ */
                                    ETHERTYPE_IP,        /* 0x0800��ʾѯ��IP��ַ */
                                    6,                    /* Ӳ����ַ���� */
                                    4,                    /* IP��ַ���� */
                                    ARPOP_REPLY,        /* ������ʽ:ARP���� */
                                    arp_mac_wrong,                /* source MAC addr */
                                    (u_int8_t *)&src_ip,    /* src proto addr */
                                    user_list[i].mac,                /* dst MAC addr */
                                    (u_int8_t *)&dst_ip,    /* dst IP addr */
                                    NULL,                /* no payload */
                                    0,                    /* payload length */
                                    handle,                /* libnet tag */
                                    0                    /* Create new one */
                                );
            }
            else
            {

                /* ����arpЭ��� */
                arp_proto_tag = libnet_build_arp(
                                    ARPHRD_ETHER,        /* Ӳ������,1��ʾ��̫��Ӳ����ַ */
                                    ETHERTYPE_IP,        /* 0x0800��ʾѯ��IP��ַ */
                                    6,                    /* Ӳ����ַ���� */
                                    4,                    /* IP��ַ���� */
                                    ARPOP_REPLY,        /* ������ʽ:ARP���� */
                                    arp_mac2,                /* source MAC addr */
                                    (u_int8_t *)&src_ip,    /* src proto addr */
                                    user_list[i].mac,                /* dst MAC addr */
                                    (u_int8_t *)&dst_ip,    /* dst IP addr */
                                    NULL,                /* no payload */
                                    0,                    /* payload length */
                                    handle,                /* libnet tag */
                                    0                    /* Create new one */
                                );
            }
            if (arp_proto_tag == -1)
            {
                printf("179\n");
                exit(-3);
            };



            if(user_list[i].from==4)
            {

                /* ����һ����̫��Э���*/
                eth_proto_tag = libnet_build_ethernet(
                                    user_list[i].mac,            /* ��̫��Ŀ�ĵ�ַ */
                                    arp_mac_wrong,            /* ��̫��Դ��ַ */
                                    ETHERTYPE_ARP,    /* ��̫���ϲ�Э�����ͣ���ʱΪARP���� */
                                    NULL,            /* ���أ�����Ϊ�� */
                                    0,                /* ���ش�С */
                                    handle,            /* Libnet��� */
                                    0                /* Э����ǣ�0��ʾ����һ���µ� */
                                );

            }
            else
            {
                /* ����һ����̫��Э���*/
                eth_proto_tag = libnet_build_ethernet(
                                    user_list[i].mac,            /* ��̫��Ŀ�ĵ�ַ */
                                    arp_mac2,            /* ��̫��Դ��ַ */
                                    ETHERTYPE_ARP,    /* ��̫���ϲ�Э�����ͣ���ʱΪARP���� */
                                    NULL,            /* ���أ�����Ϊ�� */
                                    0,                /* ���ش�С */
                                    handle,            /* Libnet��� */
                                    0                /* Э����ǣ�0��ʾ����һ���µ� */
                                );

            }
            if (eth_proto_tag == -1)
            {
                printf("build eth_header failure\n");
            }

            packet_size = libnet_write(handle);/* ����arp��ƭ�㲥 */
            printf("ARP SPOOFIND SENT  %x  %x  %x %x  %x\n",arp_mac2[0],arp_mac2[1],arp_mac2[2],arp_mac2[3],arp_mac2[4]);

            user_list[i].count++;
            if( user_list[i].from==4)
            {
                user_list[i].count=3;
            }
            else

                if(user_list[i].count>33 )
                {
                    user_list[i].flag=0;
                    user_list[i].count=0;
                }


        }

    }

    libnet_destroy(handle);                /* �ͷž�� */
}