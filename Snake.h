#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <algorithm> // find를 사용하기 위해 포함
#include "Bag.h" // Point 구조체를 사용하기 위해 포함합니다.

// 뱀의 이동 방향을 나타내는 열거형
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
    // 생성자 구현 (인라인)
    Snake(int start_col, int start_row) : current_dir(RIGHT), grow_flag(false) {
        // 초기 뱀 길이 3 (Point는 {row, col} 대신 {col, row}를 사용한다고 가정)
        // Bag.h의 Point는 {row, col}이지만, 일반적인 x(col), y(row) 순으로 초기화합니다.
        // main.cpp에서 [j][i] 대신 [i][j]를 사용하면 {row, col}이 됩니다.
        // 여기서는 Bag.h의 정의인 {row, col} 순서로 통일합니다.
        // main.cpp의 Snake 초기화: Snake snake(map_cols / 4, map_rows / 2);
        // Point 구조체: {row, col}
        body.push_back({start_row, start_col});     // 머리
        body.push_back({start_row, start_col - 1}); // 몸통 1 (좌측)
        body.push_back({start_row, start_col - 2}); // 몸통 2 (좌측)
    }

    // 뱀의 방향을 변경합니다. (인라인)
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

    // 뱀을 현재 방향으로 한 칸 이동시킵니다. (인라인)
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

    // 뱀의 길이를 늘리도록 플래그를 설정합니다. (인라인)
    void grow() {
        grow_flag = true;
    }

    // 뱀의 머리 위치를 가져옵니다. (인라인)
    Point getHead() const {
        return body.front();
    }

    // 뱀 몸통 전체를 가져옵니다. (인라인)
    const std::vector<Point>& getBody() const {
        return body;
    }

    // 현재 뱀의 방향을 가져옵니다. (인라인)
    Direction getDirection() const {
        return current_dir;
    }
    
    // 자기 자신과의 충돌 여부를 확인합니다. (인라인)
    bool checkSelfCollision() const {
        // 머리 좌표
        const Point& head = body.front();
        
        // 몸통 순회 (인덱스 1부터)
        for (size_t i = 1; i < body.size(); ++i) {
            // Point 구조체에 정의된 operator==를 사용하여 비교합니다.
            if (head == body[i]) {
                return true; // 충돌 발생
            }
        }
        return false;
    }
};

#endif