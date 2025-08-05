cmake_minimum_required(VERSION 3.25)
project(spannerlib)

set(CMAKE_CXX_STANDARD 20)

find_package(google_cloud_cpp_spanner CONFIG REQUIRED)

add_library(spannerlib SHARED shared_lib.cpp
        connection.cpp
        connection.h
        pool.cpp
        pool.h
        message.h
        library.h
        library.cpp
        rows.cpp
        rows.h
)
target_compile_definitions(spannerlib PRIVATE BUILDING_DLL)
target_link_libraries(spannerlib google-cloud-cpp::spanner)

add_executable(main main.cpp)
target_link_libraries(main spannerlib)
