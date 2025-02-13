cmake_minimum_required(VERSION 3.10)
project(shiva)

set(CMAKE_BUILD_TYPE DEBUG)

find_package(SFML 2.5 COMPONENTS system audio network REQUIRED)

set(projectSrcs)

foreach(DIR ".")
    file(GLOB_RECURSE DIR_SOURCES "${DIR}/*.cpp")
    list(APPEND projectSrcs ${DIR_SOURCES})
endforeach()

add_executable(${PROJECT_NAME} ${projectSrcs})

target_link_libraries(${PROJECT_NAME} sfml-system sfml-audio sfml-network )