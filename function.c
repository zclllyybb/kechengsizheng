#include <assert.h>
#include <stdbool.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "function.h"

typedef struct Missile
{
	double x, y;
	double speed, angle;
	bool alive;
} Missile;
typedef struct Gun
{
	int x, y;
	double speed; //	�����ٶ�
	int track_time; //	����Ŀ������ʱ��
	int aim, wait; //	Ŀ����������ǰĿ��ʣ������ʱ��
	int n_track;
	int tracking[MAXN]; //	0-based
} Gun;
typedef struct PrintPos
{
	int x, y;
} PrintPos;
typedef struct Pos
{
	double x, y;
} Pos;

wchar_t screen[MAXN][MAXN];
int width, height;
int n_missiles, n_guns, n_antis;
Missile missiles[MAXN], antis[MAXN];
Gun guns[MAXN];
extern int capture = 0, escape = 0;
int current_missiles;

CHANGE_IMAGE void init_screen()
{
	for (int i = 1; i <= height; i++)
		for (int j = 1; j <= width; j++)
			screen[i][j] = IMG_EMPTY;
	for (int i = 1; i <= n_guns; i++)
		screen[guns[i].x][guns[i].y] = IMG_GUN;
}

void init_read()
{
	setbuf(stdout, NULL);
	width = height = n_missiles = n_guns = -1;
	while (width < 0 || width > 1000)
	{
		printf("%s", "��������򳤶�(�Ƽ�300�����1000):");
		scanf("%d", &width);
	}
	while (height < 0 || height > 1000)
	{
		printf("%s", "������������(�Ƽ�100�����1000):");
		scanf("%d", &height);
	}
	while (n_missiles < 0 || n_missiles > 10)
	{
		printf("%s", "�����뵼������(���10):");
		scanf("%d", &n_missiles);
		current_missiles = n_missiles;
	}
	for (int i = 1; i <= n_missiles; i++)
	{
		double x = -1, y = -1, speed = -1, angle = -1;
		while (x < 0 || x > width || y < 0 || y > height || speed < 0 || speed > 10
			   || angle < -M_PI || angle > M_PI)
		{
			printf("%s%d%s", "�������", i, "��������ʼλ�úᡢ�����ꡢ�ٶȡ���λ��(������, -pi ~ pi):\n");
			scanf("%lf%lf%lf%lf", &x, &y, &speed, &angle);
		}
		Missile tmp = {x, y, speed, angle, true};
		missiles[i] = tmp;
	}
	while (n_guns < 0 || n_guns > 10)
	{
		printf("%s", "���������������(���5):");
		scanf("%d", &n_guns);
	}
	for (int i = 1; i <= n_guns; i++)
	{
		int x = -1, y = -1, track_time = -1;
		double speed = -1;
		while (x < 0 || x > width || y < 0 || y > height || speed < 0 || speed > 100
			   || track_time < 0 || track_time > 10)
		{
			printf("%s%d%s", "�������", i, "�ŷ�����λ�ú��������뵯ҩ�ٶ�(������100)��"
										"׷�ٵ�������ʱ��:\n");
			scanf("%d%d%lf%d", &x, &y, &speed, &track_time);
		}
		Gun tmp = {x, y, speed, track_time, -1, -1, 0};
		guns[i] = tmp;
	}
	puts("������ϡ�������ʼģ�⡣");
	sleep(2000);
}

/**
 * ��ԭս����Ϣ����Ϊ�����غϵ�ԭ��change_xxx�ı�����ܻ�����ʾ�ϳ����������Ҫ����ͳһ������
 * ��ʾ���ȼ�Ϊ������> ��������> �����ڡ�
 */
CHANGE_IMAGE void recovery()
{
	for (int i = 1; i <= n_guns; i++)
	{
		if (screen[guns[i].x][guns[i].y] == RECOVERY)
			screen[guns[i].x][guns[i].y] = IMG_GUN;
	}
	for (int i = 1; i <= n_antis; i++)
		if (antis[i].alive)
		{
			PrintPos pos = get_show_pos(antis[i].x, antis[i].y);
			if (screen[pos.x][pos.y] == RECOVERY)
				screen[pos.x][pos.y] = IMG_ANTI;
		}
	for (int i = 1; i <= n_missiles; i++)
		if (missiles[i].alive)
		{
			PrintPos pos = get_show_pos(missiles[i].x, missiles[i].y);
			if (screen[pos.x][pos.y] == RECOVERY)
				screen[pos.x][pos.y] = IMG_MISSILE;
		}
	for (int i = 1; i <= height; i++)
		for (int j = 1; j <= width; j++)
			if (screen[i][j] == RECOVERY)
				screen[i][j] = IMG_EMPTY;
}

/**
 * ��ӡս����Ϣ
 */
CHANGE_IMAGE void display()
{
	recovery(); // ���Ƚ��Ѿ��ƶ���������ԭλ��ԭΪ��ȷͼ��
	system("cls");
	for (int i = 1; i <= height; i++)
	{
		for (int j = 1; j <= width; j++)
		{
			putchar(screen[i][j]);
			if (screen[i][j] == IMG_BOOM)
				screen[i][j] = RECOVERY;
		}
		putchar('\n');
	}
}

void print_log(char log[MAXN][MAXN])
{
	for (int i = 0; i < MAXN && log[i][0] != '\0'; i++)
		puts(log[i]);
}

/**
 * ��ʵ������������ڽ�����ͼ�ν�����չʾ��λ�á�
 */
PrintPos get_show_pos(double x, double y)
{
	PrintPos rtn = {x, y};
	if (x - (int) x >= 0.5)
		rtn.x++;
	if (y - (int) y >= 0.5)
		rtn.y++;
	return rtn;
}

/**
 * ������������Ϊģ�⵼���ͷ�����������ʱ���˶����������ꡣ
 */
CHANGE_IMAGE bool change_missile(int index)
{
	PrintPos now = get_show_pos(missiles[index].x, missiles[index].y);
	screen[now.x][now.y] = RECOVERY; //��������Ķ���Ӧ��ͳһ��������Ϊԭλ����Ȼ���ܴ������塣
	
	missiles[index].x += missiles[index].speed * cos(missiles[index].angle);
	missiles[index].y += missiles[index].speed * sin(missiles[index].angle);
	
	if (missiles[index].x < 0 || missiles[index].x > width || missiles[index].y < 0 || missiles[index].y > height)
	{
		missiles[index].alive = false;
		return true; //�����������
	}
	
	now = get_show_pos(missiles[index].x, missiles[index].y);
	screen[now.x][now.y] = IMG_MISSILE;
	return false;
}

CHANGE_IMAGE int change_anti(int index)
{
	PrintPos now = get_show_pos(antis[index].x, antis[index].y);
	screen[now.x][now.y] = RECOVERY;
	
	antis[index].x += antis[index].speed * cos(antis[index].angle);
	antis[index].y += antis[index].speed * sin(antis[index].angle);
	
	now = get_show_pos(antis[index].x, antis[index].y);
	
	//�����������⸽���ĵз�������������������ջ���Ŀ�겻һ����һ��ʼ��׼��Ŀ�굼����
	for (int i = 1; i <= n_missiles; i++)
	{
		Pos pos_anti = {antis[index].x, antis[index].y};
		Pos pos_aim = {missiles[i].x, missiles[i].y};
		if (distance(pos_anti, pos_aim) < eps)
		{
			antis[index].alive = missiles[i].alive = false;
			screen[now.x][now.y] = IMG_BOOM;
			return i;
		}
	}
	
	screen[now.x][now.y] = IMG_ANTI;
	return 0;
}

CHANGE_IMAGE void add_anti(Missile anti)
{
	antis[++n_antis] = anti;
	PrintPos now = get_show_pos(anti.x, anti.y);
	screen[now.x][now.y] = IMG_ANTI;
}

double distance(Pos lhs, Pos rhs)
{
	return sqrt(pow(lhs.x - rhs.x, 2) + pow(lhs.y - rhs.y, 2));
}

/**
 * @return missile�Ƿ��Ѿ���gun�����
 */
bool tracked(int gun, int missile)
{
	for (int i = 0; i < guns[gun].n_track; i++)
		if (missile == guns[gun].tracking[i])
			return true;
	return false;
}

/**
 * Ϊ������ѡȡ����Ŀ��
 */
int choose_aim(int index)
{
	double d = INT_MAX;
	int number = -1;
	Pos gun_pos = {guns[index].x, guns[index].y};
	for (int i = 1; i <= n_missiles; i++)
	{
		if (!missiles[i].alive || tracked(index, i))
			continue;
		Pos mis_pos = {missiles[i].x, missiles[i].y};
		double now_d = distance(gun_pos, mis_pos);
		if (missiles[i].alive && now_d < d)
		{
			number = i;
			d = now_d;
		}
	}
	assert(number != -1); //��֤��ȷѡ��Ŀ��
	return number;
}

/**
 * @return ���ط��������ص�ǰ����Ŀ������ķ���Ƕȡ�
 */
double calc_meet(int index)
{
	int aim = guns[index].aim;
	//ʹ��long double����ϵͳ��
	long double va = missiles[aim].speed,
			vax = missiles[aim].speed * cos(missiles[aim].angle),
			vay = missiles[aim].speed * sin(missiles[aim].angle),
			vb = guns[index].speed,
			X = fabs(missiles[aim].x - guns[index].x),
			Y = fabs(missiles[aim].y - guns[index].y);
	// �����ǽⷽ�̹��̣�
	long double l1 = powl(va, 2) - powl(vb, 2),
			l2 = X * vax + Y * vay;
	long double A = l1, B = -2 * l2, C = -(powl(X, 2) + powl(Y, 2));
	assert(A > 0);
	long double t = (-B + sqrtl(powl(B, 2) - 4 * A * C)) / (2 * A);
	assert(t > 0);
	long double vbx = (vax * t - X) / t, vby = (vay * t - Y) / t;
	return (double) atanl(vbx / vby);
}


/**
 * @return True if NOT done.
 */
bool run()
{
	static int time = 0;
	++time;
	// ��ձ�����־
	char log[MAXN][MAXN]; //��־
	int log_count = 0;
	for (int i = 0; i < MAXN && log[i][0] != '\0'; i++)
		log[i][0] = '\0';
	
	printf("�ѽ��е�ʱ��: %d\n", time);
	
	//���������ζ���
	for (int i = 1; i <= n_guns; i++)
		if (guns[i].aim == -1) // ��ǰ����
		{
			int aim = choose_aim(i);// ѡ�������δ��������Ŀ��
			//��ʼ׷�٣��ȴ�׷�����
			guns[i].aim = aim;
			guns[i].wait = guns[i].track_time;
			//��¼��־
			char tmp_log[MAXN];
			sprintf(tmp_log, "������%d��������%d�ŵ���, Ŀ��λ��(%.3lf, %.3lf)",
					i, aim, missiles[aim].x, missiles[aim].y);
			strcpy(log[++log_count], tmp_log);
		}
		else if (--guns[i].wait == 0) // ����׷��Ŀ�굼����ֱ����ȷ�������ܹ����������λ�á�
		{
			double meet_angle = calc_meet(i); //���������λ��
			// ���䵼��
			Missile tmp = {guns[i].x, guns[i].y, guns[i].speed, meet_angle, true};
			add_anti(tmp);
			// ��¼���أ����������״̬
			guns[i].tracking[guns[i].n_track++] = guns[i].aim;
			guns[i].aim = guns[i].wait = -1;
			// ��¼��־
			char tmp_log[MAXN];
			sprintf(tmp_log, "������%d�ѷ��䷴������������Ƕ�Ϊ%.5lf", i, meet_angle);
			strcpy(log[++log_count], tmp_log);
		}
	// �����˶�
	for (int i = 1; i <= n_missiles; i++)
		if (change_missile(i)) //�ɹ�����
			escape++, current_missiles--;
	// ���յ����˶�
	for (int i = 1; i <= n_antis; i++)
	{
		int result = change_anti(i);
		if (result) // ��������״̬����change�����������¼��run����������Ҳ��һ����Ҫ���ھ�-����߼���
		{
			capture++, current_missiles--;
			char tmp_log[MAXN];
			sprintf(tmp_log, "�з�����%d�ѱ�����!", result);
			strcpy(log[++log_count], tmp_log);
		}
	}
	/**
	 * @attention ����ͼ��֡�Ļ��ƶ��ǻ�����һ֡�ı仯�����ֱ仯Ӧ���޶����ض��ļ�����������������run()��ҵ���߼��д�����
	 * ��������ͨ��CHANGE_IMAGE��ȥ�������Щ����������ҵ��-չʾ������߼��Ǳ���ġ�
	 */
	
	// ˢ�½��沢�����ǰlog��
	display();
	print_log(log);
	return current_missiles != 0;//�����е��������أ�����true��
}