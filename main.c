/**
 * @endcode GBK
 */
#include <stdio.h>

#ifdef _WIN32  //两个平台下休眠函数不同，需要编译时判断。
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#endif

#include "function.h"

int main()
{
	setbuf(stdout, NULL);
	init_read(); // 初始化读入
	init_screen();//初始化屏幕
	while (run())
		sleep(1000);
	printf("演习结束，拦截%d枚导弹，逃逸%d枚。", capture, escape);
	return 0;
}