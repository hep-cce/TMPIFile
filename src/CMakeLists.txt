cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(TMPI CXX)

#Bring the header directory into the project
include_directories(
        .
        ${MPI_INCLUDE_PATH}
        ${ROOT_INCLUDE_DIR}
)

set( ${PROJECT_NAME}_HEADERS
        TClientInfo.h
        JetEvent.h
        TMPIFile.h
)

set( ${PROJECT_NAME}_SRCS
        TClientInfo.cxx
        JetEvent.cxx
        TMPIFile.cxx
)

ROOT_GENERATE_DICTIONARY( TMPIDict ${${PROJECT_NAME}_HEADERS} LINKDEF Linkdef.h )
set( ${PROJECT_NAME}_DICTIONARIES TMPIDict.cxx)
add_library( ${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_DICTIONARIES} )
target_link_libraries( ${PROJECT_NAME} ${ROOT_LIBRARIES} )
