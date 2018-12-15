#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <stdarg.h>
#include <math.h>


//#define DEBUG						// 单步跟踪
//#define GRAPHIC_OUT				// 图形化输出开关
//#define DETAILS						// 是否输出每局比赛细节
#define TO_FILE					// 输出到文件
#define DIMENSION 20
#define PI 3.14159265
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

typedef struct {
	int times;						// 总局数
	int finalWinner;				// 定义1为参数1赢，-1为参数2赢，平局为0
	int win1;						// 参数1赢的次数
	int win2;						// 参数2赢的次数
	int draw;						// 平局次数
	double averageHands;				// 平局每局交手回合数
	double sdHands;					// 交手回合标准差
	double cv;						// 变异系数
	int p1WinSteps;					// agent1赢的比赛中总共的步数
	int p2WinSteps;					// agent2赢的比赛中总共的步数
} MatchResult;

typedef struct				     	// 棋子坐标 
{
	int ln;							// 第ln行 
	int col;				    	// 第col列 
} Coordinate;

typedef struct
{
	int int1;						// 二元组
	int int2;
}Tuple2;

Coordinate solve(short cboard[DIMENSION][DIMENSION], Parameter p);
Coordinate solveEnemy(short cboard[DIMENSION][DIMENSION], Parameter p);
double calcWeight(int self, int enemy, int empty, Parameter p);
MatchResult battle(Parameter p1, Parameter p2, int times);
void initCBoard(short cboard[DIMENSION][DIMENSION]);
int checkWinner(short cboard[DIMENSION][DIMENSION], Coordinate *tuple[5]);
void printCBoard();
void writeLine(const char *format, ...);
void writeBottom(const char *format, ...);
void printChess(short cboard[DIMENSION][DIMENSION]);
void matchAll(Parameter p[], int n);
void match2(Parameter *p1, Parameter *p2, int n);
Parameter *findMaxp(Parameter p[], int n);
FILE *fp;

//遗传算法函数集
double dTotalFitness = 0;
double crossoverRate = 0.5;
double mutationRate = 0.5;
double leftBound = 0.01;
double rightBound = 1000000000;
double maxPertubation = 0.2;
void initGen(Parameter p[], int n);
int selGenome(Parameter p[], int n);
double random();
int randomInt(int n);
Parameter mutate(Parameter p);
Parameter crossover(Parameter p1, Parameter p2);
void* getParam(Parameter *p, int i);
void produceChild(Parameter p[], int *children, const int children_count, Parameter pNew[]);
int countOfParent = 10;
int match_rounds = 20;
int pCounter =0;
int iter = 0, iterCount = 100;
int countOfParam = 10;

#define CHILDREN_COUNT  10
double perturb(double param);

int main(void)
{
	//Parameter pInit[CHILDREN_COUNT] = { 
	//	{ 7, 35, 800, 18000, 800000, 15, 400, 15000, 700000, 0.1 },
	//	{ 5, 20, 200, 20000, 200000, 10, 100, 10000, 300000, 0.1 },
	//	{ 3, 10, 500, 50000, 400000, 5, 200, 40000, 500000, 0.1 },
	//	{ 1, 10, 100, 10000, 100000, 20, 3000, 5000, 100000, 0.1 }
	//				};
	/*Parameter pInit[CHILDREN_COUNT] = {
		{ 8.81, 4.05, 249.80, 1578.46, 1631290.47, 13.42, 206.34, 5809.85, 481950.66, 0.10 },
		{ 6.56, 3.89, 90.52, 2604.71, 1912988.59, 8.80, 346.64, 11874.92, 419113.64, 0.20 },
		{ 7.12, 7.12, 112.34, 1739.12, 948232.56, 7.92, 688.86, 12746.53, 378189.71, 0.30 },
		{ 8.27, 8.69, 137.54, 7989.56, 673906.45, 6.84, 246.66, 13822.75, 480411.35, 0.40 },
		{ 8.27, 8.69, 137.54, 7989.56, 673906.45, 6.84, 246.66, 13822.75, 480411.35, 0.50 },
		{ 6.38, 15.63, 184.31, 8009.38, 480333.10, 17.58, 1529.08, 20265.61, 580366.33, 0.60 }
	};*/
	/*Parameter pInit[CHILDREN_COUNT] = {
		{ 10.28, 3.90, 141.14, 17538.15, 895110.93, 19.57, 266.97, 1945.81, 1082573.28, 0.70 },
		{ 14.94, 7.00, 85.23, 11449.61, 813227.13, 27.32, 132.19, 2422.73, 969156.73, 0.90 },
		{ 8.90, 2.91, 138.03, 21075.88, 1000779.90, 17.61, 268.31, 1609.15, 988506.30, 0.00 },
		{ 9.46, 3.79, 92.13, 10927.81, 1179476.02, 15.13, 142.01, 3560.99, 488576.70, 0.01 },
		{ 9.20, 2.89, 96.61, 18172.80, 926113.25, 17.07, 98.51, 1709.14, 988506.30, 0.01 },
		{ 18.22, 7.19, 98.02, 16427.99, 883681.85, 24.24, 152.54, 1597.57, 969156.73, 0.01 },
		{ 19.19, 4.02, 126.65, 14401.47, 710458.46, 24.82, 130.18, 1576.25, 1064714.69, 0.01 },
		{ 10.55, 3.30, 181.72, 10533.17, 776294.57, 19.57, 236.50, 2419.12, 1082573.28, 0.01 },
		{ 9.12, 5.17, 88.88, 8898.81, 1065652.23, 12.96, 225.42, 6967.19, 570947.42, 0.50 },
		{ 7.63, 3.02, 135.38, 20019.83, 929765.77, 15.97, 162.53, 1905.49, 1087396.75, 0.10 }
	};*/
	/*Parameter pInit[CHILDREN_COUNT] = {
	{3.74, 8.22, 29.38, 7143.41, 660021.42, 31.41, 171.41, 410.07, 885549.64, 0.01},
	{ 3.67, 14.99, 21.25, 12137.19, 1340561.49, 42.50, 168.71, 268.62, 1047506.96, 0.00 },
	{ 4.45, 8.63, 24.80, 8976.07, 1645803.29, 36.39, 183.11, 246.95, 1145684.52, 0.00 },
	{ 3.08, 11.57, 18.51, 9412.96, 1778222.66, 42.50, 194.58, 315.48, 1047506.96, 0.00 },
	{ 4.51, 13.96, 24.80, 8976.07, 1645803.29, 27.22, 183.11, 294.88, 1145684.52, 0.01 },
	{ 3.74, 9.93, 28.03, 7898.94, 660021.42, 27.02, 171.41, 379.30, 847668.33, 0.01 },
	{ 2.75, 11.01, 31.03, 8558.98, 1091443.85, 35.03, 229.23, 533.65, 962965.75, 0.00 },
	{ 3.86, 16.12, 17.78, 8976.07, 1200864.36, 27.73, 128.73, 307.26, 1145684.52, 0.00 },
	{ 3.86, 13.88, 27.25, 9311.80, 1701132.18, 31.89, 190.75, 352.47, 1145684.52, 0.00 },
	{ 3.57, 13.68, 27.88, 6790.32, 1638104.32, 33.01, 167.53, 332.66, 1145684.52, 0.00 }
	};*/
	/*Parameter pInit[CHILDREN_COUNT] = {
	{4.68, 7.46, 27.46, 13967.01, 705427.13, 58.02, 132.56, 331.58, 1205522.69, 0.01},
	{3.47, 10.78, 28.24, 11800.14, 975739.28, 59.12, 158.32, 299.41, 1076269.93, 0.00},
	{4.15, 5.73, 31.09, 11836.00, 1029634.83, 52.35, 212.47, 240.56, 1086091.08, 0.00},
	{4.42, 8.12, 31.57, 13967.01, 952990.01, 54.76, 161.87, 204.42, 863680.79, 0.00},
	{4.68, 7.62, 32.25, 13292.14, 972284.48, 54.33, 156.60, 328.78, 849737.95, 0.00},
	{4.53, 6.38, 33.23, 14602.64, 1029634.83, 54.44, 151.80, 315.25, 1086091.08, 0.00},
	{6.42, 7.62, 28.21, 10905.51, 952990.01, 50.61, 165.08, 285.95, 1038513.67, 0.00},
	{4.42, 9.01, 31.57, 13967.01, 819659.01, 56.62, 161.87, 188.22, 1198271.35, 0.01},
	{5.76, 7.62, 24.91, 11286.20, 871665.10, 49.51, 165.08, 387.02, 867493.01, 0.00},
	{4.81, 9.77, 26.18, 12709.85, 871665.10, 55.60, 184.59, 253.48, 1076269.93, 0.00}
	};*/
	char file_name_result[100] = "", file_name_max[100] = "";
	countOfParent = 10;
	for (int i = 0; i < countOfParent; i++)
		pInit[i].number =i-countOfParent;
	Parameter *pParent, *pNew;
	pParent = (Parameter *)malloc(CHILDREN_COUNT * sizeof(Parameter));
	memcpy(pParent, pInit, CHILDREN_COUNT * sizeof(Parameter));
	srand(time(0));
#ifdef TO_FILE
	int timestamp = time(NULL);
	sprintf(file_name_result, "result_%d.txt", timestamp);
	sprintf(file_name_max, "max_%d.txt", timestamp);
	freopen(file_name_result, "w", stdout);
	fp = fopen(file_name_max, "w");
#endif
	// loop
	while (iter < iterCount)
	{
		fprintf(stderr, "\nGeneration %d.\n", iter);
		printf("\nGeneration %d\n", iter);
		fprintf(fp, "Generation\t%d\t;", iter);
		matchAll(pParent, countOfParent);
		initGen(pParent, countOfParent);
		int children = 0;
		pNew = (Parameter *)malloc(CHILDREN_COUNT * sizeof(Parameter));
		memset(pNew, 0, sizeof(Parameter)*CHILDREN_COUNT);
		while (children < CHILDREN_COUNT)
		{
			produceChild(pParent, &children, countOfParent, pNew);
		}
		free(pParent);
		pParent = pNew;
		iter++;
		fflush(stdout);
		fflush(fp);
		countOfParent = CHILDREN_COUNT;
	}
	fprintf(stderr, "Finished.\n");
	printf("%.3fs taken.\n", (double)clock() / CLOCKS_PER_SEC);
#ifdef TO_FILE
	fclose(stdout);
	fclose(fp);
#endif
	system("pause");
	return 0;
}

void produceChild(Parameter pParent[], int *children, const int pCount, Parameter pNew[])
{
	int k1;
	int k2;
	do
	{
		k1 = selGenome(pParent, pCount);
		k2 = selGenome(pParent, pCount);
	} while (k1 == k2);
	Parameter *p1, *p2, newP1 = {}, newP2 = {};
	p1 = pParent + k1;
	p2 = pParent + k2;
	newP1 = crossover(*p1, *p2);
	newP2 = crossover(*p1, *p2);

	newP1 = mutate(newP1);
	// TODO: modifyit.
	pNew[*children] = newP1;
	pNew[*children].number = pCounter++;
	(*children)++;

	newP2 = mutate(newP2);
	pNew[*children] = newP2;
	pNew[*children].number = pCounter++;
	(*children)++;
}

void initGen(Parameter p[], int n)
{
	dTotalFitness = 0;
	for (int i = 0; i < n; i++)
	{
		dTotalFitness += p[i].fitness;
	}
	for (int i = 0;i < n;i++)
	{
		p[i].pr = p[i].fitness / dTotalFitness;
	}
}

//返回一个(0,1)之间的数
double random()
{
	int r = rand();
	return (double)((r+1)/ (double)(RAND_MAX+1));
}

//返回一个0 - n-1之间的数, n<32768
int randomInt(int n)
{
	int r = rand();
	return r % n;
}

Parameter *findMaxp(Parameter p[], int n)
{
	Parameter *pMax = NULL;
	double max = 0;
	for (int i = 0; i < n;i++)
		if ((p+i)->fitness > max)
		{
			max = (p+i)->fitness;
			pMax = p + i;
		}
	return pMax;
}

void* getParam(Parameter *p, int i)
{
	switch (i)
	{
	case 0:
		return &p->empty;
	case 1:
		return &p->s;
	case 2:
		return &p->ss;
	case 3:
		return &p->sss;
	case 4:
		return &p->ssss;
	case 5:
		return &p->e;
	case 6:
		return &p->ee;
	case 7:
		return &p->eee;
	case 8:
		return &p->eeee;
	case 9:
		return &p->polluted;
	}
	return NULL;
}

int selGenome(Parameter p[], int n)
{
	double dSlice = random();
	double soFar = 0;
	for (int i = 0;i < n;i++)
	{
		soFar += p[i].pr;
		if (dSlice <= soFar)
			return i;
	}
	return 0;
}

Parameter crossover(Parameter p1, Parameter p2)
{
	Parameter newP = {};
	for (int i = 0; i < countOfParam;i++)
	{
		if (random() <= crossoverRate)
			*(double *)getParam(&newP, i) = *(double*)getParam(&p1, i);
		else
			*(double *)getParam(&newP, i) = *(double*)getParam(&p2, i);
	}
	return newP;
}

double perturb(double param)
{
	double r = param * ((random() - 0.5) * 2 * maxPertubation + 1);
 	return r;

}

Parameter mutate(Parameter p)
{
	Parameter pnew = {};
	if (random() < mutationRate)
	{
		pnew.empty = perturb(p.empty);
		if (pnew.empty < leftBound)
			pnew.empty = leftBound;
		else if (pnew.empty > rightBound)
			pnew.empty = rightBound;
	}
	else
		pnew.empty = p.empty;

	if (random() < mutationRate)
	{
		pnew.s = perturb(p.s);
		if (pnew.s < leftBound)
			pnew.s = leftBound;
		else if (pnew.s > rightBound)
			pnew.s = rightBound;
	}
	else
		pnew.s = p.s;

	if (random() < mutationRate)
	{
		pnew.ss = perturb(p.ss);
		if (pnew.ss < leftBound)
			pnew.ss = leftBound;
		else if (pnew.ss > rightBound)
			pnew.ss = rightBound;
	}
	else
		pnew.ss = p.ss;

	if (random() < mutationRate)
	{
		pnew.sss = perturb(p.sss);
		if (pnew.sss < leftBound)
			pnew.sss = leftBound;
		else if (pnew.sss > rightBound)
			pnew.sss = rightBound;
	}
	else
		pnew.sss = p.sss;

	if (random() < mutationRate)
	{
		pnew.ssss = perturb(p.ssss);
		if (pnew.ssss < leftBound)
			pnew.ssss = leftBound;
		else if (pnew.ssss > rightBound)
			pnew.ssss = rightBound;
	}
	else
		pnew.ssss = p.ssss;

	if (random() < mutationRate)
	{
		pnew.e = perturb(p.e);
		if (pnew.e < leftBound)
			pnew.e = leftBound;
		else if (pnew.e > rightBound)
			pnew.e = rightBound;
	}
	else
		pnew.e = p.e;

	if (random() < mutationRate)
	{
		pnew.ee = perturb(p.ee);
		if (pnew.ee < leftBound)
			pnew.ee = leftBound;
		else if (pnew.ee > rightBound)
			pnew.ee = rightBound;
	}
	else
		pnew.ee = p.ee;

	if (random() < mutationRate)
	{
		pnew.eee = perturb(p.eee);
		if (pnew.eee < leftBound)
			pnew.eee = leftBound;
		else if (pnew.eee > rightBound)
			pnew.eee = rightBound;
	}
	else
		pnew.eee = p.eee;

	if (random() < mutationRate)
	{
		pnew.eeee = perturb(p.eeee);
		if (pnew.eeee < leftBound)
			pnew.eeee = leftBound;
		else if (pnew.eeee > rightBound)
			pnew.eeee = rightBound;
	}
	else
		pnew.eeee = p.eeee;

	if (random() < mutationRate)
	{
		pnew.polluted = perturb(p.polluted);
		if (pnew.polluted < leftBound)
			pnew.polluted = leftBound;
		else if (pnew.polluted > rightBound)
			pnew.polluted = rightBound;
	}
	else
		pnew.eeee = p.eeee;
	return pnew;
}

//
void matchAll(Parameter p[], int n)
{
	for (int i = 0; i < n; i++)
		for (int j = i + 1; j < n; j++)
		{
			match2(p + i, p + j ,n);
		}
	printf("================================================Generation %d Result===================================================\n", iter);
	for (int i = 0; i < n;i++)
		printf("p%d.fitess=%.2lf ", p[i].number, p[i].fitness);
	printf("\n");
	Parameter *pMax;
	pMax = findMaxp(p, n);
	printf("Maximum:p%d=\t{%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f}\tfitness=%.2lf\n",
		pMax->number, pMax->empty, pMax->s, pMax->ss, pMax->sss, pMax->ssss, pMax->e, pMax->ee, pMax->eee, pMax->eeee, pMax->polluted, pMax->fitness);
	fprintf(fp, "Maximum:p%d=\t{%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f}\t%.2lf\n",
		pMax->number, pMax->empty, pMax->s, pMax->ss, pMax->sss, pMax->ssss, pMax->e, pMax->ee, pMax->eee, pMax->eeee, pMax->polluted, pMax->fitness);
	printf("\n");
}
void match2(Parameter *p1, Parameter *p2, int n)
{
	fprintf(stderr, "Matching parameter p%d and p%d,", p1->number, p2->number);
	MatchResult result = battle(*p1, *p2, match_rounds);
	fprintf(stderr, "\tp%d wins finally.\n", (result.finalWinner == 0 ? -countOfParent : result.finalWinner == 1 ? p1->number : p2->number));
	printf("p%d:{%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f}\np%d:{%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f}\n",
		p1->number, p1->empty, p1->s, p1->ss, p1->sss, p1->ssss, p1->e, p1->ee, p1->eee, p1->eeee, p1->polluted,
		p2->number, p2->empty, p2->s, p2->ss, p2->sss, p2->ssss, p2->e, p2->ee, p2->eee, p2->eeee, p2->polluted);
	printf("times:%d\nfinalWinner:p%d\nwin1:%d\nwin2:%d\ndraw:%d\np%d WinSteps:%d\np%d WinSteps:%d\naverageHands:%.2lf\n",
		result.times, (result.finalWinner == 0 ? -countOfParent : result.finalWinner == 1 ? p1->number : p2->number), result.win1, result.win2, result.draw, p1->number, result.p1WinSteps, p2->number, result.p2WinSteps, result.averageHands);
	p1->fitness += pow(result.p1WinSteps,2)/result.averageHands /match_rounds /(n-1);
	p2->fitness += pow(result.p2WinSteps, 2) / result.averageHands / match_rounds / (n - 1);
	printf("p%d fitness:%.2lf\np%d fitness:%.2lf\n\n", p1->number, p1->fitness, p2->number, p2->fitness);
}

MatchResult battle(Parameter p1, Parameter p2, int times)
{
	MatchResult matchResult = {};
	matchResult.times = times;
	int *hands = NULL;
	hands = (int *)calloc(times, sizeof(int));
	//memset(hands, 0, sizeof(int) * times);

	int sumHands = 0;
	for (int t = 0; t < times; t++)
	{
		short cboard[DIMENSION][DIMENSION] = {};   // 棋盘矩阵, -1表示对方，0表示无子，1表示己方c
		int restCount = DIMENSION*DIMENSION;
		initCBoard(cboard);
#ifdef GRAPHIC_OUT
		printChess(cboard);
		HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
		int winner = 0;

		Coordinate *tuple[5] = {};
		do
		{
			Coordinate self = solve(cboard, p1);
			restCount--;
#ifdef GRAPHIC_OUT
			printChess(cboard);
#endif
#ifdef DETAILS
			writeLine("%d %d 1\n", self.ln, self.col);
#endif
			Coordinate enemy = solveEnemy(cboard, p2);
			restCount--;
#ifdef GRAPHIC_OUT
			printChess(cboard);
#endif
#ifdef DETAILS
			writeLine("%d %d -1\n", enemy.ln, enemy.col);
#endif
			hands[t]++;
			winner = checkWinner(cboard, tuple);
		} while (winner == 0 && restCount > 0);
		switch (winner)
		{
		case 1:
			matchResult.win1++;
			matchResult.p1WinSteps += hands[t];
			break;
		case -1:
			matchResult.win2++;
			matchResult.p2WinSteps += hands[t];
			break;
		case 0:
			matchResult.draw++;
			matchResult.p1WinSteps += hands[t];
			matchResult.p2WinSteps += hands[t];
			break;
		}
		sumHands += hands[t];
#ifdef DETAILS
		writeBottom("Winner: %d\n", winner);
		writeBottom("Hands: %d\n", hands);
		for (int i = 0; i < 5; i++)
			writeBottom("(%d,%d) ", tuple[i]->ln, tuple[i]->col);
		writeBottom("\n");
#endif
		for (int i = 0; i < 5;i++)
			free(tuple[i]);
	}
	if (matchResult.win1 > matchResult.win2)
		matchResult.finalWinner = 1;
	else if (matchResult.win1 < matchResult.win2)
		matchResult.finalWinner = -1;
	else
		matchResult.finalWinner = 0;
	matchResult.averageHands = (double)sumHands / (double)times;
	for (int t = 0; t < times; t++)
	{
		//fprintf(stderr,"%d ", hands[t]);
		matchResult.sdHands += pow((double)hands[t] - matchResult.averageHands, 2);
	}
	matchResult.sdHands = matchResult.sdHands / times;
	matchResult.sdHands = sqrt(matchResult.sdHands);
	matchResult.cv = matchResult.sdHands / matchResult.averageHands;
	free(hands);
	//fprintf(stderr,"\n");
	return matchResult;
}

// 初始化棋盘，在随机位置上放置对方子 
void initCBoard(short cboard[DIMENSION][DIMENSION])
{
#ifdef GRAPHIC_OUT
	system("mode con lines=50 cols=150");
	printCBoard();
#endif
	memset(cboard, 0, DIMENSION * DIMENSION);
	int ln = rand() % DIMENSION;
	int col = rand() % DIMENSION;
	cboard[ln][col] = -1;
#ifdef DETAILS
	writeLine("%d %d -1", ln, col);
#endif
}

// 在屏幕右侧输出步骤
void writeLine(const char *format, ...)
{
#ifdef GRAPHIC_OUT
	static COORD rem = { 90, 0 };
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (rem.Y >= 40)
	{
		rem.X += 10;
		rem.Y = 0;
	}
	//SetConsoleScreenBufferSize(hout, size);
	//if (SetConsoleWindowInfo(hout, true, &rc));
	//printf("%d", GetLastError());
	SetConsoleTextAttribute(hout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	SetConsoleCursorPosition(hout, rem);
	//printf("%d %d", GetLargestConsoleWindowSize(hout).X, GetLargestConsoleWindowSize(hout).Y);
#endif
	va_list parg;
	va_start(parg, format);
	vprintf(format, parg);
#ifdef DEBUG 
	getchar();
#endif
	va_end(parg);
#ifdef GRAPHIC_OUT
	rem.Y += 1;
#endif
}

void writeBottom(const char *format, ...)
{
#ifdef GRAPHIC_OUT
	static COORD rem = { 0, 40 };
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	SetConsoleCursorPosition(hout, rem);
#endif
	va_list parg;
	va_start(parg, format);
	vprintf(format, parg);
#ifdef DEBUG
	getchar();
#endif
	va_end(parg);
#ifdef GRAPHIC_OUT
	rem.Y += 1;
#endif
}

void printChess(short cboard[DIMENSION][DIMENSION])
{
	HANDLE consolehwnd;		//创建句柄
	consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);//实例化句柄
	SetConsoleTextAttribute(consolehwnd, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
	COORD origin = { 0,0 };
	SetConsoleCursorPosition(consolehwnd, origin);
	for (int ln = 0; ln < DIMENSION; ln++)
		for (int col = 0; col < DIMENSION; col++)
		{
			COORD pos = { origin.X + 4 * col + 2, origin.Y + 2 * (DIMENSION - 1 - ln) + 1 };
			SetConsoleCursorPosition(consolehwnd, pos);
			if (cboard[ln][col] == 1)
			{
				SetConsoleTextAttribute(consolehwnd, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				printf("●");
			}
			else if (cboard[ln][col] == -1)
			{
				SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				printf("●");
			}
		}
}

void printCBoard()
{
	HANDLE consolehwnd;		//创建句柄
	consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);//实例化句柄
	SetConsoleTextAttribute(consolehwnd, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
	COORD origin = { 0,0 };
	SetConsoleCursorPosition(consolehwnd, origin);
	printf("  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19 \n");
	printf("19┏━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┳━┓\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("18┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("17┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("16┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("15┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("14┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("13┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("12┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("11┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("10┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("9 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("8 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("7 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("6 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("5 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("4 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("3 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("2 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("1 ┣━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━╋━┫\n");
	printf("  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃  ┃\n");
	printf("0 ┗━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┻━┛\n");
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
		{
			if (weight[ln][col] >= maxWeight)
			{
				maxWeight = weight[ln][col];
				result.ln = ln;
				result.col = col;
			}
		}
	cboard[result.ln][result.col] = 1;
	return result;
}

Coordinate solveEnemy(short cboard[DIMENSION][DIMENSION], Parameter p)
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
				case -1:
					self++;
					break;
				case 1:
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
				case -1:
					self++;
					break;
				case 1:
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
				case -1:
					self++;
					break;
				case 1:
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
		for (int col = 4; col < DIMENSION; col++)
		{
			int self = 0, enemy = 0, empty = 0;
			for (int i = 0; i < 5; i++)
				switch (cboard[ln - i][col + i])
				{
				case 0:
					empty++;
					break;
				case -1:
					self++;
					break;
				case 1:
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
	cboard[result.ln][result.col] = -1;
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

// 计算赢家
int checkWinner(short cboard[DIMENSION][DIMENSION], Coordinate *tuple[5])
{
	//检查每行是否有五个相连 
	for (int ln = 0; ln < DIMENSION; ln++)
	{
		short last = 0, count = 1;
		for (int col = 0; col < DIMENSION; col++)
		{
			if (cboard[ln][col] == last && last != 0)
			{
				count++;
				if (count == 5)
				{
					for (int i = 4; i >= 0;i--)
					{
						tuple[i] = (Coordinate *)malloc(sizeof(Coordinate));
						tuple[i]->ln = ln;
						tuple[i]->col = col - (4 - i);
					}
					return last;
				}
			}
			else
				count = 1;
			last = cboard[ln][col];
		}
	}

	//检查每列是否有五个相连 
	for (int col = 0; col < DIMENSION; col++)
	{
		short last = 0, count = 1;
		for (int ln = 0; ln < DIMENSION; ln++)
		{
			if (cboard[ln][col] == last && last != 0)
			{
				count++;
				if (count == 5)
				{
					for (int i = 4; i >= 0;i--)
					{
						tuple[i] = (Coordinate *)malloc(sizeof(Coordinate));
						tuple[i]->ln = ln - (4 - i);
						tuple[i]->col = col;
					}
					return last;
				}
			}
			else
				count = 1;
			last = cboard[ln][col];
		}
	}

	//检查主对角线方向上是否有五个相连
	for (int sum = 4; sum < 2 * DIMENSION - 5; sum++) // 每一条主对角线上行与列坐标之和为常数 
	{
		short last = 0, count = 1;
		int ln;
		for (int col = (sum < DIMENSION ? 0 : sum - DIMENSION + 1); col < (sum < DIMENSION ? sum : DIMENSION - 1); col++)
		{
			ln = sum - col;
			if (cboard[ln][col] == last && last != 0)
			{
				count++;
				if (count == 5)
				{
					for (int i = 4; i >= 0;i--)
					{
						tuple[i] = (Coordinate *)malloc(sizeof(Coordinate));
						tuple[i]->ln = ln + (4 - i);
						tuple[i]->col = col - (4 - i);
					}
					return last;
				}
			}
			else
				count = 1;
			last = cboard[ln][col];
		}
	}

	//检查副对角线方向上是否有五个相连
	for (int diff = DIMENSION - 5; diff >= 5 - DIMENSION; diff--) // 每一条主对角线上行与列坐标之和为常数 
	{
		int ln;
		short last = 0, count = 1;
		for (int col = diff >= 0 ? 0 : -diff; col < (diff >= 0 ? DIMENSION - diff : DIMENSION); col++)
		{
			ln = col + diff;
			if (cboard[ln][col] == last && last != 0)
			{
				count++;
				if (count == 5)
				{
					for (int i = 4; i >= 0;i--)
					{
						tuple[i] = (Coordinate *)malloc(sizeof(Coordinate));
						tuple[i]->ln = ln - (4 - i);
						tuple[i]->col = col - (4 - i);
					}
					return last;
				}
			}
			else
				count = 1;
			last = cboard[ln][col];
		}
	}
	return 0;
}
