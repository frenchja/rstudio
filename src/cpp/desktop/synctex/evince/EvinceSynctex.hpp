/*
 * EvinceSynctex.hpp
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

#ifndef DESKTOP_SYNCTEX_EVINCESYNCTEX_HPP
#define DESKTOP_SYNCTEX_EVINCESYNCTEX_HPP

#include <QObject>
#include <QMap>
#include <QPoint>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

#include <DesktopSynctex.hpp>

#include "EvinceWindow1.hpp"
#include "EvinceWindow2.hpp"
#include "EvinceWindow3.hpp"

namespace desktop {

class MainWindow;

namespace synctex {

class EvinceDaemon;
class EvinceWindow;

class EvinceSynctex : public Synctex
{
   Q_OBJECT

public:
   EvinceSynctex(const SynctexViewerInfo& sv, MainWindow* pMainWindow);

   virtual void syncView(const QString& pdfFile,
                         const QString& srcFile,
                         const QPoint& srcLoc);

   virtual void syncView(const QString& pdfFile, int pdfPage);

private slots:
   void onFindWindowFinished(QDBusPendingCallWatcher *pCall);
   void onSyncViewFinished(QDBusPendingCallWatcher *pCall);
   void onClosed();

   void onSyncSource(const QString &source_file,
                     const QPoint &source_point);
   void onSyncSource3(const QString &source_file,
                      const QPoint &source_point,
                      uint timestamp);

private:
   // these methods manage the fact that there are 3 different
   // versions of the Evince Synctex API

   EvinceWindow* createWindow(const QString& service);

   QDBusPendingReply<> callSyncView(EvinceWindow* pWindow,
                                    const QString& srcFile,
                                    const QPoint& srcLoc,
                                    uint timestamp)
   {
      switch(windowDBusVersion_)
      {
         case 1:
            return ((EvinceWindow1*)(pWindow))->SyncView(srcFile, srcLoc);
         case 2:
            return ((EvinceWindow2*)(pWindow))->SyncView(srcFile,
                                                         srcLoc,
                                                         timestamp);
         case 3:
         default:
            return ((EvinceWindow3*)(pWindow))->SyncView(srcFile,
                                                         srcLoc,
                                                         timestamp);
      }
   }

   void connectSyncSource(EvinceWindow* pWindow);

private:
   struct SyncRequest
   {
      SyncRequest()
         : page(-1)
      {
      }

      SyncRequest(QString pdfFile, int page)
         : pdfFile(pdfFile), page(page)
      {
      }

      SyncRequest(QString pdfFile, QString srcFile, QPoint srcLoc)
         : pdfFile(pdfFile), page(-1), srcFile(srcFile), srcLoc(srcLoc)
      {
      }

      QString pdfFile;
      int page;
      QString srcFile;
      QPoint srcLoc;
   };

   void syncView(const SyncRequest& syncRequest);

   void syncView(EvinceWindow* pWindow, const SyncRequest& syncRequest);
   void syncView(EvinceWindow* pWindow,
                 const QString& srcFile,
                 const QPoint& srcLoc);

private:
   int windowDBusVersion_;

   EvinceDaemon* pEvince_;
   QMap<QString, EvinceWindow*> windows_;

   QMap<QDBusPendingCallWatcher*, SyncRequest> pendingSyncRequests_;
};


} // namespace synctex
} // namespace desktop

#endif // DESKTOP_SYNCTEX_EVINCESYNCTEX_HPP
