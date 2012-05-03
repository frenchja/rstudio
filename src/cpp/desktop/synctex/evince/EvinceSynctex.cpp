/*
 * EvinceSynctex.cpp
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

#include "EvinceSynctex.hpp"

#include <boost/format.hpp>

#include <core/Log.hpp>
#include <core/Error.hpp>
#include <core/DateTime.hpp>

#include <DesktopMainWindow.hpp>
#include <DesktopUtils.hpp>

#include "EvinceDaemon.hpp"


using namespace core;

namespace desktop {
namespace synctex {

namespace {

void logDBusError(const QDBusError& error, const ErrorLocation& location)
{
   boost::format fmt("Error %1% (%2%): %3%");
   std::string msg = boost::str(fmt % error.type() %
                                      error.name().toStdString() %
                                      error.message().toStdString());
   core::log::logErrorMessage(msg, location);
}

} // anonymous namespace

EvinceSynctex::EvinceSynctex(const SynctexViewerInfo& sv,
                             MainWindow* pMainWindow)
   : Synctex(pMainWindow)
{
   pEvince_ = new EvinceDaemon(this);

   // determine the window dbus api version based on the evince version
   // (see src/cpp/desktop/synctex/evince/README for more details on
   // the evolution of the api accross versions of evince)

   int ver = QT_VERSION_CHECK(sv.versionMajor,
                              sv.versionMinor,
                              sv.versionPatch);

   if (ver >= QT_VERSION_CHECK(2, 91, 3))
      windowDBusVersion_ = 3;
   else if (ver >= QT_VERSION_CHECK(2, 91, 0))
      windowDBusVersion_ = 2;
   else
      windowDBusVersion_ = 1;
}

void EvinceSynctex::syncView(const QString& pdfFile,
                             const QString& srcFile,
                             const QPoint& srcLoc)
{
   syncView(SyncRequest(pdfFile, srcFile, srcLoc));
}

void EvinceSynctex::syncView(const QString& pdfFile, int page)
{
   syncView(SyncRequest(pdfFile, page));
}

void EvinceSynctex::syncView(const SyncRequest& syncRequest)
{
   QString pdfFile = syncRequest.pdfFile;
   if (windows_.contains(pdfFile))
   {
      syncView(windows_.value(pdfFile), syncRequest);
   }
   else
   {
      // find the window
      QDBusPendingReply<QString> reply = pEvince_->FindDocument(
                                       QUrl::fromLocalFile(pdfFile).toString(),
                                       true);

      // wait for the results asynchronously
      QDBusPendingCallWatcher* pWatcher = new QDBusPendingCallWatcher(reply,
                                                                      this);
      pendingSyncRequests_.insert(pWatcher, syncRequest);

      QObject::connect(pWatcher,
                       SIGNAL(finished(QDBusPendingCallWatcher*)),
                       this,
                       SLOT(onFindWindowFinished(QDBusPendingCallWatcher*)));
   }
}

void EvinceSynctex::onFindWindowFinished(QDBusPendingCallWatcher* pWatcher)
{
   // get the reply and the sync request params
   QDBusPendingReply<QString> reply = *pWatcher;
   SyncRequest req = pendingSyncRequests_.value(pWatcher);
   pendingSyncRequests_.remove(pWatcher);

   if (reply.isError())
   {
      logDBusError(reply.error(), ERROR_LOCATION);
   }
   else
   {
      // initialize a connection to it
      EvinceWindow* pWindow = createWindow(reply.value());
      if (!pWindow->isValid())
      {
         logDBusError(pWindow->lastError(), ERROR_LOCATION);
         return;
      }

      // put it in our map
      windows_.insert(req.pdfFile, pWindow);

      // sign up for events
      QObject::connect(pWindow,
                       SIGNAL(Closed()),
                       this,
                       SLOT(onClosed()));

      connectSyncSource(pWindow);

      // perform sync
      syncView(pWindow, req);
   }

   // delete the watcher
   pWatcher->deleteLater();
}

void EvinceSynctex::syncView(EvinceWindow* pWindow, const SyncRequest& req)
{
   if (req.page != -1)
   {
      QStringList args;
      args.append(QString::fromAscii("-i"));
      args.append(QString::fromStdString(
                           boost::lexical_cast<std::string>(req.page)));
      args.append(req.pdfFile);
      QProcess::startDetached(QString::fromAscii("evince"), args);
   }
   else
   {
      syncView(pWindow, req.srcFile, req.srcLoc);
   }

}

void EvinceSynctex::syncView(EvinceWindow* pWindow,
                             const QString& srcFile,
                             const QPoint& srcLoc)
{
   QDBusPendingReply<> reply = callSyncView(
                                       pWindow,
                                       srcFile,
                                       srcLoc,
                                       core::date_time::secondsSinceEpoch());

   // wait for the results asynchronously
   QDBusPendingCallWatcher* pWatcher = new QDBusPendingCallWatcher(reply,
                                                                   this);
   QObject::connect(pWatcher,
                    SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this,
                    SLOT(onSyncViewFinished(QDBusPendingCallWatcher*)));
}

void EvinceSynctex::onSyncViewFinished(QDBusPendingCallWatcher* pWatcher)
{
   QDBusPendingReply<QString> reply = *pWatcher;
   if (reply.isError())
      logDBusError(reply.error(), ERROR_LOCATION);

   pWatcher->deleteLater();
}

void EvinceSynctex::onClosed()
{
   // get the window that closed and determine the associated pdf
   EvinceWindow* pWindow = static_cast<EvinceWindow*>(sender());
   QString pdfFile = windows_.key(pWindow);

   // notify base
   Synctex::onClosed(pdfFile);

   // remove window
   windows_.remove(pdfFile);
   pWindow->deleteLater();
}

EvinceWindow* EvinceSynctex::createWindow(const QString& service)
{
   switch(windowDBusVersion_)
   {
      case 1:
         return new EvinceWindow1(service);
      case 2:
         return new EvinceWindow2(service);
      case 3:
      default:
         return new EvinceWindow3(service);
   }
}

void EvinceSynctex::connectSyncSource(EvinceWindow* pWindow)
{
   switch(windowDBusVersion_)
   {
      case 1:
         QObject::connect(
               (EvinceWindow1*)pWindow,
               SIGNAL(SyncSource(const QString&,const QPoint&)),
               this,
               SLOT(onSyncSource(const QString&,const QPoint&)));
         break;
      case 2:
         QObject::connect(
               (EvinceWindow2*)pWindow,
               SIGNAL(SyncSource(const QString&,const QPoint&)),
               this,
               SLOT(onSyncSource(const QString&,const QPoint&)));
         break;
      case 3:
      default:
         QObject::connect(
               (EvinceWindow3*)pWindow,
               SIGNAL(SyncSource(const QString&,const QPoint&,uint)),
               this,
               SLOT(onSyncSource3(const QString&,const QPoint&,uint)));
         break;
   }
}


void EvinceSynctex::onSyncSource(const QString& srcFile,
                                 const QPoint& srcLoc)
{
   QUrl fileUrl(srcFile);
   Synctex::onSyncSource(fileUrl.toLocalFile(), srcLoc);
}

void EvinceSynctex::onSyncSource3(const QString& srcFile,
                                 const QPoint& srcLoc,
                                 uint)
{
   onSyncSource(srcFile, srcLoc);
}

} // namesapce synctex
} // namespace desktop
