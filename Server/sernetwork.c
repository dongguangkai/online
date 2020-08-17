#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/epoll.h>
#include <fcntl.h>

#include "sernetwork.h"
#include "serbusiness.h"

char *file[] = { "main.c", "main.cpp", "main.java", "main.py", "main.go" };

int CreateSocket(char *ip, short port)
{
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)  return -1;

	struct sockaddr_in ser_addr;
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = inet_addr(ip);

	int res = bind(listenfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
	if (res == -1) return -1;

	res = listen(listenfd, LISTENSIZE);
	if (res == -1) return -1;

	return listenfd;
}

static void InsertFdToEpoll(int epfd, int fd, short event)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = event;
	int res = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	assert(res != -1);
}

static void DeleteFdFromEpoll(int epfd, int fd)
{
	int res = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
	assert(res != -1);
}

static void GetNewLink(int epfd, int listenfd)
{
	struct sockaddr_in cli_addr;
	socklen_t len = sizeof(cli_addr);

	int c = accept(listenfd, (struct sockaddr*)&cli_addr, &len);
	if (c == -1) return;

	InsertFdToEpoll(epfd, c, EPOLLIN | EPOLLRDHUP | EPOLLET);
}

static int RecvFile(int fd)
{
	Head head;  //  language   filesize
	int n = recv(fd, &head, sizeof(head), 0);
	if (n <= 0)
	{
		return -1;
	}

	int filefd = open(file[head.language], O_WRONLY | O_CREAT | O_TRUNC, 0664);
	assert(filefd != -1);

	int sum = 0;

	while (1)
	{
		char buff[128] = { 0 };
		int size = head.filesize - sum > 127 ? 127 : head.filesize - sum;
		int n = recv(fd, buff, size, 0);
		sum += n;

		write(filefd, buff, n);

		if (sum == head.filesize)
		{
			break;
		}
	}

	close(filefd);

	return head.language;
}

static void SendFile(int fd, int flag, int size)
{
	if (!flag)
	{
		Head head;
		head.status = 0;
		head.filesize = size;
		send(fd, &head, sizeof(head), 0);

		int filefd = open("error.txt", O_RDONLY);

		while (1)
		{
			char buff[128] = { 0 };
			int n = read(filefd, buff, 127);
			if (n <= 0)
			{
				break;
			}

			send(fd, buff, n, 0);
		}

		close(filefd);
	}
}

static void DealReadyFd(int epfd, struct epoll_event *evbuff, int n, int listenfd)
{
	int i = 0;
	for (; i < n; ++i)
	{
		int fd = evbuff[i].data.fd;
		if (fd == listenfd)  //  有新的客户端链接
		{
			GetNewLink(epfd, fd);
		}
		else  //  fd这个客户端有数据到达
		{
			if (evbuff[i].events & EPOLLRDHUP)
			{
				DeleteFdFromEpoll(epfd, fd);
				close(fd);
			}
			else
			{
				// 两种选择： 1） 一次将数据读完  2）分多次将数据读完
				int language = RecvFile(fd);
				int err_size = BuildFile(language);
				if (err_size > 0)
				{
					//  将错误信息发送给客户端
					SendFile(fd, ERR, err_size);
					continue;
				}

				// 执行 a.out     java  main
			}
		}
	}
}

void Start(int listenfd)
{
	int epfd = epoll_create(EPOLLSIZE);
	assert(epfd != -1);

	InsertFdToEpoll(epfd, listenfd, EPOLLIN);

	while (1)
	{
		struct epoll_event evbuff[MAXEV];
		int n = epoll_wait(epfd, evbuff, MAXEV, -1);
		DealReadyFd(epfd, evbuff, n, listenfd);
	}
}