cmake_minimum_required(VERSION 3.0)

project(WebServices)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED MAGIC)
    message(FATAL_ERROR "Please Add The Directory Path Of The Magic Library!!!")
endif()

include_directories(
    ${MAGIC}
    ${MAGIC}/Include
    ${CMAKE_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/Include
    ${MAGIC}/ThirdParty/Asio/Include
    ${MAGIC}/ThirdParty/RapidJSON/Include
)

link_directories(${MAGIC}/Lib)
link_libraries(Magic mariadbclient)

include(${MAGIC}/Magic.cmake)

set(MODULES
    ${MAGIC}/Modules/Magic.magic
    ${MAGIC}/Modules/WebServer.magic
    ${PROJECT_SOURCE_DIR}/Modules/WebServices.magic
)

set(SOURCES
    Source/Main.cpp
    Source/Manager/DataBaseManager.cpp
    Source/Servlet/GetUpdateServlet.cpp
)

add_custom_target(Gen ALL)
add_custom_command(
    TARGET Gen
    COMMAND ${MAGIC}/Bin/Gen ${MODULES} WebServices
)

add_executable(WebServices ${SOURCES})
