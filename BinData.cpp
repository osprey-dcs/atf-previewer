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

#include "BinData.h"

BinData::BinData() {

}

BinData::~BinData() {

}

int BinData::getMaxElements2 ( QString filename, int sigIndex, unsigned long& max ) {

  std::filebuf fb;
  const unsigned int version[] { 1, 0, 0 };
  
  auto result = fb.open( filename.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return ERRINFO(EFileOpen,filename.toStdString());
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
  return ERRINFO(EMax,filename.toStdString());

}
    
int BinData::genLineSeries2 ( QString filename,
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
    
  //std::cout << "BinData::genLineSeries" << std::endl;
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
    return ERRINFO(EFileOpen,filename.toStdString());
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
    return ERRINFO(ERange,"");
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

int BinData::readTraceData2 (
 std::filebuf& fb,
 int *buf,
 int readSizeInbytes ) {

  //std::cout << "read " << readSizeInbytes << " bytes" << std::endl;

  int n = fb.sgetn( reinterpret_cast<char *>( buf ), readSizeInbytes );
  return n;

}

int BinData::getMaxElements ( QString filename, int sigIndex, unsigned long& max ) {

  std::filebuf fb;
  const unsigned int version[] { 1, 0, 0 };
  
  auto result = fb.open( filename.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return ERRINFO(EMax,"");
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
    
int BinData::genLineSeries ( QString filename,
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
    
  //std::cout << "BinData::genLineSeries" << std::endl;
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
    qWarning() << __func__ << "Unable to open:" << filename;
    return ERRINFO(EFileOpen,filename.toStdString());
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
    return ERRINFO(ERange,"");
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

int BinData::readTraceData (
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

void BinData::updateLineSeries(
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
