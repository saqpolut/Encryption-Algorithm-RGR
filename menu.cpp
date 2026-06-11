#include "menu.h"
#include "cipher_loader.h"
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
    	printf("5. Сгенерировать случайный ключ\n");
    	printf("0. Выход\n");
    	printSeparator();
    	// Вывод списка всех алгоритмов
    	printf("\nДоступные алгоритмы (плагины):\n");
    	for (int i = 0; i < getPluginsCount(); i++) {
        	printf("  %d. %s\n", i + 1, getPlugin(i)->plugin->getName());
    	}
    	printSeparator();
}
// Консольный ввод/вывод
void handleTextMode(int pluginIndex, int isEncrypt) {
	// блок обработки исключений
    	try {
        	LoadedPlugin* loaded = getPlugin(pluginIndex); // загруженный плагин по индексу
        	if (!loaded || !loaded->plugin) {
            		printf("Ошибка: плагин не загружен\n");
            		return;
        	}
        
        	CipherPlugin* p = loaded->plugin;
        	// ввод/вывод и ключ
        	char input[MAX_TEXT_LEN];
        	char output[MAX_TEXT_LEN];
        	char key[MAX_KEY_LEN];

        	// запрос исходного текста
        	printf("\nВведите текст: ");
        	fgets(input, MAX_TEXT_LEN, stdin);
        	removeNewline(input);
        
        	if (isEmpty(input)) {
            		printf("Ошибка: текст не может быть пустым!\n");
            		return;
        	}
        	
		// запрос ключа
        	printf("\n%s\n", p->getKeyHint());
        	printf("Введите ключ (или 'gen' для генерации): ");
        	fgets(key, MAX_KEY_LEN, stdin);
        	removeNewline(key);
        
        	// Генерация ключа
        	if (strcmp(key, "gen") == 0 || strcmp(key, "GEN") == 0) {
            		generateRandomKey(key, MAX_KEY_LEN);
            		printf("Сгенерирован ключ: %s\n", key);
        	}
        
        	if (!p->isKeyValid(key)) {
            		printf("Ошибка: неверный ключ\n");
            		return;
        	}
        
        	printf("\n--- %s ---\n", isEncrypt ? "ШИФРОВАНИЕ" : "ДЕШИФРОВАНИЕ");
        	printf("Алгоритм: %s\n", p->getName());
        	
		// вызов нужной функции плагина
        	if (isEncrypt) {
            		p->encryptText(input, output, key);
        	} else {
            		p->decryptText(input, output, key);
        	}
        
        	printf("Результат: %s\n", output);
        
    	} catch (const char* msg) {
        	printf("Ошибка: %s\n", msg);
    	} catch (...) {
        	printf("Неизвестная ошибка при обработке текста!\n");
    	}
}

// бинарная обработка
void handleFileMode(int pluginIndex, int isEncrypt) {
    	try {
        	LoadedPlugin* loaded = getPlugin(pluginIndex);
        	if (!loaded || !loaded->plugin) {
            		printf("Ошибка: плагин не загружен\n");
            		return;
        	}
        
        	CipherPlugin* p = loaded->plugin;
        
        	char inputPath[MAX_PATH_LEN]; // путь к исходному файлу
        	char outputPath[MAX_PATH_LEN]; // путь к результату
        	char key[MAX_KEY_LEN]; // ключ шифрования
        
        	printf("\n");
        	printf("Введите путь к исходному файлу: ");
        	fgets(inputPath, MAX_PATH_LEN, stdin);
        	removeNewline(inputPath);
        
        	printf("Введите путь к результирующему файлу: ");
        	fgets(outputPath, MAX_PATH_LEN, stdin);
        	removeNewline(outputPath);
        
        	printf("\n%s\n", p->getKeyHint());
        	printf("Введите ключ (или 'gen' для генерации): ");
        	fgets(key, MAX_KEY_LEN, stdin);
        	removeNewline(key);
        
        	// Генерация ключа
        	if (strcmp(key, "gen") == 0 || strcmp(key, "GEN") == 0) {
            		generateRandomKey(key, MAX_KEY_LEN);
            		printf("Сгенерирован ключ: %s\n", key);
        	}
        
        	if (!p->isKeyValid(key)) {
            		printf("Ошибка: неверный ключ\n");
            		return;
        	}
        
		// открытие входного файла в бинарном режиме
        	FILE* in = fopen(inputPath, "rb");
        	if (!in) {
            		printf("Ошибка: не удалось открыть файл %s\n", inputPath);
            		return;
        	}
        
		// создание выходного файла в бинарном режиме
        	FILE* out = fopen(outputPath, "wb");
        	if (!out) {
            		printf("Ошибка: не удалось создать файл %s\n", outputPath);
            		fclose(in);
            		return;
        	}
        
        	unsigned char buffer[BUFFER_SIZE]; // чтение
        	unsigned char result[BUFFER_SIZE];
        	size_t bytesRead;
        
        	while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
            		if (isEncrypt) {
                		p->encryptBlock(buffer, result, bytesRead, key);
            		} else {
                		p->decryptBlock(buffer, result, bytesRead, key);
            		}
            		fwrite(result, 1, bytesRead, out);
        	}
        
        	fclose(in);
        	fclose(out);
        
        	printf("\nОперация выполнена успешно!\n");
        	printf("Результат: %s\n", outputPath);
        
    	} catch (const char* msg) {
        	printf("Ошибка: %s\n", msg);
    	} catch (...) {
        	printf("Неизвестная ошибка при обработке файла!\n");
    	}	
}
