#include "serbusiness.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>

static char *file[] = { "main.c", "main.cpp", "main.java", "main.py", "main.go" };

int BuildFile(int language)
{
	//  C  --   gcc    CPP  --  g++     Java  ---  javac    go ---  go build
	int num = 0;
	int fds[2];
	pipe(fds);
	pid_t pid = fork();
	assert(pid != -1);
	if (pid == 0)
	{
		close(fds[0]);
		close(1);
		close(2);
		dup(fds[1]);
		dup(fds[1]);
		switch (language)
		{
		case 0:
			execl("/usr/bin/gcc", "/usr/bin/gcc", file[language]);
			perror("gcc error: ");
			break;
		case 1:
			execl("/usr/bin/g++", "/usr/bin/g++", file[language]);
			perror("g++ error: ");
			break;
		case 2:
			execl("/usr/bin/javac", "/usr/bin/javac", file[language]);
			perror("javac error: ");
			break;
		default:
			break;
		}

		close(fds[1]);
		close(1);
		close(2);
	}
	else
	{
		close(fds[1]);
		wait(NULL);
		int fd = open("error.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		assert(fd != -1);

		
		while (1)
		{
			char buff[128] = { 0 };
			int n = read(fds[0], buff, 127);
			if (n <= 0)
			{
				break;
			}

			num += n;

			write(fd, buff, n);
		}

		close(fd);
	}

	return num;  // 0    > 0
}