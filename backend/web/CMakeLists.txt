add_executable(web_backend web_backend.cc)
target_link_libraries(web_backend PRIVATE Threads::Threads)
