#pragma once

#include <cstddef>          // size_t 사용 (노드 개수, 인덱스 등)
#include <initializer_list> // {1,2,3} 형태의 초기화 지원
#include <iostream>         // show_contents 출력용
#include <iterator>         // iterator category 지정
#include <utility>          // std::swap, move 사용을 위해 포함
#include "dlist_toolkit.h"  // 이중 연결 리스트 도구 사용

// ---------------------------------------------------
// OurSet 클래스: 중복 없는 집합(Set) 구조---
// 내부적으로 dnode<Item>을 이용한 이중 연결 리스트로 구현
// ---------------------------------------------------
template <class Item>
class OurSet
{
public:
    typedef Item value_type;
    typedef std::size_t size_type;

    // 기본 생성자: 비어 있는 집합 초기화
    OurSet();

    // 초기화 리스트 생성자: {1,2,3} 형태로 간단히 초기화 가능
    OurSet(std::initializer_list<Item> init);

    // 복사 생성자: 깊은 복사 (list_copy 이용)
    OurSet(const OurSet &other);

    // 이동 생성자: noexcept → 예외 안전성 보장
    OurSet(OurSet &&other) noexcept;

    // 소멸자: 동적 메모리 해제
    ~OurSet();

    // 대입 연산자: copy-swap idiom 적용으로 중복 코드 방지
    OurSet &operator=(OurSet other);

    // -------------------------
    // 주요 연산 함수
    // -------------------------

    // 원소 삽입 (중복 시 false)
    bool insert(const Item &entry);

    // 특정 원소 삭제 (성공 시 true)
    bool erase(const Item &target);

    // 특정 원소 존재 여부 (1 또는 0 반환)
    size_type count(const Item &target) const;

    // 현재 저장된 원소 개수 반환
    size_type size() const { return used; }

    // 비었는지 여부
    bool empty() const { return used == 0; }

    // 전체 초기화 (모든 노드 삭제)
    void clear();

    // 집합 원소 전체 출력
    void show_contents(std::ostream &out = std::cout) const;

    // 버블 정렬 (단순 구현, 성능보다 원리 학습용)
    void sort();
    void sort_with_iterator(); // iterator 기반 정렬 (보너스용)

    // swap 함수: 내부 포인터 교환으로 빠른 교체
    void swap(OurSet &other) noexcept;

    // ----------------------------------------
    // iterator 정의
    // ----------------------------------------
    // iterator는 bidirectional iterator로 구현
    // fore_link(), back_link() 양방향 이동 가능
    // ----------------------------------------

    class iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Item;
        using difference_type = std::ptrdiff_t;
        using pointer = Item *;
        using reference = Item &;

        iterator() = default;
        iterator(OurSet *owner_ptr, dnode<Item> *node_ptr)
            : owner(owner_ptr), current(node_ptr) {}

        reference operator*() const { return current->data(); }
        pointer operator->() const { return &current->data(); }

        // prefix ++ (다음 노드로 이동)
        iterator &operator++()
        {
            if (current != nullptr)
            {
                current = current->fore_link();
            }
            return *this;
        }

        // postfix ++ (임시 복사 후 이동)
        iterator operator++(int)
        {
            iterator temp(*this);
            ++(*this);
            return temp;
        }

        // prefix -- (이전 노드로 이동)
        iterator &operator--()
        {
            if (current != nullptr)
            {
                current = current->back_link();
            }
            // nullptr(=end()) 상태에서 -- 하면 tail로 이동
            else if (owner != nullptr)
            {
                current = owner->tail_ptr;
            }
            return *this;
        }

        // postfix -- (임시 복사 후 이동)
        iterator operator--(int)
        {
            iterator temp(*this);
            --(*this);
            return temp;
        }

        bool operator==(const iterator &other) const { return current == other.current; }
        bool operator!=(const iterator &other) const { return !(*this == other); }

    private:
        OurSet *owner = nullptr;
        dnode<Item> *current = nullptr;

        friend class OurSet;
        friend class const_iterator;
    };

    // -----------------------------------------------------
    // const_iterator: 읽기 전용 반복자 (iterator와 기능 동일)
    // -----------------------------------------------------
    class const_iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Item;
        using difference_type = std::ptrdiff_t;
        using pointer = const Item *;
        using reference = const Item &;

        const_iterator() = default;
        const_iterator(const OurSet *owner_ptr, const dnode<Item> *node_ptr)
            : owner(owner_ptr), current(node_ptr) {}
        const_iterator(const iterator &it)
            : owner(it.owner), current(it.current) {}

        reference operator*() const { return current->data(); }
        pointer operator->() const { return &current->data(); }

        const_iterator &operator++()
        {
            if (current != nullptr)
            {
                current = current->fore_link();
            }
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator temp(*this);
            ++(*this);
            return temp;
        }

        const_iterator &operator--()
        {
            if (current != nullptr)
            {
                current = current->back_link();
            }
            else if (owner != nullptr)
            {
                current = owner->tail_ptr;
            }
            return *this;
        }

        const_iterator operator--(int)
        {
            const_iterator temp(*this);
            --(*this);
            return temp;
        }

        bool operator==(const const_iterator &other) const { return current == other.current; }
        bool operator!=(const const_iterator &other) const { return !(*this == other); }

    private:
        const OurSet *owner = nullptr;
        const dnode<Item> *current = nullptr;

        friend class OurSet;
    };

    // iterator 반환 함수들
    iterator begin() { return iterator(this, head_ptr); }
    iterator end() { return iterator(this, nullptr); }
    const_iterator begin() const { return const_iterator(this, head_ptr); }
    const_iterator end() const { return const_iterator(this, nullptr); }
    const_iterator cbegin() const { return const_iterator(this, head_ptr); }
    const_iterator cend() const { return const_iterator(this, nullptr); }

    // 집합 연산자
    OurSet operator+(const OurSet &rhs) const; // 합집합
    OurSet &operator+=(const OurSet &rhs);     // 자기 자신과 합집합
    bool operator==(const OurSet &rhs) const;  // 동등 비교
    bool operator!=(const OurSet &rhs) const { return !(*this == rhs); }

private:
    dnode<Item> *head_ptr = nullptr;
    dnode<Item> *tail_ptr = nullptr;
    size_type used = 0; // 현재 원소 개수
};

// --------------------
// 구현부 (멤버 함수 정의)
// --------------------

// 기본 생성자
template <class Item>
OurSet<Item>::OurSet() = default;

// 초기화 리스트 생성자
template <class Item>
OurSet<Item>::OurSet(std::initializer_list<Item> init)
{
    for (const auto &value : init)
    {
        insert(value); // 중복 방지는 insert 내부에서 처리됨
    }
}

// 복사 생성자 (깊은 복사)
template <class Item>
OurSet<Item>::OurSet(const OurSet<Item> &other)
{
    list_copy(other.head_ptr, other.tail_ptr, head_ptr, tail_ptr);
    used = other.used;
}

// 이동 생성자 (자원만 옮기고 원본 초기화)
template <class Item>
OurSet<Item>::OurSet(OurSet<Item> &&other) noexcept
    : head_ptr(other.head_ptr), tail_ptr(other.tail_ptr), used(other.used)
{
    other.head_ptr = nullptr;
    other.tail_ptr = nullptr;
    other.used = 0;
}

// 소멸자 (모든 노드 메모리 해제)
template <class Item>
OurSet<Item>::~OurSet()
{
    clear();
}

// 대입 연산자 (copy-swap idiom)
template <class Item>
OurSet<Item> &OurSet<Item>::operator=(OurSet<Item> other)
{
    swap(other);
    return *this;
}

// 삽입 함수
template <class Item>
bool OurSet<Item>::insert(const Item &entry)
{
    // list_tail_insert 내부에서 중복 검사 + 링크 연결 수행
    if (list_tail_insert(head_ptr, tail_ptr, entry))
    {
        ++used;
        return true;
    }
    return false;
}

// 삭제 함수
template <class Item>
bool OurSet<Item>::erase(const Item &target)
{
    // 먼저 target 노드를 찾고(list_search),
    // 찾은 노드 포인터를 list_remove로 제거
    dnode<Item> *target_ptr = list_search(head_ptr, target);
    if (list_remove(head_ptr, tail_ptr, target_ptr))
    {
        --used;
        return true;
    }
    return false;
}

// 존재 여부 확인 (읽기 전용)
template <class Item>
typename OurSet<Item>::size_type OurSet<Item>::count(const Item &target) const
{
    const dnode<Item> *const_head = head_ptr;
    return list_search(const_head, target) != nullptr ? 1 : 0;
}

// 전체 삭제
template <class Item>
void OurSet<Item>::clear()
{
    list_clear(head_ptr, tail_ptr);
    used = 0;
}

// 전체 내용 출력
template <class Item>
void OurSet<Item>::show_contents(std::ostream &out) const
{
    out << "{ ";
    bool first = true;
    for (const dnode<Item> *cursor = head_ptr; cursor != nullptr; cursor = cursor->fore_link())
    {
        if (!first)
            out << ", ";
        first = false;
        out << cursor->data();
    }
    out << " }" << std::endl;
}

// 버블 정렬 (단순 구현, 성능보단 구조 연습용)
template <class Item>
void OurSet<Item>::sort()
{
    if (used < 2)
        return;

    bool swapped;
    do
    {
        swapped = false;
        for (dnode<Item> *cursor = head_ptr; cursor && cursor->fore_link(); cursor = cursor->fore_link())
        {
            dnode<Item> *next = cursor->fore_link();
            if (next->data() < cursor->data())
            {
                std::swap(cursor->data(), next->data());
                swapped = true;
            }
        }
    } while (swapped);
}

// iterator를 활용한 정렬
template <class Item>
void OurSet<Item>::sort_with_iterator()
{
    if (used < 2)
        return;

    bool swapped;
    do
    {
        swapped = false;
        for (auto it = begin(); it != end();)
        {
            auto next = it;
            ++next;
            if (next == end())
                break;
            if (*next < *it)
            {
                std::swap(*it, *next);
                swapped = true;
            }
            it = next;
        }
    } while (swapped);
}

// swap: 내부 포인터 교체로 빠른 교환
template <class Item>
void OurSet<Item>::swap(OurSet<Item> &other) noexcept
{
    std::swap(head_ptr, other.head_ptr);
    std::swap(tail_ptr, other.tail_ptr);
    std::swap(used, other.used);
}

// 합집합 연산자 (복사 후 +=)
template <class Item>
OurSet<Item> OurSet<Item>::operator+(const OurSet<Item> &rhs) const
{
    OurSet<Item> result(*this); // 복사본 생성
    result += rhs;              // +=로 병합
    return result;
}

// += 연산자 (자기 자신에 병합)
template <class Item>
OurSet<Item> &OurSet<Item>::operator+=(const OurSet<Item> &rhs)
{
    for (const dnode<Item> *cursor = rhs.head_ptr; cursor != nullptr; cursor = cursor->fore_link())
    {
        insert(cursor->data()); // 중복은 insert 내부에서 자동 차단
    }
    return *this;
}

// 동등 비교 연산자
template <class Item>
bool OurSet<Item>::operator==(const OurSet<Item> &rhs) const
{
    if (used != rhs.used)
        return false;

    // 모든 원소가 rhs에 포함되는지 확인
    for (const dnode<Item> *cursor = head_ptr; cursor != nullptr; cursor = cursor->fore_link())
    {
        if (rhs.count(cursor->data()) == 0)
            return false;
    }
    return true;
}
