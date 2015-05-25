string(ASCII 27 Esc)
set(ColorReset "${Esc}[m")
set(Yellow      "${Esc}[33m")

cmake_minimum_required (VERSION 2.8)
project (UBOnlineDBI)

# Now set CMAKE variables for my use
set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "./lib")
set(DICT_NAME "UBOnlineDBICint")
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
  MESSAGE("Removing a dictionary file: ${Yellow}${DICT_NAME}.cxx${ColorReset}")
  file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
endif()
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.h)
  MESSAGE("Removing a dictionary file: ${Yellow}${DICT_NAME}.h${ColorReset}")
  file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.h)
endif()

# Get list of course
file( GLOB MYHEADERS *.h  )
file( GLOB LINKDEF LinkDef.h )
list( GET LINKDEF 0 LINKDEF )
list( REMOVE_ITEM MYHEADERS ${LINKDEF} )

# Pre process to generate dictionary source code + getting compiler/linker flags
execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx -c -I. -I$ENV{PGSQL_DIR}/include ${MYHEADERS} ${LINKDEF})
execute_process(COMMAND root-config --libs OUTPUT_VARIABLE ROOTLIB OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOTINC OUTPUT_STRIP_TRAILING_WHITESPACE)

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories($ENV{PGSQL_DIR}include)
set(CMAKE_SHARED_LINKER_FLAGS ${ROOTLIB})
set(CMAKE_CXX_FLAGS "${ROOTINC} -I$ENV{PGSQL_DIR}include")
MESSAGE("CMAKE_SHARED_LINKER_FLAGS:" ${CMAKE_SHARED_LINKER_FLAGS})
MESSAGE("CMAKE_CXX_FLAGS:" ${CMAKE_CXX_FLAGS})

# Compile libraries
foreach( SRCFILE ${MYSOURCES} )
string( REPLACE "." ";" SRCHOLDER ${SRCFILE} )
list( GET SRCHOLDER 0 LIBFILE )
list( APPEND MYLIBS ${LIBFILE} )
add_library( ${LIBFILE} STATIC ${SRCFILE} )
link_libraries( ${LIBFILE} )
endforeach()

add_library(${DICT_NAME} STATIC ${DICT_NAME}.cxx)
link_libraries(${DICT_NAME})
add_library(UBOnlineDBI SHARED ${MYLIBS} ${DICT_NAME})

# Final: executables
#add_executable(crap mymain.cc)
