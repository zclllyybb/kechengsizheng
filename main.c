/**
 * @endcode GBK
 */
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#endif

#include "function.h"

int main()
{
	init_read(); // ��ʼ������
	init_screen();//��ʼ����Ļ
	printf("��Ļ��ʼ�����");
	while (!run())
		sleep(1000);
	printf("��ϰ����������%dö����������%dö��", capture, escape);
	return 0;
}