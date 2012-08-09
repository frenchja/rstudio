/*
 * PamSessionMain.cpp
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

#include <boost/bind.hpp>

#include <core/Error.hpp>
#include <core/Log.hpp>

#include <core/system/PosixUser.hpp>
#include <core/system/PosixSystem.hpp>
#include <core/system/ShellUtils.hpp>
#include <core/system/Process.hpp>

#include "Pam.hpp"

using namespace core ;
using namespace server::pam;

namespace {

int inappropriateUsage(const ErrorLocation& location)
{
   return server::pam::inappropriateUsage("rserver-pam-session", location);
}


void initPamSession(PAM* pPam)
{
   pPam->initSession();
}

void assumeUserIdentity(const std::string& username,
                        boost::function<void()> onAfterGroupInit)
{
   if (core::system::realUserIsRoot())
   {
      Error error = core::system::permanentlyDropPriv(username,
                                                      onAfterGroupInit);
      if (error)
         LOG_ERROR(error);
   }
   else
   {
      onAfterGroupInit();
   }
}

} // anonymous namespace


int main(int argc, char * const argv[]) 
{
   try
   { 
      // initialize log
      initializeSystemLog("rserver-pam-session",
                          core::system::kLogLevelWarning);

      // ignore SIGPIPE
      Error error = core::system::ignoreSignal(core::system::SigPipe);
      if (error)
         LOG_ERROR(error);

      // ensure that we aren't being called inappropriately
      if (::isatty(STDIN_FILENO))
         return inappropriateUsage(ERROR_LOCATION);
      else if (::isatty(STDOUT_FILENO))
         return inappropriateUsage(ERROR_LOCATION);
      else if (argc < 3)
         return inappropriateUsage(ERROR_LOCATION);

      // read rsession path and username from command line
      std::string rsessionPath(argv[1]);
      std::string username(argv[2]);

      // copy all other command-line arguments
      core::shell_utils::ShellArgs args;
      for (int i = 3; i<argc; i++)
         args << argv[i];

      // read password
      std::string password = readPassword(username);
      if (password.empty())
         return EXIT_FAILURE;

      // initialize PAM session
      PAM pamSession(false);
      if (pamSession.login(username, password) == EXIT_SUCCESS)
      {
         // run rsession and wait for it to terminate -- the PAM destructor
         // will take care of closing the pam session
         core::system::ProcessOptions options;
         boost::function<void()> onAfterInitGroups =
                                    boost::bind(&initPamSession, &pamSession);
         options.onAfterFork = boost::bind(assumeUserIdentity,
                                           username,
                                           onAfterInitGroups);
         core::system::ProcessResult result;
         Error error = core::system::runProgram(rsessionPath,
                                                args,
                                                password,
                                                options,
                                                &result);
         if (error)
         {
            LOG_ERROR(error);
            return EXIT_FAILURE;
         }

         return EXIT_SUCCESS;
      }
      else
      {
         return EXIT_FAILURE;
      }



   }
   CATCH_UNEXPECTED_EXCEPTION
   
   // if we got this far we had an unexpected exception
   return EXIT_FAILURE ;
}

