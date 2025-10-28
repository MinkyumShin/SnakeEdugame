#include <iostream>
#include "ourset.h"

int main()
{
    // boolalpha로 true/false를 문자 그대로 출력하도록 설정
    std::cout << std::boolalpha;

    // ---------------------------------------------
    // 1. 기본 생성자 테스트
    // ---------------------------------------------
    OurSet<int> setA;
    std::cout << "Initial setA contents:\n";
    setA.show_contents();

    // ---------------------------------------------
    // 2. insert() 동작 확인 (정상 삽입)
    // ---------------------------------------------
    std::cout << "\nBefore setA.insert(10):\n";
    setA.show_contents();
    bool inserted = setA.insert(10); // 중복 없는 삽입 → true
    std::cout << "After setA.insert(10):\n";
    setA.show_contents();
    std::cout << "Insertion succeeded: " << inserted << "\n";

    std::cout << "\nBefore setA.insert(20):\n";
    setA.show_contents();
    inserted = setA.insert(20);
    std::cout << "After setA.insert(20):\n";
    setA.show_contents();
    std::cout << "Insertion succeeded: " << inserted << "\n";

    // ---------------------------------------------
    // 3. insert() 중복 테스트
    // ---------------------------------------------
    std::cout << "\nBefore duplicate setA.insert(20):\n";
    setA.show_contents();
    inserted = setA.insert(20); // 이미 존재하므로 false 반환
    std::cout << "After duplicate setA.insert(20):\n";
    setA.show_contents();
    std::cout << "Insertion succeeded: " << inserted << "\n";

    // ---------------------------------------------
    // 4. erase() 테스트 (존재하는 값 삭제)
    // ---------------------------------------------
    std::cout << "\nBefore setA.erase(10):\n";
    setA.show_contents();
    bool erased = setA.erase(10);
    std::cout << "After setA.erase(10):\n";
    setA.show_contents();
    std::cout << "Erase succeeded: " << erased << "\n";

    // ---------------------------------------------
    // 5. erase() 테스트 (존재하지 않는 값 삭제)
    // ---------------------------------------------
    std::cout << "\nBefore setA.erase(42):\n";
    setA.show_contents();
    erased = setA.erase(42); // false 반환 예상
    std::cout << "After setA.erase(42):\n";
    setA.show_contents();
    std::cout << "Erase succeeded: " << erased << "\n";

    // ---------------------------------------------
    // 6. 새로운 집합 setB 생성 및 삽입 테스트
    // ---------------------------------------------
    OurSet<int> setB;
    std::cout << "\nInitial setB contents:\n";
    setB.show_contents();

    std::cout << "\nBefore setB.insert(15):\n";
    setB.show_contents();
    inserted = setB.insert(15);
    std::cout << "After setB.insert(15):\n";
    setB.show_contents();
    std::cout << "Insertion succeeded: " << inserted << "\n";

    std::cout << "\nBefore setB.insert(5):\n";
    setB.show_contents();
    inserted = setB.insert(5);
    std::cout << "After setB.insert(5):\n";
    setB.show_contents();
    std::cout << "Insertion succeeded: " << inserted << "\n";

    // ---------------------------------------------
    // 7. operator+ 테스트 (합집합 생성)
    // ---------------------------------------------
    OurSet<int> setC;
    std::cout << "\nBefore setC = setA + setB:\n";
    std::cout << "setA:\n";
    setA.show_contents();
    std::cout << "setB:\n";
    setB.show_contents();
    std::cout << "setC:\n";
    setC.show_contents();

    setC = setA + setB; // 합집합 결과를 setC에 저장

    std::cout << "After setC = setA + setB:\n";
    std::cout << "setA:\n";
    setA.show_contents();
    std::cout << "setB:\n";
    setB.show_contents();
    std::cout << "setC:\n";
    setC.show_contents();

    // ---------------------------------------------
    // 8. operator+= 테스트 (자기 자신에 병합)
    // ---------------------------------------------
    std::cout << "\nBefore setA += setB:\n";
    std::cout << "setA:\n";
    setA.show_contents();
    std::cout << "setB:\n";
    setB.show_contents();

    setA += setB; // 중복은 자동 차단됨

    std::cout << "After setA += setB:\n";
    std::cout << "setA:\n";
    setA.show_contents();
    std::cout << "setB:\n";
    setB.show_contents();

    // ---------------------------------------------
    // 9. 복사 대입 연산자 테스트 (operator=)
    // ---------------------------------------------
    OurSet<int> setD;
    std::cout << "\nBefore setD = setA:\n";
    setA.show_contents();
    setD.show_contents();

    setD = setA; // 깊은 복사 수행

    std::cout << "After setD = setA:\n";
    std::cout << "setA:\n";
    setA.show_contents();
    std::cout << "setD:\n";
    setD.show_contents();

    // ---------------------------------------------
    // 10. operator== 테스트 (동등 비교)
    // ---------------------------------------------
    std::cout << "\nBefore comparison setA == setD:\n";
    bool equal = (setA == setD); // true 예상
    std::cout << "Equality result: " << equal << "\n";

    // ---------------------------------------------
    // 11. operator!= 테스트 (비교 반대)
    // ---------------------------------------------
    std::cout << "\nBefore inequality setA != setB:\n";
    bool not_equal = (setA != setB); // true 예상
    std::cout << "Inequality result: " << not_equal << "\n";

    // ---------------------------------------------
    // 12. 복사 생성자 테스트
    // ---------------------------------------------
    std::cout << "\nBefore constructing OurSet<int> setE(setB):\n";
    OurSet<int> setE(setB); // 복사 생성자 호출
    std::cout << "After constructing OurSet<int> setE(setB):\n";
    std::cout << "setE:\n";
    setE.show_contents();

    std::cout << "\nBefore invoking OurSet<int> setF(setC):\n";
    OurSet<int> setF(setC); // 복사 생성자 호출
    std::cout << "After invoking OurSet<int> setF(setC):\n";
    std::cout << "setF:\n";
    setF.show_contents();

    // ---------------------------------------------
    // 13. sort(), sort_with_iterator() 테스트
    // ---------------------------------------------
    std::cout << "\nBefore setC.sort():\n";
    setC.show_contents();
    setC.sort(); // 데이터 값 기준 버블 정렬
    std::cout << "After setC.sort():\n";
    setC.show_contents();

    std::cout << "\nBefore setC.sort_with_iterator():\n";
    setC.show_contents();
    setC.sort_with_iterator();
    std::cout << "After setC.sort_with_iterator():\n";
    setC.show_contents();

    // ---------------------------------------------
    // 14. clear() 테스트 (전체 삭제)
    // ---------------------------------------------
    std::cout << "\nBefore setC.clear():\n";
    setC.show_contents();
    setC.clear();
    std::cout << "After setC.clear():\n";
    setC.show_contents();

    std::cout << "\nProgram finished.\n";
    return 0;
}
