#include <stdbool.h>

#define DEBUG //测试开关

#ifdef DEBUG

#include <string.h>

#define sleep(x) 0
#elif defined _WIN32  //windows与unix平台下休眠函数不同，需要编译时判断。
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#endif

/* 常量管理 */
#define MAXN 2000
#define eps 1e-5

/* 我们使用独立的宏去集中标记所有的修改前端的功能 */
#define CHANGE_IMAGE
#define RECOVERY 255
#define IMG_EMPTY L'-'
#define IMG_MISSILE L'A'
#define IMG_ANTI L'B'
#define IMG_BOOM L'*'
#define IMG_GUN L'G'

#ifdef DEBUG  // 测试用函数

void print_log(wchar_t (*log)[MAXN]);

#endif

void init_read();

void init_screen();

typedef struct PrintPos PrintPos; //前向声明

PrintPos get_show_pos(double, double);

typedef struct Pos Pos;

double distance(Pos, Pos);

bool run();

extern int capture, escape; // 跨文件使用相同的对象，应当给出extern声明。