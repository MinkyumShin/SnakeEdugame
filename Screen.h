#pragma once
#include <windows.h>
#include <string>

// 1280x720, 1366x768, 1600x900, 1920x1080, 2560x1440, 3840x2160, 5120x2880, 7680x4320
class Screen
{
public:
	Screen(int w, int h) {
		this->width = w;
		this->height = h;
		// 원래 코드에 있던 잘못된 조건식을 고쳤습니다.
		if (this->width != 0 && this->height != 0)
			this->setScreen();
		this->updatShape();
	}

	int getColSize() const {
		return this->colSize;
	}

	int getRowSize() const {
		return this->rowSize;
	}

private:
	int width = 0;
	int height = 0;
	int colSize = 0;
	int rowSize = 0;

	void setScreen() {
		HWND hwnd = GetConsoleWindow();
		if (hwnd != NULL) { SetWindowPos(hwnd, 0, 0, 0, this->width, this->height, SWP_SHOWWINDOW | SWP_NOMOVE); }
	}

	void updatShape() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		int columns, rows;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		this->colSize = columns;
		this->rowSize = rows;
	}
};
