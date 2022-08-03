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
	init_read(); // 初始化读入
	init_screen();//初始化屏幕
	printf("屏幕初始化完成");
	while (!run())
		sleep(1000);
	printf("演习结束，拦截%d枚导弹，逃逸%d枚。", capture, escape);
	return 0;
}