cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(termME)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNETED TRUE)

# ftxui

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG 4188ee2c046ced00cd3accba4cde2318587f8c8a
)

# fmt

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG d141cdbeb0fb422a3fb7173b285fd38e0d1772dc
)

FetchContent_MakeAvailable(ftxui fmt)

# termME

set(SRCS
  src/main.cpp
)
add_executable(termME ${SRCS})
target_include_directories(termME PRIVATE include)
target_link_libraries(termME
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE fmt::fmt
)

