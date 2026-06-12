#include "menu.h"
#include "cipher_loader.h"
#include "key_manager.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

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
    
    	printf("\nВведите текст: ");
    	fgets(input, MAX_TEXT_LEN, stdin);
    	removeNewline(input);
    
    	if (isEmpty(input)) {
        	printf("Ошибка: текст не может быть пустым!\n");
        	return;
    	}
    
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
    
    	printf("\n--- %s ---\n", isEncrypt ? "ШИФРОВАНИЕ" : "ДЕШИФРОВАНИЕ");
    	printf("Алгоритм: %s\n", p->get_name());
    
    	if (isEncrypt) {
        	p->encrypt_text(input, output, key);
    	} else {
        	p->decrypt_text(input, output, key);
    	}
    
    	printf("Результат: %s\n", output);
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
