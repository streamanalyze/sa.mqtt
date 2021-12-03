set(SOURCES
  cbfns.c
  sa_mqtt.c
  )

link_directories(${SA_ENGINE_HOME}/bin)

add_library(${SA_MQTT} SHARED ${SOURCES})

# Let's first try and find the package, this will work if Paho MQTT was
# installed using CMake (always on Windows,) and hence there's a
# eclipse-paho-mqtt-cConfig.cmake around. If not, for instance if Paho MQTT
# was installed from the hand-written Makefile, we may still find the include
# files in some standard place (Linux).
find_package(eclipse-paho-mqtt-c QUIET)

if (${eclipse-paho-mqtt-c_FOUND})
  # There must be some graceful way to do this...
  target_include_directories(${SA_MQTT} PUBLIC
    ${eclipse-paho-mqtt-c_DIR}/../../../include
    )
endif()

target_include_directories(${SA_MQTT} PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${SA_ENGINE_HOME}/C
  ${INCLUDE_DIRS}
  )

target_link_libraries(${SA_MQTT} sa.kernel)
