cmake_minimum_required(VERSION 3.7)
project(source)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        GB_Bits.cpp
        GB_Bits.h
        GB_Const.cpp
        GB_Const.h
        GB_CPU.cpp
        GB_CPU.h
        GB_FileOperator.cpp
        GB_FileOperator.h
        GB_memory.cpp
        GB_memory.h
        GB_Timer.cpp
        GB_Timer.h
        GB_Type.h
        GB_Window.cpp
        GB_Window.h
        main.cpp
        main.h)
link_directories("/usr/local/Cellar/sdl2/2.0.5/lib")
add_executable(source ${SOURCE_FILES})
target_link_libraries(source libSDL2-2.0.0.dylib)