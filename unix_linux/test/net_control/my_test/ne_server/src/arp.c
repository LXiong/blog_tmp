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
#include "packet_info.h"


u_char mac[6]; ///ys ����û����
char mac_2_ip[32];///ys MAC ��Ӧ ��IP
char db_ip[24];///ys ����ƭ��Ŀ��IP
int ip1,ip2,ip3,ip4;///ys ��IP1.IP2.IP3.IP4��������ʽ����IP�ַ���ÿ���ֶε�ֵ
struct arp_user user_list[255];///ys ��ҪARP���û�
extern char my_device[64];///ys ��������
//extern u_int8_t arp_mac[6];
extern  unsigned int arp_mac2[6];///ys ������mac
unsigned int arp_mac_wrong[6]= {0x11,0x11,0x11,0x11,0x11,0x11}; ///ys ������mac
extern char arp_source[32];///ys ������IP
char ex_ip[24];///ys VIPIP1�����ᱻ���ص�IP����VIPIP
char ex_ip1[24];///ys VIPIP2
char ex_ip2[24];///ys VIPIP3
extern char sub_net[24];///ys Ҫ������������192.168
extern int newsockfd;///YS reboot CMD flag

extern char localip[32];

/**********************
�ú������ڰ�����ARP��Ŀ��IP���뵽���ǵ�APR_USRR_LIST���棬����1"const u_char *my_packet",������ͨ��LIBPCAP��ץ����ԭʼ�İ�
**********************/

/*int get_arp_user_list_from_db(MYSQL *sock)
{
    return 0;
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


*/

/**********************
�ú������ڰ�����ARP��Ŀ��IP���뵽���ǵ�APR_USRR_LIST���棬����1"const u_char *my_packet",������ͨ��LIBPCAP��ץ����ԭʼ�İ�
**********************/
/*int creat_arp_user_list(const u_char *my_packet)
{
    int j;
    struct ip * ip_header;
    struct ether_header *eth_header;

    eth_header=(struct ether_header*)my_packet; //���ݰ���֡��ͷ������˫����MACҲ��������
    ip_header = (struct ip *)(my_packet + sizeof(struct ether_header)); //���ݰ���IP��ͷ������˫����IPҲ��������

    if(strstr(inet_ntoa(ip_header->ip_src),sub_net)&&strstr(inet_ntoa(ip_header->ip_dst),sub_net)) return 0;///���������ͨ�ţ�������
    if(strstr(inet_ntoa(ip_header->ip_src),"255.255")||strstr(inet_ntoa(ip_header->ip_dst),"255.255")) return 0;///���������ͨ�ţ�������

    strcpy(mac_2_ip,inet_ntoa(ip_header->ip_src));///YS ��ȡĿ��IP

    if(strstr(mac_2_ip,sub_net))///YS �ж��ǲ������ǵ�����IP���ǵĻ��ͻ�ȡ����MAC
    {
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS ��ȡ����MAC
        //if(strlen(user_list[ip4].ip)<3||user_list[ip4].count>33 && user_list[ip4].flag==0)
        if(user_list[ip4].flag==0)///YS �����û����ARP�����IP�Ļ�,�Ͱ����IP���뵽���ǵ�ARP_USR_LIST��,���ҷ��ʹ���Ϊ1,�Ѿ�����ı�־Ϊ1
        {
            printf("insert arp_usr_list   %s    %d\n",mac_2_ip,ip4);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_shost[j];
                printf("%x|\n", user_list[ip4].mac[j]);
            }
        }
    }
    else
    {
        strcpy(mac_2_ip,inet_ntoa(ip_header->ip_dst));
        if(!strstr(mac_2_ip,sub_net)) return 0;
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
        if(strlen(user_list[ip4].ip)<3||user_list[ip4].count>33)
        {
            printf("insert arp_usr_list     %d\n",ip4);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_dhost[j];
                printf("%x|", user_list[ip4].mac[j]);
            }
        }
    }
    sprintf(user_list[ip4].ip,"%s",mac_2_ip);

    return 0;
}
*/

/**********************
�ú������ڰ�����ARP��Ŀ��IP���뵽���ǵ�APR_USRR_LIST���棬����1"const u_char *my_packet",������ͨ��LIBPCAP��ץ����ԭʼ�İ�
**********************/
int send_arp_user(const u_char *my_packet)
{
    int j;
    struct ip * ip_header;
    struct ether_header *eth_header;

    eth_header=(struct ether_header*)my_packet; //���ݰ���֡��ͷ������˫����MACҲ��������
    ip_header = (struct ip *)(my_packet + sizeof(struct ether_header)); //���ݰ���IP��ͷ������˫����IPҲ��������

    if(strstr(inet_ntoa(ip_header->ip_src),sub_net)&&strstr(inet_ntoa(ip_header->ip_dst),sub_net)) return 0;///���������ͨ�ţ�������
    if(strstr(inet_ntoa(ip_header->ip_src),"255.255")||strstr(inet_ntoa(ip_header->ip_dst),"255.255")) return 0;///���������ͨ�ţ�������

    strcpy(mac_2_ip,inet_ntoa(ip_header->ip_src));///YS ��ȡĿ��IP

    if(strstr(mac_2_ip,sub_net))///YS �ж��ǲ������ǵ�����IP���ǵĻ��ͻ�ȡ����MAC
    {
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS ��ȡ����MAC
        if(user_list[ip4].count==0 )
       // if(user_list[ip4].flag==0)///YS �����û����ARP�����IP�Ļ�,�Ͱ����IP���뵽���ǵ�ARP_USR_LIST��,���ҷ��ʹ���Ϊ1,�Ѿ�����ı�־Ϊ1
        {
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            sprintf(user_list[ip4].arp_packet,"%s","vvvv");
            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_shost[j];
                printf("%x|\n", user_list[ip4].mac[j]);
            }
        }
    }
    else
    {
        strcpy(mac_2_ip,inet_ntoa(ip_header->ip_dst));
        if(!strstr(mac_2_ip,sub_net)) return 0;
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
        if(user_list[ip4].count==0)
        {
             printf(" 111111111 %d\n",user_list[ip4].flag);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            sprintf(user_list[ip4].arp_packet,"%s","vvvv");

            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_dhost[j];
                printf("%x|", user_list[ip4].mac[j]);
            }
        }

    }
    sprintf(user_list[ip4].ip,"%s",mac_2_ip);

    return 0;
}




/*******************
arp��ƭ�����ͺ���,��apr_user_list����IPȻ��ȥ��ƭ�����˺�ͼ�1��ֱ������33�������ٿ����Է���û�м��������߶˿ڵ�UDP�����еĻ�������
***********************/
void arp_spoofind( )
{


    int i=0;
    while(1)
    {
        ///YS ÿ5���ӷ�һ��ARP
        sleep(1);

        for(i=0; i<255; i++)
        {
            if(user_list[i].flag!=1)
            {
                continue;
            }


            if(!strcmp(user_list[i].ip,db_ip))///YS ��������ݿ�IP���Ͳ���
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,ex_ip1))///YS �����VIP���Ͳ���
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,arp_source))///YS ���������IP���Ͳ���
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,ex_ip))///YS �����VIP���Ͳ���
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,ex_ip2))///YS �����VIP���Ͳ���
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,localip))///YS ������Լ�
            {
                continue;
            }

            user_list[i].count++;

            if(user_list[i].count==2)
            {
                if (newsockfd >0)///YS �����ƶ��������ˣ��Ͱ��������ⷢ
                {
                    char buff[1600];
                    memcpy(buff,&user_list[i],1544);
                    if (write(newsockfd,&user_list[i],1544)< 0)///YS ������ƶ˵�����
                    {
                        close(newsockfd);
                        newsockfd=-1;
                    }
                    else
                    {
                        printf("pass  arp>>>  %c  %c  %c  %c  \n",buff[0],buff[1],buff[2],buff[3]);
                        printf("pass  arp>>>  %s  \n",user_list[i].arp_packet);
                        printf("ARP SPOOFIND SENT %s\n",user_list[i].ip);
                    }
                }
            }
            if(user_list[i].count>33 )
            {
                user_list[i].flag=0;
                user_list[i].count=0;
            }

        }

    }

}
