cmake_minimum_required(VERSION 3.17)
project(huffman)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    #set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_BUILD_TYPE Debug)
endif()

#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(INC_PATH  ${PROJECT_SOURCE_DIR}/include)
#set(SRC_PATH  ${PROJECT_SOURCE_DIR}/src)

# headers used only by implementation i will change this later
#set(INC_PRIVATE_PATH  ${PROJECT_SOURCE_DIR}/src)

# equivalent of: strip --strip-all
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
add_link_options($<$<CONFIG:RELEASE>:-s>)

# set(CTEST_OUTPUT_ON_FAILURE ON)
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_PATH})
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -pedantic
        -Wall -Wextra -Wno-unused-function -pipe
        -fstack-protector-all -fstack-protector-strong
        -O0 -g
        #-O3 -ffast-math -funroll-loops -march=native -mtune=native
        #-funswitch-loops -ftree-vectorize -fivopts -fmodulo-sched -flto
)

# yay -S --noconfirm doctest
include(CTest)
if(BUILD_TESTING)
    message("Config build type: ${CMAKE_BUILD_TYPE}")
    add_subdirectory(tests)
endif()