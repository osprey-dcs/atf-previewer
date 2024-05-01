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

//
// Created by jws3 on 4/3/24.
//

#ifndef VIEWER_VIEWERCTLR_H
#define VIEWER_VIEWERCTLR_H

#include <iostream>
#include <list>
#include <tuple>
#include <vector>
#include <utility>
#include <cmath>
#include <memory>

#include <QList>
#include <QPointF>
#include <QVector>
#include <QObject>
#include <QWidget>
#include <QRectF>
#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include <QSharedPointer>

#include "Cnst.h"
#include "ViewerMainWin.h"
#include "UserPrefs.h"
#include "UserPrefsFac.h"
#include "DataHeader.h"
#include "DataHeaderFac.h"
#include "BinData.h"
#include "BinDataFac.h"
#include "ViewerSlider.h"
//#include "ViewStack.h"

extern "C" {
#include <math.h>
#include <fftw3.h>
};

class ViewerCtlr : public QObject {

  Q_OBJECT

public:
  static const int HaveDataRequest = 1;
  static const int HaveHorizontalPanRequest = 2;
  static const int HaveScaleRequest = 3;
  static const int HaveResetRequest = 4;
  static const int HaveRubberBandScaleRequest = 5;
  static const int HaveFftRequest = 6;
  static const int HavePrevViewRequest = 7;
  
  //                   request  widget               signal   filename
  //                                                 index
  std::list<std::tuple<int,     ViewerGraphAreaBase *,   int,     QString>> dataRequestList;

  QSharedPointer<ViewerMainWin> mainWindow;
  UserPrefsFac upfac;
  UserPrefs *up;
  double x, y, w, h;
  QRectF plotAreaDimensions[Cnst::NumGraphs];
  bool haveFile;
  QString fileName;
  bool haveHeader;
  bool readyForData;
  unsigned long curMaxElements;
  int curSliderValue;
  int prevSigIndex[Cnst::NumGraphs];
  double sampleRate;
  DataHeaderFac dhf;
  std::shared_ptr<DataHeader> dh;
  BinDataFac bdf;
  std::shared_ptr<BinData> bd;
  fftw_complex *fftIn;
  fftw_complex *fftOut;
  unsigned long numPts;
  unsigned long numFft;
  unsigned long maxFft;
  bool haveDataForFft;
  ViewerGraphAreaBase *lastDataRequestGraphArea;
  ViewerGraphAreaBase *fftVga;

  ViewerCtlr ( QSharedPointer<ViewerMainWin> mainWin );
  virtual ~ViewerCtlr ();
  ViewerCtlr ( const ViewerCtlr& ) = delete;
  ViewerCtlr ( ViewerCtlr& ) = delete;
  ViewerCtlr ( ViewerCtlr&& ) = delete;
  ViewerCtlr& operator= ( const ViewerCtlr& ) = delete;
  ViewerCtlr& operator= ( ViewerCtlr& ) = delete;
  ViewerCtlr& operator= ( ViewerCtlr&& ) = delete;

  void enableFftButton( ViewerGraphAreaBase *vga );
  ViewerGraphAreaBase *companionVga ( ViewerGraphAreaBase *primaryVga );
  int processHeaderFile ( void );
  void process ( void );
  bool allZero ( const QRectF& r );
  bool allNonZero ( const QRectF& r );
  bool arrayAllNonZero ( const QRectF r[], int n );

public slots:
  void dimension ( double, double, double, double );
  void sigNameChange ( int index );
  void sigNameChange1 ( int index, int id, QWidget *w );
  void sigAreaChanged ( const QRectF& plotArea );
  void fileSelected1 ( const QString& file );
  void haveHorizontalPan ( int, int, QString&, double, double, double, double );
  void haveScale ( int, int, QString&, double, double, double, double );
  void haveReset ( int, int, QString& );
  void haveRubberBandScale( int, int, QString& );
  void havePrevViewRequest( int, int, QString& );
  void haveDoCalcFft ( QWidget * );
  void mousePosition ( int, double, double );
  void sliderValue( QWidget *, int );
  void setSlider( QWidget *vga );

};

#endif //VIEWER_VIEWERCTLR_H
