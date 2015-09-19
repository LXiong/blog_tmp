
#include <stdio.h>
#include <arpa/inet.h>

int
main(int argc, char *argv[])
{
	char *addr = "192.168.1.256";
	struct sockaddr_in conv_addr;


	if (!inet_aton(addr, &conv_addr.sin_addr))
		fprintf(stderr, "inet_aton() failed\n");
	else
		printf("network order int addr: %#x\n", conv_addr.sin_addr.s_addr);

	return 0;
}





