cmake_minimum_required(VERSION 2.8)
project(hb-ranges)

file(TO_CMAKE_PATH "$ENV{HOME}" USER_HOME)

include_directories("${USER_HOME}/projects/boost_1_71_0/")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -fPIC")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -std=gnu++11")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN/'")

set(SOURCE_FILES main.cpp)

SET(LIBRARY_OUTPUT_PATH "${hb-ranges_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")

SET(EXECUTABLE_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
)
