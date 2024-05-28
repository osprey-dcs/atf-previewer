/*
This file is part of viewer.

viewer is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

viewer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with viewer.
If not, see <https://www.gnu.org/licenses/>. 
*/

#include <vector>
#include <iostream>

#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QFile>
#include <QThread>
#include <QDebug>
#include "ViewerMainWin.h"
#include "ViewerCtlr.h"
#include "UserPrefs.h"
#include "UserPrefsFac.h"

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  bool doOpenFile = false;
  QString fileToOpen;
  QFile testFile;

  // argv[1] can be a file to open
  if ( argc == 2 ) {
    fileToOpen = argv[1];
    testFile.setFileName( fileToOpen );
    if ( testFile.exists() ) {
      if ( testFile.open( QIODevice::ReadOnly ) ) {
        doOpenFile = true;
      }
    }
    testFile.close();
    if ( !doOpenFile ) {
      std::cout << "Cannot access file: " << fileToOpen.toStdString() << std::endl;
      fileToOpen.clear();
    }
  }

  auto const sg = QGuiApplication::screens()[0]->availableGeometry();
  //std::cout << "w = " << sg.width() << ", h = " << sg.height() << std::endl;

  UserPrefsFac upfac;
  UserPrefs *up = upfac.createUserPrefs();
  double x, y, w, h;
  if ( up->getDouble( "x", x ) ) x = 50;
  if ( up->getDouble( "y", y ) ) y = 50;
  if ( up->getDouble( "w", w ) ) w = 1800;
  if ( up->getDouble( "h", h ) ) h = 900;

  // this doesn't help my vnc session - if w is set > 1080, my vnc server crashes
  if ( w > sg.width() ) w = sg.width();
  if ( h > sg.height() ) h = sg.height();

  QSharedPointer<ViewerMainWin> mainWin = QSharedPointer<ViewerMainWin>( new ViewerMainWin( &app, x, y, w, h ) );
  mainWin->config();
  mainWin->show();

  QSharedPointer<ViewerCtlr> mainCtlr = QSharedPointer<ViewerCtlr>( new ViewerCtlr( mainWin, fileToOpen ) );

  int count = 0, maxMs = 250;
  int delay = 1000000 * Cnst::EventProcessPeriod;
  do {
    QApplication::processEvents(QEventLoop::AllEvents, maxMs);
    mainCtlr->process();
    QThread::usleep( delay );
  } while ( !mainWin->exitFlag );

  return 0;

  //return QApplication::exec();
  //int qResult = QApplication::exec();
  //return qResult;

}
