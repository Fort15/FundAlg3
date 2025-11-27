cmake_minimum_required(VERSION 3.10)

# Название проекта
project(task7 C)

# Установка стандарта языка C
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Установка флагов компилятора для отладки и предупреждений
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic -g")

# Исходные файлы
set(SOURCE_FILES 
    main.c 
    interp.c # Здесь предполагается вся ваша логика
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCE_FILES})