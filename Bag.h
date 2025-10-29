#ifndef BAG_H
#define BAG_H

#include <iostream>
#include <vector>
#include <algorithm> //find() 사용
#include <cstdlib> //srand() 사용
#include <ctime> // time() 사용
using namespace std;

// 배열 위치 받는 양식
struct Point {
    int row;
    int col;

    // 배열 비교 오버라이딩
    bool operator==(const Point& other) const {
        return row == other.row && col == other.col;
    }
};

class Bag {
private:
    vector<Point> contents;

public:
    Bag() {
        // 난수 생성기 초기화
        srand(time(0));
    }

    // 뱀 꼬리가 떠난칸 Bag에 추가
    void add(const Point& p) {
        contents.push_back(p);
    }

    // 뱀 머리가 들어갈 칸 Bag에서 제거
    void remove(const Point& p) {
        auto it = find(contents.begin(), contents.end(), p);
        if (it != contents.end()) {
            *it = contents.back(); //벡터의 맨 뒤값과 교체
            contents.pop_back(); //벡터 맨뒤 제거
        }
    }

    // Bag에서 랜덤 칸 반환 후 제거(먹이 표시할것)
    Point getRandom() {
        if (contents.empty()) {
            return {-1, -1}; // Bag이 비어있을때 반환할값
        }
        int index = rand() % contents.size();
        Point selected_point = contents[index];

        if (index != contents.size() - 1) {
            contents[index] = contents.back();
        }
        contents.pop_back();

        return selected_point;
    }
    
    // Bag이 비어있는지 확인
    bool isEmpty() const {
        return contents.empty();
    }
};

#endif
