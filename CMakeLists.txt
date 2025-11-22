cmake_minimum_required(VERSION 3.30.5)
project(GameProject)

set(CMAKE_CXX_STANDARD 20)

add_executable(GameProject main.cpp
        game.cpp
        game.h
        cmake-build-debug/objects.cpp
        cmake-build-debug/objects.h
        point.cpp
        point.h
        objects.cpp
        objects.h
        Direction.h
        utils.cpp
        utils.h
        Screen.cpp
        Screen.h
        player.cpp
        player.h
)
