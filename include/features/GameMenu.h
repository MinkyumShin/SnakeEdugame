#pragma once
#include <iostream>
#include "ScoreBoard.h"
#include "Game.h"

class GameMenu
{
private:
    char selected_menu;
    ScoreBoard score_board;

public:
    GameMenu();
    void show_menu();
};

GameMenu::GameMenu()
{
    selected_menu = -1;
}

void GameMenu::show_menu()
{
    std::cout << "=== EduSnake ===" << std::endl;
    std::cout << "1. 게임 시작\n";
    std::cout << "2. 스코어보드\n";
    std::cout << "3. 자료구조 개념 보기\n";
    std::cout << "4. 자료구조 설명 모드\n";
    std::cout << "0. 종료\n";
menu_check:
    printf("선택: ");
    std::cin >> selected_menu;
    if (selected_menu == '1')
    {
        clearScreen();
        startGame();
        string player_name;
        std::cout << "점수를 저장하시겠습니까? 저장하려면 플레이어 이름을 입력하세요:  "; // add score save, not required
        std::cin >> player_name;
        // check empty name
        if (!player_name.empty()) 
            score_board.add_score(player_name, score);
        std::cout << "계속하려면 아무 키나 누르세요..." << std::endl; 
        _getch();
    }
    else if (selected_menu == '2')
    {
        clearScreen();
        score_board.display_score_board();
    }
    else if (selected_menu == '3')
    {
        clearScreen();
        g_dsInfo.showConceptMenu();
    }
    else if (selected_menu == '4')
    {
        clearScreen();
        g_dsInfo.showModeSettingMenu();
    }
    else if (selected_menu == '0')
    {
        std::cout << "게임 종료되었습니다.\n";
        exit(0);
    }
    else
        goto menu_check;
    _getch();
}
