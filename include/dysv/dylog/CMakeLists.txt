add_library(libdylog STATIC dy_log.cpp)
set(CMAKE_CXX_STANDARD 17)

target_include_directories(libdylog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})