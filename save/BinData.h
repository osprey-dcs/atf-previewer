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

#ifndef VIEWER_BINDATA_H
#define VIEWER_BINDATA_H

#include <utility>

#include <QString>
#include <QtCharts/QLineSeries>

#include "LineSeriesBuilderSimple.h"
#include "LineSeriesBuilderMinMax.h"
#include "LineSeriesBuilderSimpleFillUnder.h"
#include "LineSeriesBuilderMinMaxFillUnder.h"
#include "Cnst.h"

extern "C" {
#include <math.h>
#include <fftw3.h>
};

class BinData {

  public:
  
  using TwoDIntPtr = int(*)[5];

  LineSeriesBuilderBase *slsb;
  LineSeriesBuilderBase *lsb;
  LineSeriesBuilderBase *sfulsb;
  LineSeriesBuilderBase *fulsb;
  //TwoDIntPtr buf;

  BinData();
  virtual ~BinData();
  BinData( const BinData& ) = delete;
  BinData( BinData& ) = delete;
  BinData( BinData&& ) = delete;
  BinData& operator=( const BinData& ) = delete;
  BinData& operator=( BinData& ) = delete;
  BinData& operator=( BinData&& ) = delete;

  int genLineSeries( QString filename,
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

  int readTraceData( std::filebuf& fb, int *buf, int readSizeInbytes );

  void updateLineSeries (
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

  double sampleFreq = 1.0;
  //double slope = 1.0, intercept = 0.0;

};

#endif //VIEWER_BINDATA_H
