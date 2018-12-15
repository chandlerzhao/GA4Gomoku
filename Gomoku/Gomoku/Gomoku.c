#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MAX_COMMAND_LENGTH 14	    // 命令最长字符数 
#define DIMENSION 20		// 棋盘大小

typedef struct				     	// 棋子坐标 
{
	int ln;							// 第ln行 
	int col;				    	// 第col列 
}Coordinate;

typedef struct {
	double empty;
	double s;		// Self
	double ss;
	double sss;
	double ssss;
	double e;		// Enemy
	double ee;
	double eee;
	double eeee;
	double polluted;
	double fitness;
	int number;
	double pr;		// 被选中的概率
} Parameter;

const char PRIMITIVE[6][MAX_COMMAND_LENGTH] = { "START",
"PLACE",
"DONE",
"TURN",
"BEGIN",
"END" }; // 命令字符串数组 

int parseCommand(char *command, char **rest);
Coordinate solve(short cboard[DIMENSION][DIMENSION], Parameter p);
double calcWeight(int self, int enemy, int empty, Parameter p);

int main(void)
{
//	Parameter p = { 3.81, 5.86, 17.02, 9208.08, 100790.85, 16.33, 124.90, 15000.00, 700000.00, 0.00 };
//	Parameter p = { 2.12, 1.52, 182.02, 1114.14, 249649.77, 25.30, 64.75, 4253.14, 302959.71, 0.00 };
//	Parameter p = { 0.79, 21.23, 135.04, 1202.14, 1064426.38, 7.32, 349.34, 8038.92, 4440197.98, 0.1};
//	Parameter p = { 10.28, 3.90, 141.14, 17538.15, 895110.93, 19.57, 266.97, 1945.81, 1082573.28, 0.00 };
//	Parameter p = {3.74, 8.22, 29.38, 7143.41, 660021.42, 31.41, 171.41, 410.07, 885549.64, 0.01};
//	Parameter p = { 4.68, 7.46, 27.46, 13967.01, 705427.13, 58.02, 132.56, 331.58, 1205522.69, 0.01 };
	Parameter p = { 3.37, 5.59, 19.30, 10115.39, 1072151.94, 49.79, 125.90, 273.74, 1404969.64, 0.01 };
	short cboard[DIMENSION][DIMENSION];   // 棋盘矩阵
	memset(cboard, 0, sizeof cboard);
	int ln, col;					  // 棋盘坐标
	int side;						  // 1表示己方，2表示对方
	char flag;						  // 棋盘某处的状态，0表示无子，1表示己方，-1表示对方。 
	int pNum;						  // 输入命令对应的序号 
	char command[MAX_COMMAND_LENGTH], // 用于存放读入的每行命令 
		*rest = NULL;			  	  // 读入每行命令提取命令后的剩余 
	Coordinate solution = { 0, 0 };		  // 我们下子的坐标

	while (true)		  // 读入命令 
	{
		memset(command, 0, sizeof(command));
		gets(command);
		pNum = parseCommand(command, &rest);
		switch (pNum)
		{
		case 0:					  // START
			break;
		case 1:					  // PLACE
			sscanf(rest, "%d%d%d", &ln, &col, &side);
			switch (side)
			{
			case 1:
				cboard[ln][col] = 1;
				break;
			case 2:
				cboard[ln][col] = -1;
				break;
			}
			break;
		case 2:
			printf("OK\n");	      // DONE
			fflush(stdout);
			break;
		case 3:					  // TURN
			sscanf(rest, "%d%d", &ln, &col);
			flag = -1;			  // 设置为置对方子 
			cboard[ln][col] = flag;
			solution = solve(cboard, p);
			cboard[solution.ln][solution.col] = 1;			// 在棋盘上记下下子位置 
			printf("%d %d\n", solution.ln, solution.col);
			fflush(stdout);
			break;
		case 4:					  // BEGIN
			solution = solve(cboard, p);
			cboard[solution.ln][solution.col] = 1;			// 在棋盘上记下下子位置 
			printf("%d %d\n", solution.ln, solution.col);
			fflush(stdout);
			break;
		case 5:					  // END
			break;
		default:
			return -1;
		}
	}
	return 0;
}



/*
* 作者：赵晨
* 输入: command 来自控制台的每一行命令
*       rest    指向后续参数的第一个字符的指针，若无参数，置为NULL
* 输出：命令号码
*/

int parseCommand(char* command, char** rest)
{
	for (unsigned int i = 0; i < 6; i++)
	{
		if (strstr(command, PRIMITIVE[i]) == command)		// 检查每个命令
		{
			if ((*rest = command + strlen(PRIMITIVE[i]) + 1) > command + strlen(command)) //看命令后面是否还有东西
				*rest = NULL;
			return i;
		}
	}
	return -1;
}


Coordinate solve(short cboard[DIMENSION][DIMENSION], Parameter p)
{
	double weight[DIMENSION][DIMENSION] = { 0 };	// 权值表格 
													//memset(weight, 0, DIMENSION*DIMENSION);
													// 枚举每个向右的五元组 
	for (int ln = 0; ln < DIMENSION; ln++)
		for (int col = 0; col < DIMENSION - 4; col++)
		{
			int self = 0, enemy = 0, empty = 0;
			for (int i = 0; i < 5; i++)
				switch (cboard[ln][col + i])
				{
				case 0:
					empty++;
					break;
				case 1:
					self++;
					break;
				case -1:
					enemy++;
					break;
				}
			double theWeight = calcWeight(self, enemy, empty, p);
			for (int i = 0; i < 5; i++)
				if (cboard[ln][col + i] == 0)
					weight[ln][col + i] += theWeight;

		}

	// 枚举每个向上的五元组 
	for (int ln = 0; ln < DIMENSION - 4; ln++)
		for (int col = 0; col < DIMENSION; col++)
		{
			int self = 0, enemy = 0, empty = 0;
			for (int i = 0; i < 5; i++)
				switch (cboard[ln + i][col])
				{
				case 0:
					empty++;
					break;
				case 1:
					self++;
					break;
				case -1:
					enemy++;
					break;
				}
			double theWeight = calcWeight(self, enemy, empty, p);
			for (int i = 0; i < 5; i++)
				if (cboard[ln + i][col] == 0)
					weight[ln + i][col] += theWeight;

		}

	// 枚举每个副对角线方向的五元组
	for (int ln = 0; ln < DIMENSION - 4; ln++)
		for (int col = 0; col < DIMENSION - 4; col++)
		{
			int self = 0, enemy = 0, empty = 0;
			for (int i = 0; i < 5; i++)
				switch (cboard[ln + i][col + i])
				{
				case 0:
					empty++;
					break;
				case 1:
					self++;
					break;
				case -1:
					enemy++;
					break;
				}
			double theWeight = calcWeight(self, enemy, empty, p);
			for (int i = 0; i < 5; i++)
				if (cboard[ln + i][col + i] == 0)
					weight[ln + i][col + i] += theWeight;

		}

	// 枚举每个主对角线方向的五元组 
	for (int ln = 4; ln < DIMENSION; ln++)
		for (int col = 0; col < DIMENSION - 4; col++)
		{
			int self = 0, enemy = 0, empty = 0;
			for (int i = 0; i < 5; i++)
				switch (cboard[ln - i][col + i])
				{
				case 0:
					empty++;
					break;
				case 1:
					self++;
					break;
				case -1:
					enemy++;
					break;
				}
			double theWeight = calcWeight(self, enemy, empty, p);
			for (int i = 0; i < 5; i++)
				if (cboard[ln - i][col + i] == 0)
					weight[ln - i][col + i] += theWeight;

		}

	// 找到权值最大的位置的坐标	
	Coordinate result = { 0, 0 };
	double maxWeight = 0;
	for (int ln = 0; ln < DIMENSION; ln++)
		for (int col = 0; col < DIMENSION; col++)
			if (weight[ln][col] >= maxWeight)
			{
				maxWeight = weight[ln][col];
				result.ln = ln;
				result.col = col;
			}
	cboard[result.ln][result.col] = 1;
	return result;
}


/*
* 作者：赵晨
* 输入：self 元组中黑色子的个数
*       enemy 白色子的个数
*       empty 空位的个数
* 输出：用于赋予每个空位的权值
*/
double calcWeight(int self, int enemy, int empty, Parameter p)
{
	if (empty == 5)
		return p.empty;
	if (empty == 0)
		return 0;
	if (self > 0 && enemy > 0)
		return p.polluted;
	if (self > 0 && enemy == 0)
		switch (self)
		{
		case 1:
			return p.s;
		case 2:
			return p.ss;
		case 3:
			return p.sss;
		case 4:
			return p.ssss;
		}
	if (self == 0 && enemy > 0)
		switch (enemy)
		{
		case 1:
			return p.e;
		case 2:
			return p.ee;
		case 3:
			return p.eee;
		case 4:
			return p.eeee;
		}
	return p.polluted;
}
