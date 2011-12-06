#
# CMakeLists.txt
#
# Copyright (C) 2009-11 by RStudio, Inc.
#
# This program is licensed to you under the terms of version 3 of the
# GNU Affero General Public License. This program is distributed WITHOUT
# ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
# MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
# AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
#
#

project (SVNCLIENT)

find_path(APR_INCLUDE_DIR apr.h
   HINTS
   /usr/include/apr-1
   /usr/local/include/apr-1
   /usr/include/apr-1.0
   /usr/local/include/apr-1.0
)
find_library(SVN_CLIENT_LIBS svn_client-1)
if (UNIX)
   set(SVN_CLIENT_LIBS
      ${SVN_CLIENT_LIBS}
      apr-1
      aprutil-1
      ssl
      gssapi_krb5
   )
else()
   # Win32
   set(SVN_CLIENT_LIBS
      ${SVN_CLIENT_LIBS}
      apr-1
      aprutil-1
      ssleay32
   )
endif()

# include files
file(GLOB_RECURSE SVNCLIENT_HEADER_FILES "*.h*")

# source files
set(SVNCLIENT_SOURCE_FILES
   SvnClientMain.cpp
   SvnClientOptions.cpp
)

# set include directories
include_directories(
   ${Boost_INCLUDE_DIRS}
   ${CORE_SOURCE_DIR}/include
   ${APR_INCLUDE_DIR}
)

# define executable
if(RSTUDIO_SESSION_WIN64)
  set(SVNCLIENT_NAME svnclient64)
else()
  set(SVNCLIENT_NAME svnclient)
endif()

add_executable(${SVNCLIENT_NAME} ${SVNCLIENT_SOURCE_FILES} ${SVNCLIENT_HEADER_FILES})

# set link dependencies
target_link_libraries(${SVNCLIENT_NAME}
   rstudio-core
   ${SVN_CLIENT_LIBS}
)

install(TARGETS ${SVNCLIENT_NAME} DESTINATION ${RSTUDIO_INSTALL_BIN})
