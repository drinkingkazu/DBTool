string(ASCII 27 Esc)
set(ColorReset "${Esc}[m")
set(Yellow      "${Esc}[33m")

cmake_minimum_required (VERSION 2.8)
project (UBOnlineDBI)

# Now set CMAKE variables for my use
set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "./lib")
set(DICT_NAME "UBOnlineDBIDict")
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
file( GLOB MYSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cxx  )
file( GLOB LINKDEF LinkDef.h )
list( GET LINKDEF 0 LINKDEF )
list( REMOVE_ITEM MYHEADERS ${LINKDEF} )

# Pre process to generate dictionary source code + getting compiler/linker flags
#execute_process(COMMAND pg_config --includedir-server OUTPUT_VARIABLE PSQL_INCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND pg_config --includedir OUTPUT_VARIABLE PSQL_INCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND pg_config --libdir OUTPUT_VARIABLE PSQL_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --incdir OUTPUT_VARIABLE ROOT_INCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libdir OUTPUT_VARIABLE ROOT_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND rootcint -f
${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx -c -I${PSQL_INCDIR} -I${ROOT_INCDIR} -I. -p  ${MYHEADERS} ${LINKDEF})
set(PSQL_LIB "-L${PSQL_LIBDIR} -lpq")
set(ROOT_LIB "-L${ROOT_LIBDIR} -lCore -lCint -lRIO")

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PSQL_INCDIR})
include_directories(${ROOT_INCDIR})
set(CMAKE_CXX_FLAGS "-std=c++11")

# Generate a package library ... make sure dictionary comes the end
add_library(UBOnlineDBI SHARED ${MYSOURCES} ${DICT_NAME}.cxx)
target_link_libraries(UBOnlineDBI ${PSQL_LIB} ${ROOT_LIB} )
set_target_properties( UBOnlineDBI
		       PROPERTIES
	               LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
if (APPLE)
   set_property(TARGET UBOnlineDBI PROPERTY PREFIX "lib")
   set_property(TARGET UBOnlineDBI PROPERTY OUTPUT_NAME "UBOnlineDBI.so")
   set_property(TARGET UBOnlineDBI PROPERTY SUFFIX "")
endif()


# Generate a rootmap file
add_custom_command( TARGET UBOnlineDBI
		    POST_BUILD
		    COMMAND rlibmap ARGS -f  -o ${CMAKE_CURRENT_SOURCE_DIR}/lib/libUBOnlineDBI.rootmap -l ${CMAKE_CURRENT_SOURCE_DIR}/lib/libUBOnlineDBI.so -c ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
		    )	    
set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/libUBOnlineDBI.rootmap" )

