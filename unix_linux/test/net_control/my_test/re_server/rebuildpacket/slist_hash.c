///YS 该本件主要用于处理数据包的保存与流的退出
#include <glib.h>
#include "packet_info.h"
#include <stdio.h>
#include "config.h"
#include <stdlib.h>
#include <string.h>

struct packet_info *pack_t2;
extern GHashTable *tcp_stream_list;
extern GHashTable *tcp_stream_pool;
extern int process_stream(struct stream * s);
extern int create_tmp_file( struct stream * s,int in_len,int out_len);
extern char net_addr[8];
char right_seq[16];
char wrong_seq[16];
char *point;
char *point2;
int sack_len;
int s_i=0;
int s_j=0;
int s_k=0;
int result_index_sack=0;
char or_hex[3];
char true_hex[10];
int sack_effect_len=0;
int mallco_num=0;
extern int ex;
/*********************************
保存数据包，把数据包保存到相应的TCP流结构体里
参数1，要处理的数据包
*********************************/
int store_packet(struct packet_info *packet_tmp2)
{

    struct packet_info *packet_tmp=NULL;
    packet_tmp=packet_tmp2;
    char tcp_stream[64];
    memset(tcp_stream,0,sizeof(tcp_stream));
    struct stream * c;
    c=NULL;
    snprintf(tcp_stream,sizeof(tcp_stream),packet_tmp->stream);
    c=g_hash_table_lookup(tcp_stream_list,tcp_stream);///YS 找一下这个包对应的流是否已经存在了
    if(c)
    {

        //dbug("ddddddd\n");
        c->time=packet_tmp->time;///YS 更新流的时间

        if(packet_tmp->direction)///in
        {
            c->out_effect_len=(int)(packet_tmp->ack-c->out_start);///YS 每个方向的有效长度，是靠返方向的应答包确定的，由应答包的ACK字段确定
            ////dbug("ack  %x  start  %x  >>>%d\n\n",packet_tmp->ack,c->out_start,c->out_effect_len);
        }
        else///out
        {
            c->in_effect_len=packet_tmp->ack-c->in_start;///YS 返方向的,BUFF里有效的长度
        }
        if(packet_tmp->packet_size==0)   return 0;///YS TCP负载数据为0，没有内容，不用继续走下去


        if(packet_tmp->direction)///方向in
        {
            long tmp_num=packet_tmp->seq-c->in_start-c->in_buff_record;
            //  dbug("111 %x --- %x    \n %ld --- %ld  \n  %ld\n",packet_tmp->seq,c->in_start,packet_tmp->seq-c->in_start,c->in_buff_record  ,tmp_num);
            if(tmp_num>995000)  ///YS 为什么不用对方的ACK？因为ACK有可能远小于 SEQ,ACK=1000000时,seq可能大于1024*1024
            {
                //dbug("imp  seq is %d   %x \n",packet_tmp->seq,packet_tmp->seq);
                //dbug("imp packet_tmp->seq-c->in_start   ===  > %d\n",packet_tmp->seq-c->in_start);
//     dbug("tmp_num   ===  > %d     in_buff_record %d    %d   %s\n",tmp_num,c->in_buff_record,c->in_effect_len,c->stream_index);
                ///YS 先放到文件里，再去HTTP等处理
                int in_count=c->in_effect_len-c->in_buff_record;
                int out_count=c->out_effect_len-c->out_buff_record;
                if(tmp_num>=0 && tmp_num<1100000)
                {
                    memmove(c->istream+tmp_num   ,    packet_tmp->raw_packet+packet_tmp->offset  ,  packet_tmp->packet_size);
                    create_tmp_file( c,in_count,out_count);///YS ，如果超过995000时，我们就为这个流创建临时文件，创建完成后，就可以清空缓冲区了
                    ///empty buff
                    memset(c->istream,0,in_count);
                    ///memCPY last buff
                    memmove(c->istream,c->istream+in_count,sizeof(c->istream)-in_count);
                    ///empty the last buff
                    memset(c->istream+in_count,0,sizeof(c->istream)-in_count);
                    c->in_buff_record=c->in_effect_len;///YS 每一次写完，我们都需要更新我们的起始位，下次开始时，可以接着写
                }
            }
            if(tmp_num>=0 && tmp_num<1100000)
            {
                //           dbug("move %x --- %x     %d --- %d    %d\n",packet_tmp->seq,c->in_start,packet_tmp->seq-c->in_start,c->in_buff_record  ,tmp_num);
                //          dbug("move2   %d --- %d  \n",packet_tmp->offset ,packet_tmp->packet_size);
                memmove(c->istream+tmp_num   ,    packet_tmp->raw_packet+packet_tmp->offset  ,  packet_tmp->packet_size);
            }
            else
            {
//                dbug("tmp_num wrong  %ld\n",tmp_num);
                // dbug("1111sss %x --- %x    %d --- %d    %d  %s\n",packet_tmp->seq,c->in_start,packet_tmp->seq,c->in_start,tmp_num,c->stream_index);
                // free(c->in_start);
                //  exit(0);
            }
        }
        else///YS 同IN方向一样，只是反方向了
        {
            long tmp_num=0;
            tmp_num=packet_tmp->seq-c->out_start-c->out_buff_record;
            if(tmp_num>995000)
            {
                ///先放到文件里，再去HTTP等处理
                int in_count=c->in_effect_len-c->in_buff_record;
                int out_count=c->out_effect_len-c->out_buff_record;
                if(tmp_num>=0 && tmp_num<1100000)
                {
                    memmove(c->ostream+(tmp_num),packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->packet_size);
                    create_tmp_file( c,in_count,out_count);
                    ///empty buff
                    memset(c->ostream,0,out_count);
                    ///memCPY last buff
                    memmove(c->ostream,c->ostream+out_count,sizeof(c->ostream)-out_count);
                    ///empty the last buff
                    memset(c->ostream+out_count,0,sizeof(c->ostream)-out_count);
                    c->out_buff_record=c->out_effect_len;
                    //            dbug("move2 sucess !!!\n");
                }
            }
            // memmove(c->ostream+(packet_tmp->seq-c->out_start),packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->packet_size);
            if(tmp_num <1100000&&tmp_num>=0)
            {
                memmove(c->ostream+(tmp_num),packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->packet_size);
            }
            else
            {
                dbug("2222 tmp_num wrong   %ld \n",tmp_num);
                //              dbug("2222 stream index %s\n",c->stream_index);
//                dbug("2222 %p --- %p     %ld --- %ld    %ld\n",packet_tmp->seq,c->out_start,packet_tmp->seq-c->out_start,c->out_buff_record  ,tmp_num);
                //    free(c->in_start);
                // exit(0);
            }
        }

    }
    else///YS 如果这个包没有找到他对应的TCP流，那就创建一个TCP流
    {
        if(packet_tmp->flat==0x02 )///YS 如果是第一个请求连接包，我们忽略
        {
            return 0 ;
        }

				// dbug("insert >>>>> %s\n",packet_tmp->stream);
        //struct stream *stream_tmp=(struct stream *)malloc(sizeof(struct stream));
        struct stream *stream_tmp=calloc(1,sizeof(struct stream));
        packet_tmp->seq=packet_tmp->seq+1;///YS 这里要加1，TCP的流SEQ是以+1开始的

        if(strstr(packet_tmp->dst_ip,net_addr))///YS 给流的IP赋值
        {
            strcpy(stream_tmp->user_ip,packet_tmp->dst_ip);
            strcpy(stream_tmp->dst_ip,packet_tmp->src_ip);
        }
        else
        {
            if(strstr(packet_tmp->src_ip,net_addr))
            {
                strcpy(stream_tmp->user_ip,packet_tmp->src_ip);
                strcpy(stream_tmp->dst_ip,packet_tmp->dst_ip);
            }
        }

        snprintf(stream_tmp->stream_index,64,"%s",packet_tmp->stream);///YS 流的索引名
        stream_tmp->pro_header_len=0;///YS 协议头长
        stream_tmp->file_offset=-1;///YS 文件的对应文件写到哪里
        stream_tmp->already_process=0;///YS 是否被处理过
        stream_tmp->in_buff_record=0;///YS IN方向的起始位记录
        stream_tmp->in_start=0;///YS IN方向开始的序列号
        stream_tmp->out_start=0;///YS OUT方向开始的序列号
        snprintf(stream_tmp->pro,32,"%s",packet_tmp->behavior_type);  ///YS 流的协议名
        if(packet_tmp->direction)///YS 序列号的获取
        {
            stream_tmp->in_start=packet_tmp->seq;
             stream_tmp->out_start=packet_tmp->ack;
        }
        else
        {
            stream_tmp->out_start=packet_tmp->seq;
            stream_tmp->in_start=packet_tmp->ack;
        }

        //dbug("11111>>>>. insert to hash list %s   instart :%x   outstart  %x\n",tcp_stream,stream_tmp->in_start,stream_tmp->out_start);
        g_hash_table_insert(tcp_stream_list, g_strdup(tcp_stream), stream_tmp);///YS 把流的指针插入到哈希表中
        mallco_num++;
        //dbug("hash list insert count  %d\n",mallco_num);
        fflush(stdout);
    }
    return 0;
}


int remove_stream(struct packet_info *packet_tmp)
{
    char tcp_stream[64];
    memset(tcp_stream,0,64);
    snprintf(tcp_stream,64,"%s",packet_tmp->stream);
    ///ulink!!!
    struct stream * c;
    c=NULL;
    c=g_hash_table_lookup(tcp_stream_list, tcp_stream);
    if(c)
    {
        int in_count=c->in_effect_len-c->in_buff_record;
        int out_count=c->out_effect_len-c->out_buff_record;
        if(in_count>0||out_count>0)
        {
            //dbug("remove it %s   inlen is %d  in_effect_len  is %d \n",c->stream_index,in_count,c->in_effect_len);
            create_tmp_file( c,in_count,out_count);
            process_stream(c);
            if(ex){
				//printf("in remove_stream\n");
				//exit(0);
				}
        }
        if(g_hash_table_remove(tcp_stream_list,tcp_stream))
        {
            //dbug("g_hash_table_remove2  success !!\n");
        }
    }
    return 0;
}

