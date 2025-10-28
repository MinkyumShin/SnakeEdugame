#include <iostream>
#include "queue.h"

int main()
{
    std::cout << std::boolalpha;
    Queue<int> q;
    std::cout << "Empty? " << q.empty() << " size=" << q.size() << "\n";

    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(10); // duplicate allowed

    q.show_contents();
    std::cout << "Front: " << q.front() << ", Back: " << q.back() << "\n";
    std::cout << "Size after 3 enqueues (incl. dup): " << q.size() << "\n";

    q.dequeue();
    q.show_contents();
    std::cout << "Front: " << q.front() << ", Size: " << q.size() << "\n";

    // Copy semantics test
    Queue<int> q2 = q; // copy ctor
    q2.enqueue(99);
    std::cout << "q2 after copy+enqueue: ";
    q2.show_contents();
    std::cout << "q2 size: " << q2.size() << "\n";

    // Assignment
    Queue<int> q3;
    q3 = q2;
    std::cout << "q3 after assign: ";
    q3.show_contents();

    q.clear();
    std::cout << "q cleared. Empty? " << q.empty() << ", size=" << q.size() << "\n";

    return 0;
}
