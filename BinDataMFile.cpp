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

#include "BinDataMFile.h"

BinDataMFile::BinDataMFile() {

  memset( (char *) &dataHdr, 0, sizeof(DataHdrType) );

}

BinDataMFile::~BinDataMFile() {

}

int BinDataMFile::newFile( std::string fileName ) {
  int64_t max;
  int st = getMaxElements( fileName, max ); // do this so header is read
  return vdisk.setFile ( fileName );
}

void BinDataMFile::initMaxBufSize( int64_t max ) {
  vdisk.setMaxSize( max );
}

int BinDataMFile::getMaxElements ( std::string fileName, int64_t& max ) {

  std::filebuf fb;
  
  auto result = fb.open( fileName, std::ios::in | std::ios::binary );
  if ( !result ) {
    return ERRINFO(EMax,"");
  }

  int st = readHeader( fb );

  fb.close();

  max = dataHdr.numBytes / sizeof(int);

  return st;

}

int BinDataMFile::getMaxElements ( std::filebuf& fb, int64_t& max ) {

  int st = readHeader( fb );

  max = dataHdr.numBytes / sizeof(int);

  return st;

}

int BinDataMFile::readHeader ( void ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  int st = readHeader( oneFb );

  return st;

}

int BinDataMFile::readHeader ( std::filebuf& fb ) {

  vdisk.readN( &fb, 0ul, sizeof(dataHdr.version), (char *) &(dataHdr.version) );
  vdisk.readN( &fb, (int64_t) sizeof(dataHdr.version), sizeof(dataHdr.numBytes), (char *) &(dataHdr.numBytes) );
  oneOffset = getHeaderSize();

  hdrRead = true;

  return 0;

}

int BinDataMFile::readVersion ( int64_t& major, int64_t& minor, int64_t& release ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  vdisk.readN( &oneFb, 0ul, sizeof(dataHdr.version), (char *) &(dataHdr.version) );
  oneOffset = sizeof(dataHdr.version);

  major = dataHdr.version[0];
  minor = dataHdr.version[1];
  release = dataHdr.version[2];

  return 0;

}

int BinDataMFile::readNumBytes ( int64_t& num ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }
  
  vdisk.readN( &oneFb, (int64_t) sizeof(dataHdr.version), sizeof(dataHdr.numBytes), (char *) &(dataHdr.numBytes) );
  oneOffset = getHeaderSize();

  num = dataHdr.numBytes;

  return 0;

}

int64_t BinDataMFile::readTraceData (
 int *buf,
 int64_t readSizeInbytes ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  if ( ( oneOffset + readSizeInbytes ) > dataHdr.numBytes ) {
    readSizeInbytes = dataHdr.numBytes - oneOffset;
    if ( readSizeInbytes <= 0 ) return 0;
  }
  
  int64_t n = vdisk.readN( &oneFb, oneOffset, readSizeInbytes, (char *) buf );
  oneOffset += readSizeInbytes;
  return n;

}

int64_t BinDataMFile::readTraceData (
 std::filebuf& fb,
 int *buf,
 int64_t readSizeInbytes ) {

  std::cout << "readSizeInbytes 1 = " << readSizeInbytes << std::endl;
  if ( ( oneOffset + readSizeInbytes ) > dataHdr.numBytes ) {
    readSizeInbytes = dataHdr.numBytes - oneOffset;
    if ( readSizeInbytes <= 0 ) return 0;
  }
  std::cout << "readSizeInbytes 2 = " << readSizeInbytes << std::endl;
  
  int64_t n = vdisk.readN( &fb, oneOffset, readSizeInbytes, (char *) buf );
  oneOffset += readSizeInbytes;
  return n;

}
