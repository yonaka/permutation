cmake_minimum_required(VERSION 3.0.0)
project(permutation VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(permutation main.cpp)
target_compile_features(permutation PUBLIC cxx_std_20)
add_compile_options(-march=native)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(test)

find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(permutation PUBLIC ${Boost_LIBRARIES})
target_include_directories(permutation PUBLIC ${Boost_INCLUDE_DIRS})