cmake_minimum_required(VERSION 3.20)
project(kite CXX)

set(CMAKE_CXX_STANDARD 20)


file(GLOB CXXSOURCES *.cc *.h)

add_executable(kite ${CXXSOURCES})
