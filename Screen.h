#pragma once
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <stdexcept>
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdio>
#endif
#include "Bag.h" // for Point used in SnakeQueue

// 1280x720, 1366x768, 1600x900, 1920x1080, 2560x1440, 3840x2160, 5120x2880, 7680x4320
class Screen
{
public:
	Screen(int w, int h)
	{
		this->width = w;
		this->height = h;

#if defined(_WIN32)
		if (this->width != 0 && this->height != 0)
			this->setScreen();
#endif
		this->updatShape();
	}

	int getColSize() const
	{
		return this->colSize;
	}

	int getRowSize() const
	{
		return this->rowSize;
	}

private:
	int width = 0;
	int height = 0;
	int colSize = 0;
	int rowSize = 0;

	// Windows: resize window
#if defined(_WIN32)
	void setScreen()
	{
		HWND hwnd = GetConsoleWindow();
		if (hwnd != NULL)
		{
			SetWindowPos(hwnd, 0, 0, 0, this->width, this->height, SWP_SHOWWINDOW | SWP_NOMOVE);
		}
	}
#endif

	void updatShape()
	{
#if defined(_WIN32)
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		int columns, rows;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		this->colSize = columns;
		this->rowSize = rows;
#else
		// Try to get terminal size; fallback to width/height or 80x25
		struct winsize ws{};
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0 && ws.ws_row > 0)
		{
			this->colSize = ws.ws_col;
			this->rowSize = ws.ws_row;
		}
		else
		{
			this->colSize = (this->width > 0 ? this->width : 80);
			this->rowSize = (this->height > 0 ? this->height : 25);
		}
#endif
	}
};

// SnakeQueue
class SnakeQueue
{
public:
	bool empty() const noexcept { return dq.empty(); }
	size_t size() const noexcept { return dq.size(); }

	// 새 머리를 앞으로 추가
	void pushHead(const Point &p)
	{
		if (capacity && dq.size() >= capacity)
		{
			throw std::overflow_error("SnakeQueue: capacity exceeded");
		}
		dq.push_front(p);
	}

	// 꼬리 제거
	void popTail()
	{
		if (dq.empty())
			throw std::underflow_error("SnakeQueue: empty");
		dq.pop_back();
	}

	// 현재 머리(가장 앞)
	const Point &head() const
	{
		if (dq.empty())
			throw std::underflow_error("SnakeQueue: empty");
		return dq.front();
	}

	// 현재 꼬리(가장 뒤)
	const Point &tail() const
	{
		if (dq.empty())
			throw std::underflow_error("SnakeQueue: empty");
		return dq.back();
	}

	bool contains(const Point &p) const
	{
		return std::find(dq.begin(), dq.end(), p) != dq.end();
	}

	void clear() noexcept { dq.clear(); }

	std::vector<Point> toVector() const { return std::vector<Point>(dq.begin(), dq.end()); }
	void fromVector(const std::vector<Point> &v) { dq.assign(v.begin(), v.end()); }

private:
	std::deque<Point> dq;
	int capacity = 0;
};
