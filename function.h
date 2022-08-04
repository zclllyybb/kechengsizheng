#include <stdbool.h>

#define MAXN 2000
#define eps 1e-5

#define CHANGE_IMAGE  //我们使用独立的宏去集中标记所有的修改前端的功能
#define RECOVERY 255
#define IMG_EMPTY L'-'
#define IMG_MISSILE L'A'
#define IMG_ANTI L'B'
#define IMG_BOOM L'*'
#define IMG_GUN L'G'

#ifdef DEBUG
void print_log(char (*log)[MAXN]);
#endif

void init_read();

void init_screen();

typedef struct PrintPos PrintPos; //前向声明

PrintPos get_show_pos(double, double);

typedef struct Pos Pos;

double distance(Pos, Pos);

bool run();

extern int capture, escape; // 跨文件使用相同的对象，应当给出extern声明。