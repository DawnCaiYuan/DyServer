CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(dyserver_example)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE DEBUG)

#[[
处理子模块，生成静态库
#]]
set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../)
add_subdirectory(${TOP_DIR}/include/dysv dysv_dir)

# 生成example
add_executable(dysv_log_example log_example.cpp)
target_link_libraries(dysv_log_example PRIVATE libdysv)