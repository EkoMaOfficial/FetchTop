# 1. Минимальная версия CMake, необходимая для сборки
cmake_minimum_required(VERSION 3.20)

# 2. Имя твоего проекта и язык программирования
project(FetchTop VERSION 1.0.0 LANGUAGES CXX)

# 3. Создаем исполняемый файл из нашей точки входа
add_executable(fetchtop 
    src/main.cpp
    src/parsers.cpp
)

# 4. Жестко задаем стандарт C++. Мы используем современный C++20.
target_compile_features(fetchtop PRIVATE cxx_std_20)

# 5. Указываем компилятору, где искать заголовочные файлы (.hpp)
target_include_directories(fetchtop PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)