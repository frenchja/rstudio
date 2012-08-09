/*
 * PamAuthMain.cpp
 *
 * Copyright (C) 2009-12 by RStudio, Inc.
 *
 * This program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */


#include <iostream>
#include <stdio.h>

#include <core/Error.hpp>
#include <core/Log.hpp>
#include <core/system/System.hpp>

#include "Pam.hpp"

using namespace core ;
using namespace server::pam;

namespace {

int inappropriateUsage(const ErrorLocation& location)
{
   return server::pam::inappropriateUsage("rserver-pam", location);
}

} // anonymous namespace


int main(int argc, char * const argv[]) 
{
   try
   { 
      // initialize log
      initializeSystemLog("rserver-pam", core::system::kLogLevelWarning);

      // ignore SIGPIPE
      Error error = core::system::ignoreSignal(core::system::SigPipe);
      if (error)
         LOG_ERROR(error);

      // ensure that we aren't being called inappropriately
      if (::isatty(STDIN_FILENO))
         return inappropriateUsage(ERROR_LOCATION);
      else if (::isatty(STDOUT_FILENO))
         return inappropriateUsage(ERROR_LOCATION);
      else if (argc != 2)
         return inappropriateUsage(ERROR_LOCATION);

      // read username from command line
      std::string username(argv[1]);

      // read password
      std::string password = readPassword(username);
      if (password.empty())
         return EXIT_FAILURE;

      // verify password
      if (PAM(false).login(username, password) == PAM_SUCCESS)
         return EXIT_SUCCESS;
      else
         return EXIT_FAILURE;
   }
   CATCH_UNEXPECTED_EXCEPTION
   
   // if we got this far we had an unexpected exception
   return EXIT_FAILURE ;
}

