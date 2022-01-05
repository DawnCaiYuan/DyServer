add_library(libdysv INTERFACE)
set(CMAKE_CXX_STANDARD 17)

set(DYSV_TOP_DIR ${CMAKE_CURRENT_LIST_DIR})
add_subdirectory(${DYSV_TOP_DIR}/dylog dylog_dir)

target_link_libraries(libdysv INTERFACE libdylog)
target_include_directories(libdysv INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})