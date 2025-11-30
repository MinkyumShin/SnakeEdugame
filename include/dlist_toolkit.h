#pragma once // 헤더 파일 중복 방지

#include <cstddef> // size_t를 위해 생성
#include "dnode.h"

// head_ptr로부터 앞으로 연결된 모든 노드의 개수를 반환한다.
template <class Item>
std::size_t list_length(const dnode<Item> *head_ptr)
{
    std::size_t count = 0;
    for (const dnode<Item> *cursor = head_ptr; cursor != nullptr; cursor = cursor->fore_link())
    {
        ++count;
    }
    return count;
}

// head_ptr부터 target 값을 검색하고, 일치하는 노드의 포인터를 반환한다.
template <class Item>
dnode<Item> *list_search(dnode<Item> *head_ptr, const Item &target)
{
    for (dnode<Item> *cursor = head_ptr; cursor != nullptr; cursor = cursor->fore_link())
    {
        if (cursor->data() == target)
        {
            return cursor;
        }
    }
    return nullptr;
}

// const 버전의 list_search (읽기 전용)
template <class Item>
const dnode<Item> *list_search(const dnode<Item> *head_ptr, const Item &target)
{
    for (const dnode<Item> *cursor = head_ptr; cursor != nullptr; cursor = cursor->fore_link())
    {
        if (cursor->data() == target)
        {
            return cursor;
        }
    }
    return nullptr;
}

// 1부터 시작하는 위치(position)에 해당하는 노드를 반환한다.
template <class Item>
dnode<Item> *list_locate(dnode<Item> *head_ptr, std::size_t position)
{
    if (position == 0)
    {
        return nullptr;
    }

    dnode<Item> *cursor = head_ptr;
    for (std::size_t index = 1; cursor != nullptr && index < position; ++index)
    {
        cursor = cursor->fore_link();
    }
    return cursor;
}

template <class Item>
const dnode<Item> *list_locate(const dnode<Item> *head_ptr, std::size_t position)
{
    if (position == 0)
    {
        return nullptr;
    }

    const dnode<Item> *cursor = head_ptr;
    for (std::size_t index = 1; cursor != nullptr && index < position; ++index)
    {
        cursor = cursor->fore_link();
    }
    return cursor;
}

// entry가 존재하지 않을 경우 리스트의 맨 앞에 삽입한다.
template <class Item>
bool list_head_insert(dnode<Item> *&head_ptr, dnode<Item> *&tail_ptr, const Item &entry)
{
    if (list_search(head_ptr, entry) != nullptr)
    {
        return false;
    }

    dnode<Item> *new_node = new dnode<Item>(entry, head_ptr, nullptr);
    if (head_ptr != nullptr)
    {
        head_ptr->set_back_link(new_node);
    }
    else
    {
        tail_ptr = new_node;
    }
    head_ptr = new_node;
    return true;
}

// entry가 존재하지 않을 경우 리스트의 맨 뒤에 삽입한다.
template <class Item>
bool list_tail_insert(dnode<Item> *&head_ptr, dnode<Item> *&tail_ptr, const Item &entry)
{
    if (list_search(head_ptr, entry) != nullptr)
    {
        return false;
    }

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
    return true;
}

// previous_ptr 바로 뒤에 entry를 삽입한다 (entry가 중복되지 않을 경우에만).
template <class Item>
bool list_insert(dnode<Item> *&head_ptr, dnode<Item> *&tail_ptr, dnode<Item> *previous_ptr, const Item &entry)
{
    if (list_search(head_ptr, entry) != nullptr)
    {
        return false;
    }

    if (previous_ptr == nullptr)
    {
        return list_head_insert(head_ptr, tail_ptr, entry);
    }

    dnode<Item> *new_node = new dnode<Item>(entry, previous_ptr->fore_link(), previous_ptr);
    if (previous_ptr->fore_link() != nullptr)
    {
        previous_ptr->fore_link()->set_back_link(new_node);
    }
    else
    {
        tail_ptr = new_node;
    }
    previous_ptr->set_fore_link(new_node);
    return true;
}

// 리스트의 첫 번째 노드를 삭제한다.
template <class Item>
bool list_head_remove(dnode<Item> *&head_ptr, dnode<Item> *&tail_ptr)
{
    if (head_ptr == nullptr)
    {
        return false;
    }

    dnode<Item> *remove_ptr = head_ptr;
    head_ptr = head_ptr->fore_link();
    if (head_ptr != nullptr)
    {
        head_ptr->set_back_link(nullptr);
    }
    else
    {
        tail_ptr = nullptr;
    }
    delete remove_ptr;
    return true;
}

// 리스트의 마지막 노드를 삭제한다.
template <class Item>
bool list_tail_remove(dnode<Item> *&head_ptr, dnode<Item> *&tail_ptr)
{
    if (tail_ptr == nullptr)
    {
        return false;
    }

    dnode<Item> *remove_ptr = tail_ptr;
    tail_ptr = tail_ptr->back_link();
    if (tail_ptr != nullptr)
    {
        tail_ptr->set_fore_link(nullptr);
    }
    else
    {
        head_ptr = nullptr;
    }
    delete remove_ptr;
    return true;
}

// 지정한 노드를 리스트에서 제거한다.
template <class Item>
bool list_remove(dnode<Item> *&head_ptr, dnode<Item> *&tail_ptr, dnode<Item> *remove_ptr)
{
    if (remove_ptr == nullptr)
    {
        return false;
    }

    if (remove_ptr->back_link() != nullptr)
    {
        remove_ptr->back_link()->set_fore_link(remove_ptr->fore_link());
    }
    else
    {
        head_ptr = remove_ptr->fore_link();
    }

    if (remove_ptr->fore_link() != nullptr)
    {
        remove_ptr->fore_link()->set_back_link(remove_ptr->back_link());
    }
    else
    {
        tail_ptr = remove_ptr->back_link();
    }

    delete remove_ptr;
    return true;
}

// 모든 노드를 삭제하고 head와 tail 포인터를 초기화한다.
template <class Item>
void list_clear(dnode<Item> *&head_ptr, dnode<Item> *&tail_ptr)
{
    while (head_ptr != nullptr)
    {
        list_head_remove(head_ptr, tail_ptr);
    }
}

// source 리스트를 복사하여 dest 리스트에 동일한 순서로 저장한다.
template <class Item>
void list_copy(const dnode<Item> *source_head, const dnode<Item> *source_tail, dnode<Item> *&dest_head, dnode<Item> *&dest_tail)
{
    (void)source_tail; // source_tail은 인터페이스 통일을 위해 남겨두었지만 사용하지 않는다.
    dest_head = nullptr;
    dest_tail = nullptr;
    for (const dnode<Item> *cursor = source_head; cursor != nullptr; cursor = cursor->fore_link())
    {
        list_tail_insert(dest_head, dest_tail, cursor->data());
    }
}
