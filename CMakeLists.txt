cmake_minimum_required(VERSION 3.0)

project(WebServices)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED MAGIC)
    message(FATAL_ERROR "Please Add The Directory Path Of The Magic Library!!!")
endif()

include_directories(
    ${MAGIC}/Include
    ${CMAKE_BINARY_DIR}/Include
    ${PROJECT_SOURCE_DIR}/Include
    ${MAGIC}/ThirdParty/Asio/Include
    ${MAGIC}/ThirdParty/RapidJSON/Include
)

link_directories(${MAGIC}/Lib)

include(${MAGIC}/Magic.cmake)

set(MODULES
    ${MAGIC}/Modules/Magic.json
    ${MAGIC}/Modules/WebServer.json
    ${PROJECT_SOURCE_DIR}/Modules/WebServices.json
)

set(SOURCES
    Source/Main.cpp
    Source/Manager/DataBaseManager.cpp

    Source/Servlet/GetInfoServlet.cpp
    Source/Servlet/ResourceServlet.cpp
    Source/Servlet/GetUpdateServlet.cpp
    Source/Servlet/SetUpdateServlet.cpp
    Source/Servlet/SetVersionServlet.cpp
    Source/Servlet/AddVersionServlet.cpp
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Include)

add_custom_target(Gen ALL)
add_custom_command(
    TARGET Gen
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Include
    COMMAND ${MAGIC}/Bin/Gen ${MODULES} WebServices
)

add_executable(WebServices ${SOURCES})

target_link_libraries(WebServices Magic ${MAGIC_DEPEND_LIBRARY})