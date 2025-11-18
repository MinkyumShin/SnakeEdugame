// Cleaned up Bag.h to remove merge markers and use Point.h
#ifndef BAG_H
#define BAG_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "Point.h"
#include "DSInfoManager.h"

class Bag
{
private:
    std::vector<Point> contents;
    bool initializing = false;

public:
    Bag() { std::srand(static_cast<unsigned int>(std::time(nullptr))); }

    void beginInit() { initializing = true; }
    void endInit() { initializing = false; }

    // 뱀 꼬리가 떠난칸 Bag에 추가
    void add(const Point &p)
    {
        contents.push_back(p);
        if (!initializing)
        {
            g_dsInfo.logBagMove("Insert empty cell: (" + std::to_string(p.row) + "," + std::to_string(p.col) + ")");
        }
    }

    // 뱀 머리가 들어갈 칸 Bag에서 제거
    void remove(const Point &p)
    {
        auto it = std::find(contents.begin(), contents.end(), p);
        if (it != contents.end())
        {
            *it = contents.back();
            contents.pop_back();
            g_dsInfo.logBagMove("Remove cell: (" + std::to_string(p.row) + "," + std::to_string(p.col) + ")");
        }
    }

    // Bag에서 랜덤 칸 반환 후 제거(먹이 표시할것)
    Point getRandom()
    {
        if (contents.empty())
            return {-1, -1};
        int index = std::rand() % contents.size();
        Point selected_point = contents[index];
        if (index != static_cast<int>(contents.size()) - 1)
        {
            contents[index] = contents.back();
        }
        contents.pop_back();
        g_dsInfo.logBag("Random pick: (" + std::to_string(selected_point.row) + "," + std::to_string(selected_point.col) + ")");
        return selected_point;
    }

    bool isEmpty() const { return contents.empty(); }

    // --- 복원/저장을 위한 접근자 추가 ---
    std::vector<Point> getContents() const { return contents; }
    void setContents(const std::vector<Point> &v) { contents = v; }
};

#endif // BAG_H
