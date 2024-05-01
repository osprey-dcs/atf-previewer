//
// Created by jws3 on 4/3/24.
//

#ifndef VIEWER_VIEWERCTRL_H
#define VIEWER_VIEWERCTRL_H

#include <iostream>
#include <list>
#include <tuple>
#include <vector>
#include <utility>
#include <cmath>

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

#include "Cnst.h"
#include "ViewerMainWin.h"
#include "UserPrefs.h"
#include "UserPrefsFac.h"
#include "PlotBuffer.h"
#include "DataHeader.h"
#include "DataHeaderFac.h"
#include "BinData.h"
#include "BinDataFac.h"

extern "C" {
#include <math.h>
#include <fftw3.h>
};

class ViewerCtrl : public QObject {

  Q_OBJECT

public:
  static const int HaveDataRequest = 1;
  static const int HaveHorizontalPanRequest = 2;
  static const int HaveScaleRequest = 3;
  static const int HaveResetRequest = 4;
  static const int HaveRubberBandScaleRequest = 5;

  //                   request  widget               signal   filename
  //                                                 index
  std::list<std::tuple<int,     ViewerGraphArea *,   int,     QString>> dataRequestList;

  ViewerMainWin *mainWindow;
  UserPrefsFac upfac;
  UserPrefs *up;
  double x, y, w, h;
  QRectF plotAreaDimensions[Cnst::NumGraphs];
  bool haveFile;
  QString fileName;
  bool haveHeader;
  bool readyForData;
  int prevSigIndex[Cnst::NumGraphs];
  double sampleRate;
  DataHeaderFac *dhf;
  DataHeader *dh;
  BinDataFac *bdf;
  BinData *bd;
  fftw_complex *fftIn;
  unsigned long numPts;
  unsigned long numFft;
  unsigned long maxFft;

  ViewerCtrl ( ViewerMainWin *mainWin );
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
  
};

#endif //VIEWER_VIEWERCTRL_H
