include(CheckLibraryExists)

include_directories(${libandfill_SOURCE_DIR}/include ${libandfill_BINARY_DIR}/include)

set(
	SOURCEFILES
	message.c
)

# A Solaris lullaby
add_definitions(-D_POSIX_PTHREAD_SEMANTICS)


find_library(PTHREAD_LIB pthread)
if(PTHREAD_LIB)
	list(APPEND LIBRARIES ${PTHREAD_LIB})
endif(PTHREAD_LIB)

add_library(andfill SHARED ${SOURCEFILES})
if(LIBRARIES)
	target_link_libraries(andfill ${LIBRARIES})
endif(LIBRARIES)