cmake_minimum_required(VERSION 3.20)
project(ordenamientos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de build" FORCE)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "/O2")
  add_compile_options(/W4)
else()
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  add_compile_options(-Wall -Wextra)
endif()

add_executable(ordenamientos
  src/main.cpp
  src/algoritmos.cpp
  src/experimento.cpp
  src/csv.cpp
)

enable_testing()
add_executable(ordenamientos_pruebas
  tests/pruebas.cpp
  src/algoritmos.cpp
  src/experimento.cpp
  src/csv.cpp
)
target_include_directories(ordenamientos_pruebas PRIVATE src)
add_test(NAME pruebas COMMAND ordenamientos_pruebas)
