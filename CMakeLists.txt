cmake_minimum_required(VERSION 2.8.12)

project(sa.mqtt)

set(SA_MQTT sa_mqtt)
set(SA_ENGINE_HOME CACHE STRING "How to link the sa.kernel library.")

if("${SA_ENGINE_HOME}" STREQUAL "")
  message(FATAL_ERROR "SA_ENGINE_HOME not set.")
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  add_definitions(-fPIC)
  add_definitions(-O2 -Wall -Werror -DUNIX=1 -DLINUX=1  -DNO_HEAP_TRACKING=1
    -DHIGH_PERFORMANCE=1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  add_definitions(-DUNIX=1 -DLINUX=1 -DOSX=1 -O2)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  add_definitions(-DOS_Windows=1)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
    "C:/Program Files (x86)/Eclipse Paho C")
endif()

add_subdirectory("src")
