#include <stdbool.h>

#define DEBUG //���Կ���

#ifdef DEBUG

#include <string.h>

#define sleep(x) 0
#elif defined _WIN32  //windows��unixƽ̨�����ߺ�����ͬ����Ҫ����ʱ�жϡ�
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#endif

/* �������� */
#define MAXN 2000
#define eps 1e-5

/* ����ʹ�ö����ĺ�ȥ���б�����е��޸�ǰ�˵Ĺ��� */
#define CHANGE_IMAGE
#define RECOVERY 255
#define IMG_EMPTY L'-'
#define IMG_MISSILE L'A'
#define IMG_ANTI L'B'
#define IMG_BOOM L'*'
#define IMG_GUN L'G'

#ifdef DEBUG  // �����ú���

void print_log(wchar_t (*log)[MAXN]);

#endif

void init_read();

void init_screen();

typedef struct PrintPos PrintPos; //ǰ������

PrintPos get_show_pos(double, double);

typedef struct Pos Pos;

double distance(Pos, Pos);

bool run();

extern int capture, escape; // ���ļ�ʹ����ͬ�Ķ���Ӧ������extern������