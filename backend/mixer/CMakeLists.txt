add_executable(test_mixer mixer.cc receiver.cc)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(test_mixer PRIVATE Threads::Threads)
