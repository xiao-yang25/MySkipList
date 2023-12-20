cmake_minimum_required(VERSION 3.16)

project(skip_list)

set(CMAKE_CXX_STANDARD 11)
add_compile_options("-g")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} 
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
install(TARGETS ${PROJECT_NAME})

add_executable(${PROJECT_NAME}_exe src/main.cc)
target_link_libraries(${PROJECT_NAME}_exe ${PROJECT_NAME})

add_executable(${PROJECT_NAME}_test test/stress_test.cc)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} pthread)
