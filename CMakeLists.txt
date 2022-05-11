cmake_minimum_required(VERSION 3.10)
project(rtperfloglib VERSION 0.0.1)

set(CMAKE_C_STANDARD 99)

add_definitions(-D_GNU_SOURCE)

add_library(rtperflog
        src/logger.c )


target_include_directories(rtperflog PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include> )


install(TARGETS rtperflog
	EXPORT rtperflogTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
	)

install(DIRECTORY include/ DESTINATION include/rtperflog
	FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")


set(cmake_configfile_install lib/cmake/rtperflog)
set(target_install_dest_name "${cmake_configfile_install}/rtperflogTargets.cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/rtperflogConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/rtperflogConfig.cmake.in"
							  "${CMAKE_CURRENT_BINARY_DIR}/cmake/rtperflogConfig.cmake"
							  INSTALL_DESTINATION "${cmake_configfile_install}"
							  PATH_VARS target_install_dest_name)

install(EXPORT rtperflogTargets
		FILE rtperflogTargets.cmake
		NAMESPACE rtperflog::
		DESTINATION "${cmake_configfile_install}"
		EXPORT_LINK_INTERFACE_LIBRARIES)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/rtperflogConfig.cmake"
			"${CMAKE_CURRENT_BINARY_DIR}/rtperflogConfigVersion.cmake"
        DESTINATION "${cmake_configfile_install}")
        












add_subdirectory(test)


