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
}

void GameMenu::show_menu(){
        std::cout << "1. start\n";
        std::cout << "2. score\n";
        std::cout << "0. exit\n";
        menu_check:
            printf("Choose: ");
            std::cin >> selected_menu;
            if(selected_menu == 1){
            }	
            else if(selected_menu == 2){
                system("cls");
                score_board.display_score_board();
            }
            
            else if(selected_menu == 0){
                std::cout << "Game exited.\n";
                exit(0);
            }
            else goto menu_check;
    }