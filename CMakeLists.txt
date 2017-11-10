cmake_minimum_required( VERSION 2.8.12 )

project( http_request_parser )

include( ExternalProject )

find_package( Boost 1.60.0 COMPONENTS system iostreams filesystem regex unit_test_framework REQUIRED )

enable_testing( )
add_definitions( -DBOOST_ALL_NO_LIB )

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
	add_definitions( -DENDIAN_BIG )
else()
	add_definitions( -DENDIAN_LITTLE )
endif()

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 /std:c++latest )
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -march=native -flto=thin -Weverything -Wno-missing-braces -Wno-c++98-compat -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
		#set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto=thin" )
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -flto -march=native -Wall -Wnonnull -Wno-deprecated-declarations)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
		#set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto" )
	endif( )
endif( )

externalproject_add(
    header_libraries_prj
    GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
include_directories( ${HEADER_FOLDER} )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FILES
	${HEADER_FOLDER}/http_req_parser.h
)

add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

add_executable( http_req_parser_test_bin ${HEADER_FILES} ${TEST_FOLDER}/http_req_parser_test.cpp )
target_link_libraries( http_req_parser_test_bin ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
add_dependencies( http_req_parser_test_bin header_libraries_prj )
add_test( http_req_parser_test http_req_parser_test_bin )

install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/crypto )

