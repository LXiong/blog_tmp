///YS SOCKET�ͻ��ˣ����ӵ��������Ǳ��Ի�ȡ���ݰ�
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "packet_info.h"
extern char server_ip[32];

extern int rawpacket_putin_buff(const int len ,const u_char * packet);


/*******************
�ͻ��ˣ�ʹ��6789�˿�ͨ�ţ�Ҫ��Ƶİ�����ͨ���ö˿ڽ��д���
********************/
int sock_client()
{
     int sockfd;
    int n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct sock_buff *my_sock_buff;
    struct arp_user *arp_user_temp;
    char buff[sizeof(struct sock_buff)];

    int portno = 6789;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)         printf("ERROR opening socket  server_ip  :%s\n",server_ip);
    server = gethostbyname(server_ip);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,          (char *)&serv_addr.sin_addr.s_addr,         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)         printf("ERROR connecting \n");
    while(1)
    {
       // n = read(sockfd,(void*)buff,sizeof(struct sock_buff));
       //dbug("OOOOOOOOOOOOOOOOOOo \n");
        n = recv(sockfd,(void*)buff,1544,MSG_WAITALL);///YS ����Ҫ��READ����Ȼ����ֲ������İ��������⺯����������������û�ж���1544ʱ��������������
        if (n <= 0)
        {
            printf("ERROR reading from socket  %s\n",server_ip);
            exit(0);
        }
        else
        {
            if(buff[0]=='v' && buff[1]=='v' &&  buff[2] =='v' && buff[3]=='v')
            {
                arp_user_temp=(struct arp_user*) buff;
                creat_arp_user_list(arp_user_temp);
            }

            if(!strncmp("reboot",buff,6))
            {
				system("pkill MYP");
				system("/mg/MYP/MYP &");
                dbug("reboot\n");
                exit(0);
            }
            my_sock_buff=(struct sock_buff*) buff;
         //   dbug("OOOOOOOOOOOOOOOOOOo %d\n",my_sock_buff->len);
           rawpacket_putin_buff (my_sock_buff->len,my_sock_buff->raw_packet);///YS д�뻺����
        }
        memset(buff,0,sizeof(struct sock_buff));
    }
    close(sockfd);
    return 0;
}
