file(GLOB_RECURSE sources ./*.cpp ./*.hpp)

add_library(hfs STATIC ${sources})

if(WIN32)
	set(OUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/bin/win32/include")
	set(OUT_LIB_DIR "${CMAKE_SOURCE_DIR}/bin/win32/lib/${CMAKE_GENERATOR_PLATFORM}")
endif(WIN32)
if(UNIX)
	set(OUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/bin/unix")
	set(OUT_LIB_DIR "${CMAKE_SOURCE_DIR}/bin/unix")
endif(UNIX)

file(GLOB_RECURSE headers ./*.hpp)
foreach(f ${headers})
	file(RELATIVE_PATH new_path "${CMAKE_SOURCE_DIR}/" ${f})
	configure_file(${f} "${OUT_INCLUDE_DIR}/${new_path}" COPYONLY)
endforeach()

set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${OUT_LIB_DIR}")
set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUT_LIB_DIR}")
set_target_properties(hfs PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUT_LIB_DIR}")