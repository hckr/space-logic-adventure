cmake_minimum_required(VERSION 2.8)

project(space-logic-adventure)

FILE(GLOB SOURCES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# e.g. for Qt Creator's "TODO" plugin to work on header files
FILE(GLOB HEADERS "*.hpp")
add_custom_target(headers SOURCES ${HEADERS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_package(SFML 2.4.2 REQUIRED system window graphics)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
endif()
