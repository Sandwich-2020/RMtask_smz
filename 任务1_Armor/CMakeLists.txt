

cmake_minimum_required(VERSION 3.10)

project(test)

find_package(OpenCV REQUIRED)

add_executable(test_1 main.cpp)

target_include_directories(test_1 PUBLIC ${OpenCV_INCLUDE_DIRS})

target_link_libraries(test_1 ${OpenCV_LIBS})

