#pragma once
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include "dlist_toolkit.h"

// ==============================
// Queue 클래스 (템플릿 기반)
// ==============================
template <class Item>
class Queue
{
public:
    typedef Item value_type;
    typedef std::size_t size_type;

    // 기본 생성자
    Queue() : head_ptr(nullptr), tail_ptr(nullptr), used(0) {}

    // 복사 생성자 (깊은 복사)
    Queue(const Queue &other)
    {
        head_ptr = nullptr;
        tail_ptr = nullptr;
        used = 0;
        // dlist_toolkit의 중복 방지 정책을 우회하기 위해 직접 노드 연결
        for (const dnode<Item> *cursor = other.head_ptr; cursor != nullptr; cursor = cursor->fore_link())
        {
            // enqueue는 중복을 허용해야 하므로 직접 tail에 삽입
            dnode<Item> *new_node = new dnode<Item>(cursor->data(), nullptr, tail_ptr);
            if (tail_ptr != nullptr)
            {
                tail_ptr->set_fore_link(new_node);
            }
            else
            {
                head_ptr = new_node;
            }
            tail_ptr = new_node;
            ++used;
        }
    }

    // 소멸자
    ~Queue()
    {
        clear();
    }

    // 복사 대입 연산자
    Queue &operator=(const Queue &other)
    {
        if (this != &other)
        {
            clear();
            // dlist_toolkit의 중복 방지 정책을 우회하기 위해 직접 노드 연결
            for (const dnode<Item> *cursor = other.head_ptr; cursor != nullptr; cursor = cursor->fore_link())
            {
                dnode<Item> *new_node = new dnode<Item>(cursor->data(), nullptr, tail_ptr);
                if (tail_ptr != nullptr)
                {
                    tail_ptr->set_fore_link(new_node);
                }
                else
                {
                    head_ptr = new_node;
                }
                tail_ptr = new_node;
                ++used;
            }
        }
        return *this;
    }

    // ---------------------
    // 주요 멤버 함수
    // ---------------------
    bool empty() const { return used == 0; }
    size_type size() const { return used; }

    // enqueue: 큐의 뒤쪽(tail)에 삽입
    void enqueue(const Item &entry)
    {
        // OurSet과 달리 Queue는 중복을 허용하므로, dlist_toolkit의 중복 검사 함수를 사용하지 않음
        dnode<Item> *new_node = new dnode<Item>(entry, nullptr, tail_ptr);
        if (tail_ptr != nullptr)
        {
            tail_ptr->set_fore_link(new_node);
        }
        else
        {
            head_ptr = new_node;
        }
        tail_ptr = new_node;
        ++used;
    }

    // dequeue: 큐의 앞쪽(head)에서 제거
    bool dequeue()
    {
        if (empty())
            return false;
        if (list_head_remove(head_ptr, tail_ptr))
        {
            --used;
            return true;
        }
        return false;
    }

    // front: 큐의 맨 앞 원소 접근
    Item &front()
    {
        if (empty())
            throw std::underflow_error("Queue is empty!");
        return head_ptr->data();
    }

    // back: 큐의 맨 뒤 원소 접근
    Item &back()
    {
        if (empty())
            throw std::underflow_error("Queue is empty!");
        return tail_ptr->data();
    }

    // 전체 비우기
    void clear()
    {
        list_clear(head_ptr, tail_ptr);
        used = 0;
    }

    // 큐 상태 출력
    void show_contents(std::ostream &out = std::cout) const
    {
        out << "[Front] ";
        for (const dnode<Item> *cursor = head_ptr; cursor != nullptr; cursor = cursor->fore_link())
        {
            out << cursor->data() << " ";
        }
        out << "[Back]" << std::endl;
    }

private:
    dnode<Item> *head_ptr; // 큐의 front
    dnode<Item> *tail_ptr; // 큐의 back
    size_type used;        // 원소 개수
};
