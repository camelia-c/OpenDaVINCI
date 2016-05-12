# OpenDaVINCI - MongoDB recorder 
# Copyright (C) 2016 Camelia Ciolac
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

PROJECT (MongoDBrecorder)


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


include_directories(${MONGO_INCLUDE_DIRS})
#include_directories(${MONGO_LIBRARY_DIRS})
#end added for MongoDB


###########################################################################
# Add a local CMake module search path dependent on the desired installation destination.
# Thus, artifacts from the complete source build can be given precendence over any installed versions.
IF(UNIX)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()
IF(WIN32)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()

###########################################################################
# Compile flags to enable C++11.
SET (CXX_OPTIONS       "-std=c++11")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${CXX_OPTIONS} -pipe -std=c++11") #added for MongoDB -std=c++11

###########################################################################
# Find OpenDaVINCI.
SET(OPENDAVINCI_DIR "${CMAKE_INSTALL_PREFIX}")
FIND_PACKAGE (OpenDaVINCI REQUIRED)


###########################################################################
# Find AutomotiveData, needed for DBRecorder, when inserting in MongoDB
#for MongoDB, but creates circular dependency

SET(AUTOMOTIVEDATA_DIR "${CMAKE_INSTALL_PREFIX}")
FIND_PACKAGE (AutomotiveData REQUIRED)

###############################################################################

# Set linking libraries to successfully link test suite binaries.
SET (LIBRARIES ${CMAKE_THREAD_LIBS_INIT} ${MONGO_LDFLAGS})# ${AUTOMOTIVEDATA_LIBRARIES}) #for MongoDB, but creates circular dependency
IF(LIBRT_FOUND)
    SET(LIBRARIES ${LIBRARIES} ${LIBRT_LIBRARIES} ${MONGO_LDFLAGS})# ${AUTOMOTIVEDATA_LIBRARIES}) #for MongoDB
ENDIF()

###############################################################################
# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES (${OPENDAVINCI_INCLUDE_DIRS})

INCLUDE_DIRECTORIES (${AUTOMOTIVEDATA_INCLUDE_DIRS})
#INCLUDE_DIRECTORIES (${AUTOMOTIVEDATA_INCLUDE_DIRS}/automotivedata) #for MongoDB
#include_directories(${MONGO_INCLUDE_DIRS})

# Set include directory.
INCLUDE_DIRECTORIES(include)


#################################################################################

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libautomotivedata")
SET(CPACK_RPM_PACKAGE_REQUIRES "libautomotivedata")

###############################################################################
# Build this project.

ADD_EXECUTABLE (mongodbrecorder "DBRecorder.cpp")
TARGET_LINK_LIBRARIES (mongodbrecorder ${OPENDAVINCI_LIBRARIES} ${MONGO_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES}) 

###############################################################################
# Install this project.
INSTALL(TARGETS mongodbrecorder RUNTIME DESTINATION bin COMPONENT MongoDBrecorder)


