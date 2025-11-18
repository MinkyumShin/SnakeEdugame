#include "DSInfoManager.h"

#include <iostream>

DSInfoManager g_dsInfo;

void DSInfoManager::toggle()
{
    enabled = !enabled;
    // 토글 메시지도 로그 버퍼에만 쌓고, 화면 그리기는 main에서 담당
    pushLog(recentLogs, std::string("[DSInfo] 자료구조 설명 모드: ") + (enabled ? "ON" : "OFF"));
}

void DSInfoManager::pushLog(std::deque<std::string> &target, const std::string &fullMsg)
{
    if (!enabled)
        return;
    target.push_back(fullMsg);
    while (target.size() > MAX_LOGS)
    {
        target.pop_front();
    }
}

void DSInfoManager::logQueue(const std::string &msg)
{
    pushLog(recentLogs, "[Queue] " + msg);
}

void DSInfoManager::logStack(const std::string &msg)
{
    pushLog(recentLogs, "[Stack] " + msg);
}

void DSInfoManager::logBag(const std::string &msg)
{
    pushLog(recentLogs, "[Bag] " + msg);
}

void DSInfoManager::logBagMove(const std::string &msg)
{
    pushLog(moveLogs, "[Bag] " + msg);
}

std::deque<std::string> DSInfoManager::getRecentLogs() const
{
    return recentLogs;
}

std::deque<std::string> DSInfoManager::getMoveLogs() const
{
    return moveLogs;
}

void DSInfoManager::showConceptMenu() const
{
    using std::cout;
    using std::endl;

    cout << "\n==============================================\n";
    cout << "        EduSnake 자료구조 개념 정리          \n";
    cout << "==============================================\n\n";

    // Queue 설명
    cout << "[Queue] (FIFO: First-In, First-Out)\n";
    cout << " - 먼저 들어온 데이터가 먼저 나가는 구조입니다.\n";
    cout << " - EduSnake에서는 뱀의 몸통 좌표를 순서대로 관리할 때\n";
    cout << "   Queue 개념으로 생각할 수 있습니다. (머리 추가, 꼬리 제거)\n\n";

    cout << "예시 코드 (개념적):\n";
    cout << "  std::queue<Point> q;\n";
    cout << "  q.push(newHead);   // 머리 추가 (enqueue)\n";
    cout << "  q.pop();           // 꼬리 제거 (dequeue)\n\n";

    // Stack 설명
    cout << "[Stack] (LIFO: Last-In, First-Out)\n";
    cout << " - 나중에 들어온 데이터가 먼저 나가는 구조입니다.\n";
    cout << " - EduSnake에서는 Undo(되돌리기)를 위해,\n";
    cout << "   직전에 저장한 게임 상태를 Stack으로 관리합니다.\n\n";

    cout << "예시 코드 (개념적):\n";
    cout << "  std::stack<GameState> st;\n";
    cout << "  st.push(state);   // 현재 상태 저장\n";
    cout << "  st.top();         // 가장 최근 상태 확인\n";
    cout << "  st.pop();         // 최근 상태 제거\n\n";

    // Bag 설명
    cout << "[Bag] (멀티셋, 순서 X, 중복 허용)\n";
    cout << " - 순서가 중요하지 않고, 같은 값이 여러 번 들어올 수 있는\n";
    cout << "   컨테이너로 볼 수 있습니다.\n";
    cout << " - EduSnake에서는 맵 위의 빈 칸들을 Bag에 넣어두었다가,\n";
    cout << "   그 중 랜덤으로 하나를 뽑아 먹이나 아이템 위치로 사용합니다.\n\n";

    cout << "예시 코드 (개념적):\n";
    cout << "  Bag bag;\n";
    cout << "  bag.add(p);           // 빈 칸 좌표 추가\n";
    cout << "  Point food = bag.getRandom(); // 랜덤 좌표 하나 반환\n\n";

    cout << "이 모드를 ON 해두면, 게임 진행 중에\n";
    cout << "Queue / Stack / Bag의 주요 동작이\n";
    cout << "[Queue], [Stack], [Bag] Prefix로 출력됩니다.\n";
    cout << "(예: [Queue] Enqueue: (x,y))\n\n";

    cout << "계속하려면 아무 키나 누르세요..." << endl;
}
