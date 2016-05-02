#ifndef CONFIG_H
#define CONFIG_H
#define cb_rawpacket_len 102 ///YS 缓冲区的长度
#define file_name_str_len 2048///YS 有关FILENAME的字符数组的长度
//#define my_device "em0"
//#define debug_open 0
#define pcre_num 1024 ///YS 支持最大的特征字模版数
#define OVECCOUNT 256 ///YS 正则式匹配的结果数组
#define Audit_count 32///YS 每种审计最多支持多个，如支持搜索32个，邮件32个
//#define net_addr "92.168"
#define Tcp 1
#define Udp 2
/*d_in   XXXXX>192.168*/
#define d_in 1  ///YS 这个数据包的出入方向
/*d_out  192.168>XXXXX*/
#define d_out 0
#define support_file_num 300 ///YS 支持的文件类型


///YS 审计插入数据到数据库的SQL
#define INSERT_STREAM  "INSERT INTO `tcp_stream` (`stream_index`) VALUES ('%s');"
#define INSERT_AUDIT_SEARCH  "INSERT INTO `audit_search` (`Key_only`,`Name`,`Uid`,`User_IP`,`Http_website`,`Time`,`Detail`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_EMAIL  "INSERT INTO `audit_email` (`Key_only`,`Name`,`Uid`,`User_IP`,`Email_type`,`Time`,`Detail`,`receiver`,`sender`,`title`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_HTTP_HOST "INSERT INTO `audit_http` (`Name`,`User_IP`,`Uid`,`Time`,`Detail`,`Key_only`,`Referer`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_BBS "INSERT INTO `audit_bbs` (`Name`,`User_IP`,`Uid`,`Time`,`Detail`,`Key_only`,`Url`,`Poster`,`Title`,`Host`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_U_OR_D "INSERT INTO `attachment` (`Name`,`IP`,`Uid`,`Datetime`,`U_or_D`,`file_name`,`Gid`)VALUES ('%s','%s','%s','%s','%d','%s','%s');"
#define INSERT_AUDIT_IM "INSERT INTO `audit_im` (`Name`,`User_IP`,`Uid`,`Im_type`,`Time`,`Sender`,`Receiver`,`Detail`,`Comment`,`Gid`)VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_EMAIL_READ "INSERT INTO `audit_email_read` (`Name`,`Uid`,`User_IP`,`Datetime`,`file_name`,`Gid`)VALUES('%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_EMAIL_POP3 "INSERT INTO `audit_email_read` (`Name`,`Uid`,`User_IP`,`Datetime`,`Gid`,`Detail`,`Receiver`,`Sender`,`Title`,`Is_Pop3`)VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%d');"
#define INSERT_FLOW "INSERT INTO `flow_statistics` (`Name`,`User_IP`,`Uid`,`Gid`,`Flow`,`Block`,`Flow_limit`)VALUES('%s','%s','%s','%s','%ld','%d','%s');"
#define UPDATE_FLOW "UPDATE `flow_statistics` set `Flow`='%ld',`Block`='%d'where `User_IP`='%s';"
#define PUT_FILE_NAME_2_DB "INSERT INTO `file_name_tmp` (`name`, `ACK`, `create_time`,`stream`) VALUES ('%s', '%x', '%d','%s'); "


///从数据库里读取配置数据
#define GET_CONFIG_DEBUG  "select config_data from `system_configure` WHERE (`Config_name`='debug');"
#define GET_CONFIG_SUB_NET  "select config_data from `system_configure` WHERE (`Config_name`='SUB_NET');"
#define GET_CONFIG_NET_INTERFACE  "select config_data from `system_configure` WHERE (`Config_name`='NET_interface_2_re');"
#define GET_CONFIG_http_enable  "select config_data from `system_configure` WHERE (`Config_name`='http_enable');"
#define GET_CONFIG_search_enable  "select config_data from `system_configure` WHERE (`Config_name`='search_enable');"
#define GET_CONFIG_bbs_enable  "select config_data from `system_configure` WHERE (`Config_name`='bbs_enable');"
#define GET_CONFIG_email_enable  "select config_data from `system_configure` WHERE (`Config_name`='email_enable');"
#define GET_CONFIG_arp_mac "select config_data from `system_configure` WHERE (`Config_name`='arp_mac');"
#define GET_CONFIG_arp_source_ip "select config_data from `system_configure` WHERE (`Config_name`='arp_source');"

#define GET_CONFIG_db_ip "select config_data from `system_configure` WHERE (`Config_name`='DBSERV_IP');"
#define GET_CONFIG_ex_ip "select config_data from `system_configure` WHERE (`Config_name`='ex_ip');"
#define GET_CONFIG_ex_ip1 "select config_data from `system_configure` WHERE (`Config_name`='ex_ip1');"
#define GET_CONFIG_ex_ip2 "select config_data from `system_configure` WHERE (`Config_name`='ex_ip2');"


#define GET_feature_list "select Filetype from file_header where Enable =1"
#define GET_flow_list "select User_IP,Flow,Flow_limit from flow_statistics"
#define GET_file_name_header_list "select Filetype,Fileheader from file_header where enable =1"
#define GET_app_im_feature_list "select * from im_feature"
#define SQL_GET_PORT "select sport,name,offset_type,offset from port_table_statistics where enable =1"
#define GET_NAME_FEATURE "select name,start,lenth,lenth_type from file_name_feature"
#define GET_FILE_NAME_USE_ACK_OR_STREAM "select name from file_name_tmp where ACK='%x' or stream='%s';"
#define GET_FILE_NAME_USE_STREAM "select name from file_name_tmp where stream='%s';"
#define GET_USER_NAME_UID "select Name,Uid,Gid,Flow from user where User_IP ='%s';"
#define DEL_FILE_NAME  "DELETE FROM `file_name_tmp` WHERE (`name`='%s');"

#define GET_CONFIG_SERVERIP  "select config_data from `system_configure` WHERE (`Config_name`='serverIP');"
#define GET_CONFIG_net_version "select config_data from `system_configure` WHERE (`Config_name`='version_re');"
#define GET_CONFIG_LOGUPDATESETTING  "select config_data from `system_configure` WHERE (`Config_name`='logUpdateSetting');"
#define GET_CONFIG_LOG  "select config_data from `system_configure` WHERE (`Config_name`='log');"
#define GET_CONFIG_DBSERV_IP  "select config_data from `system_configure` WHERE (`Config_name`='DBSERV_IP');"
#define GET_CONFIG_LOGEFFECTIVEDAYS  "select config_data from `system_configure` WHERE (`Config_name`='logEffectiveDays');"

///YS 我们存放文件的路径
#define tmp_download_file_path "/home/www/var/download/%s/%s/%s"
#define tmp_upload_file_path "/home/www/var/upload/%s/%s/%s"
#define tmp_email_file_path "/home/www/var/email/%s/%s/%s/%d.html"
#define tmp_email_file_dir "/home/www/var/email/%s/%s/%s"


#define system_str " dd if=/dev/zero of=\"%s\" bs=%ld count=1"  ///YS 该命令可以生成一个空的文件


#endif
