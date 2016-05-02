///YS �����Զ�������ݣ��ṹ��ͨ����������

#ifndef PACKET_INFO_H
#define PACKET_INFO_H
#include <pcre.h>
#include <sys/types.h>
#include <time.h>

///YS TCPFLAG��ֵ
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
#define	TH_ECE	0x40
#define	TH_CWR	0x80

///YS ����ʽ�ṹ��
typedef struct pcre_info
{
    char name[32];///YS Э������
    char feature[128]; ///YS Э�������
    pcre *p_pcre;///YS Э�������ʽ������ģ��
} S_pcre;


///YS �û�������Ϣ�Ľṹ��
typedef struct user_flow
{
    long flow;///YS �û�����
    char ip[32];///YS �û�IP
    int    sub_net;///YS �û����ڵ�����
    long limit;///YS �û�����������
} S_user_flow;


///YS �е�Э�����ڴ��ļ�ʱ��ǰ���в���������û�õģ���TFTP
typedef struct pro_header_offset
{
    char pro_name[32];///YS Э������
    char offset_feature[16];///YS ƫ����������
    int offset_type;///YS ƫ�����ͣ��ǹ̶��ĳ��ȣ�������������Ϊ׼
    int offset_len;///YS ƫ�ƵĹ̶�����
} S_pro_header_offset;

///YS ����Ӧ�õĽṹ��
typedef struct pcre_info_im
{
    char name[32];///YS Э������
    char feature[128];///YS Э������
    int case_value;///YS ID
    char content_feature_s[32];///YS �����ֶεĿ�ʼ������
    char content_feature_e[32];///YS �����ֶεĽ���������
    char sender_s[32];
    char sender_e[32];
    char receiver_s[64];
    char receiver_e[64];
    char login_user_s[64];
    char login_user_e[64];
    char login_user_ex[64];///YS ���ִ�EX�ı�������һ��Ҫ��������۲������ҵ���
    char user_name[64];
    char content_feature_ex[64];
    char receiver_ex[64];
    char sender_ex[64];
    char in_content_feature_s[64];
    char in_content_feature_e[64];
    char in_sender_s[64];
    char in_sender_e[64];
    char in_receiver_s[64];
    char in_receiver_e[64];
    char in_ex[64];///YS ����������յ�������Ϣ��������
    char ex_1[64];

} S_pcre_info_im;




typedef struct
{
    int flag;
    unsigned long int seq_start;
    unsigned long int seq_end;
} sack_info;


/*typedef struct stream
{
     sack_info sack_info_array[6];//6 maybe a bug
    int use;
    int last_sack_len;
    char stream_index[64]; //steam  index ,ip+prot+ip+prot
    unsigned long int in_seq;
    unsigned long int in_next_seq;
    unsigned long int in_next_seq2;
    unsigned long int in_ack;
    unsigned long int out_seq;
    unsigned long int out_next_seq;
    unsigned long int out_next_seq2;
    unsigned long int out_ack;
    char file_name[64];
    int len;
    int file_offset;
    char len_sign[32];
    char ostream[1024*2120];//4120 K B
    char istream[1024*2120];//4120KB
    int in_offset;
    int out_offset;
    char pro[32];//HTTP FTP  TFTP
    int time;
}S_stream;
*/


///YS BBSӦ�õĽṹ�壬������Ӧ�ò��
typedef struct bbs_info
{
    int case_value;
    char host[64];
    char host_1[32];///YS �е�BBS�Ƕ�����������ʽ���ֵ�
    char host_2[32];
    int host_url_type;
    char post_url_feature_s[32];
    char post_url_feature_s_2[32];
    char post_url_feature_e[32];
    char post_url_feature_e_2[32];
    char content_feature_s[32];
    char content_feature_e[32];
    char content_feature_s_2[32];
    char content_feature_e_2[32];
    char poster_s[32];
    char poster_e[32];
    char poster_s_2[32];
    char poster_e_2[32];
    char title_s[32];
    char title_e[32];
    char title_s_2[32];
    char title_e_2[32];
} S_bbs;


/*****************
  arp�û�
*****************/
typedef struct arp_user
{
    char arp_packet[4];
    char ip[32]; ///YS IP�û�
    u_int8_t mac[6];///YS ��Ӧ���û���MAC
    int count; ///YS �ۼƷ��͵�APR����
    int flag;///YS �Ƿ���Ҫ��ŵ�ARP_USER_LIST
    int from;///YS from==4ʱ���Ǿ���OVER FLOW���û�
} S_arp_user;



///YS ����Ӧ�õĽṹ�壬������Ӧ�ò��
typedef struct search_info
{
    int case_value;
    char host[64];
    char key_word_s[32];
    char key_word_e[32];
    char key_word_s_2[32];
    char key_word_e_2[32];
    char key_word_s_3[32];
    char key_word_e_3[32];
    char auto_search[32];///YS ����ʱ���Զ���ʾ
} S_search;

///YS �ʼ�����Ӧ�õĽṹ�壬������Ӧ�ò��
typedef struct email_info
{
    int case_value;
    char host[64];
    char content_feature_s[32];
    char content_feature_e[32];
    char content_feature_s_2[32];
    char content_feature_e_2[32];
    char sender_s[32];
    char sender_e[32];
    char sender_s_2[32];
    char sender_e_2[32];

    char receiver_s[64];
    char receiver_e[64];
    char receiver_s_2[64];
    char receiver_e_2[64];

    char title_s[32];
    char title_e[32];
    char title_s_2[32];
    char title_e_2[32];
} S_email;



///YS �ʼ�����õĽṹ�壬������Ӧ�ò��
typedef struct email_html_info
{
    int case_value;
    char host[64];
    char get_feature[244];
    char http_respont_feature[244];
    char save_flag_1[244];
    char save_flag_2[244];

    int pop3;
    char content_s[244];
    char content_e[244];
    char sender_s[244];
    char sender_e[244];
    char receiver_s[244];
    char receiver_e[244];
    char title_s[244];
    char title_e[244];
    char title_e2[244];
    int gzip;///YS �Ƿ���GZIPѹ���ˣ�ʵ���ϵĻ���������ͬһ���ʼ�ϵͳ��Ҳ��һ��ȫ�Ǳ�ѹ���ˣ��������ֵ�����ˣ�����Ҫ���ж�
} S_email_html;


///YS �����Զ�������Ľṹ��
typedef struct stream
{
    char stream_index[64]; ///YS ÿһ������������ʹ�ø����֣�ip+prot+ip+prot


    unsigned long int in_start;///YS IN������ʼ�����к�
    long in_effect_len;///YS IN�������Ч����
    unsigned long int in_ack;///YS IN�����ȷ�Ϻ�
    long in_buff_record;///YS IN����д��������Կ���Ϊistream[1024*1024]���������BUFF���α�

    unsigned long int out_start;///YS OUT������ʼ�����к�
    long out_buff_record;///YS out����д��������Կ���Ϊistream[1024*1024]���������BUFF���α�
    long out_effect_len;///YS OUT�������Ч����
    unsigned long int out_ack;///YS OUT�����ȷ�Ϻ�

    char file_name[512];///YS ��������漰�����ļ���
    int file_len;///YS �ļ�����
    long file_offset;///YS �ļ���ƫ����
    int pro_header_len;///YS ��TFTP���֣�����һ���̶��ĳ���
    int write_to_file;///YS �Ƿ�д���ļ�
    char ostream[1024*1024];///YS 4120 KB ��Out��������Ļ���
    char istream[1024*1024];///YS 4120 KB ��In��������Ļ���
    char pro[32];///YS Э��
    time_t time;///YS ʱ�����������Ϊ�����Ƿ��Ѿ��������
    char user_ip[24];///YS �û�IP
    char dst_ip[24];///YS Ŀ��IP
    int last_file_len;///YS ��ʱû��ʹ��
    int already_process;///YS �Ƿ񱻴������������Ļ�����һЩ�����Ѿ�������Ҫ���¿�ʼȡֵ
} S_stream;




///YS �ļ����ͽṹ��
typedef struct file_type
{
    char filetype[8];///YS .JPG  .RAR .ZIP  �ȵ�
    char filetype_header[256];///YS �ļ����ļ�ͷ
    pcre *p_pcre;///YS ��Ӧ���ļ����ļ�ͷ������ʽ������ģ��
} S_file_type;





///YS HTTP_INFO �ṹ��
typedef struct http_info
{
    int http_hl;///YS httpͷ����Ϣ�ĳ���
    int type;///YS 1:get  2:post  3:head
    char filename[512];///YS �ļ���
    char url[2024];///YS URL
    char user_ip[24];///YS �û�IP
    char dst_ip[24];///YS Ŀ��IP
    char  time[32];///YS ʱ���
    long file_len;///YS �ļ�����
    long file_offset;///YS �ļ���ƫ����
    char content_length[512];///YS ���ݵĳ���
    char content_range[512];///YS ���ݵ���ʼƫ��
    char attache_filename[1440];///YS �Ƿ���attache������filename
    char referer[1440];///YS refererURL

} S_http;



///YS SOCKET���յ����ݰ�
typedef struct sock_buff
{
    int len;///YS ����
    unsigned char raw_packet[1600];///YS ԭʼ���ݰ�
} S_sock_buff;


///YS �����Զ���İ�
typedef struct packet_info
{
    int time;  ///YS �������ʱ���
    unsigned char  raw_packet[1520];//the true len is 1514;
    char gid[8]; ///YS ��ID
    int flat;///YS �����������ò���
    int direction;///YS  ���� 1  in  |||  0 out
    int packet_size;///YS ���ݰ�����Ч���س���
    int cap_size;///YS ���ݰ�������
    struct timeval cap_time;///YS ץ������ʱ��
    char behavior_type[32];///YS Э����
    u_short src_port;///YS Դ�˿���
    u_short dst_port;///YS Ŀ����
    u_short ip_id;///YS IP���ĵ�ID
    char src_ip[32];///YS ԭIP
    char dst_ip[32];///YS Ŀ��IP
    unsigned long int seq;///YS SEQ
    unsigned long int ack;///YS ACK
    char stream[64];///YS TCP��������
    int packet_type;///YS 1 for tcp ,0for udp
    int offset;///YS ���ݰ���ͷ��ƫ�ƣ��������ƫ�ƣ��������ݰ�����Ч����
    //int tcp_options;//YS 1 for SACK
    //int gid;
    //struct HTTP_HEADER http_header;
} ElemType;



#endif
