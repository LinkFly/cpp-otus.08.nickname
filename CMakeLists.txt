cmake_minimum_required(VERSION 3.2)
project(nickname VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

set(SOURCES
	main.cpp
	)
set(HEADERS
	share.h
	logger.h
	node.h
	nickname.h
	codepage-manager.h
	)
set(TESTS_HEADERS
	tests-utils.h
	)
set(DATA_FILES
	Readme.md

	)

add_executable(nickname ${SOURCES} ${HEADERS} ${DATA_FILES})
add_executable(tests tests.cpp ${TESTS_HEADERS} ${HEADERS})
set_target_properties(nickname tests PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	)

find_package(Boost COMPONENTS unit_test_framework filesystem REQUIRED)

target_link_libraries(nickname
	${Boost_LIBRARIES}
)

target_link_libraries(tests
	${Boost_LIBRARIES}
)

set_target_properties(nickname tests PROPERTIES
	COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
	INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
	LINK_DIRECTORIES ${Boost_LIBRARY_DIRS}
)

IF(WIN32)
set_target_properties(nickname tests PROPERTIES
  #COMPILE_OPTIONS "-Wall" # A lot of messages (in std libraries)
  COMPILE_OPTIONS ""
)
ELSE()
set_target_properties(nickname tests PROPERTIES
	COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-Wno-c++17-extensions"
)
ENDIF()

### Tests ###
enable_testing()
add_test(nickname_tests tests)
###

### Packaging ###
install(TARGETS nickname RUNTIME DESTINATION bin)
install(TARGETS tests RUNTIME DESTINATION lib/nickname/tests)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT linkfly@yandex.ru)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
