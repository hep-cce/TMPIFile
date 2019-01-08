cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(test_tmpi CXX)

#Bring the header directory into the project
include_directories(
        .
        ${MPI_INCLUDE_PATH}
        ${ROOT_INCLUDE_DIR}
        ${TMPI_SOURCE_DIR}
)

add_executable(test_tmpi test_tmpi.C)
target_link_libraries(test_tmpi TMPI ${ROOT_LIBRARIES})

install(
        TARGETS
        test_tmpi
        RUNTIME DESTINATION bin
)
