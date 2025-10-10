#include <iostream>
#include <vector>
#include <algorithm> // find() 사용
#include <cstdlib>   // rand() 사용
#include "Point.h"

class Bag
{
private:
    std::vector<Point> contents;

public:
    Bag()
    {
        // srand는 프로그램 시작(main)에서 한 번만 호출하세요
    }

    // 뱀 꼬리가 떠난칸 Bag에 추가
    void add(const Point &p)
    {
        contents.push_back(p);
    }

    // 뱀 머리가 들어갈 칸 Bag에서 제거
    void remove(const Point &p)
    {
        auto it = std::find(contents.begin(), contents.end(), p);
        if (it != contents.end())
        {
            *it = contents.back();
            contents.pop_back();
        }
    }

    // Bag에서 랜덤 칸 반환 후 제거(먹이 표시할것)
    Point getRandom()
    {
        if (contents.empty())
            return {-1, -1};
        int index = rand() % static_cast<int>(contents.size());
        Point selected_point = contents[index];
        if (index != static_cast<int>(contents.size()) - 1)
            contents[index] = contents.back();
        contents.pop_back();
        return selected_point;
    }

    bool isEmpty() const { return contents.empty(); }
    void clear() { contents.clear(); }

    void fillAll(int rows, int cols, const std::vector<Point> &exclude = {})
    {
        contents.clear();
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                Point p{r, c};
                if (std::find(exclude.begin(), exclude.end(), p) == exclude.end())
                {
                    contents.push_back(p);
                }
            }
        }
    }
};

int main()
{
    return 0;
}