#pragma once
// 헤더 파일 중복 방지

#include <cstddef> // size_t를 위해 생성

template <class Item>
class dnode
{
public:
    typedef Item value_type; // value_type 정의를 통해 Item 타입을 외부에 노출

    // 생성자
    dnode(const Item &init_data = Item(),
          dnode<Item> *init_fore = nullptr,
          dnode<Item> *init_back = nullptr)
        : data_field(init_data), link_fore(init_fore), link_back(init_back) {}

    // 접근자
    Item data() const { return data_field; }
    Item &data() { return data_field; }
    dnode<Item> *fore_link() const { return link_fore; }
    dnode<Item> *back_link() const { return link_back; }

    // 수정자
    void set_data(const Item &new_data) { data_field = new_data; }
    void set_fore_link(dnode<Item> *new_fore) { link_fore = new_fore; }
    void set_back_link(dnode<Item> *new_back) { link_back = new_back; }

private:
    Item data_field;
    dnode<Item> *link_fore; // 다음 노드
    dnode<Item> *link_back; // 이전 노드
};
