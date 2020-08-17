#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "sernetwork.h"

int main()
{
	int listenfd = CreateSocket("127.0.0.1", 6000);
	assert(listenfd != -1);

	Start(listenfd);
}

