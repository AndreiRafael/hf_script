file(GLOB_RECURSE sources ./*.cpp ./*.hpp)

add_library(hfs STATIC ${sources})

if(WIN32)
	if(${CMAKE_GENERATOR_PLATFORM} EQUAL x64 OR ${CMAKE_GENERATOR_PLATFORM} EQUAL ARM64)
		set(DESTINATION_DIR "${CMAKE_SOURCE_DIR}/bin/win32/x64")
	else()
		set(DESTINATION_DIR "${CMAKE_SOURCE_DIR}/bin/win32/x86")
	endif()
endif(WIN32)
if(UNIX)
	set(DESTINATION_DIR "${CMAKE_SOURCE_DIR}/bin/unix")
endif(UNIX)

file(GLOB_RECURSE headers ./*.hpp)
foreach(f ${headers})
	file(RELATIVE_PATH new_path "${CMAKE_SOURCE_DIR}/" ${f})
	configure_file(${f} "${DESTINATION_DIR}/include/${new_path}" COPYONLY)
endforeach()

set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DESTINATION_DIR}/lib")
set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${DESTINATION_DIR}/lib")
set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${DESTINATION_DIR}/lib")