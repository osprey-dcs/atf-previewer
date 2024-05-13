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
// Created by jws3 on 4/5/24.
//

#ifndef VIEWER_BINDATABASE_H
#define VIEWER_BINDATABASE_H

#include <utility>
#include <memory>

#include <QString>
#include <QtCharts/QLineSeries>
#include <QSharedPointer>

#include "LineSeriesBuilderSimple.h"
#include "LineSeriesBuilderMinMax.h"
#include "LineSeriesBuilderSimpleFillUnder.h"
#include "LineSeriesBuilderMinMaxFillUnder.h"
#include "Cnst.h"

extern "C" {
#include <math.h>
#include <fftw3.h>
};

class BinDataBase {

  public:
  
  using TwoDIntPtr = int(*)[5];

  std::shared_ptr<LineSeriesBuilderBase> xlsb;
  std::shared_ptr<LineSeriesBuilderBase> slsb;
  std::shared_ptr<LineSeriesBuilderBase> lsb;
  std::shared_ptr<LineSeriesBuilderBase> sfulsb;
  std::shared_ptr<LineSeriesBuilderBase> fulsb;

  BinDataBase ();
  virtual ~BinDataBase ();
  BinDataBase ( const BinDataBase& ) = delete;
  BinDataBase ( BinDataBase& ) = delete;
  BinDataBase ( BinDataBase&& ) = delete;
  virtual BinDataBase& operator= ( const BinDataBase& ) = delete;
  virtual BinDataBase& operator= ( BinDataBase& ) = delete;
  virtual BinDataBase& operator= ( BinDataBase&& ) = delete;

  virtual int newFile( QString filename );

  virtual void initMaxBufSize( unsigned long max );

  virtual int getRecordRangeForTime( QString filename, double sampleRate, double minTime, double maxTime,
                                     unsigned long& min, unsigned long& max );
  
  virtual void inputSeekToStartOfData( std::filebuf &fb, unsigned long firstDataByte );

  virtual void outputSeekToStartOfData( std::filebuf &fb, unsigned long firstDataByte );

  virtual int getMaxElements ( QString filename, int sigIndex, unsigned long& max );

  virtual int genLineSeries ( QString filename,
                     int sigIndex,
                     double slope,
                     double intercept,
                     int plotAreaWidthPixels,
                     double startTimeInSec,
                     double endTimeInSec,
                     double dataTimeIncrementInSec,
                     unsigned long& numPts,
                     QtCharts::QLineSeries& qls,
                     double& miny,
                     double& maxy,
                     unsigned long maxFft,
                     unsigned long& numFft,
                     fftw_complex *array  );

  virtual int readTraceData( std::filebuf& fb, int *buf, int readSizeInbytes );

  virtual int getMaxElements2 ( QString filename, int sigIndex, unsigned long& max );

  virtual int genLineSeries2 ( QString filename,
                     int sigIndex,
                     double slope,
                     double intercept,
                     int plotAreaWidthPixels,
                     double startTimeInSec,
                     double endTimeInSec,
                     double dataTimeIncrementInSec,
                     unsigned long& numPts,
                     QtCharts::QLineSeries& qls,
                     double& miny,
                     double& maxy,
                     unsigned long maxFft,
                     unsigned long& numFft,
                     fftw_complex *array  );

  virtual int readTraceData2 ( std::filebuf& fb, int *buf, int readSizeInbytes );

  virtual void updateLineSeries (
    int readOpCount,
    QPointF *pts,
    double slope,
    double intercept,
    double& timeStep,
    int plotAreaWidthPixels,
    double startTimeInSec,
    double endTimeInSec,
    double dataTimeIncrementInSec,
    int numBytesToProcess,
    int *buf,
    LineSeriesBuilderBase *lsb,
    double& miny,
    double &maxy );

  virtual int genFftFillUnderLineSeriesFromBuffer (
    int num,
    fftw_complex *buf,
    double sampleRate,
    int plotAreaWidthPixels,
    QtCharts::QLineSeries& qls,
    double& minx,
    double& maxx,
    double& miny,
    double& maxy );

  virtual int genFftFillUnderLineSeriesFromBufferByFreq (
    int num,
    fftw_complex *buf,
    double sampleRate,
    int plotAreaWidthPixels,
    double freqMin,
    double freqMax,
    QtCharts::QLineSeries& qls,
    double& minx,
    double& maxx,
    double& miny,
    double& maxy );

virtual int genFftLineSeriesFromBuffer (
 int num,
 fftw_complex *buf,
 double sampleRate,
 int plotAreaWidthPixels,
 QtCharts::QLineSeries& qls,
 double& minx,
 double& maxx,
 double& miny,
 double& maxy,
 bool suppressZeros=false );

virtual int genFftLineSeriesFromBufferByFreq (
 int num,
 fftw_complex *buf,
 double sampleRate,
 int plotAreaWidthPixels,
 double freqMin,
 double freqMax,
 QtCharts::QLineSeries& qls,
 double& minx,
 double& maxx,
 double& miny,
 double& maxy,
 bool suppressZeros=false );
  
};

#endif //VIEWER_BINDATABASE_H
