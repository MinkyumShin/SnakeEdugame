#include <iostream>
#include <stdexcept>
#include <vector>
#include "Point.h"

// GameState: 스택에 저장될 게임 상태 (뱀 몸통, 방향, 점수, 먹이/아이템 위치, Bag 내용)
struct GameState {
    std::vector<Point> snake_body;
    int dir = 0; // Direction을 int로 저장 (cast 사용)
    int score = 0;
    Point food_pos = {-1, -1};
    Point r_pos = {-1, -1};
    std::vector<Point> bag_contents;
};

//Custom Stack for GameState
class MyStack {
private:
    static const int maxSize = 100;
    GameState arr[maxSize];
    int topIndex;

public:
    //initialize top index = -1;
    MyStack() : topIndex(-1) {}

    bool empty() const { return topIndex == -1; }
    bool full() const { return topIndex == maxSize - 1; }

    void push(const GameState& state) {
        if (full()) {
            throw std::overflow_error("Stack Overflow: Stack is full");
        }
        arr[++topIndex] = state;
    }

    void pop() {
        if (empty()) {
            throw std::underflow_error("Stack Underflow: Stack is empty");
        }
        topIndex--;
    }

    const GameState& top() const {
        if (empty()) {
            throw std::underflow_error("Stack Underflow: Stack is empty");
        }
        return arr[topIndex];
    }
};

//undo item count
int undo_item_count = 0;
MyStack state_history;

void save_state(const GameState& current_state) {
    state_history.push(current_state);
}

bool use_undo_item(GameState& current_state) {

    if (undo_item_count <= 0) {
        return false;
    }

    if (state_history.empty()) {
        return false;
    }

    undo_item_count--;
    current_state = state_history.top();
    state_history.pop();
    return true;

}
