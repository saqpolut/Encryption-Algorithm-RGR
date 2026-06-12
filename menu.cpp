
#include "menu.h"
#include "cipher_loader.h"
#include "key_manager.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Функция для преобразования HEX строки в байты
int hexToBytes(const char* hex, unsigned char* bytes, int maxBytes) {
    int len = strlen(hex);
    int byteCount = 0;
    
    for (int i = 0; i < len && byteCount < maxBytes; i++) {
        // Пропускаем пробелы
        if (hex[i] == ' ') continue;
        
        // Проверяем, что есть два символа
        if (i + 1 >= len) break;
        
        char hexByte[3] = {hex[i], hex[i+1], '\0'};
        bytes[byteCount] = (unsigned char)strtol(hexByte, NULL, 16);
        byteCount++;
        i++; // Пропускаем второй символ
    }
    
    return byteCount;
}

void showMainMenu() {
    printSeparator();
    printf("     ПРОГРАММА ШИФРОВАНИЯ\n");
    printf("1. Шифрование текста\n");
    printf("2. Дешифрование текста\n");
    printf("3. Шифрование файла\n");
    printf("4. Дешифрование файла\n");
    printf("0. Выход\n");
    printSeparator();
    
    printf("\nДоступные алгоритмы (плагины):\n");
    for (int i = 0; i < getPluginsCount(); i++) {
        LoadedPlugin* p = getPlugin(i);
        printf("  %d. %s\n", i + 1, p->get_name());
    }
    printSeparator();
}

void handleTextMode(int pluginIndex, int isEncrypt) {
    LoadedPlugin* p = getPlugin(pluginIndex);
    if (p == 0) {
        printf("Ошибка: плагин не загружен\n");
        return;
    }
    
    char input[MAX_TEXT_LEN];
    char output[MAX_TEXT_LEN];
    char key[MAX_KEY_LEN];
    char hexInput[2 * MAX_TEXT_LEN];
    
    if (isEncrypt) {
        printf("\nВведите текст: ");
        fgets(input, MAX_TEXT_LEN, stdin);
        removeNewline(input);
    } else {
        printf("\nВведите зашифрованный текст (или HEX): ");
        fgets(hexInput, 2 * MAX_TEXT_LEN, stdin);
        removeNewline(hexInput);
        
        // Проверяем, похоже ли на HEX (содержит пробелы и шестнадцатеричные цифры)
        int looksLikeHex = 0;
        for (int i = 0; hexInput[i] != '\0'; i++) {
            if ((hexInput[i] >= '0' && hexInput[i] <= '9') ||
                (hexInput[i] >= 'A' && hexInput[i] <= 'F') ||
                (hexInput[i] >= 'a' && hexInput[i] <= 'f')) {
                looksLikeHex = 1;
            }
        }
        
        if (looksLikeHex && strchr(hexInput, ' ') != NULL) {
            // Похоже на HEX строку (есть пробелы и hex-цифры)
            unsigned char bytes[MAX_TEXT_LEN];
            int byteLen = hexToBytes(hexInput, bytes, MAX_TEXT_LEN);
            for (int i = 0; i < byteLen; i++) {
                input[i] = bytes[i];
            }
            input[byteLen] = '\0';
            printf("HEX распознан, преобразовано %d байт\n", byteLen);
        } else {
            // Обычный текст
            strcpy(input, hexInput);
        }
    }
    
    if (strlen(input) == 0) {
        printf("Ошибка: текст не может быть пустым!\n");
        return;
    }
    
    printf("\n%s\n", p->get_key_hint());
    printf("Введите ключ (или 'gen' для генерации): ");
    fgets(key, MAX_KEY_LEN, stdin);
    removeNewline(key);
    
    // Генерация ключа через key_manager
    if (strcmp(key, "gen") == 0 || strcmp(key, "GEN") == 0) {
        int isSkitala = (strcmp(p->get_name(), "Скитала") == 0);
        generateKey(key, MAX_KEY_LEN, isSkitala);
        printf("Сгенерирован ключ: %s\n", key);
    }
    
    if (!p->is_key_valid(key)) {
        printf("Ошибка: неверный ключ\n");
        return;
    }
    
    printf("\n--- %s ---\n", isEncrypt ? "ШИФРОВАНИЕ" : "ДЕШИФРОВАНИЕ");
    printf("Алгоритм: %s\n", p->get_name());
    
    if (isEncrypt) {
        p->encrypt_text(input, output, key);
    } else {
        p->decrypt_text(input, output, key);
    }
    
    int len = strlen(output);
    printf("Результат (текст): %s\n", output);
    printf("Результат (HEX): ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", (unsigned char)output[i]);
    }
    printf("\n");
}

void handleFileMode(int pluginIndex, int isEncrypt) {
    LoadedPlugin* p = getPlugin(pluginIndex);
    if (p == 0) {
        printf("Ошибка: плагин не загружен\n");
        return;
    }
    
    char inputPath[MAX_PATH_LEN];
    char outputPath[MAX_PATH_LEN];
    char key[MAX_KEY_LEN];
    
    printf("\n");
    printf("Введите путь к исходному файлу: ");
    fgets(inputPath, MAX_PATH_LEN, stdin);
    removeNewline(inputPath);
    
    printf("Введите путь к результирующему файлу: ");
    fgets(outputPath, MAX_PATH_LEN, stdin);
    removeNewline(outputPath);
    
    printf("\n%s\n", p->get_key_hint());
    printf("Введите ключ (или 'gen' для генерации): ");
    fgets(key, MAX_KEY_LEN, stdin);
    removeNewline(key);
    
    if (strcmp(key, "gen") == 0 || strcmp(key, "GEN") == 0) {
        int isSkitala = (strcmp(p->get_name(), "Скитала") == 0);
        generateKey(key, MAX_KEY_LEN, isSkitala);
        printf("Сгенерирован ключ: %s\n", key);
    }
    
    if (!p->is_key_valid(key)) {
        printf("Ошибка: неверный ключ\n");
        return;
    }
    
    FILE* in = fopen(inputPath, "rb");
    if (in == 0) {
        printf("Ошибка: не удалось открыть файл %s\n", inputPath);
        return;
    }
    
    FILE* out = fopen(outputPath, "wb");
    if (out == 0) {
        printf("Ошибка: не удалось создать файл %s\n", outputPath);
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
    
    printf("\nОперация выполнена успешно!\n");
    printf("Результат: %s\n", outputPath);
}
