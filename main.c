/**
 * @endcode GBK
 */
#include "function.h"
#include <stdio.h>

int main()
{
	setbuf(stdout, NULL); //关闭输出缓冲，强制实时交互。
#ifdef DEBUG
	puts("DEBUGGING!");
#endif
	init_read(); // 初始化读入
	init_screen();//初始化屏幕
	while (run())
		sleep(1000);
	printf("演习结束，拦截%d枚导弹，逃逸%d枚。", capture, escape);
	return 0;
}