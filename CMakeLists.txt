cmake_minimum_required(VERSION 3.10)
project(XOR_TEA_Crypto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создаём КОНСОЛЬНОЕ приложение (исполняемый файл)
add_executable(rgr_app
    xor_plugin.cpp
    tea_plugin.cpp
    xor_key.cpp
    tea_key.cpp
    file_xor_tea.cpp
    logic_xor_tea.cpp
    plugin_loader_xor_tea.cpp
    main.cpp
)

# Для Linux
if(UNIX)
    target_link_libraries(rgr_app stdc++fs)
endif()

# Включаем поддержку UTF-8
target_compile_options(rgr_app PRIVATE -Wall -Wextra)
