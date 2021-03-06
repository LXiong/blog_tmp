/*
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netpacket/packet.h>




/* /usr/include/linux/if_ether.h */
#define ETH_ALEN		6	/* Octets in one ethernet addr */
#define ETH_P_802_3		0x0001	/* Dummy type for 802.3 frames */
#define ETH_P_ALL		0x0003	/* Every packet (be careful!!!) */
#define ETHERTYPE_IP		0x0800	/* IP */
#define ETHERTYPE_ARP		0x0806	/* ARP: Address resolution */
#define ETHERTYPE_REVARP	0x8035	/* RARP: Reverse ARP */


/* header for ethernet */
/* /usr/include/linux/if_ether.h */
/* /usr/include/net/ethernet.h */
struct ether_header {
	unsigned char	ether_dst[ETH_ALEN];	/* destination ethernet address */
	unsigned char	ether_src[ETH_ALEN];	/* source ethernet address */
	unsigned short	ether_type;		/* ethernet pachet type */
};


#define ARPHRD_ETHER	1	/* Ethernet 10/100Mbps.  */
#define ARP_REQUEST	0x0001	/* ARP request */
#define ARP_REPLY	0x0002	/* ARP reply */

/* header for arp */
/* /usr/include/net/if_arp.h */
/* /usr/include/netinet/if_ether.h */
struct arp_header {
	unsigned short arp_hrd;			/* Format of hardware address */
	unsigned short arp_pro;			/* Format of protocol address */
	unsigned char arp_hln;			/* Length of hardware address */
	unsigned char arp_pln;			/* Length of protocol address */
	unsigned short arp_op;			/* ARP opcode (command) */

	unsigned char arp_sha[ETH_ALEN];	/* Sender hardware address */
	unsigned char arp_spa[4];		/* Sender IP address */
	unsigned char arp_tha[ETH_ALEN];	/* Target hardware address */
	unsigned char arp_tpa[4];		/* Target IP address */
};


/* struct for arp package */
struct arp_packet
{
	struct ether_header eth;
	struct arp_header arp;
};





void
send_arp()
{
	/* fill ethernet header */


	/* fill arp header */
}



int
main(int argc, char *argv[])
{
	int sfd, n;
	struct in_addr dst_addr, src_addr;

	/* netpacket/packet.h */
	struct sockaddr_ll toaddr;	/* 不能使用struct sockaddr_in结构 */
	struct ifreq ifr;		/* linux/if_ether.h */

	unsigned char src_mac[ETH_ALEN] = { 0 };
	/* 全网广播ARP请求 */
	unsigned char dst_mac[ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	struct arp_packet packet;


	if (argc != 3) {
		fprintf(stderr, "Usage: %s <dev_name> <dst_ip>\n", argv[0]);
		exit(1);
	}


	if ((sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
		perror("socket() failed");
		exit(1);
	}

	memset(&toaddr, 0, sizeof(toaddr));

	/* 获取网卡接口索引 */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
	if (ioctl(sfd, SIOCGIFINDEX, &ifr) == -1) {
		perror("get dev index error");
		exit(1);
	}
	toaddr.sll_ifindex = ifr.ifr_ifindex;
	printf("interface index: %d\n", ifr.ifr_ifindex);

	/* 获取网卡接口IP地址 */
	if (ioctl(sfd, SIOCGIFADDR, &ifr) == -1) {
		perror("get ip addr error");
		//exit(1);
	} else {
		src_addr = ((struct sockaddr_in *) &(ifr.ifr_addr))->sin_addr;
		printf("IP addr: %s\n", inet_ntoa(src_addr));
	}

	/* 获取网卡接口MAC地址 */
	if (ioctl(sfd, SIOCGIFHWADDR, &ifr) == -1) {
		perror("get dev mac addr error");
		exit(1);
	} else {
		memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
		printf("MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
			src_mac[0], src_mac[1], src_mac[2], src_mac[3],
			src_mac[4], src_mac[5]);
	}


	memset(&packet, 0, sizeof(packet));
	/* 开始填充，构造以太网头部 */
	memcpy(packet.eth.ether_dst, dst_mac, ETH_ALEN);
	memcpy(packet.eth.ether_src, src_mac, ETH_ALEN);
	packet.eth.ether_type = htons(ETHERTYPE_ARP);

	/* 开始填充ARP报文 */
	packet.arp.arp_hrd = htons(ARPHRD_ETHER);	/* 硬件类型为以太 */
	packet.arp.arp_pro = htons(ETHERTYPE_IP);	/* 协议类型为IP */

	/* 硬件地址长度和IPV4地址长度分别是6字节和4字节 */
	packet.arp.arp_hln = ETH_ALEN;
	packet.arp.arp_pln = 4;

	/* 操作码，这里我们发送ARP请求 */
	packet.arp.arp_op = htons(ARP_REQUEST);

	/* 填充发送端的MAC和IP地址 */
	memcpy(&(packet.arp.arp_sha), src_mac, ETH_ALEN);
	memcpy(&(packet.arp.arp_spa), &src_addr, 4);

	/* 填充目的端IP地址，MAC地址不用管 */
	inet_pton(AF_INET, argv[2], &dst_addr);
	memcpy(&(packet.arp.arp_tpa), &dst_addr, 4);

	toaddr.sll_family = PF_PACKET;

	/* 为什么我发送一个request包，wireshark或者接收程序会抓到两个包，也就是
	ARP包发送了两遍? */
	n = sendto(sfd, &packet, sizeof(packet), 0,
		(struct sockaddr *) &toaddr, sizeof(toaddr));

	close(sfd);
	return 0;
}









