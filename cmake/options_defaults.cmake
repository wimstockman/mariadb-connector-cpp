#
#  Copyright (C) 2021,2023 MariaDB Corporation AB
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the COPYING-CMAKE-SCRIPTS file.
#

OPTION(CONC_WITH_UNIT_TESTS "Build C/C unit tests" OFF)
OPTION(WITH_ASAN "Compile with ASAN" OFF)
OPTION(WITH_UBSAN "Enable undefined behavior sanitizer" OFF)
OPTION(WITH_MSAN "Enable memory sanitizer" OFF)

IF(WIN32 AND NOT MINGW)
  OPTION(WITH_MSI "Build MSI installation package" ON)
  OPTION(CONC_WITH_MSI "Build C/C MSI installation package" OFF)
  OPTION(WITH_SIGNCODE "Digitally sign files" OFF)

  OPTION(MARIADB_LINK_DYNAMIC "Link Connector/C library dynamically" OFF)
  OPTION(ALL_PLUGINS_STATIC "Compile all plugins in, i.e. make them static" OFF)
  SET(CLIENT_PLUGIN_PVIO_NPIPE "STATIC")
  # We don't provide its support in ODBC yet, thus there is no need to bloat the library size
  #SET(CLIENT_PLUGIN_PVIO_SHMEM "STATIC")
  SET(WITH_UBSAN OFF)
  SET(WITH_MSAN OFF)
ELSE()
  OPTION(WITH_MSI "Build MSI installation package" OFF)
  IF(APPLE)
    OPTION(MARIADB_LINK_DYNAMIC "Link Connector/C library dynamically" OFF)
    OPTION(WITH_SIGNCODE "Digitally sign files" OFF)
    IF(NOT "${DEVELOPER_ID}" STREQUAL "")
      SET(WITH_SIGNCODE ON)
    ENDIF()
  ELSE()
    OPTION(MARIADB_LINK_DYNAMIC "Link Connector/C library dynamically" ON)
  ENDIF()
  OPTION(ALL_PLUGINS_STATIC "Compile all plugins in, i.e. make them static" OFF)
ENDIF()

OPTION(WITH_SSL "Enables use of TLS/SSL library" ON)
OPTION(WITH_UNIT_TESTS "Build test suite" ON)

IF(MINGW)
  OPTION(USE_SYSTEM_INSTALLED_LIB "Use installed in the system C/C library and do not build one" ON)
ELSE()
  OPTION(USE_SYSTEM_INSTALLED_LIB "Use installed in the system C/C library and do not build one" OFF)
ENDIF()

# This is to be used for some testing scenarious, obviously. e.g. testing of the connector installation. 
OPTION(BUILD_TESTS_ONLY "Build only tests and nothing else" OFF)

IF(BUILD_TESTS_ONLY)
  SET(WITH_UNIT_TESTS ON)
ENDIF()
IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
  SET(WITH_UNIT_TESTS OFF)
ENDIF()

IF(WITH_UNIT_TESTS)
  SET_VALUE(TEST_HOST           "tcp://localhost:3306" "Defines Unit Tests default server")
  SET_VALUE(TEST_SCHEMA         "test"                 "Defines Unit Tests default Database")
  SET_VALUE(TEST_UID            "root"                 "Defines Unit Tests default login user")
  SET_VALUE(TEST_PASSWORD       "root"                 "Defines Unit Tests default login user password")
  SET_VALUE(TEST_USETLS         "false"                "Defines useTls option value to use in tests")

  IF("${TEST_USETLS}" STREQUAL "")
    SET(TEST_USETLS "false")
  ENDIF()
ELSE()
  SET(CONC_WITH_UNIT_TESTS OFF)
ENDIF()

IF(NOT EXISTS ${CMAKE_SOURCE_DIR}/libmariadb)
  SET(USE_SYSTEM_INSTALLED_LIB ON)
ENDIF()

IF(APPLE)
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
  CMAKE_POLICY(SET CMP0042 NEW)
  CMAKE_POLICY(SET CMP0068 NEW)
  SET(CMAKE_INSTALL_RPATH "")
  SET(CMAKE_INSTALL_NAME_DIR "")
  SET(CMAKE_MACOSX_RPATH ON)
ENDIF()

IF(WIN32) 
  # Currently limiting this feature to Windows only, where it's most probably going to be only used
  IF(ALL_PLUGINS_STATIC)
    SET(CLIENT_PLUGIN_AUTH_GSSAPI_CLIENT "STATIC")
    SET(CLIENT_PLUGIN_DIALOG "STATIC")
    SET(CLIENT_PLUGIN_CLIENT_ED25519 "STATIC")
    SET(CLIENT_PLUGIN_CACHING_SHA2_PASSWORD "STATIC")
    SET(CLIENT_PLUGIN_SHA256_PASSWORD "STATIC")
    SET(CLIENT_PLUGIN_MYSQL_CLEAR_PASSWORD "STATIC")
    SET(CLIENT_PLUGIN_MYSQL_OLD_PASSWORD "STATIC")
    SET(MARIADB_LINK_DYNAMIC OFF)
  ENDIF()
ELSE()
  SET(WITH_MSI OFF)
ENDIF()
