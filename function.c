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
	double speed; //	导弹速度
	int track_time; //	锁定目标所需时间
	int aim, wait; //	目标与锁定当前目标剩余所需时间
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
		wprintf(L"%s", "请输入空域长度(推荐300，最大1000):"); // 由于使用了中文，所以应当使用宽字符处理。
		scanf("%d", &width);
	}
	while (height < 0 || height > 1000)
	{
		wprintf(L"%s", "请输入空域宽度(推荐100，最大1000):");
		scanf("%d", &height);
	}
	while (n_missiles < 0 || n_missiles > 10)
	{
		wprintf(L"%s", "请输入导弹数量(最大10):");
		scanf("%d", &n_missiles);
		current_missiles = n_missiles;
	}
	for (int i = 1; i <= n_missiles; i++)
	{
		double x = -1, y = -1, speed = -1, angle = -1;
		while (x < 0 || x > width || y < 0 || y > height || speed < 0 || speed > 10
																		 || angle < -M_PI || angle > M_PI)
		{
			wprintf(L"%s%d%s", "请输入第", i, "发导弹初始位置横、纵坐标，速度，方位角(弧度制, -pi ~ pi):\n");
			scanf("%lf%lf%lf%lf", &x, &y, &speed, &angle);
		}
		Missile tmp = {x, y, speed, angle, true};
		missiles[i] = tmp;
	}
	while (n_guns < 0 || n_guns > 10)
	{
		wprintf(L"%s", "请输入防空炮数量(最大5):");
		scanf("%d", &n_guns);
	}
	for (int i = 1; i <= n_guns; i++)
	{
		int x = -1, y = -1, track_time = -1;
		double speed = -1;
		while (x < 0 || x > width || y < 0 || y > height || speed < 0 || speed > 100
																		 || track_time < 0 || track_time > 10)
		{
			wprintf(L"%s%d%s", "请输入第", i, "门防空炮位置横纵坐标与弹药速度(不超过100)、"
											  "追踪导弹所需时间:\n");
			scanf("%d%d%lf%d", &x, &y, &speed, &track_time);
		}
		Gun tmp = {x, y, speed, track_time, -1, -1, 0};
		guns[i] = tmp;
	}
	puts("配置完毕。即将开始模拟。");
	sleep(2000);
}

/**
 * 还原战场信息。因为坐标重合等原因change_xxx的变更可能会在显示上出错，因此需要进行统一修正。
 * 显示优先级为：导弹> 反导导弹> 防空炮。
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
 * 打印战场信息
 */
CHANGE_IMAGE void display()
{
	recovery(); // 首先将已经移动过的物体原位还原为正确图像。
	system("cls");
	printf("已进行的时间: %d\n", time);
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
 * 由实际坐标计算最邻近的在图形界面上展示的位置。
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
 * 以下两个函数为模拟导弹和反导导弹随着时间运动，计算坐标。
 */
CHANGE_IMAGE bool change_missile(int index)
{
	PrintPos now = get_show_pos(missiles[index].x, missiles[index].y);
	screen[now.x][now.y] = RECOVERY; //所有清除的动作应当统一处理，因为原位置仍然可能存在物体。
	
	missiles[index].x += missiles[index].speed * cos(missiles[index].angle);
	missiles[index].y += missiles[index].speed * sin(missiles[index].angle);
	
	if (missiles[index].x < 0 || missiles[index].x > width || missiles[index].y < 0 || missiles[index].y > height)
	{
		missiles[index].alive = false;
		return true; //逃离防空区域
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
	
	//反导导弹会检测附近的敌方导弹并引爆，因此最终击毁目标不一定是一开始瞄准的目标导弹。
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
 * @return missile是否已经被gun打击过
 */
bool tracked(int gun, int missile)
{
	for (int i = 0; i < guns[gun].n_track; i++)
		if (missile == guns[gun].tracking[i])
			return true;
	return false;
}

/**
 * 为防空炮选取攻击目标
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
	assert(number != -1); //保证正确选择目标
	return number;
}

/**
 * @return 返回防空炮拦截当前锁定目标所需的发射角度。
 * 先把坐标都变化为导弹速度坐标系上。然后解方程即可。
 * A为导弹，B为炮塔。
 */
double calc_meet(int index)
{
	double pi = acos(-1);
	int aim = guns[index].aim;
	double theta1 = atan2(missiles[aim].y, missiles[aim].x); // A在原坐标下极角, no use
	assert(theta1 > 0 && theta1 < pi / 2);
	double dBx = guns[index].x - missiles[aim].x, dBy = guns[index].y - missiles[aim].y; // 相差位置
	double theta2 = missiles[aim].angle - atan2(dBy, dBx), // B 到 A速度方向 的投影角度
	theta3 = atan2(dBy, dBx); // 原坐标系下AB之间的角度
	double dB = sqrt(pow(dBx, 2) + pow(dBy, 2)); //AB相差距离
	double nx = dB * cos(theta2), ny = dB * sin(theta2); // 投影后坐标
	//解二次方程
	double vA = missiles[index].speed, vB = guns[index].speed; // 两者速度
	double w2 = pow(vA, 2) - pow(vB, 2),
			w1 = -2 * vA * nx, w0 = pow(nx, 2) + pow(ny, 2); // 二次方程系数
	double t = (sqrt((pow(w1, 2) - 4 * w0 * w2)) - w1) / (2 * w2); // 求根公式
#ifdef DEBUG
	printf("%d\n", t);
#endif
	assert(t > 0);
	double thetaB = asin(ny / (vB * t)); // B 相对于 A速度方向 的发射角度
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
	// 清空本期日志
	static wchar_t log[MAXN][MAXN]; //日志，中文宽字符。
	int log_count = 0;
	for (int i = 0; i < MAXN && wcslen(log[i]) > 0; i++)
		swprintf(log[i], 10000, L"");
	
	//防空炮依次动作
	for (int i = 1; i <= n_guns; i++)
		if (guns[i].aim == -1) // 当前空闲
		{
			int aim = choose_aim(i);// 选择最近的未攻击过的目标
			//开始追踪，等待追踪完成
			guns[i].aim = aim;
			guns[i].wait = guns[i].track_time;
			//记录日志
			wchar_t tmp_log[MAXN];
			swprintf(tmp_log, 10000, L"%s%d%s%d%s(%.3lf, %.3lf)",
					 "防空炮", i, "正在锁定", aim, "号导弹, 目标位于",
					 missiles[aim].x, missiles[aim].y);
			wcscpy(log[++log_count], tmp_log);
		}
		else if (--guns[i].wait == 0) // 继续追踪目标导弹，直到正确锁定，能够计算出拦截位置。
		{
			double meet_angle = calc_meet(i); //计算出拦截位置
			// 发射导弹
			Missile tmp = {guns[i].x, guns[i].y, guns[i].speed, meet_angle, true};
			add_anti(tmp);
			// 记录拦截，并进入空闲状态
			guns[i].tracking[guns[i].n_track++] = guns[i].aim;
			guns[i].aim = guns[i].wait = -1;
			// 记录日志
			wchar_t tmp_log[MAXN];
			swprintf(tmp_log, 10000, L"%s%d%s%.5lf",
					 "防空炮", i, "已发射反导导弹！发射角度为", meet_angle);
			wcscpy(log[++log_count], tmp_log);
		}
	// 导弹运动
	for (int i = 1; i <= n_missiles; i++)
		if (change_missile(i)) //成功逃逸
			escape++, current_missiles--;
	// 防空导弹运动
	for (int i = 1; i <= n_antis; i++)
	{
		int result = change_anti(i);
		if (result) // 导弹具体状态交给change函数，其余记录由run来处理，这也是一种重要的内聚-耦合逻辑。
		{
			capture++, current_missiles--;
			wchar_t tmp_log[MAXN];
			swprintf(tmp_log, 10000, L"%s%d%s!",
					 "敌方导弹", result, "已被拦截");
			wcscpy(log[++log_count], tmp_log);
		}
	}
	/**
	 * @attention 所有图形帧的绘制都是基于上一帧的变化，这种变化应当限定于特定的几个函数，而不能在run()的业务逻辑中处理。
	 * 这里我们通过CHANGE_IMAGE宏去标记了这些函数。这种业务-展示分离的逻辑是必须的。
	 */
	// 刷新界面并输出当前log。
	display();
	print_log(log);
	return current_missiles != 0;//若尚有导弹需拦截，返回true。
}