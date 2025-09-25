cmake_minimum_required(VERSION 3.15)
project(trajectory_analizer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra -pedantic")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
    endif()
endif()

add_compile_definitions(_USE_MATH_DEFINES)
find_package(Threads REQUIRED)
include_directories(include)

## Lista IMPLÍCITA de archivos fuente
# set(SOURCE_DIR src)
# file(GLOB SOURCES 
#     "${SOURCE_DIR}/*.cpp"
# )

# Lista EXPLÍCITA de archivos fuente
set(SOURCES
    src/AnalizadorBase.cpp
    src/AnalizadorDensidad.cpp
    src/AnalizadorPresion.cpp
    src/Configuracion.cpp
    src/Geometria.cpp
    src/ThreadPool.cpp
    src/main.cpp
)

message(STATUS "==========================================")
message(STATUS "Proyecto: ${PROJECT_NAME}")
message(STATUS "Versión: ${PROJECT_VERSION}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Estándar C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Modo de compilación: ${CMAKE_BUILD_TYPE}")
message(STATUS "Archivos fuente encontrados: ${SOURCES}")
message(STATUS "==========================================")

add_executable(trajectory_analizer ${SOURCES})
target_link_libraries(trajectory_analizer Threads::Threads m)
