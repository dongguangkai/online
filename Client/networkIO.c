/*
   所有网络操作相关方法
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include <fcntl.h>

#include "networkIO.h"

static char*   file[] = { "main.c", "main.cpp", "main.java", "main.py", "main.go" };

int LinkServer(char *ip, short port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) return -1;

	struct sockaddr_in ser_addr;
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = inet_addr(ip);

	int res = connect(sockfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
	if (res == -1) return -1;

	return sockfd;
}

void SendFile(int sockfd, int language)
{
	struct stat st;
	stat(file[language], &st);
	Head head;
	head.language = language;
	head.filesize = st.st_size;

	send(sockfd, &head, sizeof(head), 0);

	int fd = open(file[language], O_RDONLY);
	assert(fd != -1);

	while (1)
	{
		char buff[128] = { 0 };
		int n = read(fd, buff, 127);
		if (n <= 0)
		{
			break;
		}

		send(sockfd, buff, n, 0);
	}

	close(fd);
}

void RecvResult(int sockfd)
{
	Head head;  //  language   filesize
	int n = recv(sockfd, &head, sizeof(head), 0);
	if (n <= 0)
	{
		return -1;
	}
	if (head.status == 0)
	{
		printf("Build ERROR:::\n");
	}

	int sum = 0;

	while (1)
	{
		char buff[128] = { 0 };
		int size = head.filesize - sum > 127 ? 127 : head.filesize - sum;
		int n = recv(sockfd, buff, size, 0);
		sum += n;

		printf("%s", buff);

		if (sum == head.filesize)
		{
			break;
		}
	}
}