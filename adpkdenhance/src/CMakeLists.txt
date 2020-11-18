cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(adpkdenhance)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(adpkdenhance code.cxx)


target_link_libraries(adpkdenhance ${ITK_LIBRARIES})
