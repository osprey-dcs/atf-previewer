//
// Created by jws3 on 6/15/24.
//

#ifndef STATUSREPORT_STATUSFILEBASE_H
#define STATUSREPORT_STATUSFILEBASE_H


#include <utility>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "Cnst.h"
#include "ErrHndlr.h"

class StatusFileBase : public ErrHndlr {

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

  std::filebuf readFb;
  std::filebuf writeFb;
  bool isOpenRead = false;
  bool isOpenWrite = false;
  bool hdrRead = false;
  int64_t readOffset = 0l;
  int64_t writeOffset = 0l;

  StatusFileBase ();
  virtual ~StatusFileBase ();
  StatusFileBase ( const StatusFileBase& ) = delete;
  StatusFileBase ( StatusFileBase& ) = delete;
  StatusFileBase ( StatusFileBase&& ) = delete;
  virtual StatusFileBase& operator= ( const StatusFileBase& ) = delete;
  virtual StatusFileBase& operator= ( StatusFileBase& ) = delete;
  virtual StatusFileBase& operator= ( StatusFileBase&& ) = delete;

  virtual int openRead ( const std::string& name );
  virtual int openWrite ( const std::string& name );

  virtual int closeRead ( void );
  virtual int closeWrite ( void );

  virtual int readHeader ( void );

  virtual void getVersion ( int64_t& major, int64_t& minor, int64_t& release );
  virtual int64_t getRecSize ( void );
  virtual int64_t getNumBytes ( void );
  virtual int64_t getMaxElements ( void );
  virtual std::string getFileType ( void );
  virtual std::string getCccr ( void );
  virtual void getSummaryRecord( int *rec );

  virtual int64_t getHeaderSize ( void );

  virtual int getDataFullTimeRange ( double& minTime, double& maxTime );

  virtual int getRecordRangeForTime ( double minTime, double maxTime, int64_t& min, int64_t& max );

  virtual void inputSeekToStartOfData ( int64_t firstDataByte );

  virtual void outputSeekToStartOfData ( int64_t firstDataByte );

  virtual void seekToReadOffset ( int64_t offset );

  virtual int64_t readData ( int *buf, int64_t readSizeInbytes );

};


#endif //STATUSREPORT_STATUSFILEBASE_H
