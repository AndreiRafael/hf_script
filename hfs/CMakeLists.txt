file(GLOB_RECURSE sources ./*.cpp ./*.hpp)

#compile lib
add_library(hfs STATIC ${sources})

#compile executable (for testing)
add_executable(hfs_test ${sources})

if(WIN32)
	set(OUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/bin/win32/include")
	set(OUT_LIB_DIR "${CMAKE_SOURCE_DIR}/bin/win32/lib/${CMAKE_GENERATOR_PLATFORM}")
	set(OUT_TEST_DIR "${CMAKE_SOURCE_DIR}/bin/win32/test/${CMAKE_GENERATOR_PLATFORM}")
endif(WIN32)
if(UNIX)
	set(OUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/bin/unix/include")
	set(OUT_LIB_DIR "${CMAKE_SOURCE_DIR}/bin/unix/lib")
	set(OUT_TEST_DIR "${CMAKE_SOURCE_DIR}/bin/unix/test")
endif(UNIX)

file(GLOB_RECURSE headers ./*.hpp)
foreach(f ${headers})
	file(RELATIVE_PATH new_path "${CMAKE_SOURCE_DIR}/" ${f})
	configure_file(${f} "${OUT_INCLUDE_DIR}/${new_path}" COPYONLY)
endforeach()

set_target_properties(hfs_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUT_TEST_DIR}")
set_target_properties(hfs_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUT_TEST_DIR}")
set_target_properties(hfs_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUT_TEST_DIR}")

set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${OUT_LIB_DIR}")
set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUT_LIB_DIR}")
set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUT_LIB_DIR}")