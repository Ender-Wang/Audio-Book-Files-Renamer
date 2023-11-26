cmake_minimum_required(VERSION 3.26)
project(Audio_Book_Files_Renamer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

include_directories(${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})

add_executable(Audio_Book_Files_Renamer main.cpp)

target_link_libraries(Audio_Book_Files_Renamer ${CURL_LIBRARIES} ${LIBXML2_LIBRARIES})