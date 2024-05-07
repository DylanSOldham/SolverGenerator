cmake_minimum_required(VERSION 3.5.0)

project(DiffEqSolverTest)

set(CMAKE_CXX_STANDARD 17)

find_package(SUNDIALS REQUIRED)

find_package(googletest CONFIG)
if (NOT FOUND_googletest)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(modgen ./src_modgen/modulegenerator.cpp ./src_modgen/expression.cpp ./src_modgen/parse.cpp ./src_modgen/tokenize.cpp)

add_executable(solver ./src_solver/main.cpp)
target_link_libraries(solver SUNDIALS::cvode SUNDIALS::nvecserial)

add_executable(tests ./test/test.cpp ./src_modgen/tokenize.cpp)
target_link_libraries(tests GTest::gtest_main)