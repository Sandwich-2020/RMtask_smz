cmake_minimum_required(VERSION 3.8)

project(task6)

# set(OpenCV_DIR "D:\\MSYS2\\mingw64")
# include_directories(D:/opencv_cpp_env/eigen-3.4.0/eigen-3.4.0)   #加入这句，其中../eigen 是我eigen解压出来的路径
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)

add_executable(task6 main.cpp)

target_include_directories(task6 PUBLIC ${OpenCV_INCLUDE_DIRS})

target_link_libraries(task6 ${OpenCV_LIBS})