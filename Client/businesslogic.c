#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <fcntl.h>

#include "businesslogic.h"

char*   file[] = {"main.c", "main.cpp", "main.java", "main.py", "main.go"};

//C   C++   Java   Python   Golang
int ChoseLanguage()
{
	int num = 0;

	while (1)
	{
		printf("******************Please Chose Language*******************\n");
		printf("               1 ----> C \n");
		printf("               2 ----> CPP \n");
		printf("               3 ----> Java \n");
		printf("               4 ----> Python \n");
		printf("               5 ----> GoLang \n");
		printf("*********************************************************\n");
		printf("Please input a num: ");
		
		scanf("%d", &num);
		if (num < 0 || num > 5)
		{
			printf("input error\n");
		}
		else
		{
			break;
		}
	}

	return num - 1;
}

int ChoseNext()
{
	int num = 0;
	while (1)
	{
		printf("******************Please Chose Next*******************\n");
		printf("               0 ----> exit \n");
		printf("               1 ----> next \n");
		printf("               2 ----> change coding \n");
		printf("*********************************************************\n");
		printf("Please input a num: ");

		scanf("%d", &num);
		if (num < 0 || num > 2)
		{
			printf("input error\n");
		}
		else
		{
			break;
		}
	}

	return num;
}

void RemoveFile(int language)
{
	remove(file[language]);
}

void InputCode(int language)
{
	pid_t pid = fork();
	assert(pid != -1);

	if (pid == 0)
	{
		execl("/usr/bin/vim", "/usr/bin/vim", file[language], (char*)0);
		perror("execl err: ");
		exit(0);
	}
	else
	{
		wait(NULL);
	}
}