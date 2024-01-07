cmake_minimum_required(VERSION 3.11)

project(emacs-odbc C)

add_library(emacs-odbc-module SHARED src/utils.c src/emacs-odbc-module.c)
set_target_properties(emacs-odbc-module PROPERTIES
  C_STANDARD 99
  C_VISIBILITY_PRESET "hidden"
  POSITION_INDEPENDENT_CODE ON
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(emacs-odbc-module PUBLIC odbc)
