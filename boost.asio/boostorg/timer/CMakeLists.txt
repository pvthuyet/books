add_executable(time_t_timer time_t_timer.cpp)
target_include_directories(time_t_timer PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(time_t_timer PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})