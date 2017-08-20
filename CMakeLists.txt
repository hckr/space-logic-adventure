cmake_minimum_required(VERSION 2.8)

project(space-logic-adventure)
add_executable(${PROJECT_NAME} "main.cpp" "level.cpp")

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_package(SFML 2 REQUIRED system window graphics)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
endif()
