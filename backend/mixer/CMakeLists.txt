<<<<<<< HEAD
add_executable(mixer mixer.cc test.cc)
=======
add_executable(mixer mixer.cc main.cc receiver.cc)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(mixer PRIVATE Threads::Threads)
>>>>>>> 7a255336e51889af84d2a33d4b31c0717307b9b2
