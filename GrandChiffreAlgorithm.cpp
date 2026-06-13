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
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace Crypto;

// ========== Вспомогательные функции ==========
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ========== Подсистема 1: Виженер + Скитала ==========
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

    char output[8192];
    if (isEncrypt)
        plugin->encrypt_text(input, output, key);
    else
        plugin->decrypt_text(input, output, key);

    cout << "Результат: " << output << endl;
    cout << "Результат (HEX): ";
    for (int i = 0; output[i] != '\0'; ++i) {
        printf("%02X ", (unsigned char)output[i]);
    }
    cout << endl;
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

// Перечисление для меню Виженер/Скитала
enum VigenereMenuAction {
    VIGENERE_EXIT = 0,
    VIGENERE_ENCRYPT_TEXT = 1,
    VIGENERE_DECRYPT_TEXT = 2,
    VIGENERE_ENCRYPT_FILE = 3,
    VIGENERE_DECRYPT_FILE = 4
};

void runVigenereSkitala() {
    cout << "\n========================================\n";
    cout << "    ПОДСИСТЕМА: ВИЖЕНЕР + СКИТАЛА\n";
    cout << "========================================\n";

    if (loadAllPlugins() == 0) {
        cout << "ОШИБКА: Не удалось загрузить плагины!" << endl;
        cout << "Убедитесь, что рядом с программой есть:" << endl;
        cout << "  - vigenere_plugin.dll" << endl;
        cout << "  - skitala_plugin.dll" << endl;
        return;
    }

    int choice, pluginIdx;
    bool isEncrypt, isFile;

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

        if (choice == VIGENERE_EXIT) break;

        if (choice < VIGENERE_ENCRYPT_TEXT || choice > VIGENERE_DECRYPT_FILE) {
            cout << "Неверный выбор!" << endl;
            continue;
        }

        isFile = (choice == VIGENERE_ENCRYPT_FILE || choice == VIGENERE_DECRYPT_FILE);
        isEncrypt = (choice == VIGENERE_ENCRYPT_TEXT || choice == VIGENERE_ENCRYPT_FILE);

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
vector<uint32_t> bytesToUint32(const vector<uint8_t>& bytes, size_t blockSize) {
    vector<uint32_t> result;
    size_t numBlocks = (bytes.size() + blockSize - 1) / blockSize;
    result.reserve(numBlocks * (blockSize / 4));
    
    for (size_t i = 0; i < bytes.size(); i += blockSize) {
        uint32_t chunk[2] = {0, 0};
        for (size_t j = 0; j < blockSize && i + j < bytes.size(); ++j) {
            int bytePos = j % 4;
            int wordIdx = j / 4;
            chunk[wordIdx] |= (static_cast<uint32_t>(bytes[i + j]) << (bytePos * 8));
        }
        result.push_back(chunk[0]);
        if (blockSize == 8) result.push_back(chunk[1]);
    }
    return result;
}

vector<uint8_t> uint32ToBytes(const vector<uint32_t>& data, size_t originalSize) {
    vector<uint8_t> result;
    result.reserve(originalSize);
    for (size_t i = 0; i < data.size(); ++i) {
        uint32_t val = data[i];
        for (int j = 0; j < 4; ++j) {
            result.push_back(static_cast<uint8_t>((val >> (j * 8)) & 0xFF));
        }
    }
    if (result.size() > originalSize) result.resize(originalSize);
    return result;
}

string processTextXorTea(const string& input, const string& key, bool encrypt) {
    vector<uint8_t> inBytes(input.begin(), input.end());
    string algo = get_plugin_name();
    size_t blockSize = (algo == "TEA") ? 8 : 4;
    
    vector<uint32_t> inUint32 = bytesToUint32(inBytes, blockSize);
    vector<uint32_t> outUint32(inUint32.size());
    
    process_data(inUint32.data(), inUint32.size(), key.c_str(), outUint32.data());
    vector<uint8_t> outBytes = uint32ToBytes(outUint32, inBytes.size());
    return string(outBytes.begin(), outBytes.end());
}

bool processFileXorTea(const string& inPath, const string& outPath, const string& key, bool encrypt) {
    ifstream inFile(inPath, ios::binary);
    if (!inFile) {
        cerr << "Ошибка: не удалось открыть входной файл " << inPath << endl;
        return false;
    }
    vector<uint8_t> inBytes((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    if (inBytes.empty()) {
        cerr << "Входной файл пуст" << endl;
        return false;
    }
    string algo = get_plugin_name();
    size_t blockSize = (algo == "TEA") ? 8 : 4;
    vector<uint32_t> inUint32 = bytesToUint32(inBytes, blockSize);
    vector<uint32_t> outUint32(inUint32.size());
    process_data(inUint32.data(), inUint32.size(), key.c_str(), outUint32.data());
    vector<uint8_t> outBytes = uint32ToBytes(outUint32, inBytes.size());
    ofstream outFile(outPath, ios::binary);
    if (!outFile) {
        cerr << "Ошибка: не удалось создать выходной файл " << outPath << endl;
        return false;
    }
    outFile.write(reinterpret_cast<const char*>(outBytes.data()), outBytes.size());
    return true;
}

// Перечисление для меню XOR/TEA
enum XorTeaMenuAction {
    XOR_TEA_BACK = 0,
    XOR_TEA_SELECT_ALGO = 1,
    XOR_TEA_SHOW_ALGO = 2,
    XOR_TEA_GEN_KEY = 3,
    XOR_TEA_TEXT = 4,
    XOR_TEA_FILE = 5,
    XOR_TEA_ARRAY = 6
};

void runXorTea() {
    cout << "\n========================================\n";
    cout << "    ПОДСИСТЕМА: XOR + TEA\n";
    cout << "========================================\n";
    
    bool exitSub = false;
    int ch, subCh;
    char algo[10];
    char key[256];
    size_t len;
    vector<unsigned int> input, output;
    string text, processedText;
    string inPath, outPath;
    
    while (!exitSub) {
        cout << "\n--- МЕНЮ XOR/TEA ---\n";
        cout << "1. Выбрать алгоритм (XOR/TEA)\n";
        cout << "2. Показать текущий алгоритм\n";
        cout << "3. Сгенерировать ключ\n";
        cout << "4. Шифрование/дешифрование ТЕКСТА\n";
        cout << "5. Шифрование/дешифрование ФАЙЛА\n";
        cout << "6. Обработка массива чисел (старый режим)\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";
        
        cin >> ch;
        clearInput();
        
        if (ch == XOR_TEA_BACK) break;
        
        switch (ch) {
            case XOR_TEA_SELECT_ALGO:
                cout << "Введите алгоритм (XOR или TEA): ";
                cin.getline(algo, 10);
                load_plugin(algo);
                cout << "Текущий алгоритм: " << get_plugin_name() << endl;
                break;
            case XOR_TEA_SHOW_ALGO:
                cout << "Текущий алгоритм: " << get_plugin_name() << endl;
                break;
            case XOR_TEA_GEN_KEY:
                generate_key(key, sizeof(key));
                cout << "Сгенерированный ключ: " << key << endl;
                break;
            case XOR_TEA_TEXT: {
                cout << "\n1. Шифрование текста\n2. Дешифрование текста\nВыбор: ";
                cin >> subCh;
                clearInput();
                if (subCh != 1 && subCh != 2) {
                    cout << "Неверный выбор.\n";
                    break;
                }
                bool encrypt = (subCh == 1);
                cout << "Введите текст: ";
                getline(cin, text);
                cout << "Введите ключ: ";
                cin.getline(key, sizeof(key));
                try {
                    processedText = processTextXorTea(text, key, encrypt);
                    cout << "\nРезультат (" << (encrypt ? "зашифровано" : "расшифровано") << "):\n";
                    cout << "HEX: ";
                    for (unsigned char c : processedText) {
                        printf("%02X ", c);
                    }
                    cout << "\nТекст: " << processedText << endl;
                } catch (const exception& e) {
                    cerr << "Ошибка: " << e.what() << endl;
                }
                break;
            }
            case XOR_TEA_FILE: {
                cout << "\n1. Шифрование файла\n2. Дешифрование файла\nВыбор: ";
                cin >> subCh;
                clearInput();
                if (subCh != 1 && subCh != 2) {
                    cout << "Неверный выбор.\n";
                    break;
                }
                bool encrypt = (subCh == 1);
                cout << "Входной файл: ";
                getline(cin, inPath);
                cout << "Выходной файл: ";
                getline(cin, outPath);
                cout << "Введите ключ: ";
                cin.getline(key, sizeof(key));
                if (processFileXorTea(inPath, outPath, key, encrypt)) {
                    cout << "Операция выполнена успешно. Результат: " << outPath << endl;
                } else {
                    cout << "Ошибка при обработке файла.\n";
                }
                break;
            }
            case XOR_TEA_ARRAY: {
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
                break;
            }
            default:
                cout << "Неверный выбор.\n";
        }
    }
}

// ========== Подсистема 4: Gronsfeld ==========
bool processFileGronsfeld(CryptoLibraryLoader& loader, const string& inPath, const string& outPath, const string& key, bool encrypt) {
    ifstream inFile(inPath, ios::binary);
    if (!inFile) {
        cerr << "Ошибка: не удалось открыть входной файл " << inPath << endl;
        return false;
    }
    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    if (content.empty()) {
        cerr << "Входной файл пуст" << endl;
        return false;
    }
    char* outputText = nullptr;
    int result;
    if (encrypt) {
        result = loader.encrypt(content.c_str(), (int)content.size(), key.c_str(), &outputText);
    } else {
        result = loader.decrypt(content.c_str(), (int)content.size(), key.c_str(), &outputText);
    }
    if (result < 0 || !outputText) {
        cerr << "Ошибка обработки данных" << endl;
        return false;
    }
    ofstream outFile(outPath, ios::binary);
    if (!outFile) {
        cerr << "Ошибка: не удалось создать выходной файл " << outPath << endl;
        loader.freeMemory(outputText);
        return false;
    }
    outFile.write(outputText, result);
    outFile.close();
    loader.freeMemory(outputText);
    return true;
}

// Перечисление для меню Gronsfeld
enum GronsfeldMenuAction {
    GRONSFELD_BACK = 0,
    GRONSFELD_GEN_KEY = 1,
    GRONSFELD_ENCRYPT_TEXT = 2,
    GRONSFELD_DECRYPT_TEXT = 3,
    GRONSFELD_ENCRYPT_FILE = 4,
    GRONSFELD_DECRYPT_FILE = 5,
    GRONSFELD_VALIDATE_KEY = 6
};

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
    string inPath, outPath;

    while (!exitSub) {
        cout << "\n--- МЕНЮ GRONSFELD ---\n";
        cout << "1. Сгенерировать ключ\n";
        cout << "2. Зашифровать текст\n";
        cout << "3. Расшифровать текст\n";
        cout << "4. Зашифровать файл\n";
        cout << "5. Расшифровать файл\n";
        cout << "6. Проверить ключ\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";

        cin >> ch;
        clearInput();

        if (ch == GRONSFELD_BACK) break;

        switch (ch) {
            case GRONSFELD_GEN_KEY:
                cout << "Введите длину ключа: ";
                cin >> keyLen;
                clearInput();
                if (loader.generateKey(key, sizeof(key), keyLen) > 0) {
                    cout << "Сгенерированный ключ: " << key << endl;
                } else {
                    cout << "Ошибка генерации ключа" << endl;
                }
                break;

            case GRONSFELD_ENCRYPT_TEXT:
            case GRONSFELD_DECRYPT_TEXT: {
                bool encrypt = (ch == GRONSFELD_ENCRYPT_TEXT);
                cout << "Введите текст: ";
                cin.getline(inputText, sizeof(inputText));
                cout << "Введите ключ (цифры): ";
                cin.getline(key, sizeof(key));
                if (!loader.validateKey(key)) {
                    cout << "ОШИБКА: Неверный ключ. Ключ должен состоять только из цифр." << endl;
                    continue;
                }
                int res;
                if (encrypt) {
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
                break;
            }

            case GRONSFELD_ENCRYPT_FILE:
            case GRONSFELD_DECRYPT_FILE: {
                bool encrypt = (ch == GRONSFELD_ENCRYPT_FILE);
                cout << "Входной файл: ";
                getline(cin, inPath);
                cout << "Выходной файл: ";
                getline(cin, outPath);
                cout << "Введите ключ (цифры): ";
                cin.getline(key, sizeof(key));
                if (!loader.validateKey(key)) {
                    cout << "ОШИБКА: Неверный ключ. Ключ должен состоять только из цифр." << endl;
                    continue;
                }
                if (processFileGronsfeld(loader, inPath, outPath, key, encrypt)) {
                    cout << "Операция выполнена успешно. Результат: " << outPath << endl;
                } else {
                    cout << "Ошибка при обработке файла." << endl;
                }
                break;
            }

            case GRONSFELD_VALIDATE_KEY:
                cout << "Введите ключ: ";
                cin.getline(key, sizeof(key));
                cout << (loader.validateKey(key) ? "Ключ корректен" : "Ключ НЕ корректен") << endl;
                break;

            default:
                cout << "Неверный выбор." << endl;
        }
    }
    loader.unload();
}

// ========== Подсистема 5: GrandChiffre ==========
// Перечисление для меню GrandChiffre
enum GrandChiffreMenuAction {
    GRANDCHIFFRE_BACK = 0,
    GRANDCHIFFRE_GEN_KEY = 1,
    GRANDCHIFFRE_TEXT = 2,
    GRANDCHIFFRE_FILE = 3
};

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
        cout << "\n--- МЕНЮ GRANDCHIFFRE ---\n";
        cout << "1. Сгенерировать случайный ключ\n";
        cout << "2. Зашифровать/расшифровать текст\n";
        cout << "3. Зашифровать/расшифровать файл\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";

        cin >> ch;
        clearInput();

        if (ch == GRANDCHIFFRE_BACK) break;

        switch (ch) {
            case GRANDCHIFFRE_GEN_KEY:
                cout << "Введите длину ключа (" << alg->getMinKeyLength() << "-" << alg->getMaxKeyLength() << "): ";
                cin >> keySize;
                clearInput();

                vector<unsigned char> newKey = alg->generateKey(keySize);
                cout << "Сгенерированный ключ (HEX): ";
                for (i = 0; i < newKey.size(); ++i) {
                    printf("%02X ", newKey[i]);
                }
                cout << endl;
                break;

            case GRANDCHIFFRE_TEXT:
                cout << "Режим (1 - шифрование, 2 - дешифрование): ";
                cin >> mode;
                clearInput();

                cout << "Введите текст: ";
                getline(cin, inputText);
                cout << "Введите ключ в HEX (например, 01 02 03): ";
                getline(cin, hexKey);

                keyBytes.clear();
                for (i = 0; i < hexKey.length(); ) {
                    if (hexKey[i] == ' ') { ++i; continue; }
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
                break;

            case GRANDCHIFFRE_FILE:
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
                    if (hexKey[i] == ' ') { ++i; continue; }
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
                break;

            default:
                cout << "Неверный выбор." << endl;
        }
    }
    unload_plugin();
}

// ========== ГЛАВНОЕ МЕНЮ ==========
enum MainMenuAction {
    MAIN_EXIT = 0,
    MAIN_VIGENERE_SKITALA = 1,
    MAIN_RSA_ENIGMA = 2,
    MAIN_XOR_TEA = 3,
    MAIN_GRONSFELD = 4,
    MAIN_GRANDCHIFFRE = 5
};

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
            case MAIN_VIGENERE_SKITALA: runVigenereSkitala(); break;
            case MAIN_RSA_ENIGMA:       runRSAEnigma();       break;
            case MAIN_XOR_TEA:          runXorTea();          break;
            case MAIN_GRONSFELD:        runGronsfeld();       break;
            case MAIN_GRANDCHIFFRE:     runGrandChiffre();    break;
            case MAIN_EXIT:
                running = false;
                cout << "\nДо свидания!" << endl;
                break;
            default:
                cout << "Неверный выбор!" << endl;
        }
    }
    return 0;
}