cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(TEST_BIN ${PROJECT_NAME}_test)
add_executable(${TEST_BIN} ${PROJECT_NAME}_test.cpp)

# doctest framework
# pacman -S --noconfirm doctest
find_package(doctest REQUIRED)
target_link_libraries(${TEST_BIN} PRIVATE doctest::doctest)

# self - lib
#target_link_libraries(${TEST_BIN} PRIVATE ${PROJECT_NAME})
target_include_directories(${TEST_BIN} PUBLIC ${INC_PATH})

target_compile_definitions(${PROJECT_NAME} PRIVATE -DDEBUG)
target_compile_options(${PROJECT_NAME}
        PRIVATE
            -Wall -Wextra -Wno-unused-function -pipe -pedantic
            -fstack-protector-all -fstack-protector-strong -O0
)

add_test(
        NAME ${TEST_BIN}
        COMMAND ${EXECUTABLE_OUTPUT_PATH}/${TEST_BIN} --exit=true --force-colors=true -s
)