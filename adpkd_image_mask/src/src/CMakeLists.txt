cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(adpkd_image_mask)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(adpkd_image_mask code.cxx)


target_link_libraries(adpkd_image_mask ${ITK_LIBRARIES})