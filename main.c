/**
 * @endcode GBK
 */
#include "function.h"
#include <stdio.h>

int main()
{
	setbuf(stdout, NULL); //�ر�������壬ǿ��ʵʱ������
#ifdef DEBUG
	puts("DEBUGGING!");
#endif
	init_read(); // ��ʼ������
	init_screen();//��ʼ����Ļ
	while (run())
		sleep(1000);
	printf("��ϰ����������%dö����������%dö��", capture, escape);
	return 0;
}