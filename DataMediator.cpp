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

#include "DataMediator.h"

DataMediator::DataMediator() : ErrHndlr( NumErrs, errMsgs ) {
}

DataMediator::DataMediator( const std::string& name ) : ErrHndlr( NumErrs, errMsgs ) {

  binDataName = name;
  bd = this->bdf.createBinData( Cnst::BinDataName );
  bd->initMaxBufSize( Cnst::MaxMFileBufSize );
  slsb = std::shared_ptr<LineSeriesBuilderSimple>( new LineSeriesBuilderSimple() );
  lsb = std::shared_ptr<LineSeriesBuilderMinMax>( new LineSeriesBuilderMinMax() );
  sfulsb = std::shared_ptr<LineSeriesBuilderSimpleFillUnder>( new LineSeriesBuilderSimpleFillUnder() );
  fulsb = std::shared_ptr<LineSeriesBuilderMinMaxFillUnder>( new LineSeriesBuilderMinMaxFillUnder() );

}

DataMediator::~DataMediator () {}

int DataMediator::newFile( std::string filename ) {
  return bd->newFile( filename );
}

int DataMediator::getMaxElements( std::string filename, int sigIndex, int64_t &max ) {
  int st = bd->getMaxElements( filename, sigIndex, max );
  //std::cout << "DataMediator::getMaxElements max = " << max << std::endl;
  return st;
}

int64_t DataMediator::readTraceData( int *buf, int64_t readSizeInbytes ) {
  //std::cout << "DataMediator::readTraceData 1 readSizeInbytes = " << readSizeInbytes << std::endl;
  return bd->readTraceData( buf, readSizeInbytes );
}

int64_t DataMediator::readTraceData( std::filebuf& fb, int *buf, int64_t readSizeInbytes ) {
  //std::cout << "DataMediator::readTraceData 2 readSizeInbytes = " << readSizeInbytes << std::endl;
  return bd->readTraceData( fb, buf, readSizeInbytes );
}

int DataMediator::genLineSeries( std::string filename, int sigIndex, double slope, double intercept, int plotAreaWidthPixels,
                                double startTimeInSec, double endTimeInSec, double dataTimeIncrementInSec,
                                int64_t &numPts, QtCharts::QLineSeries &qls, double &miny, double &maxy, int64_t maxFft,
                                int64_t &numFft, fftw_complex *fftArray ) {

  
  //std::cout << "DataMediator::genLineSeries" << std::endl;

  int st = bd->openRead( filename );
  if ( st ) {
    bd->dspErrMsg( st );
    return st;
  }

  int64_t major, minor, release;
  st = bd->readVersion( major, minor, release );
  if ( st ) {
    bd->dspErrMsg( st );
    return st;
  }

  int64_t numBytes;
  st = bd->readNumBytes( numBytes );
  if ( st ) {
    bd->dspErrMsg( st );
    return st;
  }

  int64_t headerSize = bd->getHeaderSize();
  int64_t offset = headerSize;
  
  int64_t startingpoint = round( startTimeInSec / dataTimeIncrementInSec );
  if ( startingpoint > numBytes/sizeof(int) ) {
    st = bd->closeFile();
    if ( st ) bd->dspErrMsg( st );
    return ERRINFO(ERange,"");
  }

  int64_t endingpoint = round( endTimeInSec / dataTimeIncrementInSec );
  if ( endingpoint > numBytes/sizeof(int) ) {
    endingpoint = numBytes/sizeof(int);
  }

  int64_t totalpoints = endingpoint - startingpoint + 1;
  if ( totalpoints > numBytes/sizeof(int) ) totalpoints = numBytes/sizeof(int);

  int64_t startingOffset = startingpoint * sizeof(int);

  offset += startingOffset;

  //std::cout << "offset: " << offset << std::endl;

  int i, ifft = 0;

  if ( totalpoints < ( plotAreaWidthPixels * 5 ) ) { // build simple line series

    this->slsb->setXPixelWidth( plotAreaWidthPixels );
    this->slsb->setXAxisLimits( startTimeInSec, endTimeInSec );
    this->slsb->setLineSeries( &qls );
    this->slsb->startNewSeries();

    // read data in maximum chunks of 4000 bytes (1000 ints)
    int64_t numBytesRead;
    int *buf = new int[1000];
    int64_t numReadOps = (totalpoints * sizeof(int)) / 4000;
    int64_t finalReadSize = (totalpoints * sizeof(int)) % 4000;
    int64_t iread;
    QPointF pts[4];
    double timeStep = startTimeInSec;
    numPts = 0;
    for (iread = 0; iread < numReadOps; iread++) {
      bd->seekToReadOffset( offset );
      numBytesRead = bd->readTraceData( buf, 4000 );
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
      bd->seekToReadOffset( offset );
      numBytesRead = bd->readTraceData( buf, finalReadSize );
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
    }

    delete[] buf;
    buf = nullptr;

  }
  else { // build line series with first, min, max, and last

    this->lsb->setXPixelWidth( plotAreaWidthPixels );
    this->lsb->setXAxisLimits( startTimeInSec, endTimeInSec );
    this->lsb->setLineSeries( &qls );
    this->lsb->startNewSeries();
      
    // read data in maximum chunks of 4000 bytes (1000 ints)
    int64_t numBytesRead;
    int *buf = new int[1000];
    int64_t numReadOps = (totalpoints * sizeof(int)) / 4000;
    int64_t finalReadSize = (totalpoints * sizeof(int)) % 4000;
    //std::cout << "finalReadSize 1: " << finalReadSize << std::endl;
    int64_t iread;
    QPointF pts[4];
    double timeStep = startTimeInSec;
    numPts += 0;
    for (iread = 0; iread < numReadOps; iread++) {
      bd->seekToReadOffset( offset );
      numBytesRead = bd->readTraceData( buf, 4000 );
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
      bd->seekToReadOffset( offset );
      numBytesRead = bd->readTraceData( buf, finalReadSize );
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
    }

    delete[] buf;
    buf = nullptr;

  }

  if ( Cnst::UseHanning ) {
    for ( i=0; i<numFft; i++ ) {
      fftArray[ifft][0] *= 0.5 * ( 1.0 - cos( 2 * M_PI * i / ( maxFft ) ) );
    }
  }

  bd->closeFile();
  
  return 0;

}

void DataMediator::updateLineSeries(int64_t readOpCount, QPointF *pts, double slope, double intercept, double &timeStep,
                                    int plotAreaWidthPixels, double startTimeInSec, double endTimeInSec,
                                    double dataTimeIncrementInSec, int64_t numBytesToProcess, int *buf,
                                    LineSeriesBuilderBase *ls, double &miny, double &maxy) {

  //std::cout << "DataMediator::updateLineSeries" << std::endl;
  
  double binSize;

  if (numBytesToProcess < 1) {
    return;
  }

  double deltaTimeInSec = endTimeInSec - startTimeInSec;
  if (deltaTimeInSec != 0.0) {
    binSize = deltaTimeInSec / plotAreaWidthPixels;
  }

  int num = numBytesToProcess / sizeof(int);
  double time = startTimeInSec + readOpCount * 1000 * dataTimeIncrementInSec;

  if ( readOpCount == 0 ) {
    miny = maxy = slope * buf[0] + intercept;
  }

  for ( int i=0; i<num; i++ ) {

    double v = slope * (double) buf[i] + intercept;

    ls->addPoint( time, v );

    time += dataTimeIncrementInSec;

    maxy = std::fmax( maxy, v );
    miny = std::fmin( miny, v );

  }

}

// from BinaryDataBase class

void DataMediator::initMaxBufSize( int64_t max ) {
  bd->initMaxBufSize( max );
}

int DataMediator::getDataFullTimeRange( std::string filename, double sampleRate, double& minTime, double& maxTime ) {
  return bd->getDataFullTimeRange( filename, sampleRate, minTime, maxTime );
}

int DataMediator::getRecordRangeForTime( std::string filename, double sampleRate, double minTime, double maxTime,
                                         int64_t& min, int64_t& max ) {

  return bd->getRecordRangeForTime( filename, sampleRate, minTime, maxTime,
                                    min, max );

}

void DataMediator::inputSeekToStartOfData( std::filebuf& fb, int64_t firstDataByte ) {

  bd->inputSeekToStartOfData( fb, firstDataByte );

}

void DataMediator::outputSeekToStartOfData( std::filebuf& fb, int64_t firstDataByte ) {

  bd->outputSeekToStartOfData( fb, firstDataByte );

}

int DataMediator::genFftFillUnderLineSeriesFromBuffer (
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

int DataMediator::genFftFillUnderLineSeriesFromBufferByFreq (
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

int DataMediator::genFftLineSeriesFromBuffer (
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

int DataMediator::genFftLineSeriesFromBufferByFreq (
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
