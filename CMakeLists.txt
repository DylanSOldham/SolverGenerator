cmake_minimum_required(VERSION 3.5.0)

project(DiffEqSolverTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(EXAMPLES_ENABLE_C OFF)
set(BUILD_ARKODE OFF)
set(BUILD_IDA OFF)
set(BUILD_IDAS OFF)
set(BUILD_KINSOL OFF)
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
  SUNDIALS
  URL https://github.com/LLNL/sundials/releases/download/v7.0.0/sundials-7.0.0.tar.gz
)
set(HOLDER ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/.build/lib)
FetchContent_MakeAvailable(SUNDIALS)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HOLDER})

add_executable(generator ./src_generator/main.cpp ./src_generator/generator.cpp ./src_generator/expression.cpp ./src_generator/parse.cpp ./src_generator/tokenize.cpp)

add_executable(solver ./src_solver/main.cpp)
target_link_libraries(solver SUNDIALS::cvode SUNDIALS::nvecserial)

add_executable(tests ./test/test.cpp ./src_generator/tokenize.cpp ./src_generator/parse.cpp ./src_generator/expression.cpp)
target_link_libraries(tests GTest::gtest_main)