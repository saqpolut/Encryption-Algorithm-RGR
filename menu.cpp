#include "menu.h"
#include "cipher_loader.h"
#include "key_manager.h"
#include "utils.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

int hexToBytes(const char* hex, unsigned char* bytes, int maxBytes) {
    int len = strlen(hex);
    int byteCount = 0;
    
    for (int i = 0; i < len && byteCount < maxBytes; i++) {
        if (hex[i] == ' ') continue;
        
        if (i + 1 >= len) break;
        
        char hexByte[3] = {hex[i], hex[i+1], '\0'};
        bytes[byteCount] = (unsigned char)strtol(hexByte, NULL, 16);
        byteCount++;
        i++;
    }
    
    return byteCount;
}

void showMainMenu() {
    printSeparator();
    cout << "     ПРОГРАММА ШИФРОВАНИЯ" << endl;
    cout << "1. Шифрование текста" << endl;
    cout << "2. Дешифрование текста" << endl;
    cout << "3. Шифрование файла" << endl;
    cout << "4. Дешифрование файла" << endl;
    cout << "0. Выход" << endl;
    printSeparator();
    
    cout << "\nДоступные алгоритмы (плагины):" << endl;
    for (int i = 0; i < getPluginsCount(); i++) {
        LoadedPlugin* p = getPlugin(i);
        cout << "  " << (i + 1) << ". " << p->get_name() << endl;
    }
    printSeparator();
}

void handleTextMode(int pluginIndex, int isEncrypt) {
    LoadedPlugin* p = getPlugin(pluginIndex);
    if (p == nullptr) {
        cout << "Ошибка: плагин не загружен" << endl;
        return;
    }
    
    char input[MAX_TEXT_LEN];
    char output[MAX_TEXT_LEN];
    char key[MAX_KEY_LEN];
    char hexInput[2 * MAX_TEXT_LEN];
    
    if (isEncrypt) {
        cout << "\nВведите текст: ";
        cin.getline(input, MAX_TEXT_LEN);
    } else {
        cout << "\nВведите зашифрованный текст (или HEX): ";
        cin.getline(hexInput, 2 * MAX_TEXT_LEN);
        
        int looksLikeHex = 0;
        for (int i = 0; hexInput[i] != '\0'; i++) {
            if ((hexInput[i] >= '0' && hexInput[i] <= '9') ||
                (hexInput[i] >= 'A' && hexInput[i] <= 'F') ||
                (hexInput[i] >= 'a' && hexInput[i] <= 'f')) {
                looksLikeHex = 1;
            }
        }
        
        if (looksLikeHex && strchr(hexInput, ' ') != NULL) {
            unsigned char bytes[MAX_TEXT_LEN];
            int byteLen = hexToBytes(hexInput, bytes, MAX_TEXT_LEN);
            for (int i = 0; i < byteLen; i++) {
                input[i] = bytes[i];
            }
            input[byteLen] = '\0';
            cout << "HEX распознан, преобразовано " << byteLen << " байт" << endl;
        } else {
            strcpy(input, hexInput);
        }
    }
    
    if (strlen(input) == 0) {
        cout << "Ошибка: текст не может быть пустым!" << endl;
        return;
    }
    
    cout << "\n" << p->get_key_hint() << endl;
    cout << "Введите ключ (или 'gen' для генерации): ";
    cin.getline(key, MAX_KEY_LEN);
    
    if (strcmp(key, "gen") == 0 || strcmp(key, "GEN") == 0) {
        int isSkitala = (strcmp(p->get_name(), "Скитала") == 0);
        generateKey(key, MAX_KEY_LEN, isSkitala);
        cout << "Сгенерирован ключ: " << key << endl;
    }
    
    if (!p->is_key_valid(key)) {
        cout << "Ошибка: неверный ключ" << endl;
        return;
    }
    
    cout << "\n--- " << (isEncrypt ? "ШИФРОВАНИЕ" : "ДЕШИФРОВАНИЕ") << " ---" << endl;
    cout << "Алгоритм: " << p->get_name() << endl;
    
    if (isEncrypt) {
        p->encrypt_text(input, output, key);
    } else {
        p->decrypt_text(input, output, key);
    }
    
    int len = strlen(output);
    cout << "Результат (текст): " << output << endl;
    cout << "Результат (HEX): ";
    for (int i = 0; i < len; i++) {
        cout << hex << uppercase << (int)(unsigned char)output[i] << dec << " ";
    }
    cout << endl;
}

void handleFileMode(int pluginIndex, int isEncrypt) {
    LoadedPlugin* p = getPlugin(pluginIndex);
    if (p == nullptr) {
        cout << "Ошибка: плагин не загружен" << endl;
        return;
    }
    
    char inputPath[MAX_PATH_LEN];
    char outputPath[MAX_PATH_LEN];
    char key[MAX_KEY_LEN];
    
    cout << "\n";
    cout << "Введите путь к исходному файлу: ";
    cin.getline(inputPath, MAX_PATH_LEN);
    
    cout << "Введите путь к результирующему файлу: ";
    cin.getline(outputPath, MAX_PATH_LEN);
    
    cout << "\n" << p->get_key_hint() << endl;
    cout << "Введите ключ (или 'gen' для генерации): ";
    cin.getline(key, MAX_KEY_LEN);
    
    if (strcmp(key, "gen") == 0 || strcmp(key, "GEN") == 0) {
        int isSkitala = (strcmp(p->get_name(), "Скитала") == 0);
        generateKey(key, MAX_KEY_LEN, isSkitala);
        cout << "Сгенерирован ключ: " << key << endl;
    }
    
    if (!p->is_key_valid(key)) {
        cout << "Ошибка: неверный ключ" << endl;
        return;
    }
    
    FILE* in = fopen(inputPath, "rb");
    if (in == nullptr) {
        cout << "Ошибка: не удалось открыть файл " << inputPath << endl;
        return;
    }
    
    FILE* out = fopen(outputPath, "wb");
    if (out == nullptr) {
        cout << "Ошибка: не удалось создать файл " << outputPath << endl;
        fclose(in);
        return;
    }
    
    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];
    size_t bytesRead;
    
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        if (isEncrypt) {
            p->encrypt_block(buffer, result, bytesRead, key);
        } else {
            p->decrypt_block(buffer, result, bytesRead, key);
        }
        fwrite(result, 1, bytesRead, out);
    }
    
    fclose(in);
    fclose(out);
    
    cout << "\nОперация выполнена успешно!" << endl;
    cout << "Результат: " << outputPath << endl;
}
