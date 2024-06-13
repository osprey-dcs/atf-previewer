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
// Created by jws3 on 6/12/24.
//

#ifndef VIEWER_DATAMEDIATOR_H
#define VIEWER_DATAMEDIATOR_H

#include <memory>
#include <string>

#include <QtCharts/QLineSeries>
#include <QSharedPointer>

#include "LineSeriesBuilderSimple.h"
#include "LineSeriesBuilderMinMax.h"
#include "LineSeriesBuilderSimpleFillUnder.h"
#include "LineSeriesBuilderMinMaxFillUnder.h"
#include "Cnst.h"
#include "DataHeaderFac.h"
#include "BinDataFac.h"
#include "ErrHndlr.h"

extern "C" {
#include <math.h>
#include <fftw3.h>
};

class DataMediator : public ErrHndlr {

  public:

  static const int NumErrs = 8;
  static const int ESuccess = 0;
  static const int EFileOpen = 1;
  static const int EFileRead = 2;
  static const int ESampleRate = 3;
  static const int ESampleSize = 4;
  static const int EMax = 5;
  static const int ERange = 6;
  static const int ENoFile = 7;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "File open failure: " },
    { "File read failure: " },
    { "Illegal sample rate: " },
    { "Data set size too small" },
    { "Failure to read data size from file: " },
    { "Index out of range" },
    { "File not open" }
  };

  DataHeaderFac dhf;
  std::shared_ptr<DataHeader> dh;
  BinDataFac bdf;
  std::shared_ptr<BinDataBase> bd;
  std::string binDataName;
  std::shared_ptr<LineSeriesBuilderBase> slsb;
  std::shared_ptr<LineSeriesBuilderBase> lsb;
  std::shared_ptr<LineSeriesBuilderBase> sfulsb;
  std::shared_ptr<LineSeriesBuilderBase> fulsb;

  DataMediator();
  explicit DataMediator( const std::string& name );
  virtual ~DataMediator ();
  DataMediator ( const DataMediator& ) = delete;
  DataMediator ( DataMediator& ) = delete;
  DataMediator ( DataMediator&& ) = delete;
  DataMediator& operator= ( const DataMediator& ) = delete;
  DataMediator& operator= ( DataMediator& ) = delete;
  DataMediator& operator= ( DataMediator&& ) = delete;

  int newFile( std::string filename );

  int getMaxElements ( std::string filename, int64_t& max );

  int genLineSeries ( std::string filename,
                      int sigIndex,
                      double slope,
                      double intercept,
                      int plotAreaWidthPixels,
                      double startTimeInSec,
                      double endTimeInSec,
                      double dataTimeIncrementInSec,
                      int64_t& numPts,
                      QtCharts::QLineSeries& qls,
                      double& miny,
                      double& maxy,
                      int64_t maxFft,
                      int64_t& numFft,
                      fftw_complex *array  );

  int64_t readTraceData( int *buf, int64_t readSizeInbytes );

  int64_t readTraceData( std::filebuf& fb, int *buf, int64_t readSizeInbytes );
  
  void updateLineSeries ( int64_t readOpCount,
                          QPointF *pts,
                          double slope,
                          double intercept,
                          double& timeStep,
                          int plotAreaWidthPixels,
                          double startTimeInSec,
                          double endTimeInSec,
                          double dataTimeIncrementInSec,
                          int64_t numBytesToProcess,
                          int *buf,
                          LineSeriesBuilderBase *lsb,
                          double& miny,
                          double &maxy );

// from BinaryDataBase class

  void initMaxBufSize( int64_t max );

  int getDataFullTimeRange( std::string filename, double sampleRate, double& minTime, double& maxTime );

  int getRecordRangeForTime( std::string filename, double sampleRate, double minTime, double maxTime,
                             int64_t& min, int64_t& max );

  void inputSeekToStartOfData( std::filebuf& fb, int64_t firstDataByte );

  void outputSeekToStartOfData( std::filebuf& fb, int64_t firstDataByte );

  int genFftFillUnderLineSeriesFromBuffer (
    int num,
    fftw_complex *buf,
    double sampleRate,
    int plotAreaWidthPixels,
    QtCharts::QLineSeries& qls,
    double& minx,
    double& maxx,
    double& miny,
    double& maxy );

  int genFftFillUnderLineSeriesFromBufferByFreq (
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

  int genFftLineSeriesFromBuffer (
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

  int genFftLineSeriesFromBufferByFreq (
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

#endif //VIEWER_DATAMEDIATOR_H
