cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(aod2tree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(aliphysics-lite REQUIRED)
find_package(ROOT REQUIRED)

include(GNUInstallDirs)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stage/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stage/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stage/bin)

set(BUILD_SHARED_LIBS ON)

add_library(aod2tree SHARED)
target_sources(aod2tree PRIVATE
 TaskDimuonppData.cxx)

target_link_libraries(aod2tree PUBLIC 
      aliphysics-lite-muon 
      aliphysics-lite-oadb-multiplicity)

target_include_directories(aod2tree
    PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    INTERFACE
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

root_generate_dictionary(G__aod2tree
  TaskDimuonppData.h
                         aod2tree-linkdef.h
  MODULE aod2tree)
 
install(TARGETS aod2tree)
