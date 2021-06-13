add_executable(fastest-way-read-file main.cpp)
target_include_directories(fastest-way-read-file PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(fastest-way-read-file PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})