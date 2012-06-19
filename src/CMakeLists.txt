
cmake_minimum_required(VERSION 2.6)
project(jFUSE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules" 
        "${CMAKE_CURRENT_SOURCE_DIR}/jFUSE/lowlevel/lib/cmake_modules")




add_subdirectory(jFUSE/lowlevel)



