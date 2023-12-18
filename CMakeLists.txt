cmake_minimum_required(3.16)

project(skip_list)

set(CMAKE_CXX_STANDARD 11)

add_library(${PROJECT_NAME} )
target_include_directories(${PROJECT_NAME} PUBLIC src/skip_list.h)
# install(TARGETS ${PROJECT_NAME})

add_executable(${PROJECT_NAME}_exe src/main.cc)
target_link_libraries(${PROJECT_NAME}_exe ${PROJECT_NAME})

add_executable(${PROJECT_NAME}_test test/stress_test.cc)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})