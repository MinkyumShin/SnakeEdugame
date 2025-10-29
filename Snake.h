#pragma once
#include <vector>
#include <algorithm>
#include "Point.h"

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake
{
private:
    std::vector<Point> body;
    Direction current_dir;
    bool grow_flag;

public:
    // start at (col, row)
    Snake(int start_col, int start_row) : current_dir(RIGHT), grow_flag(false)
    {
        body.push_back({start_row, start_col});
        body.push_back({start_row, start_col - 1});
        body.push_back({start_row, start_col - 2});
    }

    void changeDirection(Direction new_dir)
    {
        if ((current_dir == UP && new_dir == DOWN) ||
            (current_dir == DOWN && new_dir == UP) ||
            (current_dir == LEFT && new_dir == RIGHT) ||
            (current_dir == RIGHT && new_dir == LEFT))
        {
            return;
        }
        current_dir = new_dir;
    }

    void move()
    {
        Point new_head = body.front();
        switch (current_dir)
        {
        case UP:
            new_head.row--;
            break;
        case DOWN:
            new_head.row++;
            break;
        case LEFT:
            new_head.col--;
            break;
        case RIGHT:
            new_head.col++;
            break;
        }
        body.insert(body.begin(), new_head);
        if (!grow_flag)
        {
            body.pop_back();
        }
        else
        {
            grow_flag = false;
        }
    }

    void grow() { grow_flag = true; }

    Point getHead() const { return body.front(); }
    const std::vector<Point> &getBody() const { return body; }
    Direction getDirection() const { return current_dir; }

    bool checkSelfCollision() const
    {
        const Point &head = body.front();
        for (size_t i = 1; i < body.size(); ++i)
        {
            if (head == body[i])
                return true;
        }
        return false;
    }
};
