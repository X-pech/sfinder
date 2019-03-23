cmake_minimum_required(VERSION 3.12.1)
project(sfinder-rework)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wshadow -pedantic -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -fvisibility=hidden")

add_executable(sfinder
        IndexWorker.cpp
        IndexWorker.h
        main.cpp
        MainWindow.cpp
        MainWindow.h
        SeachWorker.cpp
        SearchWorker.h
        indexrunner.cpp
        indexrunner.h
        Trigram.cpp
        Trigram.h)
qt5_use_modules(sfinder Widgets)


