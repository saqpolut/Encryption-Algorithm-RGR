#include "auth.h"
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

static char defaultPassword[64] = "admin123";

void setDefaultPassword(const char* pwd) {
    if (pwd && strlen(pwd) < sizeof(defaultPassword)) {
        strcpy(defaultPassword, pwd);
    }
}

bool login() {
    string input;
    int attempts = 3;
    
    cout << "\n╔════════════════════════════════════════════════════╗" << endl;
    cout << "║                    АВТОРИЗАЦИЯ                     ║" << endl;
    cout << "╚════════════════════════════════════════════════════╝" << endl;
    cout << "Пароль (по умолчанию: admin123):" << endl;
    
    while (attempts > 0) {
        cout << "> ";
        getline(cin, input);
        
        if (input == defaultPassword) {
            cout << "Доступ разрешен." << endl;
            return true;
        }
        
        attempts--;
        if (attempts > 0) {
            cout << "Неверно. Осталось попыток: " << attempts << endl;
        }
    }
    
    return false;
}