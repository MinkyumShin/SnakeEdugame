#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <algorithm> // find�� ����ϱ� ���� ����
#include "Bag.h" // Point ����ü�� ����ϱ� ���� �����մϴ�.

// ���� �̵� ������ ��Ÿ���� ������
enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake {
private:
    std::vector<Point> body;
    Direction current_dir;
    bool grow_flag; // ���̸� �Ծ����� üũ�ϴ� �÷���

public:
    // ������ ���� (�ζ���)
    Snake(int start_col, int start_row) : current_dir(RIGHT), grow_flag(false) {
        // �ʱ� �� ���� 3 (Point�� {row, col} ��� {col, row}�� ����Ѵٰ� ����)
        // Bag.h�� Point�� {row, col}������, �Ϲ����� x(col), y(row) ������ �ʱ�ȭ�մϴ�.
        // main.cpp���� [j][i] ��� [i][j]�� ����ϸ� {row, col}�� �˴ϴ�.
        // ���⼭�� Bag.h�� ������ {row, col} ������ �����մϴ�.
        // main.cpp�� Snake �ʱ�ȭ: Snake snake(map_cols / 4, map_rows / 2);
        // Point ����ü: {row, col}
        body.push_back({start_row, start_col});     // �Ӹ�
        body.push_back({start_row, start_col - 1}); // ���� 1 (����)
        body.push_back({start_row, start_col - 2}); // ���� 2 (����)
    }

    // ���� ������ �����մϴ�. (�ζ���)
    void changeDirection(Direction new_dir) {
        // 180�� ������ ��ȯ ����
        if ((current_dir == UP && new_dir == DOWN) ||
            (current_dir == DOWN && new_dir == UP) ||
            (current_dir == LEFT && new_dir == RIGHT) ||
            (current_dir == RIGHT && new_dir == LEFT)) {
            return;
        }
        current_dir = new_dir;
    }

    // ���� ���� �������� �� ĭ �̵���ŵ�ϴ�. (�ζ���)
    void move() {
        // 1. ���ο� �Ӹ� ��ġ ���
        Point new_head = body.front();
        switch (current_dir) {
            case UP:    new_head.row--; break;
            case DOWN:  new_head.row++; break;
            case LEFT:  new_head.col--; break;
            case RIGHT: new_head.col++; break;
        }

        // 2. ���ο� �Ӹ��� �� �տ� �߰�
        body.insert(body.begin(), new_head);

        // 3. ���� ���� �Ǵ� ����
        if (!grow_flag) {
            // ���̸� ���� �ʾ����� ���� ���� -> ���� ����
            body.pop_back();
        } else {
            // ���̸� �Ծ����� ���� ���� �� �� -> ���� ����
            // ���� ���� ���� grow_flag �ʱ�ȭ
            grow_flag = false; 
        }
    }

    // ���� ���̸� �ø����� �÷��׸� �����մϴ�. (�ζ���)
    void grow() {
        grow_flag = true;
    }

    // ���� �Ӹ� ��ġ�� �����ɴϴ�. (�ζ���)
    Point getHead() const {
        return body.front();
    }

    // �� ���� ��ü�� �����ɴϴ�. (�ζ���)
    const std::vector<Point>& getBody() const {
        return body;
    }

    // ���� ���� ������ �����ɴϴ�. (�ζ���)
    Direction getDirection() const {
        return current_dir;
    }
    
    // �ڱ� �ڽŰ��� �浹 ���θ� Ȯ���մϴ�. (�ζ���)
    bool checkSelfCollision() const {
        // �Ӹ� ��ǥ
        const Point& head = body.front();
        
        // ���� ��ȸ (�ε��� 1����)
        for (size_t i = 1; i < body.size(); ++i) {
            // Point ����ü�� ���ǵ� operator==�� ����Ͽ� ���մϴ�.
            if (head == body[i]) {
                return true; // �浹 �߻�
            }
        }
        return false;
    }
};

#endif