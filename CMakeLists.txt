cmake_minimum_required(VERSION 3.16)
project(safety_incident_log LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig QUIET)
find_library(MYSQLCPPCONN_LIB mysqlcppconn)
find_path(MYSQLCPPCONN_INCLUDE mysql_driver.h
    PATHS /usr/include/cppconn /usr/local/include/cppconn
)

add_executable(app
    src/main.cpp
    src/presentation/ConsoleMenu.cpp
    src/business/IncidentService.cpp
    src/data/InMemoryIncidentRepository.cpp
    src/data/MySQLIncidentRepository.cpp
)

target_include_directories(app PRIVATE src ${MYSQLCPPCONN_INCLUDE}/..)
target_link_libraries(app PRIVATE ${MYSQLCPPCONN_LIB})