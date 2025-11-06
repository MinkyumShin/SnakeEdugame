#include <iostream>
#include "ScoreBoard.h"


class GameMenu {
private:
    int selected_menu;
    ScoreBoard score_board;
public:
    GameMenu();
    void show_menu();
};

GameMenu::GameMenu() {
    selected_menu = -1;
    score_board = ScoreBoard();
}

void GameMenu::show_menu(){
        std::cout << "1. 게임시작" << std::endl;
        std::cout << "2. 스코어보드" << std::endl;
        std::cout << "0. 종료" << std::endl;
        menu_check:
            printf("선택: ");
            std::cin >> selected_menu;
            if(selected_menu == 1){
            }	
            else if(selected_menu == 2){
            }
            
            else if(selected_menu == 0){
                std::cout << "게임을 종료합니다." << std::endl;
                exit(0);
            }
            else goto menu_check;
    }