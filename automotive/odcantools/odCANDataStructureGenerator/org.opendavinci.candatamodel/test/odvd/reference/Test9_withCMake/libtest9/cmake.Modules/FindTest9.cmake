# Test9 - Data structure library generated by CANDataStructureGenerator.
###########################################################################
# Try to find Test9 library.
# The user can specify an additional search path using the CMake variable
# TEST9_DIR
# First, search at the specific user path setting.
IF(NOT ("${TEST9_DIR}" STREQUAL ""))
    FIND_PATH(TEST9_INCLUDE_DIR test9/GeneratedHeaders_Test9.h
                 NAMES test9
                 PATHS ${TEST9_DIR}/include
                 NO_DEFAULT_PATH)
    FIND_LIBRARY(TEST9_LIBRARY
                 NAMES test9 test9-static
                 PATHS ${TEST9_DIR}/lib
                 NO_DEFAULT_PATH)
ENDIF()
IF(   ("${TEST9_INCLUDE_DIR}" STREQUAL "TEST9_INCLUDE_DIR-NOTFOUND")
   OR ("${TEST9_DIR}" STREQUAL "") )
    MESSAGE(STATUS "Trying to find Test9 in default paths.")
    # If not found, use the system's search paths.
    FIND_PATH(TEST9_INCLUDE_DIR test9/GeneratedHeaders_Test9.h
                 NAMES test9
                 PATHS /usr/include
                       /usr/local/include)
    FIND_LIBRARY(TEST9_LIBRARY
                 NAMES test9 test9-static
                 PATHS /usr/lib
                       /usr/lib64
                       /usr/local/lib
                       /usr/local/lib64)
ENDIF()
IF("${TEST9_INCLUDE_DIR}" STREQUAL "")
    MESSAGE(FATAL_ERROR "Could not find Test9 library.")
ENDIF()
###########################################################################
# Set linking libraries.
SET(TEST9_LIBRARIES ${TEST9_LIBRARY})
SET(TEST9_INCLUDE_DIRS ${TEST9_INCLUDE_DIR})
###########################################################################
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibTest9 DEFAULT_MSG
                                  TEST9_LIBRARY TEST9_INCLUDE_DIR)
MARK_AS_ADVANCED(TEST9_INCLUDE_DIR TEST9_LIBRARY)
