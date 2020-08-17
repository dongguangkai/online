#pragma once

#define LISTENSIZE 5
#define EPOLLSIZE 5

#define MAXEV 100

typedef struct Head
{
	union 
	{
		int language;
		int status;
	};
	int filesize;
}Head;

#define ERR 0
#define SUC 1

int CreateSocket(char *ip, short port);

void Start(int listenfd);