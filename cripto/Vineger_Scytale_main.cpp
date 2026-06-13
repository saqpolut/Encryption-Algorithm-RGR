#include "menu.h"
#include "cipher_loader.h"
#include "utils.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    try {
        printSeparator();
        cout << "   Шифры: ВИЖЕНЕР + СКИТАЛА" << endl;
        cout << "   Динамически подключаемые библиотеки" << endl;
        printSeparator();
        
        if (loadAllPlugins() == 0) {
            cout << "ОШИБКА: Не удалось загрузить плагины!" << endl;
            cout << "Убедитесь, что рядом с программой есть:" << endl;
            cout << "  - libvigenere.so" << endl;
            cout << "  - libskitala.so" << endl;
            return 1;
        }
        
        int choice, pluginIdx, isEncrypt, isFile;
        
        while (1) {
            showMainMenu();
            cin >> choice;
            clearInputBuffer();
            
            if (choice == 0) {
                cout << "\nДо свидания!" << endl;
                break;
            }
            
            if (choice < 1 || choice > 4) {
                cout << "Неверный выбор!" << endl;
                continue;
            }
            
            isFile = (choice == 3 || choice == 4);
            isEncrypt = (choice == 1 || choice == 3);
            
            cout << "\nВыберите алгоритм (1-" << getPluginsCount() << "): ";
            cin >> pluginIdx;
            clearInputBuffer();
            
            if (pluginIdx < 1 || pluginIdx > getPluginsCount()) {
                cout << "Неверный выбор!" << endl;
                continue;
            }
            
            if (isFile) {
                handleFileMode(pluginIdx - 1, isEncrypt);
            } else {
                handleTextMode(pluginIdx - 1, isEncrypt);
            }
            
            waitForEnter();
        }
        
        unloadAllPlugins();
        
    } catch (...) {
        cout << "Критическая ошибка в программе!" << endl;
        return 1;
    }
    
    return 0;
}
