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
#include "UndoItem.h" // undo 기능 (선택 브랜치: stack_undoitem_ver3)

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const char MAP_CHAR = '#';
const char SNAKE_HEAD_CHAR = '@';
const char SNAKE_BODY_CHAR = 'o';
const char FOOD_CHAR = '$';
const char R_CHAR = 'R';			  // R 아이템 문자
static const int GAME_SPEED_MS = 100; // 단순 고정 속도 (diff 브랜치 난이도 기능 제거)
int currentDirection = RIGHT;		  // 현재 방향 추적 (방향 표시만 유지)
bool gameOver = false;
int score = 0;
Point food_pos = {-1, -1};		// 현재 먹이 위치 (row, col)
Point prev_food_pos = {-1, -1}; // 이전 프레임 먹이 위치 (잔상 제거용)

// R 아이템 관련
Point r_item_pos = {-1, -1};
Point prev_r_pos = {-1, -1};

// undo 요청 플래그 (입력에서 설정)
bool undo_request = false;

// 먹이 섭취 카운터: 2개 먹을 때마다 R 생성
int foods_eaten_count = 0;

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
void spawnR(Bag *bag); // R 아이템 생성 함수

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

	// (초기에는 R 자동 생성하지 않음 — 이제 먹이를 3개 먹을 때마다 생성)
	// spawnR(&emptySpaces);

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
	if (r_item_pos.row != -1)
	{
		gotoXY(r_item_pos.col, r_item_pos.row);
		putchar(R_CHAR);
	}
	gotoXY(0, map_rows);
	cout << "Score: " << score << " | Length: " << init_body.size() << " | Undo: " << undo_item_count << "  ";

	while (!gameOver)
	{
		input(&snake);

		Point old_tail = snake.getBody().back();
		Point new_head;
		bool ate = logic(&snakeMap, &emptySpaces, &snake, old_tail, new_head);

		drawDynamicUpdate(&snakeMap, &snake, old_tail, ate);

		// undo 처리 (간단 버전)
		if (undo_request)
		{
			GameState restored;
			if (use_undo_item(restored))
			{
				snake.setBody(restored.snake_body);
				snake.setDirection(static_cast<Direction>(restored.dir));
				score = restored.score;
				food_pos = restored.food_pos;
				r_item_pos = {-1, -1};
				prev_r_pos = {-1, -1};
				emptySpaces.setContents(restored.bag_contents);
				// 전체 리프레시
				clearScreen();
				drawStaticMap(&snakeMap);
				const auto &body = snake.getBody();
				for (size_t i = 0; i < body.size(); ++i)
				{
					const Point &p = body[i];
					gotoXY(p.col, p.row);
					putchar(i == 0 ? SNAKE_HEAD_CHAR : SNAKE_BODY_CHAR);
				}
				if (food_pos.row != -1)
				{
					gotoXY(food_pos.col, food_pos.row);
					putchar(FOOD_CHAR);
				}
				gotoXY(0, snakeMap.getRow());
				cout << "Score: " << score << " | Length: " << body.size() << " | Undo: " << undo_item_count << "  ";
			}
			undo_request = false;
		}

		sleep_ms(GAME_SPEED_MS); // 고정 속도 딜레이
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

// 맵의 빈 칸을 Bag에 채우고, 뱀의 현재 위치는 제거
void initializeBag(SnakeMap *map, Bag *bag, Snake *snake)
{
	int map_rows = map->getRow();
	int map_cols = map->getColumn();
	int **board = map->getBoard();

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

	for (const auto &seg : snake->getBody())
	{
		bag->remove(seg);
	}
}

// 먹이 생성
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

// R 아이템 생성
void spawnR(Bag *bag)
{
	if (bag->isEmpty())
	{
		prev_r_pos = r_item_pos;
		r_item_pos = {-1, -1};
		return;
	}
	prev_r_pos = {-1, -1};
	r_item_pos = bag->getRandom();
}
bool logic(SnakeMap *map, Bag *bag, Snake *snake, Point &old_tail, Point &new_head)
{
	old_tail = snake->getBody().back();
	snake->move();
	new_head = snake->getHead();

	bool ate = (new_head == food_pos);
	bool ateR = (new_head == r_item_pos);

	// bag 업데이트
	if (!ate && !ateR)
	{
		bag->add(old_tail);
		bag->remove(new_head);
	}
	else if (ate)
	{
		// 먹이 섭취 처리
		snake->grow();
		score += 10;
		spawnFood(bag); // 새로운 먹이 생성

		// 먹이 카운트 증가 -> 2개 먹으면 R 생성
		++foods_eaten_count;
		if (foods_eaten_count >= 2)
		{
			spawnR(bag);
			foods_eaten_count = 0;
		}
	}
	else if (ateR)
	{
		// R 아이템 섭취: 꼬리/머리 처리(성장X), 그 상황을 스택에 저장
		bag->add(old_tail);
		bag->remove(new_head);

		// 상태 저장 (먹은 상황 저장)
		GameState gs;
		gs.snake_body = snake->getBody();
		gs.dir = static_cast<int>(snake->getDirection());
		gs.score = score;
		gs.food_pos = food_pos;
		gs.r_pos = r_item_pos;
		gs.bag_contents = bag->getContents();
		save_state(gs);
		++undo_item_count;

		// R을 즉시 재생성하지 않음 — 먹이를 2개 먹을 때 생성되도록 변경
		r_item_pos = {-1, -1};
		prev_r_pos = {-1, -1};
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
	// R 아이템 그리기
	if (r_item_pos.row != -1)
	{
		gotoXY(r_item_pos.col, r_item_pos.row);
		putchar(R_CHAR);
	}
	// 이전 R 잔상 제거
	if (prev_r_pos.row != -1 && prev_r_pos != r_item_pos)
	{
		gotoXY(prev_r_pos.col, prev_r_pos.row);
		putchar(' ');
		prev_r_pos = {-1, -1};
	}

	// 상태 라인 갱신
	gotoXY(0, map_rows);
	cout << "Score: " << score << " | Length: " << body.size() << " | Undo: " << undo_item_count << "  ";
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
			currentDirection = UP;
			break;
		case 's':
		case 'S':
			snake->changeDirection(DOWN);
			currentDirection = DOWN;
			break;
		case 'a':
		case 'A':
			snake->changeDirection(LEFT);
			currentDirection = LEFT;
			break;
		case 'd':
		case 'D':
			snake->changeDirection(RIGHT);
			currentDirection = RIGHT;
			break;
		case 'x':
		case 'X':
			gameOver = true;
			break;
		case 'r':
		case 'R':
			undo_request = true;
			break;
		}
	}
}
