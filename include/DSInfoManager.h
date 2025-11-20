#pragma once
#include <string>
#include <deque>

// 전역 교육용 자료구조 정보 매니저
class DSInfoManager
{
public:
    bool enabled = false;

    void toggle();

    void logQueue(const std::string &msg);
    void logStack(const std::string &msg);
    void logBag(const std::string &msg);
    void logBagMove(const std::string &msg);
    void showModeSettingMenu();

    // 최근 DS 로그 N개 반환용 (메인에서 하단 고정 영역 렌더링)
    std::deque<std::string> getRecentLogs() const;
    std::deque<std::string> getMoveLogs() const;

    // 콘솔에 개념 설명 및 예제 출력
    void showConceptMenu() const;

private:
    static const std::size_t MAX_LOGS = 20;
    std::deque<std::string> recentLogs;
    std::deque<std::string> moveLogs; // 뱀 이동에 따른 Bag 변화 로그
    void pushLog(std::deque<std::string> &target, const std::string &fullMsg);
};

// 전역 싱글톤 객체 선언
extern DSInfoManager g_dsInfo;
