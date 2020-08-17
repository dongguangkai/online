#pragma once


typedef struct Head
{
	union
	{
		int language;
		int status;
	};
	int filesize;
}Head;

int LinkServer(char *ip, short port);

void SendFile(int sockfd, int language);

void RecvResult(int sockfd);
