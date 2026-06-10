#include "logic_rsa_enigma.h"
#include "plugin_loader_rsa_enigma.h"
#include "file_rsa_enigma.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>

enum MenuAction {
    ACTION_EXIT = 0,
    ACTION_KEY_GENERATION = 1,
    ACTION_TEXT_PROCESSING = 2,
    ACTION_FILE_PROCESSING = 3
};

static void handle_key_generation(Plugin& rsa, Plugin& enigma) {
    std::cout << "\nДоступные алгоритмы:\n";
    std::cout << "1. " << rsa.get_name() << " (Ключ: " << rsa.get_format() << ")\n";
    std::cout << "2. " << enigma.get_name() << " (Ключ: " << enigma.get_format() << ")\n";
    std::cout << "Выберите алгоритм (1 или 2): ";
    
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore(32767, '\n');
    
    char key[128];
    if (choice == 1) {
        rsa.gen_key(key, sizeof(key));
        std::cout << "\n--- Ключи RSA сгенерированы ---\n";
        std::cout << "Ключ (n,e,d): " << key << "\n";
    } else if (choice == 2) {
        enigma.gen_key(key, sizeof(key));
        std::cout << "\n--- Ключ Энигмы сгенерирован ---\n";
        std::cout << "Позиции роторов: " << key << "\n";
    } else {
        throw std::runtime_error("Неверный выбор алгоритма.");
    }
}

static void handle_processing(Plugin& rsa, Plugin& enigma, bool is_file_mode) {
    std::cout << "\nДоступные алгоритмы:\n";
    std::cout << "1. " << rsa.get_name() << "\n";
    std::cout << "2. " << enigma.get_name() << "\n";
    std::cout << "Выберите (1 или 2): ";
    
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore(32767, '\n');
    
    if (choice != 1 && choice != 2) {
        throw std::runtime_error("Неверный выбор.");
    }

    char key_buf[128];
    std::cout << "Введите ключ (" << (choice == 1 ? rsa.get_format() : enigma.get_format()) << "): ";
    std::cin.getline(key_buf, sizeof(key_buf));
    
    if (std::strlen(key_buf) == 0) {
        throw std::runtime_error("Ошибка: Ключ не может быть пустым.");
    }

    std::cout << "Режим (1 - Шифрование, 2 - Дешифрование): ";
    int mode = 0;
    std::cin >> mode;
    std::cin.ignore(32767, '\n');

    Plugin& active = (choice == 1) ? rsa : enigma;

    if (!is_file_mode) {
        std::cout << "Введите данные: ";
        std::string input_text;
        std::getline(std::cin, input_text);
        
        std::vector<uint32_t> input(input_text.begin(), input_text.end());
        std::vector<uint32_t> output(input.size());
        
        active.process(input.data(), input.size(), key_buf, output.data());
        
        std::cout << "Результат: ";
        if (choice == 1) {
            for (uint32_t b : output) {
                std::cout << std::hex << b << " ";
            }
            std::cout << std::dec << "\n";
        } else {
            std::string res(output.begin(), output.end());
            std::cout << res << "\n";
        }
    } else {
        std::string in_path, out_path;
        std::cout << "Путь к входному файлу: ";
        std::getline(std::cin, in_path);
        std::cout << "Путь к выходному файлу: ";
        std::getline(std::cin, out_path);

        std::vector<uint32_t> input_data;
        std::vector<uint32_t> output_data;

        if (mode == 1) {
            std::vector<uint8_t> raw_bytes = file::read_binary_file(in_path);
            input_data.assign(raw_bytes.begin(), raw_bytes.end());
            output_data.resize(input_data.size());
            
            std::cout << "Обработка данных... ";
            std::cout.flush();
            active.process(input_data.data(), input_data.size(), key_buf, output_data.data());
            std::cout << "Готово!\n";
            
            file::write_encrypted_file(out_path, output_data);
        } else { // Дешифрование
            input_data = file::read_encrypted_file(in_path);
            output_data.resize(input_data.size());
            
            std::cout << "Обработка данных... ";
            std::cout.flush();
            active.process(input_data.data(), input_data.size(), key_buf, output_data.data());
            std::cout << "Готово!\n";
            
            file::write_decrypted_file(out_path, output_data);
        }
        std::cout << "Результат сохранен в: " << out_path << "\n"; 
    }
}

void run_application() {
    Plugin rsa{}, enigma{};
    
    if (!load_plugin("./librsa_plugin.so", rsa) && !load_plugin("rsa_plugin.dll", rsa)) {
        throw std::runtime_error("Не удалось загрузить плагин RSA. Проверьте наличие librsa_plugin.so или rsa_plugin.dll.");
    }
    if (!load_plugin("./libenigma_plugin.so", enigma) && !load_plugin("enigma_plugin.dll", enigma)) {
        throw std::runtime_error("Не удалось загрузить плагин Enigma. Проверьте наличие libenigma_plugin.so или enigma_plugin.dll.");
    }

#ifdef _WIN32
    system("chcp 65001 > nul");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif

    bool running = true;
    while (running) {
        std::cout << "\n=== Encryption Algorithm RGR ===\n";
        std::cout << "1. Генерация ключей шифрования\n";
        std::cout << "2. Обработка текстовых данных (шифрование/дешифрование)\n";
        std::cout << "3. Обработка файлов (шифрование/дешифрование)\n";
        std::cout << "0. Завершение работы программы\n";
        std::cout << "Выберите действие: ";
        
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            std::cout << "Неверный ввод.\n";
            continue;
        }
        std::cin.ignore(32767, '\n');

        try {
            switch (choice) {
                case ACTION_KEY_GENERATION:
                    handle_key_generation(rsa, enigma);
                    break;
                case ACTION_TEXT_PROCESSING:
                    handle_processing(rsa, enigma, false);
                    break;
                case ACTION_FILE_PROCESSING:
                    handle_processing(rsa, enigma, true);
                    break;
                case ACTION_EXIT:
                    running = false;
                    std::cout << "Завершение работы.\n";
                    break;
                default:
                    std::cout << "Неверный выбор.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "\n[ОШИБКА] " << e.what() << "\n\n";
        }
    }

    free_plugin(rsa);
    free_plugin(enigma);
}