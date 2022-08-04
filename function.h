#include <stdbool.h>

#define MAXN 2000
#define eps 1e-5

#define CHANGE_IMAGE  //����ʹ�ö����ĺ�ȥ���б�����е��޸�ǰ�˵Ĺ���
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

typedef struct PrintPos PrintPos; //ǰ������

PrintPos get_show_pos(double, double);

typedef struct Pos Pos;

double distance(Pos, Pos);

bool run();

extern int capture, escape; // ���ļ�ʹ����ͬ�Ķ���Ӧ������extern������