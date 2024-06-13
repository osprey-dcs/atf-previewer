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
#include <iostream>
#include <iomanip>
#include <fstream>

#include <QString>

#include "Cnst.h"
#include "ErrHndlr.h"

class BinDataBase : public ErrHndlr {

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
  
  using TwoDIntPtr = int(*)[5];

  std::filebuf oneFb;
  bool isOpenRead = false;
  int64_t version[3];
  int64_t numBytes;
  int64_t oneOffset = 0l;

  BinDataBase ();
  virtual ~BinDataBase ();
  BinDataBase ( const BinDataBase& ) = delete;
  BinDataBase ( BinDataBase& ) = delete;
  BinDataBase ( BinDataBase&& ) = delete;
  virtual BinDataBase& operator= ( const BinDataBase& ) = delete;
  virtual BinDataBase& operator= ( BinDataBase& ) = delete;
  virtual BinDataBase& operator= ( BinDataBase&& ) = delete;

  virtual int openRead ( const std::string& name );

  virtual int closeFile ( void );

  virtual int readVersion ( int64_t& major, int64_t& minor, int64_t& release );

  virtual int readNumBytes ( int64_t& num );

  virtual int64_t getHeaderSize( void );

  virtual int newFile ( QString filename );

  virtual void initMaxBufSize( int64_t max );

  virtual int getDataFullTimeRange( QString filename, double sampleRate, double& minTime, double& maxTime );
  
  virtual int getRecordRangeForTime( QString filename, double sampleRate, double minTime, double maxTime,
                                     int64_t& min, int64_t& max );
  
  virtual void inputSeekToStartOfData( std::filebuf& fb, int64_t firstDataByte );

  virtual void outputSeekToStartOfData( std::filebuf& fb, int64_t firstDataByte );

  virtual int getMaxElements ( QString filename, int sigIndex, int64_t& max );

  virtual void seekToReadOffset( int64_t offset );
  
  virtual int64_t readTraceData( int *buf, int64_t readSizeInbytes );

  virtual int64_t readTraceData( std::filebuf& fb, int *buf, int64_t readSizeInbytes );

};

#endif //VIEWER_BINDATABASE_H
