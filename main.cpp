#include "features/GameMenu.h"

int main()
{
    #if __has_include(<windows.h>) // Enable UTF-8 console on local pc
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    GameMenu menu;
    g_dsInfo.showModeSettingMenu();
    while (true)
    {
        clearScreen();
        menu.show_menu();
    }
}
