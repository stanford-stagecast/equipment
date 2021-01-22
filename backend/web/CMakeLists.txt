add_executable(web_backend main.cc listener.cc client.cc dispatcher.cc
  manager.cc cuelist.cc cue.cc transmitter.cc)
target_link_libraries(web_backend ${Boost_LIBRARIES} crypto)
