# odrecorder - Tool for recording data
# Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (odrecorder)

#########################################################################
#added for MongoDB
SET(ENV{PKG_CONFIG_PATH} "/home/camelia/Documents/MYCDRIVER/lib/pkgconfig")
find_package(PkgConfig REQUIRED)

pkg_search_module(MONGO REQUIRED libmongocxx)

#message(SEND_ERROR "A ${MONGO_FOUND}")        # Error:A 1
#message(SEND_ERROR "B ${MONGO_LIBRARIES}")    # Error:B mongocxx;bsoncxx
#message(SEND_ERROR "C ${MONGO_LIBRARY_DIRS}") # Error:C /usr/local/lib
#message(SEND_ERROR "D ${MONGO_LDFLAGS}")      # Error:D -L/usr/local/lib;-lmongocxx;-lbsoncxx
#message(SEND_ERROR "E ${MONGO_LDFLAGS_OTHER}")# Error:E
#message(SEND_ERROR "F ${MONGO_INCLUDE_DIRS}") # Error:F /home/camelia/Documents/MYCDRIVER/include/libbson-1.0;/home/camelia/Documents/MYCDRIVER/include/libmongoc-1.0;/usr/local/include/mongocxx/v_noabi;/usr/local/include/bsoncxx/v_noabi
#message(SEND_ERROR "G ${MONGO_CFLAGS}")       # Error:G -I/home/camelia/Documents/MYCDRIVER/include/libbson-1.0;-I/home/camelia/Documents/MYCDRIVER/include/libmongoc-1.0;-I/usr/local/include/mongocxx/v_noabi;-I/usr/local/include/bsoncxx/v_noabi
#message(SEND_ERROR "H ${MONGO_CFLAGS_OTHER}") # Error:H


#include_directories(${MONGO_INCLUDE_DIRS})
#include_directories(${MONGO_LIBRARY_DIRS})
#end added for MongoDB


###########################################################################


###########################################################################
# Set the search path for .cmake files.
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake.Modules" ${CMAKE_MODULE_PATH})

# Add a local CMake module search path dependent on the desired installation destination.
# Thus, artifacts from the complete source build can be given precendence over any installed versions.
IF(UNIX)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()
IF(WIN32)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()

###########################################################################
# Include flags for compiling.
INCLUDE (CompileFlags)
#message(SEND_ERROR "--------------------------------------------------------------- ${CMAKE_CXX_FLAGS}") # here it sees -std=c++11
#SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # this gives error in  Performing build step for 'odtools', [ 50%] Building CXX object odrecorder/CMakeFiles/odrecorderlib-static.dir/src/DBRecorderModule.cpp.o -> requires -std=c++11 or -std=gnu++11 compiler options
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") #this gives other error, afterwards, in Scanning dependencies of target odrecorderlib-static , [ 50%] Building CXX object odrecorder/CMakeFiles/odrecorderlib-static.dir/src/RecorderModule.cpp.o  --> c++: fatal error: no input files , compilation terminated. , /bin/sh: 1: -std=c++11: not found


###########################################################################
# Find and configure CxxTest.
SET (CXXTEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../cxxtest") 
INCLUDE (CheckCxxTestEnvironment)

###########################################################################
# Find OpenDaVINCI.
SET(OPENDAVINCI_DIR "${CMAKE_INSTALL_PREFIX}")
FIND_PACKAGE (OpenDaVINCI REQUIRED)

###############################################################################
# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES (${OPENDAVINCI_INCLUDE_DIRS}/opendavinci)
# Set include directory.
INCLUDE_DIRECTORIES(include)
#for MongoDB
include_directories(${MONGO_INCLUDE_DIRS})

###############################################################################
# Build this project.
FILE(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
ADD_LIBRARY (${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
ADD_EXECUTABLE (${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${PROJECT_NAME}lib-static ${OPENDAVINCI_LIBRARIES}) 

###############################################################################
# Enable CxxTest for all available testsuites.
IF(CXXTEST_FOUND)
    FILE(GLOB thisproject-testsuites "${CMAKE_CURRENT_SOURCE_DIR}/testsuites/*.h")
    
    FOREACH(testsuite ${thisproject-testsuites})
        STRING(REPLACE "/" ";" testsuite-list ${testsuite})

        LIST(LENGTH testsuite-list len)
        MATH(EXPR lastItem "${len}-1")
        LIST(GET testsuite-list "${lastItem}" testsuite-short)

        SET(CXXTEST_TESTGEN_ARGS ${CXXTEST_TESTGEN_ARGS} --world=${PROJECT_NAME}-${testsuite-short})
        CXXTEST_ADD_TEST(${testsuite-short}-TestSuite ${testsuite-short}-TestSuite.cpp ${testsuite})
        IF(UNIX)
            IF( (   ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
                 OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
                 OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "DragonFly") )
                AND (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") )
                SET_SOURCE_FILES_PROPERTIES(${testsuite-short}-TestSuite.cpp PROPERTIES COMPILE_FLAGS "-Wno-effc++ -Wno-float-equal -Wno-error=suggest-attribute=noreturn")
            ELSE()
                SET_SOURCE_FILES_PROPERTIES(${testsuite-short}-TestSuite.cpp PROPERTIES COMPILE_FLAGS "-Wno-effc++ -Wno-float-equal")
            ENDIF()
        ENDIF()
        IF(WIN32)
            SET_SOURCE_FILES_PROPERTIES(${testsuite-short}-TestSuite.cpp PROPERTIES COMPILE_FLAGS "")
        ENDIF()
        SET_TESTS_PROPERTIES(${testsuite-short}-TestSuite PROPERTIES TIMEOUT 3000)
        TARGET_LINK_LIBRARIES(${testsuite-short}-TestSuite ${PROJECT_NAME}lib-static ${OPENDAVINCI_LIBRARIES})
    ENDFOREACH()
ENDIF(CXXTEST_FOUND)

###############################################################################
# Install this project.
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT odtools)
INSTALL(FILES man/${PROJECT_NAME}.1 DESTINATION man/man1 COMPONENT odtools)

