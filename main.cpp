#include "auth.h"
#include "cipher_loader.h"
#include "logic_rsa_enigma.h"
#include "plugin_loader_xor_tea.h"
#include "cryptoLibraryLoader_gronsfeld.h"
#include "CryptoLoader_chiffre.h"
#include "FileCrypto_chiffre.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <exception>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace Crypto;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ========== Вспомогательные функции для плагинов Виженер/Скитала ==========
void handleTextMode(int pluginIdx, bool isEncrypt) {
    LoadedPlugin* plugin = getPlugin(pluginIdx);
    if (!plugin) {
        cout << "Ошибка: плагин не найден!" << endl;
        return;
    }

    cout << "\n--- " << (isEncrypt ? "ШИФРОВАНИЕ" : "ДЕШИФРОВАНИЕ") << " ТЕКСТА ---" << endl;
    cout << "Алгоритм: " << plugin->get_name() << endl;
    cout << "Подсказка по ключу: " << plugin->get_key_hint() << endl;

    char key[256];
    cout << "Введите ключ: ";
    cin.getline(key, sizeof(key));

    if (!plugin->is_key_valid(key)) {
        cout << "Ошибка: недопустимый ключ!" << endl;
        return;
    }

    char input[4096];
    cout << "Введите текст: ";
    cin.getline(input, sizeof(input));

    char output[8192]; // достаточный буфер для результата
    if (isEncrypt)
        plugin->encrypt_text(input, output, key);
    else
        plugin->decrypt_text(input, output, key);

    cout << "Результат: " << output << endl;
}

void handleFileMode(int pluginIdx, bool isEncrypt) {
    LoadedPlugin* plugin = getPlugin(pluginIdx);
    if (!plugin) {
        cout << "Ошибка: плагин не найден!" << endl;
        return;
    }

    cout << "\n--- " << (isEncrypt ? "ШИФРОВАНИЕ" : "ДЕШИФРОВАНИЕ") << " ФАЙЛА ---" << endl;
    cout << "Алгоритм: " << plugin->get_name() << endl;
    cout << "Подсказка по ключу: " << plugin->get_key_hint() << endl;

    char key[256];
    cout << "Введите ключ: ";
    cin.getline(key, sizeof(key));

    if (!plugin->is_key_valid(key)) {
        cout << "Ошибка: недопустимый ключ!" << endl;
        return;
    }

    char inPath[512], outPath[512];
    cout << "Входной файл: ";
    cin.getline(inPath, sizeof(inPath));
    cout << "Выходной файл: ";
    cin.getline(outPath, sizeof(outPath));

    FILE* inFile = fopen(inPath, "rb");
    if (!inFile) {
        cout << "Ошибка: не удалось открыть " << inPath << endl;
        return;
    }
    FILE* outFile = fopen(outPath, "wb");
    if (!outFile) {
        cout << "Ошибка: не удалось создать " << outPath << endl;
        fclose(inFile);
        return;
    }

    const size_t BUFFER_SIZE = 4096;
    char* inBuf = new char[BUFFER_SIZE];
    char* outBuf = new char[BUFFER_SIZE];

    size_t bytesRead;
    while ((bytesRead = fread(inBuf, 1, BUFFER_SIZE, inFile)) > 0) {
        if (isEncrypt)
            plugin->encrypt_block(inBuf, outBuf, bytesRead, key);
        else
            plugin->decrypt_block(inBuf, outBuf, bytesRead, key);
        fwrite(outBuf, 1, bytesRead, outFile);
    }

    delete[] inBuf;
    delete[] outBuf;
    fclose(inFile);
    fclose(outFile);

    cout << "Операция завершена. Результат: " << outPath << endl;
}

// ========== Подсистема 1: Виженер + Скитала ==========
void runVigenereSkitala() {
    cout << "\n========================================\n";
    cout << "    ПОДСИСТЕМА: ВИЖЕНЕР + СКИТАЛА\n";
    cout << "========================================\n";

    if (loadAllPlugins() == 0) {
        cout << "ОШИБКА: Не удалось загрузить плагины!" << endl;
        cout << "Убедитесь, что рядом с программой есть:" << endl;
        cout << "  - libvigenere.so (или .dll)" << endl;
        cout << "  - libskitala.so  (или .dll)" << endl;
        return;
    }

    int choice, pluginIdx, isEncrypt, isFile;

    while (true) {
        cout << "\n--- МЕНЮ ВИЖЕНЕР/СКИТАЛА ---" << endl;
        cout << "1. Шифрование текста" << endl;
        cout << "2. Дешифрование текста" << endl;
        cout << "3. Шифрование файла" << endl;
        cout << "4. Дешифрование файла" << endl;
        cout << "0. Выход в главное меню" << endl;
        cout << "Выбор: ";

        cin >> choice;
        clearInput();

        if (choice == 0) break;

        if (choice < 1 || choice > 4) {
            cout << "Неверный выбор!" << endl;
            continue;
        }

        isFile = (choice == 3 || choice == 4);
        isEncrypt = (choice == 1 || choice == 3);

        cout << "\nВыберите алгоритм (1-" << getPluginsCount() << "): ";
        cin >> pluginIdx;
        clearInput();

        if (pluginIdx < 1 || pluginIdx > getPluginsCount()) {
            cout << "Неверный выбор!" << endl;
            continue;
        }

        if (isFile)
            handleFileMode(pluginIdx - 1, isEncrypt);
        else
            handleTextMode(pluginIdx - 1, isEncrypt);

        cout << "\nНажмите Enter для продолжения...";
        cin.get();
    }

    unloadAllPlugins();
}

// ========== Подсистема 2: RSA + Enigma ==========
void runRSAEnigma() {
    cout << "\n========================================\n";
    cout << "    ПОДСИСТЕМА: RSA + ENIGMA\n";
    cout << "========================================\n";
    run_application();
}

// ========== Подсистема 3: XOR + TEA ==========
void runXorTea() {
    cout << "\n========================================\n";
    cout << "    ПОДСИСТЕМА: XOR + TEA\n";
    cout << "========================================\n";

    bool exitSub = false;
    int ch;
    char algo[10];
    char key[256];
    size_t len;
    vector<unsigned int> input, output;

    while (!exitSub) {
        cout << "\n1. Выбрать алгоритм" << endl;
        cout << "2. Показать текущий" << endl;
        cout << "3. Сгенерировать ключ" << endl;
        cout << "4. Обработать данные" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> ch;
        clearInput();

        if (ch == 0) break;

        if (ch == 1) {
            cout << "XOR или TEA: ";
            cin.getline(algo, 10);
            load_plugin(algo);
            cout << "Выбран: " << get_plugin_name() << endl;
        }
        else if (ch == 2) {
            cout << "Текущий алгоритм: " << get_plugin_name() << endl;
        }
        else if (ch == 3) {
            generate_key(key, sizeof(key));
            cout << "Сгенерированный ключ: " << key << endl;
        }
        else if (ch == 4) {
            cout << "Длина массива: ";
            cin >> len;
            clearInput();

            input.resize(len);
            output.resize(len);

            cout << "Введите " << len << " чисел: ";
            for (size_t i = 0; i < len; ++i) {
                cin >> input[i];
            }
            clearInput();

            cout << "Введите ключ: ";
            cin.getline(key, sizeof(key));

            process_data(input.data(), len, key, output.data());

            cout << "Результат: ";
            for (size_t i = 0; i < len; ++i) {
                cout << output[i] << " ";
            }
            cout << endl;
        }
    }
}

// ========== Подсистема 4: Gronsfeld ==========
void runGronsfeld() {
    cout << "\n========================================\n";
    cout << "    ПОДСИСТЕМА: GRONSFELD\n";
    cout << "========================================\n";

    CryptoLibraryLoader loader;
    string libPath;

#ifdef _WIN32
    libPath = "gronsfeld_plugin.dll";
#else
    libPath = "./libgronsfeld.so";
#endif

    if (!loader.load(libPath)) {
        cout << "ОШИБКА: Не удалось загрузить плагин Gronsfeld" << endl;
        return;
    }

    cout << "Алгоритм: " << loader.getAlgorithmName() << endl;

    bool exitSub = false;
    int ch;
    char key[256];
    char inputText[4096];
    char* outputText = nullptr;
    int keyLen;

    while (!exitSub) {
        cout << "\n1. Сгенерировать ключ" << endl;
        cout << "2. Зашифровать текст" << endl;
        cout << "3. Расшифровать текст" << endl;
        cout << "4. Проверить ключ" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> ch;
        clearInput();

        if (ch == 0) break;

        if (ch == 1) {
            cout << "Введите длину ключа: ";
            cin >> keyLen;
            clearInput();
            if (loader.generateKey(key, sizeof(key), keyLen) > 0) {
                cout << "Сгенерированный ключ: " << key << endl;
            } else {
                cout << "Ошибка генерации ключа" << endl;
            }
        }
        else if (ch == 2 || ch == 3) {
            cout << "Введите текст: ";
            cin.getline(inputText, sizeof(inputText));

            cout << "Введите ключ (цифры): ";
            cin.getline(key, sizeof(key));

            if (!loader.validateKey(key)) {
                cout << "ОШИБКА: Неверный ключ. Ключ должен состоять только из цифр." << endl;
                continue;
            }

            int res;
            if (ch == 2) {
                res = loader.encrypt(inputText, (int)strlen(inputText), key, &outputText);
                cout << "Зашифрованный текст: ";
            } else {
                res = loader.decrypt(inputText, (int)strlen(inputText), key, &outputText);
                cout << "Расшифрованный текст: ";
            }

            if (res > 0 && outputText) {
                cout << outputText << endl;
                loader.freeMemory(outputText);
                outputText = nullptr;
            } else {
                cout << "Ошибка обработки" << endl;
            }
        }
        else if (ch == 4) {
            cout << "Введите ключ: ";
            cin.getline(key, sizeof(key));
            cout << (loader.validateKey(key) ? "Ключ корректен" : "Ключ НЕ корректен") << endl;
        }
    }

    loader.unload();
}

// ========== Подсистема 5: GrandChiffre ==========
void runGrandChiffre() {
    cout << "\n========================================\n";
    cout << "    ПОДСИСТЕМА: GRANDCHIFFRE\n";
    cout << "========================================\n";

    string libPath;

#ifdef _WIN32
    libPath = "grandchiffre_plugin.dll";
#else
    libPath = "./libgrandchiffre.so";
#endif

    if (!load_plugin(libPath)) {
        cout << "ОШИБКА: Не удалось загрузить плагин GrandChiffre" << endl;
        return;
    }

    ICryptoAlgorithm* alg = get_algorithm();
    if (!alg) {
        cout << "ОШИБКА: Не удалось получить экземпляр алгоритма" << endl;
        return;
    }

    cout << "Длина ключа: от " << alg->getMinKeyLength() << " до " << alg->getMaxKeyLength() << " байт" << endl;

    bool exitSub = false;
    int ch;
    int mode;
    size_t keySize;
    string inputText;
    string hexKey;
    string inPath, outPath;
    vector<unsigned char> keyBytes;
    vector<unsigned char> data;
    vector<unsigned char> fileBuffer;
    FILE* inFile = nullptr;
    FILE* outFile = nullptr;
    size_t bytesRead;
    size_t i;
    char hex[3];

    while (!exitSub) {
        cout << "\n1. Сгенерировать случайный ключ" << endl;
        cout << "2. Зашифровать/расшифровать текст" << endl;
        cout << "3. Зашифровать/расшифровать файл" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> ch;
        clearInput();

        if (ch == 0) break;

        if (ch == 1) {
            cout << "Введите длину ключа (" << alg->getMinKeyLength() << "-" << alg->getMaxKeyLength() << "): ";
            cin >> keySize;
            clearInput();

            vector<unsigned char> newKey = alg->generateKey(keySize);
            cout << "Сгенерированный ключ (HEX): ";
            for (i = 0; i < newKey.size(); ++i) {
                printf("%02X ", newKey[i]);
            }
            cout << endl;
        }
        else if (ch == 2) {
            cout << "Режим (1 - шифрование, 2 - дешифрование): ";
            cin >> mode;
            clearInput();

            cout << "Введите текст: ";
            getline(cin, inputText);

            cout << "Введите ключ в HEX (например, 01 02 03): ";
            getline(cin, hexKey);

            keyBytes.clear();
            for (i = 0; i < hexKey.length(); ) {
                if (hexKey[i] == ' ') {
                    ++i;
                    continue;
                }
                if (i + 1 >= hexKey.length()) break;
                hex[0] = hexKey[i];
                hex[1] = hexKey[i+1];
                hex[2] = '\0';
                keyBytes.push_back((unsigned char)strtoul(hex, nullptr, 16));
                i += 2;
            }

            data.assign(inputText.begin(), inputText.end());

            try {
                alg->process(keyBytes, data);
                cout << "Результат (HEX): ";
                for (i = 0; i < data.size(); ++i) {
                    printf("%02X ", data[i]);
                }
                cout << endl;
            } catch (const exception& e) {
                cout << "Ошибка: " << e.what() << endl;
            }
        }
        else if (ch == 3) {
            cout << "Режим (1 - шифрование, 2 - дешифрование): ";
            cin >> mode;
            clearInput();

            cout << "Входной файл: ";
            getline(cin, inPath);

            cout << "Выходной файл: ";
            getline(cin, outPath);

            cout << "Введите ключ в HEX: ";
            getline(cin, hexKey);

            keyBytes.clear();
            for (i = 0; i < hexKey.length(); ) {
                if (hexKey[i] == ' ') {
                    ++i;
                    continue;
                }
                if (i + 1 >= hexKey.length()) break;
                hex[0] = hexKey[i];
                hex[1] = hexKey[i+1];
                hex[2] = '\0';
                keyBytes.push_back((unsigned char)strtoul(hex, nullptr, 16));
                i += 2;
            }

            inFile = fopen(inPath.c_str(), "rb");
            if (!inFile) {
                cout << "Ошибка: не удалось открыть файл " << inPath << endl;
                continue;
            }

            outFile = fopen(outPath.c_str(), "wb");
            if (!outFile) {
                cout << "Ошибка: не удалось создать файл " << outPath << endl;
                fclose(inFile);
                continue;
            }

            fileBuffer.resize(65536);

            while ((bytesRead = fread(fileBuffer.data(), 1, 65536, inFile)) > 0) {
                vector<unsigned char> chunk(fileBuffer.begin(), fileBuffer.begin() + bytesRead);
                alg->process(keyBytes, chunk);
                fwrite(chunk.data(), 1, chunk.size(), outFile);
            }

            fclose(inFile);
            fclose(outFile);

            cout << "Операция выполнена успешно. Результат: " << outPath << endl;
        }
    }

    unload_plugin();
}

// ========== Главное меню ==========
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    if (!login()) {
        cout << "\nНеверный пароль. Программа завершает работу." << endl;
        return 1;
    }

    bool running = true;
    int choice;

    while (running) {
        cout << "\n╔════════════════════════════════════════════════════╗" << endl;
        cout << "║     Encryption-Algorithm-RGR                       ║" << endl;
        cout << "╠════════════════════════════════════════════════════╣" << endl;
        cout << "║  1. Виженер + Скитала                              ║" << endl;
        cout << "║  2. RSA + Enigma                                   ║" << endl;
        cout << "║  3. XOR + TEA                                      ║" << endl;
        cout << "║  4. Gronsfeld                                      ║" << endl;
        cout << "║  5. GrandChiffre                                   ║" << endl;
        cout << "║  0. Выход                                          ║" << endl;
        cout << "╚════════════════════════════════════════════════════╝" << endl;
        cout << "Выбор: ";

        cin >> choice;
        clearInput();

        switch (choice) {
            case 1: runVigenereSkitala(); break;
            case 2: runRSAEnigma();       break;
            case 3: runXorTea();          break;
            case 4: runGronsfeld();       break;
            case 5: runGrandChiffre();    break;
            case 0:
                running = false;
                cout << "\nДо свидания!" << endl;
                break;
            default:
                cout << "Неверный выбор!" << endl;
        }
    }

    return 0;
}