#include <iostream>
#include <conio.h>   // _kbhit, _getch (Windows console-specific)
#include <windows.h> // Sleep, Console functions
#include <algorithm> // min, max
#include <cstdio>    // putchar
#include <limits>
#include "Screen.h"
#include "SnakeMap.h"
#include "Bag.h"
#include "Snake.h"

using namespace std;

// =========================================================
// 전역 설정 및 유틸리티 함수
// =========================================================
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const char MAP_CHAR = '#';
const char SNAKE_HEAD_CHAR = '@';
const char SNAKE_BODY_CHAR = 'o';
const char FOOD_CHAR = '$';
const int GAME_SPEED_MS = 100; // 게임 속도 (100ms마다 업데이트)

bool gameOver = false;
int score = 0;
// Point 구조체는 Bag.h에 정의되어 있습니다.
Point food_pos = { -1, -1 }; // 현재 먹이 위치 (row, col)
Point prev_food_pos = { -1, -1 }; // 이전 프레임 먹이 위치 (잔상 제거용)

// 콘솔 커서 위치를 이동시키는 함수
void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x; // col
	coord.Y = y; // row
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void CursorView()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;        // 커서 크기 (1~100)
	cursorInfo.bVisible = FALSE;  // 커서 숨김
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 콘솔 버퍼/윈도우 크기를 맵 크기에 맞춰 설정합니다.
// 너무 큰 값으로 설정하면 실패할 수 있으므로 현재 시스템 허용치로 클램프합니다.
void configureConsoleForMap(int cols, int rows)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

	// 윈도우에서 허용하는 최대 칸 수를 얻습니다.
	COORD largest = GetLargestConsoleWindowSize(hOut);

	// 표시할 윈도우 크기 (버퍼 크기보다 작거나 같아야 함)
	int winCols = min(cols, (int)largest.X);
	int winRows = min(rows + 5, (int)largest.Y); // 상태 라인 여유 공간 추가

	// 버퍼 크기는 윈도우보다 크거나 같아야 합니다.
	COORD bufferSize;
	bufferSize.X = max(cols, winCols);
	bufferSize.Y = max(rows + 5, winRows);

	// 먼저 버퍼 크기 설정 (실패할 수 있으므로 반환값은 체크하지 않음)
	SetConsoleScreenBufferSize(hOut, bufferSize);

	// 그 다음 윈도우 크기 설정 (버퍼 크기 내로 클램프되어야 함)
	SMALL_RECT sr;
	sr.Left = 0;
	sr.Top = 0;
	sr.Right = winCols - 1;
	sr.Bottom = winRows - 1;
	SetConsoleWindowInfo(hOut, TRUE, &sr);
}


// =========================================================
// 게임 함수 선언
// =========================================================
void initializeBag(SnakeMap* map, Bag* bag, Snake* snake);
void spawnFood(Bag* bag);

// 변경: logic은 꼬리 좌표와 새 머리 좌표를 출력 파라미터로 받고,
// 먹이 섭취 여부를 반환합니다.
bool logic(SnakeMap* map, Bag* bag, Snake* snake, Point& old_tail, Point& new_head);

void drawStaticMap(SnakeMap* map); // 테두리(벽) 등 고정 요소를 한 번만 그림
void drawDynamicUpdate(SnakeMap* map, Snake* snake, const Point& old_tail, bool ate); // 매 프레임 변경된 좌표만 업데이트
void input(Snake* snake);

int main() {
	// 사용자에게 맵 크기 입력 받기
	int map_cols = 0, map_rows = 0;
	cout << "맵 가로(cols)와 세로(rows)를 입력하세요 (예: 40 20). 최소 10x5 권장: ";
	if (!(cin >> map_cols >> map_rows) || map_cols <= 0 || map_rows <= 0) {
		// 입력 실패 시 기본값 사용
		map_cols = 80;
		map_rows = 25;
		cin.clear();
	}
	// 최소/최대 값 간단 제한
	if (map_cols < 10) map_cols = 10;
	if (map_rows < 5) map_rows = 5;

	// 1. 초기화
	CursorView();
	Screen screen(WINDOW_WIDTH, WINDOW_HEIGHT);

	// SnakeMap을 명시적 크기로 생성
	SnakeMap snakeMap(map_cols, map_rows);
	snakeMap.initMap(0); // 기본 맵 (테두리 벽)

	map_cols = snakeMap.getColumn();
	map_rows = snakeMap.getRow();

	// 콘솔 버퍼/윈도우 크기를 맵에 맞춰 조정합니다.
	// (콘솔이 맵보다 작으면 출력 중 스크롤이 발생합니다.)
	configureConsoleForMap(map_cols, map_rows);

	// 뱀 초기화 (맵 중앙 근처)
	Snake snake(map_cols / 4, map_rows / 2); // col, row 순서로 전달

	// 빈 공간을 관리할 Bag 초기화
	Bag emptySpaces;
	initializeBag(&snakeMap, &emptySpaces, &snake);

	// 첫 번째 먹이 생성
	spawnFood(&emptySpaces);

	// 게임 시작 대기
	system("cls");
	cout << "=== C++ Console Snake Game ===" << endl;
	cout << "Press any key to start..." << endl;
	_getch();

	// 시작 직후: 정적 맵(테두리) 한 번 그림, 그리고 초기 스네이크/먹이 그리기
	system("cls");
	drawStaticMap(&snakeMap);

	// 초기 스네이크와 먹이 화면에 출력
	const auto& init_body = snake.getBody();
	for (size_t i = 0; i < init_body.size(); ++i) {
		const Point& p = init_body[i];
		gotoXY(p.col, p.row);
		putchar(i == 0 ? SNAKE_HEAD_CHAR : SNAKE_BODY_CHAR);
	}
	if (food_pos.row != -1) {
		gotoXY(food_pos.col, food_pos.row);
		putchar(FOOD_CHAR);
	}
	gotoXY(0, map_rows);
	cout << "Score: " << score << " | Length: " << init_body.size() << "  ";

	// 2. 메인 게임 루프
	while (!gameOver) {
		input(&snake);

		Point old_tail;
		Point new_head;
		bool ate = logic(&snakeMap, &emptySpaces, &snake, old_tail, new_head);

		// 변경된 좌표만 업데이트
		drawDynamicUpdate(&snakeMap, &snake, old_tail, ate);

		Sleep(GAME_SPEED_MS);
	}

	// 3. 게임 오버
	gotoXY(map_cols / 2 - 10, map_rows / 2);
	cout << "!!! GAME OVER !!!" << endl;
	gotoXY(map_cols / 2 - 10, map_rows / 2 + 1);
	cout << "Final Score: " << score << endl;
	gotoXY(0, map_rows + 2); // 커서를 화면 밖으로 이동

	_getch();

	return 0;
}


// =========================================================
// 게임 함수 구현
// =========================================================

/**
 * 맵의 빈 공간 좌표들을 Bag에 추가하고, 뱀이 차지한 공간은 제거합니다.
 */
void initializeBag(SnakeMap* map, Bag* bag, Snake* snake) {
	int map_rows = map->getRow();
	int map_cols = map->getColumn();
	int** board = map->getBoard();

	// 맵 순회하며 벽이 아닌 칸(0)을 Bag에 추가
	for (int i = 0; i < map_rows; i++) { // i: row
		for (int j = 0; j < map_cols; j++) { // j: col
			if (board[i][j] == 0) { // 빈 공간
				bag->add({ i, j }); // Point: {row, col}
			}
		}
	}

	// 뱀의 초기 위치는 Bag에서 제거
	for (const auto& segment : snake->getBody()) {
		bag->remove(segment);
	}
}

/**
 * Bag에서 랜덤한 빈 공간을 먹이 위치로 설정합니다.
 * 이전 먹이 위치를 prev_food_pos에 저장하여 잔상 제거에 사용합니다.
 */
void spawnFood(Bag* bag) {
	if (bag->isEmpty()) {
		prev_food_pos = food_pos;
		food_pos = { -1, -1 }; // 더 이상 빈 공간이 없음
		gameOver = true; // 게임 클리어 처리도 가능
		return;
	}

	// 이전 먹이 위치 기록
	prev_food_pos = food_pos;
	// Bag의 getRandom() 함수는 랜덤 칸을 반환하고 Bag에서 제거합니다.
	food_pos = bag->getRandom();
}

/**
 * logic: 이동, 먹이 처리, Bag 업데이트, 충돌 검사
 * 반환값: 먹이 섭취 여부(ate)
 * out params:
 *   old_tail - 이동 전 꼬리 좌표 (꼬리 지우기용)
 *   new_head - 이동 후 머리 좌표 (디버깅/로깅 필요 시)
 */
bool logic(SnakeMap* map, Bag* bag, Snake* snake, Point& old_tail, Point& new_head) {
	// 1. 이동 전 꼬리 위치 저장 (Bag/화면 업데이트용)
	old_tail = snake->getBody().back();

	// 2. 뱀 이동
	snake->move();
	new_head = snake->getHead();

	// 3. 먹이 획득 여부 판단
	bool ate = (new_head == food_pos);

	// 4. Bag 업데이트 (꼬리 복구는 먹지 않았을 때만)
	if (!ate) {
		bag->add(old_tail);      // 꼬리가 비워진 칸은 빈 공간이 됩니다.
		bag->remove(new_head);   // 머리가 들어간 칸은 빈 공간에서 제거
	}
	else {
		// 먹이 섭취 처리
		snake->grow();
		score += 10;
		spawnFood(bag); // 새로운 먹이 생성 (Bag에서 제거됨)
	}

	// 5. 충돌 확인
	int** board = map->getBoard();

	if (new_head.row <= map->getTopIdx() || new_head.row >= map->getBottomIdx() ||
		new_head.col <= map->getLeftIdx() || new_head.col >= map->getRightIdx() ||
		board[new_head.row][new_head.col] == 1) {

		gameOver = true;
		return ate;
	}

	if (snake->checkSelfCollision()) {
		gameOver = true;
		return ate;
	}

	return ate;
}

/**
 * 정적 맵(테두리 등)을 한 번만 그립니다.
 */
void drawStaticMap(SnakeMap* map) {
	int map_rows = map->getRow();
	int map_cols = map->getColumn();
	int** board = map->getBoard();

	// 한 번에 라인 출력 (테두리 포함, 내부는 공백으로 둠)
	for (int i = 0; i < map_rows; ++i) {
		string line;
		line.reserve(map_cols);
		for (int j = 0; j < map_cols; ++j) {
			if (board[i][j] == 1) line.push_back(MAP_CHAR);
			else line.push_back(' ');
		}
		gotoXY(0, i);
		cout << line;
	}
}

/**
 * 동적 업데이트: 매 프레임 변경된 좌표만 갱신
 * - 새 머리: SNAKE_HEAD_CHAR
 * - 새 두번째 세그먼트(머리가 이동하면서 그 자리가 몸이 됨): SNAKE_BODY_CHAR
 * - (먹지 않았으면) 이전 꼬리 위치: ' '로 지움
 * - 먹이 위치: FOOD_CHAR (존재하면 항상 그려줌)
 * - prev_food_pos를 이용해 이전 먹이 잔상 제거
 */
void drawDynamicUpdate(SnakeMap* map, Snake* snake, const Point& old_tail, bool ate) {
	int map_rows = map->getRow();

	const auto& body = snake->getBody();
	if (body.empty()) return;

	// 1) 새 머리 그리기
	const Point& head = body[0];
	gotoXY(head.col, head.row);
	putchar(SNAKE_HEAD_CHAR);

	// 2) 머리가 이동하기 전 자리는 이제 몸이 되므로 두번째 세그먼트 위치를 그립니다.
	if (body.size() >= 2) {
		const Point& second = body[1];
		gotoXY(second.col, second.row);
		putchar(SNAKE_BODY_CHAR);
	}

	// 3) 꼬리 지우기 (먹었으면 꼬리는 유지되므로 지우지 않음)
	if (!ate) {
		gotoXY(old_tail.col, old_tail.row);
		putchar(' ');
	}
	else {
		// 먹었으면 꼬리는 몸으로 남음 -> 이미 snake.grow()로 몸 길이가 늘어난 상태이므로
		// 이전 꼬리를 몸 문자로 덮어씌우는 것이 안전
		gotoXY(old_tail.col, old_tail.row);
		putchar(SNAKE_BODY_CHAR);
	}

	// 4) 이전 먹이 위치가 있고 (그리고 현재 뱀이 차지하지 않는다면) 지운다 — 잔상 제거
	if (prev_food_pos.row != -1) {
		bool occupiedBySnake = false;
		for (const auto& p : body) {
			if (p == prev_food_pos) { occupiedBySnake = true; break; }
		}
		// 벽이면 지우지 않음(벽은 drawStaticMap에서 그려짐)
		int** board = map->getBoard();
		if (!occupiedBySnake && board[prev_food_pos.row][prev_food_pos.col] == 0) {
			gotoXY(prev_food_pos.col, prev_food_pos.row);
			putchar(' ');
		}
		// reset prev_food_pos so we don't repeatedly clear
		prev_food_pos = { -1, -1 };
	}

	// 5) 먹이 그리기 (존재하면 위치에 그려줌)
	if (food_pos.row != -1) {
		gotoXY(food_pos.col, food_pos.row);
		putchar(FOOD_CHAR);
	}

	// 6) 상태 라인 갱신
	gotoXY(0, map_rows);
	cout << "Score: " << score << " | Length: " << body.size() << "  ";
}


/**
 * 키 입력을 처리합니다.
 */
void input(Snake* snake) {
	if (_kbhit()) {
		char key = _getch();
		switch (key) {
		case 'w': case 'W': snake->changeDirection(UP); break;
		case 's': case 'S': snake->changeDirection(DOWN); break;
		case 'a': case 'A': snake->changeDirection(LEFT); break;
		case 'd': case 'D': snake->changeDirection(RIGHT); break;
		case 'x': case 'X': gameOver = true; break;
		}
	}
}