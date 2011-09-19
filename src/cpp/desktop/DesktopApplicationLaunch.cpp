/*
 * DesktopApplicationLaunch.cpp
 *
 * Copyright (C) 2009-11 by RStudio, Inc.
 *
 * This program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */

#include "DesktopApplicationLaunch.hpp"

#include <core/FilePath.hpp>

using namespace core;

namespace desktop {

QString ApplicationLaunch::instanceIdFromFilename(const QString& filename)
{
   QString instanceId(QString::fromAscii("RStudio"));
   FilePath filePath(filename.toUtf8().constData());
   if (filePath.exists() &&
       !filePath.isDirectory() &&
       filePath.extension() == ".Rproj")
   {
      // get parent directory
      instanceId = QString::fromStdString(filePath.parent().absolutePath());

      // linux paths are case-sensitive while win32 and apple pahts are
      // not, so convert to lower so long we aren't building for linux
#ifndef Q_WS_X11
      instanceId = instanceId.toLower();
#endif
   }

   // return the instance-id
   return instanceId;
}

} // namespace desktop
