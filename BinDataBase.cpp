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

#include <string.h> // for memset

#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include <QString>

#include "BinDataBase.h"

BinDataBase::BinDataBase() : ErrHndlr( NumErrs, errMsgs ) {

  memset( (char *) &dataHdr, 0, sizeof(DataHdrType) );
  dataHdr.recSize = sizeof(int);

}

BinDataBase::~BinDataBase() {

}

int BinDataBase::openRead( const std::string& name ) {

  try {
    auto result = oneFb.open( name, std::ios::in | std::ios::binary );
    if ( !result ) {
      return ERRINFO(EFileOpen,name);
    }
  }
  catch ( const std::exception& e ) {
    QString qmsg = QString("file name is %s, %s").arg(name.c_str()).arg(e.what());
    return ERRINFO(EFileOpen,qmsg.toStdString());
  }

  newFile( name );

  isOpenRead = true;

  return 0;

}

int BinDataBase::closeFile ( void ) {
  
  if ( !isOpenRead ) {
    return ENoFile;
  }

  oneFb.close();

  return 0;

}

int BinDataBase::readHeader ( void ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  return readHeader( oneFb );

}

int BinDataBase::readHeader ( std::filebuf& fb ) {

  try {
    fb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
    fb.sgetn( (char *) &(dataHdr.version), sizeof(dataHdr.version) );
    fb.sgetn( (char *) &(dataHdr.numBytes), sizeof(dataHdr.numBytes) );
  }
  catch ( const std::exception& e ) {
    return ERRINFO(EFileRead,e.what());
  }

  hdrRead = true;

  return 0;

}

void BinDataBase::getVersion ( int64_t& major, int64_t& minor, int64_t& release ) {

  if ( !hdrRead ) {
    major = minor = release = 0;
    return;
  }

  major = dataHdr.version[0];
  minor = dataHdr.version[1];
  release = dataHdr.version[2];

}

int64_t BinDataBase::getRecSize ( void ) {

  if ( !hdrRead ) return 0;
  
  return dataHdr.recSize;

}

int64_t BinDataBase::getNumBytes ( void ) {

  if ( !hdrRead ) return 0;
  
  return dataHdr.numBytes;

}

std::string BinDataBase::getFileType ( void ) {

  if ( !hdrRead ) return "";

  std::string str = dataHdr.fileType;
  return str;

}

std::string BinDataBase::getCccr ( void ) {

  if ( !hdrRead ) return "";

  std::string str = dataHdr.cccr;
  return str;

}

int BinDataBase::readVersion ( int64_t& major, int64_t& minor, int64_t& release ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  try {
    oneFb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
    oneFb.sgetn( (char *) &(dataHdr.version), sizeof(dataHdr.version) );
  }
  catch ( const std::exception& e ) {
    return ERRINFO(EFileRead,e.what());
  }

  major = dataHdr.version[0];
  minor = dataHdr.version[1];
  release = dataHdr.version[2];

  return 0;

}

int BinDataBase::readNumBytes ( int64_t& num ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  try {
    oneFb.pubseekoff( (int64_t) sizeof(dataHdr.version), std::ios::beg, std::ios::in );
    oneFb.sgetn( (char *) &(dataHdr.numBytes), sizeof(dataHdr.numBytes) );
  }
  catch ( const std::exception& e ) {
    return ERRINFO(EFileRead,e.what());
  }

  num = dataHdr.numBytes;

  return 0;

}

int64_t BinDataBase::getHeaderSize( void ) {

  return (int64_t) ( sizeof(dataHdr.version) + sizeof(dataHdr.numBytes) );

}

int BinDataBase::newFile( std::string fileName ) { // BinDataMFile class uses this
  int64_t max;
  return getMaxElements( fileName, max ); // do this so header is read
}

void BinDataBase::initMaxBufSize( int64_t max ) {
}

int BinDataBase::getDataFullTimeRange( std::string fileName, double sampleRate, double& minTime, double& maxTime ) {

  if ( sampleRate <= 0.0 ) return ERRINFO(ESampleRate,"");

  int64_t maxElements;

  int st = this->getMaxElements( fileName, maxElements );
  if ( st ) {
    dspErrMsg( st );
    return ERRINFO(st,"");
  }

  minTime = 0.0;
  maxTime = maxElements / sampleRate;

  return 0;

}
  
int BinDataBase::getRecordRangeForTime( std::string fileName, double sampleRate, double minTime, double maxTime,
                                        int64_t& min, int64_t& max ) {

  if ( sampleRate == 0.0 ) return ERRINFO(ESampleRate,"");

  int64_t maxElements;

  int st = this->getMaxElements( fileName, maxElements );
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

int BinDataBase::getMaxElements ( std::string fileName, int64_t& max ) {

  std::filebuf fb;
  
  try {
    auto result = fb.open( fileName, std::ios::in | std::ios::binary );
    if ( !result ) {
      return ERRINFO(EMax,"");
    }
  }
  catch ( const std::exception& e ) {
    QString qmsg = QString("file name is %s, %s").arg(fileName.c_str()).arg(e.what());
    return ERRINFO(EMax,qmsg.toStdString());
  }

  int st = readHeader( fb );

  fb.close();

  max = dataHdr.numBytes / sizeof(int);

  return st;

}
    
int BinDataBase::getMaxElements ( std::filebuf& fb, int64_t& max ) {

  int st = readHeader( fb );

  max = dataHdr.numBytes / sizeof(int);

  return st;

}

int64_t BinDataBase::getMaxElements ( void ) {

  if ( !hdrRead ) return 0;
  
  return (int64_t) ( dataHdr.numBytes / sizeof(int) );

}
    
void BinDataBase::seekToReadOffset( int64_t offset ) {
  oneFb.pubseekoff(offset, std::ios::beg, std::ios::in);
  oneOffset = offset;
}

int64_t BinDataBase::readTraceData (
 int *buf,
 int64_t readSizeInbytes ) {

  return 0;

}

int64_t BinDataBase::readTraceData (
 std::filebuf& fb,
 int *buf,
 int64_t readSizeInbytes ) {

  return 0;

}
