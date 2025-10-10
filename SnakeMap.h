#pragma once
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

		this->adjustBoard();
	}

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
		switch (map_diffi)
		{
		case 2:
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < columns; j++)
					if (i == 0 || i == (rows - 1) || j == (columns / 2)) // set wall top, bottom
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
	int** board;
	Screen* screen = NULL;
	int columns = 0, rows = 0;
	int top_idx = 0, left_inx = 0, bottom_idx, right_idx;

	void adjustBoard() {
		this->board = new int* [rows];
		for (int i = 0; i < rows; i++)
			this->board[i] = new int[columns] {0};
	}

};
