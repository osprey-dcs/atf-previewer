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

static int qlsInit = 1;

BinDataBase::BinDataBase() {

  xlsb = std::shared_ptr<LineSeriesBuilderSimple>( new LineSeriesBuilderSimple );
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

void BinDataBase::initMaxBufSize( unsigned long max ) {
}

int BinDataBase::getRecordRangeForTime( QString fileName, double sampleRate, double minTime, double maxTime,
                                        unsigned long& min, unsigned long& max ) {

  unsigned long maxElements;

  if ( sampleRate == 0.0 ) return -1;

  int st = this->getMaxElements( fileName, 0, maxElements );
  if ( st ) return st;

  unsigned long maxBytes = maxElements * sizeof(int) - 1;

  min = sampleRate * minTime * sizeof(int);
  if ( min % 4 ) min -= min % 4;
  max = sampleRate * maxTime * sizeof(int);
  if ( max % 4 ) max = max - ( max % 4 ) + 4;
  if ( max > maxBytes ) max = maxBytes;

  return 0;

}

int BinDataBase::getMaxElements2 ( QString filename, int sigIndex, unsigned long& max ) {

  std::filebuf fb;
  const unsigned int version[] { 1, 0, 0 };
  
  auto result = fb.open( filename.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return -1;
  }

  // read version
  fb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
  fb.sgetn( (char *) version, sizeof(version) );

  // get number of signals
  unsigned int numSigs;
  fb.pubseekoff( (unsigned long) sizeof( version ), std::ios::beg, std::ios::in );
  fb.sgetn( (char *) (&numSigs), sizeof(numSigs) );

  // get num of elements for given signal index
  unsigned int headerSize = sizeof(numSigs) + sizeof(version);
  unsigned long value;
  unsigned long offset = headerSize + numSigs * sizeof(value);
  for ( unsigned int i=0; i<numSigs; i++ ) {
    fb.sgetn((char *) &value, sizeof(value));
    if ( i == sigIndex ) {
      fb.close();
      max = value / sizeof(int);
      return 0;
    }
    if ( i < sigIndex ) {
      offset += value;
    }
  }

  fb.close();
  max = 0;
  return -1;

}
    
int BinDataBase::genLineSeries2 ( QString filename,
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
                            fftw_complex *fftArray ) {
    
  //std::cout << "BinDataBase::genLineSeries" << std::endl;
  //std::cout << "maxFft = " <<  maxFft << std::endl;
  //std::cout << "sigIndex = " <<  sigIndex << std::endl;
  //std::cout << "plotAreaWidthPixels = " <<  plotAreaWidthPixels << std::endl;
  //std::cout << "x0 = " << startTimeInSec << ", x1 = " << endTimeInSec << std::endl;
  //std::cout << "dataTimeIncrementInSec = " << dataTimeIncrementInSec << std::endl;
  //std::cout << "qls address = " <<  &qls << std::endl;
  //std::cout << std::endl;

  std::filebuf fb;
  const unsigned int version[] { 1, 0, 0 };

  auto result = fb.open( filename.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return -1;
  }

  // read version
  fb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
  fb.sgetn( (char *) version, sizeof(version) );

  // get number of signals
  unsigned int numSigs;
  fb.pubseekoff( (unsigned long) sizeof( version ), std::ios::beg, std::ios::in );
  fb.sgetn( (char *) (&numSigs), sizeof(numSigs) );

  unsigned int headerSize = sizeof(numSigs) + sizeof(version);
  unsigned long numSigbytes;
  unsigned long offset = headerSize + numSigs * sizeof(numSigbytes);
  //std::cout << "offset 1: " << offset << std::endl;
  unsigned long value;
  for ( unsigned int i=0; i<numSigs; i++ ) {
    fb.sgetn((char *) &value, sizeof(value));
    //std::cout << "num data bytes for trace " << i << " = " << value << std::endl;
    if ( i == sigIndex ) {
      numSigbytes = value;
    }
    if ( i < sigIndex ) {
      offset += value;
      //std::cout << "offset: " << offset << std::endl;
    }
  }

  unsigned long startingpoint = round( startTimeInSec / dataTimeIncrementInSec );
  //std::cout << "startingpoint 1: " << startingpoint << std::endl;
  if ( startingpoint > numSigbytes/sizeof(int) ) {
    fb.close();
    return -1;
  }

  unsigned long endingpoint = round( endTimeInSec / dataTimeIncrementInSec );
  //std::cout << "endingpoint: 1 " << endingpoint << std::endl;
  if ( endingpoint > numSigbytes/sizeof(int) ) {
    endingpoint = numSigbytes/sizeof(int);
  }

  unsigned long totalpoints = endingpoint - startingpoint + 1;
  if ( totalpoints > numSigbytes/sizeof(int) ) totalpoints = numSigbytes/sizeof(int);

  unsigned long startingOffset = startingpoint * sizeof(int);
  //std::cout << "startingOffset: " <<  startingOffset<< std::endl;
  //std::cout << "offset 2: " << offset << std::endl;

  offset += startingOffset;
  //std::cout << "offset 3: " << offset << std::endl;

  //std::cout << "totalpoints: " << totalpoints << std::endl;
  //std::cout << "plotAreaWidthPixels * 5: " << plotAreaWidthPixels * 5 << std::endl;

  int i, ifft = 0;

  //std::cout << "x0 = " << startTimeInSec << ", x1 = " << endTimeInSec << std::endl;
  //std::cout << "dataTimeIncrementInSec = " << dataTimeIncrementInSec << std::endl;

  if ( totalpoints < ( plotAreaWidthPixels * 5 ) ) { // build simple line series

    this->slsb->setXPixelWidth( plotAreaWidthPixels );
    this->slsb->setXAxisLimits( startTimeInSec, endTimeInSec );
    this->slsb->setLineSeries( &qls );
    this->slsb->startNewSeries();

    // read data in maximum chunks of 4000 bytes (1000 ints)
    int numBytesRead, buf[1000];
    unsigned long numReadOps = (totalpoints * sizeof(int)) / 4000;
    unsigned long finalReadSize = (totalpoints * sizeof(int)) % 4000;
    //std::cout << "finalReadSize 1: " << finalReadSize << std::endl;
    unsigned long iread;
    QPointF pts[4];
    double timeStep = startTimeInSec;
    numPts = 0;
    for (iread = 0; iread < numReadOps; iread++) {
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, 4000);
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, slsb.get(), miny, maxy);
      for ( i=0; ( i<1000 && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += 4000;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "numBytesRead: " << numBytesRead << std::endl;
    }
    if (finalReadSize) {
      //std::cout << "finalReadSize 2: " << finalReadSize << std::endl;
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, finalReadSize);
      int n = numBytesRead / sizeof( int );
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, slsb.get(), miny, maxy);
      for ( i=0; ( i<n && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += finalReadSize;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "final numBytesRead: " << numBytesRead << std::endl;
      //std::cout << "final numWordsRead: " << numBytesRead/sizeof(int) << std::endl;
    }
    //std::cout << "numFft: " << numFft << std::endl;

  }
  else { // build line series with first, min, max, and last

    this->lsb->setXPixelWidth( plotAreaWidthPixels );
    this->lsb->setXAxisLimits( startTimeInSec, endTimeInSec );
    this->lsb->setLineSeries( &qls );
    this->lsb->startNewSeries();
      
    // read data in maximum chunks of 4000 bytes (1000 ints)
    int numBytesRead, buf[1000];
    unsigned long numReadOps = (totalpoints * sizeof(int)) / 4000;
    unsigned long finalReadSize = (totalpoints * sizeof(int)) % 4000;
    //std::cout << "finalReadSize 1: " << finalReadSize << std::endl;
    unsigned long iread;
    QPointF pts[4];
    double timeStep = startTimeInSec;
    numPts += 0;
    for (iread = 0; iread < numReadOps; iread++) {
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, 4000);
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, lsb.get(), miny, maxy);
      for ( i=0; ( i<1000 && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += 4000;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "numBytesRead: " << numBytesRead << std::endl;
    }
    if (finalReadSize) {
      //std::cout << "finalReadSize 2: " << finalReadSize << std::endl;
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, finalReadSize);
      int n = numBytesRead / sizeof( int );
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, lsb.get(), miny, maxy);
      for ( i=0; ( i<n && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += finalReadSize;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "final numBytesRead: " << numBytesRead << std::endl;
      //std::cout << "final numWordsRead: " << numBytesRead/sizeof(int) << std::endl;
    }
    //std::cout << "numFft: " << numFft << std::endl;

  }

  if ( Cnst::UseHanning ) {
    for ( i=0; i<numFft; i++ ) {
      fftArray[ifft][0] *= 0.5 * ( 1.0 - cos( 2 * M_PI * i / ( maxFft ) ) );
    }
  }

  fb.close();
  return 0;

}

int BinDataBase::readTraceData2 (
 std::filebuf& fb,
 int *buf,
 int readSizeInbytes ) {

  //std::cout << "read " << readSizeInbytes << " bytes" << std::endl;

  int n = fb.sgetn( reinterpret_cast<char *>( buf ), readSizeInbytes );
  return n;

}

void BinDataBase::inputSeekToStartOfData( std::filebuf &fb, unsigned long firstDataByte ) {

  //unsigned long headerSize = sizeof(numSigbytes) + sizeof(version);
  unsigned long loc = sizeof(unsigned long) + sizeof(unsigned int) * 3 + firstDataByte;
  fb.pubseekoff( loc, std::ios::beg, std::ios::in );

}

void BinDataBase::outputSeekToStartOfData( std::filebuf &fb, unsigned long firstDataByte ) {

  //unsigned long headerSize = sizeof(numSigbytes) + sizeof(version);
  unsigned long loc = sizeof(unsigned long) + sizeof(unsigned int) * 3 + firstDataByte;
  fb.pubseekoff( loc, std::ios::beg, std::ios::out );

}

int BinDataBase::getMaxElements ( QString filename, int sigIndex, unsigned long& max ) {

  std::filebuf fb;
  const unsigned int version[] { 1, 0, 0 };
  
  auto result = fb.open( filename.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return -1;
  }

  // read version
  fb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
  fb.sgetn( (char *) version, sizeof(version) );

  // get num of elements
  unsigned long value;
  fb.sgetn((char *) &value, sizeof(value));
 
  fb.close();

  max = value / sizeof(int);
  
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
                            unsigned long& numPts,
                            QtCharts::QLineSeries& qls,
                            double& miny,
                            double& maxy,
                            unsigned long maxFft,
                            unsigned long& numFft,
                            fftw_complex *fftArray ) {
    
  //std::cout << "BinDataBase::genLineSeries" << std::endl;
  //std::cout << "maxFft = " <<  maxFft << std::endl;
  //std::cout << "sigIndex = " <<  sigIndex << std::endl;
  //std::cout << "plotAreaWidthPixels = " <<  plotAreaWidthPixels << std::endl;
  //std::cout << "x0 = " << startTimeInSec << ", x1 = " << endTimeInSec << std::endl;
  //std::cout << "dataTimeIncrementInSec = " << dataTimeIncrementInSec << std::endl;
  //std::cout << "qls address = " <<  &qls << std::endl;
  //std::cout << std::endl;

  std::filebuf fb;
  const unsigned int version[] { 1, 0, 0 };

  auto result = fb.open( filename.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return -1;
  }

  // read version
  fb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
  fb.sgetn( (char *) version, sizeof(version) );

  // read numSigbytes
  unsigned long numSigbytes;
  fb.pubseekoff( (unsigned long) sizeof( version ), std::ios::beg, std::ios::in );
  fb.sgetn((char *) &numSigbytes, sizeof(numSigbytes));

  unsigned int headerSize = sizeof(numSigbytes) + sizeof(version);
  unsigned long offset = headerSize;

  unsigned long startingpoint = round( startTimeInSec / dataTimeIncrementInSec );
  if ( startingpoint > numSigbytes/sizeof(int) ) {
    fb.close();
    return -1;
  }

  unsigned long endingpoint = round( endTimeInSec / dataTimeIncrementInSec );
  if ( endingpoint > numSigbytes/sizeof(int) ) {
    endingpoint = numSigbytes/sizeof(int);
  }

  unsigned long totalpoints = endingpoint - startingpoint + 1;
  if ( totalpoints > numSigbytes/sizeof(int) ) totalpoints = numSigbytes/sizeof(int);

  unsigned long startingOffset = startingpoint * sizeof(int);
  //std::cout << "startingOffset: " <<  startingOffset<< std::endl;
  //std::cout << "offset 2: " << offset << std::endl;

  offset += startingOffset;
  //std::cout << "offset 3: " << offset << std::endl;

  //std::cout << "totalpoints: " << totalpoints << std::endl;
  //std::cout << "plotAreaWidthPixels * 5: " << plotAreaWidthPixels * 5 << std::endl;

  int i, ifft = 0;

  //std::cout << "x0 = " << startTimeInSec << ", x1 = " << endTimeInSec << std::endl;
  //std::cout << "dataTimeIncrementInSec = " << dataTimeIncrementInSec << std::endl;

  if ( totalpoints < ( plotAreaWidthPixels * 5 ) ) { // build simple line series

    this->slsb->setXPixelWidth( plotAreaWidthPixels );
    this->slsb->setXAxisLimits( startTimeInSec, endTimeInSec );
    this->slsb->setLineSeries( &qls );
    this->slsb->startNewSeries();

    // read data in maximum chunks of 4000 bytes (1000 ints)
    int numBytesRead, buf[1000];
    unsigned long numReadOps = (totalpoints * sizeof(int)) / 4000;
    unsigned long finalReadSize = (totalpoints * sizeof(int)) % 4000;
    //std::cout << "finalReadSize 1: " << finalReadSize << std::endl;
    unsigned long iread;
    QPointF pts[4];
    double timeStep = startTimeInSec;
    numPts = 0;
    for (iread = 0; iread < numReadOps; iread++) {
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, 4000);
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, slsb.get(), miny, maxy);
      for ( i=0; ( i<1000 && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += 4000;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "numBytesRead: " << numBytesRead << std::endl;
    }
    if (finalReadSize) {
      //std::cout << "finalReadSize 2: " << finalReadSize << std::endl;
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, finalReadSize);
      int n = numBytesRead / sizeof( int );
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, slsb.get(), miny, maxy);
      for ( i=0; ( i<n && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += finalReadSize;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "final numBytesRead: " << numBytesRead << std::endl;
      //std::cout << "final numWordsRead: " << numBytesRead/sizeof(int) << std::endl;
    }
    //std::cout << "numFft: " << numFft << std::endl;

  }
  else { // build line series with first, min, max, and last

    this->lsb->setXPixelWidth( plotAreaWidthPixels );
    this->lsb->setXAxisLimits( startTimeInSec, endTimeInSec );
    this->lsb->setLineSeries( &qls );
    this->lsb->startNewSeries();
      
    // read data in maximum chunks of 4000 bytes (1000 ints)
    int numBytesRead, buf[1000];
    unsigned long numReadOps = (totalpoints * sizeof(int)) / 4000;
    unsigned long finalReadSize = (totalpoints * sizeof(int)) % 4000;
    //std::cout << "finalReadSize 1: " << finalReadSize << std::endl;
    unsigned long iread;
    QPointF pts[4];
    double timeStep = startTimeInSec;
    numPts += 0;
    for (iread = 0; iread < numReadOps; iread++) {
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, 4000);
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, lsb.get(), miny, maxy);
      for ( i=0; ( i<1000 && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += 4000;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "numBytesRead: " << numBytesRead << std::endl;
    }
    if (finalReadSize) {
      //std::cout << "finalReadSize 2: " << finalReadSize << std::endl;
      fb.pubseekoff(offset, std::ios::beg, std::ios::in);
      numBytesRead = readTraceData(fb, buf, finalReadSize);
      int n = numBytesRead / sizeof( int );
      updateLineSeries(iread, pts, slope, intercept, timeStep, plotAreaWidthPixels, startTimeInSec, endTimeInSec,
                       dataTimeIncrementInSec, numBytesRead, buf, lsb.get(), miny, maxy);
      for ( i=0; ( i<n && ifft<(maxFft-1) ); i++, ifft++ ) {
        fftArray[ifft][0] = slope * (double) buf[i] + intercept;
        fftArray[ifft][1] = 0.0;
      }
      numFft = ifft;
      offset += finalReadSize;
      numPts += numBytesRead / sizeof(int);
      //std::cout << "final numBytesRead: " << numBytesRead << std::endl;
      //std::cout << "final numWordsRead: " << numBytesRead/sizeof(int) << std::endl;
    }
    //std::cout << "numFft: " << numFft << std::endl;

  }

  if ( Cnst::UseHanning ) {
    for ( i=0; i<numFft; i++ ) {
      fftArray[ifft][0] *= 0.5 * ( 1.0 - cos( 2 * M_PI * i / ( maxFft ) ) );
    }
  }

  fb.close();
  return 0;

}

int BinDataBase::readTraceData (
 std::filebuf& fb,
 int *buf,
 int readSizeInbytes ) {

  //std::cout << "read " << readSizeInbytes << " bytes" << std::endl;

  int n = fb.sgetn( reinterpret_cast<char *>( buf ), readSizeInbytes );
  return n;

}

static const int firstp = 0;
static const int lastp = 1;
static const int minp = 2;
static const int maxp = 3;

void BinDataBase::updateLineSeries(
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
  LineSeriesBuilderBase *ls,
  double& miny,
  double& maxy ) {

    double binSize;

    if (numBytesToProcess < 1) {
      return;
    }

    double deltaTimeInSec = endTimeInSec - startTimeInSec;
    if (deltaTimeInSec != 0.0) {
      binSize = deltaTimeInSec / plotAreaWidthPixels;
    }

    //std::cout << "deltaTimeInSec = " << deltaTimeInSec << std::endl;
    //std::cout << "plotAreaWidthPixels = " << plotAreaWidthPixels << std::endl;
    //std::cout << "binSize = " << binSize << std::endl;

    int num = numBytesToProcess / sizeof(int);
    double time = startTimeInSec + readOpCount * 1000 * dataTimeIncrementInSec;
    //std::cout << "startTimeInSec = " << startTimeInSec << std::endl;
    //std::cout << "readOpCount = " << readOpCount << std::endl;
    //std::cout << "dataTimeIncrementInSec = " << dataTimeIncrementInSec << std::endl;
    //std::cout << "time = " << time << std::endl;

  //std::cout << "numBytesToProcess = " << numBytesToProcess << std::endl;
  //std::cout << "num = " << num << std::endl;

  if ( readOpCount == 0 ) {
    miny = maxy = slope * buf[0] + intercept;
  }

  for ( int i=0; i<num; i++ ) {

    //std::cout << "i: " << i << "   point is " << time << ", " << buf[i] << std::endl;

    double v = slope * (double) buf[i] + intercept;

    ls->addPoint( time, v );

    time += dataTimeIncrementInSec;

    maxy = std::fmax( maxy, v );
    miny = std::fmin( miny, v );

  }

  //std::cout << "ls->show()" << std::endl;
  //ls->show();
  //std::cout << "end ls->show()" << std::endl;

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
    return -1;
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

  //std::cout << "BinDataBase::genFftFillUnderLineSeriesFromBufferByFreq" << std::endl;
  //std::cout << "freqMin = " << freqMin << ", freqMax = " << freqMax << std::endl;

  bool firstSample = true;
  double minYVal, maxYVal, minXVal, maxXVal, freq, y;
      
  if ( num <= 5 ) {
    return -1;
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

  return 0;

}

