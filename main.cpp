#include "features/GameMenu.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    GameMenu menu;
    g_dsInfo.showModeSettingMenu();
    while (true)
    {
        clearScreen();
        menu.show_menu();
    }
}
