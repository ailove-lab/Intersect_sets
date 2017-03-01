cmake_minimum_required(VERSION 3.6)
project(Intersect_sets)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(intersect_sets intersect_sets.cpp)
add_executable(uids_activity uids_activity.cpp)
add_executable(uids_matrix uids_matrix.cpp)
add_executable(build_user_model build_user_model.cpp)
add_executable(smooth_activity smooth_activity.cpp)
