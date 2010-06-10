include(CheckIncludeFiles)
include(CheckSymbolExists)

# Test Macro
# 
# This macro compiles CMake/tests.c with the appropriate
# -DTESTNAME flag for the requested test. To implement a
# test, just add a section in CMake/tests.c encased by a
# #define HAVE_<TESTNAME> preprocessor directive. This section
# necessarily has to contain a main function returning
# 0 for a positive test result or some value >0 for a failed
# test.
macro(LF_INTERNAL_TEST_RUN LF_TEST)
	if("${LF_TEST}_COMPILE" MATCHES "^${LF_TEST}_COMPILE$")
		set(MACRO_CHECK_FUNCTION_DEFINITIONS "-D${LF_TEST} ${CMAKE_REQUIRED_FLAGS}")

		if(CMAKE_REQUIRED_LIBRARIES)
			set(LF_TEST_ADD_LIBRARIES "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
		endif(CMAKE_REQUIRED_LIBRARIES)
		
		message(STATUS "Performing libandfill Test ${LF_TEST}")
		try_run(
			${LF_TEST}
			${LF_TEST}_COMPILE
			${CMAKE_BINARY_DIR}
			${libandfill_SOURCE_DIR}/CMake/tests.c
			CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
			"${LF_TEST_ADD_LIBRARIES}"
			OUTPUT_VARIABLE OUTPUT
		)
		
		if(${LF_TEST}_COMPILE AND NOT ${LF_TEST})
			set(${LF_TEST} 1 CACHE INTERNAL "libandfill test ${FUNCTION}")
			message(STATUS "Performing libandfill Test ${LF_TEST} - Success")
		else(${LF_TEST}_COMPILE AND NOT ${LF_TEST})
			message(STATUS "Performing libandfill Test ${LF_TEST} - Failed")
			set(${LF_TEST} "" CACHE INTERNAL "libandfill test ${FUNCTION}")
			file(
				APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log"
				"Performing libandfill Test ${LF_TEST} failed with the following output:\n"
				"${OUTPUT}"
			)
			if(${LF_TEST}_COMPILE)
				file(
					APPEND
					"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log"
					"There was a problem running this test\n"
				)
			endif(${LF_TEST}_COMPILE)
			file(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log" "\n\n")
		endif(${LF_TEST}_COMPILE AND NOT ${LF_TEST})
	endif("${LF_TEST}_COMPILE" MATCHES "^${LF_TEST}_COMPILE$")
endmacro(LF_INTERNAL_TEST_RUN)

# Run test macro for several tests
foreach(LF_TEST
	HAVE_GLIBC_STRERROR_R
	HAVE_POSIX_STRERROR_R
	)
	by_internal_test_run(${LF_TEST})
endforeach(LF_TEST)

# Write result of tests into config.h
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(landfill)