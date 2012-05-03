/*
 * DesktopSynctex.cpp
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

#include "DesktopSynctex.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <core/Error.hpp>
#include <core/SafeConvert.hpp>
#include <core/system/Process.hpp>

#include "DesktopUtils.hpp"

// per-platform synctex implemetnations
#if defined(Q_WS_MACX)

#elif defined(Q_OS_WIN)
#include "synctex/sumatra/SumatraSynctex.hpp"
#elif defined(Q_OS_LINUX)
#include "synctex/evince/EvinceSynctex.hpp"
#endif

using namespace core;

namespace desktop {

namespace {

#if defined(Q_OS_WIN)

SynctexViewerInfo discoverViewer()
{
  SynctexViewerInfo sv;
  sv.name = QString::fromAscii("SumatraPDF");
  sv.versionMajor = 2;
  sv.versionMinor = 0;
  sv.versionPatch = 1;
  return sv;
}

#elif defined(Q_OS_LINUX)

SynctexViewerInfo discoverViewer()
{
   // probe for evince version
   core::system::ProcessResult result;
   Error error = core::system::runCommand("evince --version",
                                          core::system::ProcessOptions(),
                                          &result);
   if (error)
   {
      LOG_ERROR(error);
      return SynctexViewerInfo();
   }
   else if (result.exitStatus != EXIT_SUCCESS)
   {
      return SynctexViewerInfo();
   }

   // trim output
   std::string stdOut = boost::algorithm::trim_copy(result.stdOut);

   // extract version
   boost::smatch match;
   boost::regex re("^.*(\\d+)\\.(\\d+)\\.(\\d)+$");
   if (boost::regex_match(stdOut, match, re))
   {
      SynctexViewerInfo sv;
      sv.name = QString::fromAscii("Evince");
      sv.versionMajor = safe_convert::stringTo<int>(match[1], 2);
      sv.versionMinor = safe_convert::stringTo<int>(match[2], 0);
      sv.versionPatch = safe_convert::stringTo<int>(match[3], 0);

      // synctex was introduced in evince 2.31.5
      if (QT_VERSION_CHECK(sv.versionMajor, sv.versionMinor, sv.versionPatch) >=
          QT_VERSION_CHECK(2, 31, 5))
      {
         return sv;
      }
      else
      {
         return SynctexViewerInfo();
      }
   }
   else
   {
      return SynctexViewerInfo();
   }
}

#else

SynctexViewerInfo discoverViewer()
{
   return SynctexViewerInfo();
}

#endif

} // anonymous namespace

const SynctexViewerInfo& Synctex::desktopViewer()
{
   static SynctexViewerInfo viewer;
   if (viewer.empty())
      viewer = discoverViewer();

   return viewer;
}

Synctex* Synctex::create(MainWindow* pMainWindow)
{
   // per-platform synctex implemetnations
#if defined(Q_WS_MACX)
   return new Synctex(pMainWindow);
#elif defined(Q_OS_WIN)
   return new synctex::SumatraSynctex(pMainWindow);
#elif defined(Q_OS_LINUX)
   return new synctex::EvinceSynctex(desktopViewer(), pMainWindow);
#else
   return new Synctex(pMainWindow);
#endif
}

void Synctex::onClosed(const QString& pdfFile)
{
   pMainWindow_->onPdfViewerClosed(pdfFile);
}

void Synctex::onSyncSource(const QString &srcFile, const QPoint &srcLoc)
{
   desktop::raiseAndActivateWindow(pMainWindow_);

   pMainWindow_->onPdfViewerSyncSource(srcFile, srcLoc.x(), srcLoc.y());
}

} // namespace desktop
