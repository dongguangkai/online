/*
   �����ͻ��˳������ڴ���
*/
#include "networkIO.h"
#include "businesslogic.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

int main()
{
	int sockfd = LinkServer("127.0.0.1", 6000);
	assert(sockfd != -1);

	int language = ChoseLanguage();

	while (1)
	{
		//1�����û�������룬���ұ������
		InputCode(language);

		SendFile(sockfd, language);

		RecvResult(sockfd);

		int next = ChoseNext();
		if (next == 0)
		{
			break;
		}
		else if (next == 1)
		{
			RemoveFile(language);
		}
	}

	close(sockfd);
}