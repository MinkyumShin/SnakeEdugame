#include <iostream>
#include <algorithm> // min, max
#include <cstdio>	 // putchar
#include <limits>
#if defined(_WIN32)
#include <conio.h>	 // _kbhit, _getch
#include <windows.h> // Sleep, Console functions
#else
#include <unistd.h> // usleep
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#endif
#include "Screen.h"
#include "SnakeMap.h"
#include "Bag.h"
#include "Snake.h"

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const char MAP_CHAR = '#';
const char SNAKE_HEAD_CHAR = '@';
const char SNAKE_BODY_CHAR = 'o';
const char FOOD_CHAR = '$';
const int GAME_SPEED_MS = 100; // 게임 속도 (100ms마다 업데이트)

bool gameOver = false;
int score = 0;
Point food_pos = {-1, -1};		// 현재 먹이 위치 (row, col)
Point prev_food_pos = {-1, -1}; // 이전 프레임 먹이 위치 (잔상 제거용)

// 콘솔 커서 위치를 이동시키는 함수
void gotoXY(int x, int y)
{
#if defined(_WIN32)
	COORD coord;
	coord.X = x; // col
	coord.Y = y; // row
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
	// ANSI escape: 1-based row;col
	std::printf("\033[%d;%dH", y + 1, x + 1);
	std::fflush(stdout);
#endif
}

void CursorView()
{

#if defined(_WIN32)
	CONSOLE_CURSOR_INFO cursorInfo = {
		0,
	};
	cursorInfo.dwSize = 1;		 // 커서 크기 (1~100)
	cursorInfo.bVisible = FALSE; // 커서 숨김
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
#else
	// Hide cursor using ANSI
	std::printf("\033[?25l");
	std::fflush(stdout);
#endif
}

// 콘솔 버퍼/윈도우 크기를 맵 크기에 맞춰 설정
void configureConsoleForMap(int cols, int rows)
{
#if defined(_WIN32)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
		return;

	COORD largest = GetLargestConsoleWindowSize(hOut);
	int winCols = min(cols, (int)largest.X);
	int winRows = min(rows + 5, (int)largest.Y); // 상태 라인 여유 공간 추가

	COORD bufferSize;
	bufferSize.X = max(cols, winCols);
	bufferSize.Y = max(rows + 5, winRows);
	SetConsoleScreenBufferSize(hOut, bufferSize);

	SMALL_RECT sr;
	sr.Left = 0;
	sr.Top = 0;
	sr.Right = winCols - 1;
	sr.Bottom = winRows - 1;
	SetConsoleWindowInfo(hOut, TRUE, &sr);
#else
	(void)cols;
	(void)rows; // no-op on Linux terminal
#endif
}

#if !defined(_WIN32)
// --- Linux replacements for _kbhit and _getch ---
static struct termios orig_termios;
static bool termios_initialized = false;

static void disableRawMode()
{
	if (termios_initialized)
	{
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
		termios_initialized = false;
		// show cursor back
		std::printf("\033[?25h");
	}
}

static void enableRawMode()
{
	if (!termios_initialized)
	{
		tcgetattr(STDIN_FILENO, &orig_termios);
		atexit(disableRawMode);
		struct termios raw = orig_termios;
		raw.c_lflag &= ~(ECHO | ICANON);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
		termios_initialized = true;
	}
}

static int _kbhit()
{
	timeval tv{0, 0};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
}

static int _getch()
{
	unsigned char c = 0;
	if (read(STDIN_FILENO, &c, 1) == 1)
		return c;
	return 0;
}

static void clearScreen()
{
	std::printf("\033[2J\033[H");
}

static void sleep_ms(int ms)
{
	usleep(ms * 1000);
}
#else
static void clearScreen() { system("cls"); }
static void sleep_ms(int ms) { Sleep(ms); }
#endif

void initializeBag(SnakeMap *map, Bag *bag, Snake *snake);
void spawnFood(Bag *bag);

bool logic(SnakeMap *map, Bag *bag, Snake *snake, Point &old_tail, Point &new_head);

void drawStaticMap(SnakeMap *map);
void drawDynamicUpdate(SnakeMap *map, Snake *snake, const Point &old_tail, bool ate);
void input(Snake *snake);

int main()
{
	// 사용자에게 맵 크기 입력 받기
	int map_cols = 0, map_rows = 0;
	cout << "맵 가로(cols)와 세로(rows)를 입력하세요 (예: 40 20). 최소 10x5 권장: ";
	if (!(cin >> map_cols >> map_rows) || map_cols <= 0 || map_rows <= 0)
	{
		map_cols = 80;
		map_rows = 25;
		cin.clear();
	}

	if (map_cols < 10)
		map_cols = 10;
	if (map_rows < 5)
		map_rows = 5;

#if !defined(_WIN32)
	enableRawMode();
#endif
	CursorView();
	Screen screen(WINDOW_WIDTH, WINDOW_HEIGHT);
	SnakeMap snakeMap(map_cols, map_rows);
	snakeMap.initMap(0);

	map_cols = snakeMap.getColumn();
	map_rows = snakeMap.getRow();

	configureConsoleForMap(map_cols, map_rows);

	// 뱀 초기화 (맵 중앙 근처)
	Snake snake(map_cols / 4, map_rows / 2);

	// Bag 초기화
	Bag emptySpaces;
	initializeBag(&snakeMap, &emptySpaces, &snake);

	// 첫 번째 먹이 생성
	spawnFood(&emptySpaces);

	clearScreen();
	cout << "=== C++ Console Snake Game ===" << endl;
	cout << "Press any key to start..." << endl;
	_getch();

	// 시작 직후: 맵 한 번 그림, 그리고 초기 스네이크/먹이 그리기
	clearScreen();
	drawStaticMap(&snakeMap);

	const auto &init_body = snake.getBody();
	for (size_t i = 0; i < init_body.size(); ++i)
	{
		const Point &p = init_body[i];
		gotoXY(p.col, p.row);
		putchar(i == 0 ? SNAKE_HEAD_CHAR : SNAKE_BODY_CHAR);
	}
	if (food_pos.row != -1)
	{
		gotoXY(food_pos.col, food_pos.row);
		putchar(FOOD_CHAR);
	}
	gotoXY(0, map_rows);
	cout << "Score: " << score << " | Length: " << init_body.size() << "  ";

	while (!gameOver)
	{
		input(&snake);

		Point old_tail;
		Point new_head;
		bool ate = logic(&snakeMap, &emptySpaces, &snake, old_tail, new_head);

		drawDynamicUpdate(&snakeMap, &snake, old_tail, ate);

		sleep_ms(GAME_SPEED_MS);
	}

	// 게임 오버
	gotoXY(map_cols / 2 - 10, map_rows / 2);
	cout << "!!! GAME OVER !!!" << endl;
	gotoXY(map_cols / 2 - 10, map_rows / 2 + 1);
	cout << "Final Score: " << score << endl;
	gotoXY(0, map_rows + 2);

	_getch();

	return 0;
}

// 맵의 빈 공간 좌표 Bag에 추가, 뱀이 차지한 공간 제거
void initializeBag(SnakeMap *map, Bag *bag, Snake *snake)
{
	int map_rows = map->getRow();
	int map_cols = map->getColumn();
	int **board = map->getBoard();

	// 맵 순회하며 벽이 아닌 칸(0)을 Bag에 추가
	for (int i = 0; i < map_rows; i++)
	{
		for (int j = 0; j < map_cols; j++)
		{
			if (board[i][j] == 0)
			{
				bag->add({i, j});
			}
		}
	}

	// 뱀의 초기 위치는 Bag에서 제거
	for (const auto &segment : snake->getBody())
	{
		bag->remove(segment);
	}
}

void spawnFood(Bag *bag)
{
	if (bag->isEmpty())
	{
		prev_food_pos = food_pos;
		food_pos = {-1, -1};
		gameOver = true;
		return;
	}

	prev_food_pos = {-1, -1};
	food_pos = bag->getRandom();
}

// old_tail - 이동 전 꼬리 좌표 (꼬리 지우기용)
// new_head - 이동 후 머리 좌표

bool logic(SnakeMap *map, Bag *bag, Snake *snake, Point &old_tail, Point &new_head)
{
	old_tail = snake->getBody().back();
	snake->move();
	new_head = snake->getHead();

	bool ate = (new_head == food_pos);

	// bag 업데이트
	if (!ate)
	{
		bag->add(old_tail);
		bag->remove(new_head);
	}
	else
	{
		// 먹이 섭취 처리
		snake->grow();
		score += 10;
		spawnFood(bag); // 새로운 먹이 생성
	}

	int **board = map->getBoard();

	if (new_head.row <= map->getTopIdx() || new_head.row >= map->getBottomIdx() ||
		new_head.col <= map->getLeftIdx() || new_head.col >= map->getRightIdx() ||
		board[new_head.row][new_head.col] == 1)
	{

		gameOver = true;
		return ate;
	}

	if (snake->checkSelfCollision())
	{
		gameOver = true;
		return ate;
	}

	return ate;
}

// 맵 한번만 그리기
void drawStaticMap(SnakeMap *map)
{
	int map_rows = map->getRow();
	int map_cols = map->getColumn();
	int **board = map->getBoard();

	for (int i = 0; i < map_rows; ++i)
	{
		string line;
		line.reserve(map_cols);
		for (int j = 0; j < map_cols; ++j)
		{
			if (board[i][j] == 1)
				line.push_back(MAP_CHAR);
			else
				line.push_back(' ');
		}
		gotoXY(0, i);
		cout << line;
	}
}

// 동적 업데이트: 매 프레임 변경된 좌표만 갱신
// 새 머리: SNAKE_HEAD_CHAR
//(먹지 않았으면) 이전 꼬리 위치: ' '로 지움
// 먹이 위치: FOOD_CHAR (존재하면 항상 그려줌)
// prev_food_pos를 이용해 이전 먹이 잔상 제거

void drawDynamicUpdate(SnakeMap *map, Snake *snake, const Point &old_tail, bool ate)
{
	int map_rows = map->getRow();

	const auto &body = snake->getBody();
	if (body.empty())
		return;

	// 새 머리 그리기
	const Point &head = body[0];
	gotoXY(head.col, head.row);
	putchar(SNAKE_HEAD_CHAR);

	// 머리가 이동하기 전 자리는 이제 몸이 되므로 두번째 세그먼트 그리기
	if (body.size() >= 2)
	{
		const Point &second = body[1];
		gotoXY(second.col, second.row);
		putchar(SNAKE_BODY_CHAR);
	}

	// 꼬리 지우기
	gotoXY(old_tail.col, old_tail.row);
	putchar(' ');

	// 먹이 그리기 (존재하면 위치에 그려줌)
	if (food_pos.row != -1)
	{
		gotoXY(food_pos.col, food_pos.row);
		putchar(FOOD_CHAR);
	}

	// 상태 라인 갱신
	gotoXY(0, map_rows);
	cout << "Score: " << score << " | Length: " << body.size() << "  ";
}

void input(Snake *snake)
{
	if (_kbhit())
	{
		char key = _getch();
		switch (key)
		{
		case 'w':
		case 'W':
			snake->changeDirection(UP);
			break;
		case 's':
		case 'S':
			snake->changeDirection(DOWN);
			break;
		case 'a':
		case 'A':
			snake->changeDirection(LEFT);
			break;
		case 'd':
		case 'D':
			snake->changeDirection(RIGHT);
			break;
		case 'x':
		case 'X':
			gameOver = true;
			break;
		}
	}
}
