add_executable(web_backend main.cc listener.cc client.cc dispatcher.cc
  manager.cc)
target_link_libraries(web_backend ${Boost_LIBRARIES})
