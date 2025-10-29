#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <algorithm> // find
#include "Bag.h" // Point

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
    // ������ ����
    Snake(int start_col, int start_row) : current_dir(RIGHT), grow_flag(false) {
        body.push_back({start_row, start_col});     // �Ӹ�
        body.push_back({start_row, start_col - 1}); // ���� 1 (����)
        body.push_back({start_row, start_col - 2}); // ���� 2 (����)
    }

    // ���� ������ ����
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

    // ���� ���� �������� �� ĭ �̵�
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

    // ���� ���̸� �ø����� �÷��׸� ����
    void grow() {
        grow_flag = true;
    }

    //Get snake head
    Point getHead() const {
        return body.front();
    }
	//Get snake body
    const std::vector<Point>& getBody() const {
        return body;
    }

	//Get current direction
    Direction getDirection() const {
        return current_dir;
    }
    
    // �ڱ� �ڽŰ��� �浹 ���θ� Ȯ��
    bool checkSelfCollision() const {
        // �Ӹ� ��ǥ
        const Point& head = body.front();
        
        for (size_t i = 1; i < body.size(); ++i) {
            if (head == body[i]) {
                return true; // �浹 �߻�
            }
        }
        return false;
    }
};

#endif