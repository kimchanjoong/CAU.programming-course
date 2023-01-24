#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include "cursor.h"

enum { ESC = 27, LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80 };
#define putchxy(x, y, c) {gotoxy(x, y);putch(c);}
#define putsxy(x, y, s) {gotoxy(x, y);puts(s);}
char arStage[][18][21] = {
	{
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111000111111111111",
		"11111200111111111111",
		"11111002111111111111",
		"11100202011111111111",
		"11101011011111111111",
		"10001011011111003311",
		"10200200040000003311",
		"11111011101011003311",
		"11111000001111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
	},
	{
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11113300100000111111",
		"11113300102002001111",
		"11113300121111001111",
		"11113300004011001111",
		"11113300101002011111",
		"11111111101120201111",
		"11111102002020201111",
		"11111100001000001111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
	},
	{
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111110000041111",
		"11111111110212011111",
		"11111111110200211111",
		"11111111111202011111",
		"11111111110201011111",
		"11333300110200200111",
		"11133300002002000111",
		"11333300111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
		"11111111111111111111",
	},
};
char ns[18][21];
int nx, ny;
char* arTile[] = { "  ", "■", "⊙", "◇", "★" };
enum { EMPTY = 48, WALL, PACK, DEPOT, MAN };
int stage = 0, count = 0; //스테이지랑 움직인 횟수를 나타냄
int current_dx, current_dy;
int chance; //취소 기회

int main()
{
	int ch;
	int x, y;
	int dx, dy;
	char move_count[20];
	char move_time[20];
	char chance_num[20];
	BOOL bEnd;
	BOOL move_pack = FALSE; //팩을 움직였는지 판별하기

	clock_t start, finish;
	double duration;

	start = clock();
	showcursor(FALSE);

	// 전체 무한 루프
	for (;;) {
		// 게임판 초기화
		chance = 1; //기회를 1로 초기화
		memcpy(ns, arStage[stage], sizeof(ns));
		for (y = 0; y < 18; y++) {
			for (x = 0; x < 20; x++) {
				// 주인공의 위치 찾아 놓는다.
				if (ns[y][x] == MAN) {
					nx = x;
					ny = y;
					ns[y][x] = EMPTY;
				}
			}
		}
		clrscr();
		putsxy(45, 2, "SOKOBAN");
		putsxy(45, 4, "ESC:종료");
		putsxy(45, 6, "1, 2, 3:스테이지 이동");
		putsxy(45, 8, "c:동작취소");
		sprintf(move_count, "움직인 횟수는 %d회 입니다.", count);
		putsxy(45, 10, move_count);
		sprintf(chance_num, "남은 찬스는 %d회 입니다.", chance);
		putsxy(45, 12, chance_num);
		// 게임 진행 루프
		for (;;) {
			// 화면을 그린다.
			for (y = 0; y < 18; y++) {
				for (x = 0; x < 20; x++) {
					putsxy(x * 2, y, arTile[ns[y][x] - '0']);
				}
			}
			putsxy(nx * 2, ny, arTile[MAN - '0']);
			// 주인공 이동
			ch = getch();
			if (ch == 0xE0 || ch == 0) {
				ch = getch();
				dx = dy = 0;
				switch (ch) {
				case LEFT:
					dx = -1;
					count++;
					current_dx = -1;
					current_dy = 0;
					break;
				case RIGHT:
					dx = 1;
					count++;
					current_dx = 1;
					current_dy = 0;
					break;
				case UP:
					dy = -1;
					count++;
					current_dx = 0;
					current_dy = -1;
					break;
				case DOWN:
					dy = 1;
					count++;
					current_dx = 0;
					current_dy = 1;
					break;
				}
				// 벽이 아니어야 한다.
				if (ns[ny + dy][nx + dx] != WALL) {
					move_pack = FALSE;
					//짐을 미는 경우
					if (ns[ny + dy][nx + dx] == PACK) {
						move_pack = TRUE;
						// 그 다음 칸이 비어 있거나 창고여야 한다. 
						if (ns[ny + dy * 2][nx + dx * 2] == EMPTY ||
							ns[ny + dy * 2][nx + dx * 2] == DEPOT) {
							if (arStage[stage][ny + dy][nx + dx] == DEPOT) {
								ns[ny + dy][nx + dx] = DEPOT;
							}
							else {
								ns[ny + dy][nx + dx] = EMPTY;
							}
							ns[ny + dy * 2][nx + dx * 2] = PACK;
						}
						else {
							dx = dy = 0;
						}
					}
					// 새 위치로 이동
					nx += dx;
					ny += dy;
				}
				else //앞이 벽일 때
				{
					count--;
				}
				sprintf(move_count, "움직인 횟수는 %d회 입니다.", count);
				putsxy(45, 10, move_count);
			}
			else {
				if (ch == 'c') //chance 키는 c이다.
				{
					if (chance) //chance가 1일때만 작동
					{
						chance = 0;
						nx = nx - current_dx;
						ny = ny - current_dy;
						if (move_pack) //상자를 움직였을 시 복구하기
						{
							ns[ny + current_dy * 2][nx + current_dx * 2] = EMPTY;
							ns[ny + current_dy][nx + current_dx] = PACK;
						}
						sprintf(chance_num, "남은 찬스는 %d회 입니다.", chance);
						putsxy(45, 12, chance_num);
					}
				}
				if (ch == ESC) {
					showcursor(TRUE);
					return;
				}
				if (ch == '1') {
					stage = 0;
					break;
				}
				if (ch == '2') {
					stage = 1;
					break;
				}
				if (ch == '3') {
					stage = 2;
					break;
				}
			}

			finish = clock();
			duration = (double)(finish - start) / CLOCKS_PER_SEC;
			sprintf(move_time, "경과시간은 %.4lf초 입니다", duration);
			putsxy(45, 14, move_time);

			// 게임 끝 처리
			bEnd = TRUE;
			for (y = 0; y < 18; y++) {
				for (x = 0; x < 20; x++) {
					if (ns[y][x] == DEPOT) {
						bEnd = FALSE;
					}
				}
			}

			if (bEnd) {
				clrscr();
				putsxy(10, 10, "참 잘했어요. 다음 스테이지로 이동합니다.");
				delay(2000);
				stage++;
				if (stage == 3) stage = 0;
				break;
			}
		}
	}
}

