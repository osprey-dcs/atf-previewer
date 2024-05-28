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

#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include <QList>
#include <QPointF>
#include <QLineSeries>
#include <QDebug>

#include "BinDataBase.h"

BinDataBase::BinDataBase() : ErrHndlr( NumErrs, errMsgs ) {

  this->slsb = std::shared_ptr<LineSeriesBuilderSimple>( new LineSeriesBuilderSimple() );
  this->lsb = std::shared_ptr<LineSeriesBuilderMinMax>( new LineSeriesBuilderMinMax() );
  this->sfulsb = std::shared_ptr<LineSeriesBuilderSimpleFillUnder>( new LineSeriesBuilderSimpleFillUnder() );
  this->fulsb = std::shared_ptr<LineSeriesBuilderMinMaxFillUnder>( new LineSeriesBuilderMinMaxFillUnder() );

}

BinDataBase::~BinDataBase() {

}

int BinDataBase::newFile( QString filename ) {
  return 0;
}

void BinDataBase::initMaxBufSize( int64_t max ) {
}

int BinDataBase::getDataFullTimeRange( QString filename, double sampleRate, double& minTime, double& maxTime ) {

  if ( sampleRate <= 0.0 ) return ERRINFO(ESampleRate,"");

  int64_t maxElements;

  int st = this->getMaxElements( filename, 0, maxElements );
  if ( st ) {
    dspErrMsg( st );
    return ERRINFO(st,"");
  }

  minTime = 0.0;
  maxTime = maxElements / sampleRate;

  //std::cout << "sampleRate = " << sampleRate<< std::endl;
  //std::cout << "maxElements = " << maxElements << std::endl;

  return 0;

}
  
int BinDataBase::getRecordRangeForTime( QString fileName, double sampleRate, double minTime, double maxTime,
                                        int64_t& min, int64_t& max ) {

  if ( sampleRate == 0.0 ) return ERRINFO(ESampleRate,"");

  int64_t maxElements;

  int st = this->getMaxElements( fileName, 0, maxElements );
  if ( st ) {
    dspErrMsg( st );
    return ERRINFO(st,"");
  }

  int64_t maxBytes = maxElements * sizeof(int);

  min = sampleRate * minTime * sizeof(int);
  if ( min % 4 ) min -= min % 4;
  max = sampleRate * maxTime * sizeof(int);
  if ( max % 4 ) max = max - ( max % 4 ) + 4;
  if ( max > maxBytes ) max = maxBytes;

  return 0;

}

int BinDataBase::getMaxElements2 ( QString filename, int sigIndex, int64_t& max ) {

  return 0;

}
    
int BinDataBase::genLineSeries2 ( QString filename,
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
                            fftw_complex *fftArray ) {
    
  return 0;

}

int64_t BinDataBase::readTraceData2 (
 std::filebuf& fb,
 int *buf,
 int64_t readSizeInbytes ) {

  return 0;

}

void BinDataBase::inputSeekToStartOfData( std::filebuf &fb, int64_t firstDataByte ) {

  //int64_t headerSize = sizeof(numSigbytes) + sizeof(version);
  int64_t loc = sizeof(int64_t) + sizeof(unsigned int) * 3 + firstDataByte;
  fb.pubseekoff( loc, std::ios::beg, std::ios::in );

}

void BinDataBase::outputSeekToStartOfData( std::filebuf &fb, int64_t firstDataByte ) {

  //int64_t headerSize = sizeof(numSigbytes) + sizeof(version);
  int64_t loc = sizeof(int64_t) + sizeof(unsigned int) * 3 + firstDataByte;
  fb.pubseekoff( loc, std::ios::beg, std::ios::out );

}

int BinDataBase::getMaxElements ( QString filename, int sigIndex, int64_t& max ) {

  return 0;

}
    
int BinDataBase::genLineSeries ( QString filename,
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
                            fftw_complex *fftArray ) {
    
  return 0;

}

int64_t BinDataBase::readTraceData (
 std::filebuf& fb,
 int *buf,
 int64_t readSizeInbytes ) {

  return 0;

}

void BinDataBase::updateLineSeries(
  int64_t readOpCount,
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
  LineSeriesBuilderBase *ls,
  double& miny,
  double& maxy ) {

}

// generate line series using entire buffer
int BinDataBase::genFftFillUnderLineSeriesFromBuffer (
 int num,
 fftw_complex *buf,
 double sampleRate,
 int plotAreaWidthPixels,
 QtCharts::QLineSeries& qls,
 double& minx,
 double& maxx,
 double& miny,
 double& maxy ) {

  if ( num <= 5 ) {
    return ERRINFO(ESampleSize,"");
  }

  //std::cout << "sampleRate = " << sampleRate << std::endl;

  if ( num == 0 ) num = 1;
  double frac = 2.0 / (double) num;

  if ( num < ( plotAreaWidthPixels * 5 ) ) { // build simple line series

    this->sfulsb->setXPixelWidth( plotAreaWidthPixels );
    this->sfulsb->setXAxisLimits( 0.0, sampleRate );
    this->sfulsb->setLineSeries( &qls );
    this->sfulsb->startNewSeries();

    double minYVal = sqrt( buf[0][0]*buf[0][0] + buf[0][1]*buf[0][1] ) * frac;
    double maxYVal = minYVal;
    double minXVal = 0.0;
    double maxXVal = sampleRate;
    double freq, y;
    for ( int i=0; i<num; i++ ) {
      freq = (double) i / (double) num * sampleRate;
      y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;
      minYVal = std::fmin( y, minYVal );
      maxYVal = std::fmax( y, maxYVal );
      this->sfulsb->addPoint( freq, y );
    }
    this->sfulsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }
  else {

    this->fulsb->setXPixelWidth( plotAreaWidthPixels );
    this->fulsb->setXAxisLimits( 0.0, sampleRate );
    this->fulsb->setLineSeries( &qls );
    this->fulsb->startNewSeries();

    double minYVal = sqrt( buf[0][0]*buf[0][0] + buf[0][1]*buf[0][1] ) * frac;
    double maxYVal = minYVal;
    double minXVal = 0.0;
    double maxXVal = sampleRate;
    double freq, y;
    for ( int i=0; i<num; i++ ) {
      freq = (double) i / (double) num * sampleRate;
      y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;
      minYVal = std::fmin( y, minYVal );
      maxYVal = std::fmax( y, maxYVal );
      this->fulsb->addPoint( freq, y );
    }
    this->fulsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }
    
  return 0;

}

// generate line series from freq min to freq max
int BinDataBase::genFftFillUnderLineSeriesFromBufferByFreq (
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
 double& maxy ) {

  bool firstSample = true;
  double minYVal, maxYVal, minXVal, maxXVal, freq, y;
      
  if ( num <= 5 ) {
    return ERRINFO(ESampleSize,"");
  }

  if ( num == 0 ) num = 1;
  double frac = 2.0 / (double) num;

  if ( num < ( plotAreaWidthPixels * 5 ) ) { // build simple line series

    this->sfulsb->setXPixelWidth( plotAreaWidthPixels );
    this->sfulsb->setXAxisLimits( freqMin, freqMax );
    this->sfulsb->setLineSeries( &qls );
    this->sfulsb->startNewSeries();

    for ( int i=0; i<num; i++ ) {
      
      freq = (double) i / (double) num * sampleRate;

      if ( ( freq >= freqMin ) && ( freq <= freqMax ) ) {

        y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;

        if ( firstSample ) {
          firstSample = false;
          minYVal = maxYVal = y;
          minXVal = maxXVal = freq;
        }

                minYVal = std::fmin( y, minYVal );
        maxYVal = std::fmax( y, maxYVal );
        minXVal = std::fmin( freq, minXVal );
        maxXVal = std::fmax( freq, maxXVal );
        this->sfulsb->addPoint( freq, y );
        
      }

    }
      
    this->sfulsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }
  else {

    this->fulsb->setXPixelWidth( plotAreaWidthPixels );
    this->fulsb->setXAxisLimits(  freqMin, freqMax );
    this->fulsb->setLineSeries( &qls );
    this->fulsb->startNewSeries();

    for ( int i=0; i<num; i++ ) {
      
      freq = (double) i / (double) num * sampleRate;

      if ( ( freq >= freqMin ) && ( freq <= freqMax ) ) {
      
        y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;
      
        if ( firstSample ) {
          firstSample = false;
          minYVal = maxYVal = y;
          minXVal = maxXVal = freq;
        }
        
        minYVal = std::fmin( y, minYVal );
        maxYVal = std::fmax( y, maxYVal );
        minXVal = std::fmin( freq, minXVal );
        maxXVal = std::fmax( freq, maxXVal );
        this->fulsb->addPoint( freq, y );

      }
        
    }
      
    this->fulsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }
    
  return 0;

}

int BinDataBase::genFftLineSeriesFromBuffer (
 int num,
 fftw_complex *buf,
 double sampleRate,
 int plotAreaWidthPixels,
 QtCharts::QLineSeries& qls,
 double& minx,
 double& maxx,
 double& miny,
 double& maxy,
 bool suppressZeros ) {

  if ( num <= 5 ) {
    return ERRINFO(ESampleSize,"");
  }

  //std::cout << "BinData::genFftLineSeriesFromBuffer" << std::endl;
  //std::cout << "sampleRate = " << sampleRate << std::endl;

  if ( num == 0 ) num = 1;
  double frac = 2.0 / (double) num;

  double nyquist = sampleRate / 2.0;

  if ( num < ( plotAreaWidthPixels * 5 ) ) { // build simple line series

    this->slsb->setXPixelWidth( plotAreaWidthPixels );
    this->slsb->setXAxisLimits( 0.0, sampleRate/2 );
    this->slsb->setLineSeries( &qls );
    this->slsb->startNewSeries();

    double minYVal = sqrt( buf[0][0]*buf[0][0] + buf[0][1]*buf[0][1] ) * frac;
    double maxYVal = minYVal;
    double minXVal = 0.0;
    double maxXVal = sampleRate / 2.0;
    double freq, y;

    for ( int i=0; i<num; i++ ) {

      freq = (double) i / (double) num * sampleRate;

      if ( freq <= nyquist ) {
      
        y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;

        if ( ( y >= Cnst::MinLogArg ) || !suppressZeros ) {
          
          minYVal = std::fmin( y, minYVal );
          maxYVal = std::fmax( y, maxYVal );
          this->slsb->addPoint( freq, y );
          
        }
        
      }

    }
    this->slsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }
  else {

    this->lsb->setXPixelWidth( plotAreaWidthPixels );
    this->lsb->setXAxisLimits( 0.0, sampleRate/2 );
    this->lsb->setLineSeries( &qls );
    this->lsb->startNewSeries();

    double minYVal = sqrt( buf[0][0]*buf[0][0] + buf[0][1]*buf[0][1] ) * frac;
    double maxYVal = minYVal;
    double minXVal = 0.0;
    double maxXVal = sampleRate / 2.0;
    double freq, y;
    
    for ( int i=0; i<num; i++ ) {

      //std::cout << "num = " << num << "i = " << i << std::endl;
      
      freq = (double) i / (double) num * sampleRate;

      if ( freq <= nyquist ) {
      
        y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;

        if ( ( y >= Cnst::MinLogArg ) || !suppressZeros ) {

          minYVal = std::fmin( y, minYVal );
          maxYVal = std::fmax( y, maxYVal );
          this->lsb->addPoint( freq, y );
          
        }
        
      }

    }
    this->lsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }

  return 0;

}

int BinDataBase::genFftLineSeriesFromBufferByFreq (
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
 bool suppressZeros ) {

  bool firstSample = true;
  double minYVal, maxYVal, minXVal, maxXVal, freq, y;
      
  if ( num <= 5 ) {
    return ERRINFO(ESampleSize,"");
  }

  if ( num == 0 ) num = 1;
  double frac = 2.0 / (double) num;

  double nyquist = sampleRate / 2.0;

  freqMax = std::fmin( freqMax, nyquist );

  if ( num < ( plotAreaWidthPixels * 5 ) ) { // build simple line series

    this->slsb->setXPixelWidth( plotAreaWidthPixels );
    this->slsb->setXAxisLimits( freqMin, freqMax );
    this->slsb->setLineSeries( &qls );
    this->slsb->startNewSeries();

    double freqRange = freqMax - freqMin;

    for ( int i=0; i<num; i++ ) {
      
      freq = (double) i / (double) num * freqRange + freqMin;

      if ( ( freq >= freqMin ) && ( freq <= freqMax ) ) {

        y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;

        if ( ( y >= Cnst::MinLogArg ) || !suppressZeros ) {

          if ( firstSample ) {
            firstSample = false;
            minYVal = maxYVal = y;
            minXVal = maxXVal = freq;
          }

          minYVal = std::fmin( y, minYVal );
          maxYVal = std::fmax( y, maxYVal );
          this->slsb->addPoint( freq, y );
          
        }
        
      }

    }
      
    this->slsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }
  else {

    this->lsb->setXPixelWidth( plotAreaWidthPixels );
    this->lsb->setXAxisLimits(  freqMin, freqMax );
    this->lsb->setLineSeries( &qls );
    this->lsb->startNewSeries();

    for ( int i=0; i<num; i++ ) {
      
      freq = (double) i / (double) num * sampleRate;

      if ( ( freq >= freqMin ) && ( freq <= freqMax ) ) {
      
        y = sqrt( buf[i][0]*buf[i][0] + buf[i][1]*buf[i][1] ) * frac;

        if ( ( y >= Cnst::MinLogArg ) || !suppressZeros ) {
          
          if ( firstSample ) {
            firstSample = false;
            minYVal = maxYVal = y;
            minXVal = maxXVal = freq;
          }
        
          minYVal = std::fmin( y, minYVal );
          maxYVal = std::fmax( y, maxYVal );
          minXVal = std::fmin( freq, minXVal );
          maxXVal = std::fmax( freq, maxXVal );
          this->lsb->addPoint( freq, y );

        }

      }
        
    }
      
    this->lsb->processLastPoint();

    minx = minXVal;
    maxx = maxXVal;
    miny = minYVal;
    maxy = maxYVal;

  }
    
  return 0;

}

