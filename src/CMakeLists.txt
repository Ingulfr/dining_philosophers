cmake_minimum_required(VERSION 3.5)

add_executable(${PROJECT_NAME} dining_philosophers.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif(UNIX)

