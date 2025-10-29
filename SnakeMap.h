#pragma once
<<<<<<< HEAD
#include <string>
#include "Screen.h"

#define MAP_WALL 1

class SnakeMap
{
public:
	// 기존 Screen 기반 생성자 유지
	SnakeMap(Screen* screen) {
		this->screen = screen;
		columns = this->screen->getColSize();
		rows = this->screen->getRowSize();
		this->right_idx = columns - 1;
		this->bottom_idx = rows - 1;
=======
#include<string>
#include "Screen.h"

#define MAP_WALL 1;
class SnakeMap
{
public:
	SnakeMap(Screen* screen) {
		this->screen = screen;
		columns = this->screen->getColSize(), rows = this->screen->getRowSize();
		this->right_idx = columns - 1;
		this->bottom_idx= rows - 1;
>>>>>>> 61c3e1c4a5fe5db3634e4383b3a9936349c9ab1f

		this->adjustBoard();
	}

<<<<<<< HEAD
	// 신규: 명시적 컬럼/로우로 맵 크기 지정
	SnakeMap(int columns, int rows) {
		if (columns < 3) columns = 3;
		if (rows < 3) rows = 3;
		this->screen = nullptr;
		this->columns = columns;
		this->rows = rows;
		this->right_idx = columns - 1;
		this->bottom_idx = rows - 1;

		this->adjustBoard();
	}

	~SnakeMap() {
		if (board) {
			for (int i = 0; i < rows; ++i) {
				delete[] board[i];
			}
			delete[] board;
			board = nullptr;
		}
	}

=======
>>>>>>> 61c3e1c4a5fe5db3634e4383b3a9936349c9ab1f
	Screen* getScreen() const {
		return this->screen;
	}

	int getColumn() const {
		return this->columns;
	}

	int getRow() const {
		return this->rows;
	}

	int getTopIdx() const {
		return this->top_idx;
	}

	int getLeftIdx() const {
		return this->left_inx;
	}

	int getBottomIdx() const {
		return this->bottom_idx;
	}

	int getRightIdx() const {
		return this->right_idx;
	}

	int** getBoard() const {
		return this->board;
	}

	void initMap(int map_diffi) {
<<<<<<< HEAD
		// 기본은 0으로 초기화되며, 벽(1)을 설정
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
				this->board[i][j] = 0;

=======
>>>>>>> 61c3e1c4a5fe5db3634e4383b3a9936349c9ab1f
		switch (map_diffi)
		{
		case 2:
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < columns; j++)
<<<<<<< HEAD
					if (i == 0 || i == (rows - 1) || j == (columns / 2)) // set wall top, bottom, middle column
=======
					if (i == 0 || i == (rows - 1) || j == (columns / 2)) // set wall top, bottom
>>>>>>> 61c3e1c4a5fe5db3634e4383b3a9936349c9ab1f
						this->board[i][j] = MAP_WALL;
			break;
		case 3:
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < columns; j++)
					if ((i == 0 && j < columns - 10) || i == (rows / 2) || j == 0 || j == (columns / 2) || (i == (rows - 1) && j < columns - 10))
						this->board[i][j] = MAP_WALL;
			break;
		default:
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < columns; j++)
					if (i == 0 || j == 0 || i == (rows - 1) || j == (columns - 1))
						this->board[i][j] = MAP_WALL; // set wall top, left, right, bottom
		}
	}
private:
<<<<<<< HEAD
	int** board = nullptr;
	Screen* screen = NULL;
	int columns = 0;
	int rows = 0;
	int top_idx = 0;
	int left_inx = 0;
	int bottom_idx = 0;
	int right_idx = 0;
=======
	int** board;
	Screen* screen = NULL;
	int columns = 0, rows = 0;
	int top_idx = 0, left_inx = 0, bottom_idx, right_idx;
>>>>>>> 61c3e1c4a5fe5db3634e4383b3a9936349c9ab1f

	void adjustBoard() {
		this->board = new int* [rows];
		for (int i = 0; i < rows; i++)
			this->board[i] = new int[columns] {0};
	}

};
