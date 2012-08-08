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

project (PAM)

# include files
file(GLOB_RECURSE PAM_HEADER_FILES "*.h*")

# find PAM
find_package(PAM REQUIRED)


# set include directories
include_directories(
   ${Boost_INCLUDE_DIRS}
   ${CORE_SOURCE_DIR}/include
   ${PAM_INCLUDE_DIR}
)

#### pam auth utility

# source files
set(PAM_AUTH_SOURCE_FILES
   Pam.cpp
   PamAuthMain.cpp
)

# define executable
add_executable(rserver-pam ${PAM_AUTH_SOURCE_FILES} ${PAM_HEADER_FILES})

# set link dependencies
target_link_libraries(rserver-pam
   rstudio-core
   ${PAM_LIBRARIES}
)

# installation rules
install(TARGETS rserver-pam DESTINATION ${RSTUDIO_INSTALL_BIN})


#### pam session utility

# source files
set(PAM_SESSION_SOURCE_FILES
   Pam.cpp
   PamSessionMain.cpp
)

# define executable
add_executable(rserver-pam-session ${PAM_SESSION_SOURCE_FILES} ${PAM_HEADER_FILES})

# set link dependencies
target_link_libraries(rserver-pam-session
   rstudio-core
   ${PAM_LIBRARIES}
)

# installation rules
install(TARGETS rserver-pam-session DESTINATION ${RSTUDIO_INSTALL_BIN})



