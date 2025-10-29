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
    bool grow_flag; // 먹이를 먹었는지 체크하는 플래그

public:
    // 생성자 구현
    Snake(int start_col, int start_row) : current_dir(RIGHT), grow_flag(false) {
        body.push_back({start_row, start_col});     // 머리
        body.push_back({start_row, start_col - 1}); // 몸통 1 (좌측)
        body.push_back({start_row, start_col - 2}); // 몸통 2 (좌측)
    }

    // 뱀의 방향을 변경
    void changeDirection(Direction new_dir) {
        // 180도 역방향 전환 방지
        if ((current_dir == UP && new_dir == DOWN) ||
            (current_dir == DOWN && new_dir == UP) ||
            (current_dir == LEFT && new_dir == RIGHT) ||
            (current_dir == RIGHT && new_dir == LEFT)) {
            return;
        }
        current_dir = new_dir;
    }

    // 뱀을 현재 방향으로 한 칸 이동
    void move() {
        // 1. 새로운 머리 위치 계산
        Point new_head = body.front();
        switch (current_dir) {
            case UP:    new_head.row--; break;
            case DOWN:  new_head.row++; break;
            case LEFT:  new_head.col--; break;
            case RIGHT: new_head.col++; break;
        }

        // 2. 새로운 머리를 맨 앞에 추가
        body.insert(body.begin(), new_head);

        // 3. 꼬리 제거 또는 유지
        if (!grow_flag) {
            // 먹이를 먹지 않았으면 꼬리 제거 -> 길이 유지
            body.pop_back();
        } else {
            // 먹이를 먹었으면 꼬리 제거 안 함 -> 길이 증가
            // 다음 턴을 위해 grow_flag 초기화
            grow_flag = false; 
        }
    }

    // 뱀의 길이를 늘리도록 플래그를 설정
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
    
    // 자기 자신과의 충돌 여부를 확인
    bool checkSelfCollision() const {
        // 머리 좌표
        const Point& head = body.front();
        
        for (size_t i = 1; i < body.size(); ++i) {
            if (head == body[i]) {
                return true; // 충돌 발생
            }
        }
        return false;
    }
};

#endif