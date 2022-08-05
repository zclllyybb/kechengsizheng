#include "function.h"
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

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
int current_missiles, time;

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
		wprintf(L"%s", "��������򳤶�(�Ƽ�300�����1000):"); // ����ʹ�������ģ�����Ӧ��ʹ�ÿ��ַ�����
		scanf("%d", &width);
	}
	while (height < 0 || height > 1000)
	{
		wprintf(L"%s", "�����������(�Ƽ�100�����1000):");
		scanf("%d", &height);
	}
	while (n_missiles < 0 || n_missiles > 10)
	{
		wprintf(L"%s", "�����뵼������(���10):");
		scanf("%d", &n_missiles);
		current_missiles = n_missiles;
	}
	for (int i = 1; i <= n_missiles; i++)
	{
		double x = -1, y = -1, speed = -1, angle = -1;
		while (x < 0 || x > width || y < 0 || y > height || speed < 0 || speed > 10
																		 || angle < -M_PI || angle > M_PI)
		{
			wprintf(L"%s%d%s", "�������", i, "��������ʼλ�úᡢ�����꣬�ٶȣ���λ��(������, -pi ~ pi):\n");
			scanf("%lf%lf%lf%lf", &x, &y, &speed, &angle);
		}
		Missile tmp = {x, y, speed, angle, true};
		missiles[i] = tmp;
	}
	while (n_guns < 0 || n_guns > 10)
	{
		wprintf(L"%s", "���������������(���5):");
		scanf("%d", &n_guns);
	}
	for (int i = 1; i <= n_guns; i++)
	{
		int x = -1, y = -1, track_time = -1;
		double speed = -1;
		while (x < 0 || x > width || y < 0 || y > height || speed < 0 || speed > 100
																		 || track_time < 0 || track_time > 10)
		{
			wprintf(L"%s%d%s", "�������", i, "�ŷ�����λ�ú��������뵯ҩ�ٶ�(������100)��"
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
 * ��ԭս����Ϣ����Ϊ�����غϵ�ԭ��change_xxx�ı�����ܻ�����ʾ�ϳ��������Ҫ����ͳһ������
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
	printf("�ѽ��е�ʱ��: %d\n", time);
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

void print_log(wchar_t (*log)[MAXN])
{
	for (int i = 0; i < MAXN && wcslen(log[i]) > 0; i++)
		wprintf(L"%ls", log[i]);
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
	screen[now.x][now.y] = RECOVERY; //��������Ķ���Ӧ��ͳһ������Ϊԭλ����Ȼ���ܴ������塣
	
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
 * �Ȱ����궼�仯Ϊ�����ٶ�����ϵ�ϡ�Ȼ��ⷽ�̼��ɡ�
 * AΪ������BΪ������
 */
double calc_meet(int index)
{
	double pi = acos(-1);
	int aim = guns[index].aim;
	double theta1 = atan2(missiles[aim].y, missiles[aim].x); // A��ԭ�����¼���, no use
	assert(theta1 > 0 && theta1 < pi / 2);
	double dBx = guns[index].x - missiles[aim].x, dBy = guns[index].y - missiles[aim].y; // ���λ��
	double theta2 = missiles[aim].angle - atan2(dBy, dBx), // B �� A�ٶȷ��� ��ͶӰ�Ƕ�
	theta3 = atan2(dBy, dBx); // ԭ����ϵ��AB֮��ĽǶ�
	double dB = sqrt(pow(dBx, 2) + pow(dBy, 2)); //AB������
	double nx = dB * cos(theta2), ny = dB * sin(theta2); // ͶӰ������
	//����η���
	double vA = missiles[index].speed, vB = guns[index].speed; // �����ٶ�
	double w2 = pow(vA, 2) - pow(vB, 2),
			w1 = -2 * vA * nx, w0 = pow(nx, 2) + pow(ny, 2); // ���η���ϵ��
	double t = (sqrt((pow(w1, 2) - 4 * w0 * w2)) - w1) / (2 * w2); // �����ʽ
#ifdef DEBUG
	printf("%d\n", t);
#endif
	assert(t > 0);
	double thetaB = asin(ny / (vB * t)); // B ����� A�ٶȷ��� �ķ���Ƕ�
	double ans = pi / 2 - thetaB + theta2 + theta3;
	assert(ans > -pi && ans < pi);
	return ans;
}

/**
 * @return True if NOT done.
 */
bool run()
{
	time = 0;
	++time;
	// ��ձ�����־
	static wchar_t log[MAXN][MAXN]; //��־�����Ŀ��ַ���
	int log_count = 0;
	for (int i = 0; i < MAXN && wcslen(log[i]) > 0; i++)
		swprintf(log[i], 10000, L"");
	
	//���������ζ���
	for (int i = 1; i <= n_guns; i++)
		if (guns[i].aim == -1) // ��ǰ����
		{
			int aim = choose_aim(i);// ѡ�������δ��������Ŀ��
			//��ʼ׷�٣��ȴ�׷�����
			guns[i].aim = aim;
			guns[i].wait = guns[i].track_time;
			//��¼��־
			wchar_t tmp_log[MAXN];
			swprintf(tmp_log, 10000, L"%s%d%s%d%s(%.3lf, %.3lf)",
					 "������", i, "��������", aim, "�ŵ���, Ŀ��λ��",
					 missiles[aim].x, missiles[aim].y);
			wcscpy(log[++log_count], tmp_log);
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
			wchar_t tmp_log[MAXN];
			swprintf(tmp_log, 10000, L"%s%d%s%.5lf",
					 "������", i, "�ѷ��䷴������������Ƕ�Ϊ", meet_angle);
			wcscpy(log[++log_count], tmp_log);
		}
	// �����˶�
	for (int i = 1; i <= n_missiles; i++)
		if (change_missile(i)) //�ɹ�����
			escape++, current_missiles--;
	// ���յ����˶�
	for (int i = 1; i <= n_antis; i++)
	{
		int result = change_anti(i);
		if (result) // ��������״̬����change�����������¼��run��������Ҳ��һ����Ҫ���ھ�-����߼���
		{
			capture++, current_missiles--;
			wchar_t tmp_log[MAXN];
			swprintf(tmp_log, 10000, L"%s%d%s!",
					 "�з�����", result, "�ѱ�����");
			wcscpy(log[++log_count], tmp_log);
		}
	}
	/**
	 * @attention ����ͼ��֡�Ļ��ƶ��ǻ�����һ֡�ı仯�����ֱ仯Ӧ���޶����ض��ļ�����������������run()��ҵ���߼��д���
	 * ��������ͨ��CHANGE_IMAGE��ȥ�������Щ����������ҵ��-չʾ������߼��Ǳ���ġ�
	 */
	// ˢ�½��沢�����ǰlog��
	display();
	print_log(log);
	return current_missiles != 0;//�����е��������أ�����true��
}