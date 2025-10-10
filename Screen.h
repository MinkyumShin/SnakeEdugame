#pragma once
#include<windows.h>
#include<string>
#include<vector>

// 1280x720, 1366x768, 1600x900, 1920x1080, 2560x1440, 3840x2160, 5120x2880, 7680x4320
class Screen
{
public:
	Screen(int w, int h) {
		this->width = w;
		this->height = h;
		if (!this->width != 0 && this->height != 0)
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

// SnakeQueue
class SnakeQueue
{
public:
	bool empty() const noexcept { return dq.empty(); }
	size_t size() const noexcept { return dq.size(); }

	// 새 머리를 앞으로 추가
	void pushHead(const Point& p) {
		if (capacity && dq.size() >= capacity) {
			throw std::overflow_error("SnakeQueue: capacity exceeded");
		}
		dq.push_front(p);
	}

	// 꼬리 제거
	void popTail() {
		if (dq.empty()) throw std::underflow_error("SnakeQueue: empty");
		dq.pop_back();
	}

	// 현재 머리(가장 앞)
	const Point& head() const {
		if (dq.empty()) throw std::underflow_error("SnakeQueue: empty");
		return dq.front();
	}

	// 현재 꼬리(가장 뒤)
	const Point& tail() const {
		if (dq.empty()) throw std::underflow_error("SnakeQueue: empty");
		return dq.back();
	}

	bool contains(const Point& p) const {
		return std::find(dq.begin(), dq.end(), p) != dq.end();
	}

	void clear() noexcept { dq.clear(); }

	std::vector<Point> toVector() const { return std::vector<Point>(dq.begin(), dq.end()); }
	void fromVector(const std::vector<Point>& v) { dq.assign(v.begin(), v.end()); }
private:
	std::deque<Point> dq;
	int capacity;
};
