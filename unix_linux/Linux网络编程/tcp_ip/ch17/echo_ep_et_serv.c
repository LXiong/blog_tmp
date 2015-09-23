/*
 * 对echo_epoll_et_server.c的完善
 * 边缘触发套接字一定要改为非阻塞
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>


#define BUF_SIZE	5
#define BACK_LOG	128
#define EPOLL_SIZE	100



/*
 * create IPv4 TCP socket for server
 */
int
create_socket(const char *port)
{
	struct sockaddr_in svaddr;
	int fd, opt;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		return -1;
	}

	opt = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
		fprintf(stderr, "setsockopt() failed: %s\n", strerror(errno));  
		return -1;
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(atoi(port));


	if (bind(fd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		close(fd);
		return -1;
	}

	if (listen(fd, BACK_LOG) == -1) {
		fprintf(stderr, "listen() failed: %s\n", strerror(errno));
		close(fd);
		return -1;
	}

	return fd;
}

void
set_nonblocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}


int
main(int argc, char *argv[])
{
	int i, str_len;

	char buf[BUF_SIZE];
	int timeout;

	struct sockaddr_in cli_addr;
	socklen_t socklen;
	int socket_fd, connect_fd;

	struct epoll_event event;
	struct epoll_event *wait_events;
	int epollfd, event_cnt;


	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	socket_fd = create_socket(argv[1]);
	if (socket_fd == -1) {
		fprintf(stderr, "create_socket() failed.\n");
		exit(EXIT_FAILURE);
	}

	set_nonblocking(socket_fd);

	// 初始化
	epollfd = epoll_create(EPOLL_SIZE);
	wait_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = socket_fd;

	epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_fd, &event);


	socklen = sizeof(cli_addr);
	while (1) {

		// -1是阻塞，0是不会阻塞，大于0至多阻塞timeout毫秒
		timeout = 1000;

		event_cnt = epoll_wait(epollfd, wait_events, EPOLL_SIZE, timeout);
		if (event_cnt == -1) {
			fprintf(stderr, "epoll() failed\n");
			break;
		} else if (event_cnt == 0) {
			printf("time out!\n");
			continue;
		}

		// 注意BUF_SIZE的大小
		puts("return epoll wait\n");

		//
		for (i = 0; i < event_cnt; ++i) {

			// new client connection
			if (wait_events[i].data.fd == socket_fd) {
				connect_fd = accept(socket_fd,
					(struct sockaddr *) &cli_addr,
					&socklen);

				set_nonblocking(connect_fd);

				event.events = EPOLLIN | EPOLLET;	// 改为边缘触发
				event.data.fd = connect_fd;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, connect_fd, &event);
				printf("new client connected\n");
			} else {
				while (1) {
					// read msg
					str_len = read(wait_events[i].data.fd, buf, sizeof(buf));
					if (str_len == 0) {
						// client closed
						epoll_ctl(epollfd, EPOLL_CTL_DEL, wait_events[i].data.fd, NULL);
						close(wait_events[i].data.fd);
						printf("client closed\n");
						break;
					} else if (str_len < 0) {
						if (errno == EAGAIN)
							break;
					} else {
						// echo
						write(wait_events[i].data.fd, buf, str_len);
					}
				}
			}

		}
	}
	close(epollfd);
	close(socket_fd);

	return 0;
}

