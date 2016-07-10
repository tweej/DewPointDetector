cmake_minimum_required(VERSION 2.8)
project(dewpointdetector)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_BUILD_TYPE Release)

file(GLOB SRCFILES *.cc)
add_executable(dewpointdetector main.cpp ${SRCFILES})

find_package(Threads)
target_link_libraries(dewpointdetector ${CMAKE_THREAD_LIBS_INIT})

find_package(Boost COMPONENTS date_time log REQUIRED)
target_link_libraries(dewpointdetector ${Boost_LIBRARIES})

find_package(Jsoncpp REQUIRED)
include_directories(SYSTEM ${Jsoncpp_INCLUDE_DIR})
target_link_libraries(dewpointdetector ${Jsoncpp_LIBRARIES})

find_package(Poco REQUIRED COMPONENTS PocoFoundation PocoNet)
include_directories(SYSTEM ${POCO_INCLUDE_DIRS})
target_link_libraries(dewpointdetector ${POCO_LIBRARIES})

install(TARGETS dewpointdetector RUNTIME DESTINATION bin)
