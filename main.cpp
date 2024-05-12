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
#include <memory>

#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>
#include <QString>
#include <QFile>
#include <QThread>
#include <QDebug>
#include "ViewerMainWin.h"
#include "ViewerCtlr.h"
#include "UserPrefs.h"
#include "UserPrefsFac.h"
#include "ViewerCtlr.h"
#include "ChanSelector.h"

int main(int argc, char *argv[]) {

/*  ChanSelector chan;
  int st = chan.setText( "1, 2, 55, 4-10" );
  if ( st ) std::cout << "error " << st << " from chan.setText" << std::endl;
  std::list<int> l = chan.getList();
  chan.report();
  std::cout << "list: " << std::endl;
  for ( int item : l ) {
    std::cout << item << " ";
  }
  std::cout << std::endl;
  return 0;*/

  QApplication app(argc, argv);

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

  QSharedPointer<ViewerCtlr> mainCtlr = QSharedPointer<ViewerCtlr>( new ViewerCtlr( mainWin ) );

  int count = 0, maxMs = 250;
  int delay = 1000000 * Cnst::EventProcessPeriod;
  do {
    app.processEvents(QEventLoop::AllEvents, maxMs);
    mainCtlr->process();
    QThread::usleep( delay );
  } while ( !mainWin->exitFlag );

  return 0;

  //return QApplication::exec();
  int qResult = QApplication::exec();

  return qResult;

}
