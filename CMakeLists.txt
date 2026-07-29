cmake_minimum_required(VERSION 3.5)
project (MiniCarver VERSION 1.0.0 LANGUAGES CXX )

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

set( APP_NAME "mini-carver" )
add_executable( ${APP_NAME}
    src/main.cpp
    src/cli.cpp
    src/fileinfo.cpp
    src/ui.cpp
  )

target_include_directories( ${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib PRIVATE ${CMAKE_SOURCE_DIR}/include )
target_compile_features( ${APP_NAME}  PUBLIC cxx_std_17 )
target_compile_options( ${APP_NAME} PRIVATE -Wall -Wextra -pedantic -O3)
