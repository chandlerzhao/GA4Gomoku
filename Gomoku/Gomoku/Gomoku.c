#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MAX_COMMAND_LENGTH 14	    // ������ַ��� 
#define DIMENSION 20		// ���̴�С

typedef struct				     	// �������� 
{
	int ln;							// ��ln�� 
	int col;				    	// ��col�� 
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
	double pr;		// ��ѡ�еĸ���
} Parameter;

const char PRIMITIVE[6][MAX_COMMAND_LENGTH] = { "START",
"PLACE",
"DONE",
"TURN",
"BEGIN",
"END" }; // �����ַ������� 

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
	short cboard[DIMENSION][DIMENSION];   // ���̾���
	memset(cboard, 0, sizeof cboard);
	int ln, col;					  // ��������
	int side;						  // 1��ʾ������2��ʾ�Է�
	char flag;						  // ����ĳ����״̬��0��ʾ���ӣ�1��ʾ������-1��ʾ�Է��� 
	int pNum;						  // ���������Ӧ����� 
	char command[MAX_COMMAND_LENGTH], // ���ڴ�Ŷ����ÿ������ 
		*rest = NULL;			  	  // ����ÿ��������ȡ������ʣ�� 
	Coordinate solution = { 0, 0 };		  // �������ӵ�����

	while (true)		  // �������� 
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
			flag = -1;			  // ����Ϊ�öԷ��� 
			cboard[ln][col] = flag;
			solution = solve(cboard, p);
			cboard[solution.ln][solution.col] = 1;			// �������ϼ�������λ�� 
			printf("%d %d\n", solution.ln, solution.col);
			fflush(stdout);
			break;
		case 4:					  // BEGIN
			solution = solve(cboard, p);
			cboard[solution.ln][solution.col] = 1;			// �������ϼ�������λ�� 
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
* ���ߣ��Գ�
* ����: command ���Կ���̨��ÿһ������
*       rest    ָ����������ĵ�һ���ַ���ָ�룬���޲�������ΪNULL
* ������������
*/

int parseCommand(char* command, char** rest)
{
	for (unsigned int i = 0; i < 6; i++)
	{
		if (strstr(command, PRIMITIVE[i]) == command)		// ���ÿ������
		{
			if ((*rest = command + strlen(PRIMITIVE[i]) + 1) > command + strlen(command)) //����������Ƿ��ж���
				*rest = NULL;
			return i;
		}
	}
	return -1;
}


Coordinate solve(short cboard[DIMENSION][DIMENSION], Parameter p)
{
	double weight[DIMENSION][DIMENSION] = { 0 };	// Ȩֵ��� 
													//memset(weight, 0, DIMENSION*DIMENSION);
													// ö��ÿ�����ҵ���Ԫ�� 
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

	// ö��ÿ�����ϵ���Ԫ�� 
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

	// ö��ÿ�����Խ��߷������Ԫ��
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

	// ö��ÿ�����Խ��߷������Ԫ�� 
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

	// �ҵ�Ȩֵ����λ�õ�����	
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
* ���ߣ��Գ�
* ���룺self Ԫ���к�ɫ�ӵĸ���
*       enemy ��ɫ�ӵĸ���
*       empty ��λ�ĸ���
* ��������ڸ���ÿ����λ��Ȩֵ
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
