cmake_minimum_required(VERSION 3.0.0)
project(permutation_test VERSION 0.1.0)

add_executable(permutation_test permutation_test.cpp)
target_compile_features(permutation_test PUBLIC cxx_std_20)
target_link_libraries(permutation_test PRIVATE gtest gtest_main pthread)