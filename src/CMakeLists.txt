set(SOURCES
  cbfns.c
  sa_circular_buff.c
  sa_mqtt.c
  )

link_directories(${SA_ENGINE_HOME}/bin)

add_library(${SA_MQTT} SHARED ${SOURCES})

target_include_directories(${SA_MQTT} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${SA_MQTT} sa.kernel)
