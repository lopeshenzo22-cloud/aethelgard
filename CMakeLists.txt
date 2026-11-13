cmake_minimum_required(VERSION 3.16)
project(Aethelgard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 2.6.1 e baixado e compilado automaticamente na primeira build
include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(aethelgard
  src/main.cpp
  src/Game.cpp
)
target_include_directories(aethelgard PRIVATE src)
target_link_libraries(aethelgard PRIVATE sfml-graphics sfml-window sfml-system)

if(MSVC)
  target_compile_options(aethelgard PRIVATE /W4 /utf-8)
else()
  target_compile_options(aethelgard PRIVATE -Wall -Wextra)
endif()

# Copia a pasta assets para junto do executavel
add_custom_command(TARGET aethelgard POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:aethelgard>/assets)
