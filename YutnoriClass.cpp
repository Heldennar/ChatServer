// #define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "YutnoriClass.h"
#include "ChatServer.h"
#include "ChatServerDlg.h"
#include <random>


YutnoriClass::YutnoriClass()
{
	b_len = sizeof(board) / sizeof(int);
	for (int i = 0; i < 4; i++) { player1[i] = 0; } //말 리셋
	for (int i = 0; i < 4; i++) { player2[i] = 0; }
	for (int i = 0; i < b_len; i++) { board[i] = 0; } //보드 리셋
	for (int i = 0; i < 4; i++) { dice[i] = 0; }
	playing = true;
	turn = NULL;
	roll_m = 0;
}
int YutnoriClass::roll(int i)
{
	int rollv = 0;
	bool temp = true;

	moveNum = new int[i + 1];

	if (i > 0) {
		int* tempNum = new int[i + 1];
		for (int r = 0; r < i; r++)
			tempNum[r] = moveNum[r];
		delete[] moveNum;
		moveNum = tempNum;
		delete[] tempNum;
		tempNum = NULL;
	}
	for (int j = 0; j < 4; j++) {
		// 시드값을 얻기 위한 random_device 생성.
		std::random_device rd;

		// random_device 를 통해 난수 생성 엔진을 초기화 한다.
		std::mt19937 gen(rd());

		// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
		std::uniform_int_distribution<int> dis(0, 1);
		dice[j] = dis(gen);
		rollv += dice[j];
	}
	moveNum[i] = rollv;
	return rollv;
}
void YutnoriClass::move(int player)
{
	int num = 0;
	while (moveNum[0] != NULL) {
		printf("이동 가능한 윷 :");
		for (int i = 0; i < _msize(moveNum) / sizeof(int); i++)
			printf("%d / ", moveNum[i]);
		printf("이동 : ");
		//scanf("%d", &num);
		int whereami = player1[player];
		int checknum = player1[player] + moveNum[num];
		printf("선택된 값은 %d입니다. 말의 현재 위치는 %d입니다. 이동후 예상 위치는 %d입니다.\n", moveNum[num], player1[player], checknum);
		if (checknum != 0 && board[checknum] != 0) // 이동시킬 자리에 무언가 다른정보가 있을 경우
		{
			if (catchs(checknum))
				printf("상대편 말을 잡았습니다.");
			else
				printf("말을 업었습니다.");
		}
		for (int i = 0; i < 4; i++) {			//업힌 상태인 경우 한번에 이동
			if ((whereami > 0) && (whereami == player1[i]))
				player1[i] = checknum;
		}
		if (player1[player] == 0)
			player1[player] = checknum;
		printf("%d번말이 %d번으로 이동 했습니다.\n", player, player1[player]);
		moveNum[num] = NULL;
		for (int i = num; i < (_msize(moveNum) / sizeof(int)) - 1; i++) {
			moveNum[i] = moveNum[i + 1];
			moveNum[i + 1] = NULL;
		}
		printf("이동완료 \n");
		if (player1[player] > 20)
			goal();
	}
}
bool YutnoriClass::randT()
{
	srand(time(NULL));
	int temp;
	if (playing) {
		temp = rand() % 2;
		if (temp == 0)
		{
			this->turn = true; printf("플레이어1이 먼저 시작합니다.");
		}
		else if (temp == 1) {
			this->turn = false; printf("플레이어2가 먼저 시작합니다.");
		}
	}
	return true;
}
bool YutnoriClass::bonusT() //쓸모 없을듯?
{
	return true;
}
bool YutnoriClass::catchs(int num) //잡은것인지 아닌지 체크
{
	for (int i = 0; i < 4; i++) {
		if (player1[i] == num) //업은 것일 경우
			return false;
		else if (player2[i] == num) { //잡은 경우
			player2[i] = 0;
			return true;
		}
		else
			return false;
	}
	return false;
}
bool YutnoriClass::goal()
{
	for (int i = 0; i < 4; i++) {
		if (player1[i] > 20) {
			player1[i] = -1;
			return true;
		}
		if (player2[i] > 20) {
			player2[i] = -1;
			return true;
		}
	}
	return false;
}
bool YutnoriClass::win()
{
	int temp = 0;
	for (int i = 0; i < 4; i++)
		temp -= player1[i];
	if (temp == -4)
		printf("승리하였습니다!");
	for (int i = 0; i < 4; i++)
		temp -= player2[i];
	if (temp == -4)
		printf("패배하였습니다!");
	return false;
}
void YutnoriClass::showPlayer()
{
	printf("움직일 말을 고르십시오. (출발하지 않은 말:");
	for (int i = 0; i < 4; i++)
	{
		if (player1[i] == 0)
			printf("%d / ", i + 1);
	}
	printf(")\n");
	printf("(보드 위에 있는 말 : ");
	for (int i = 0; i < 4; i++)
	{
		if (player1[i] > 0 && player1[i] < 21)
			printf("%d = %d번 위치 / ", i + 1, player1[i]);
	}
	printf(")\n");
}
