#pragma once
#include <deque>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "Point.h"

class SnakeQueue
{
private:
    std::deque<Point> dq;
    size_t capacity = 0; // 0 == unlimited

public:
    explicit SnakeQueue(size_t cap = 0) : capacity(cap) {}

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
};