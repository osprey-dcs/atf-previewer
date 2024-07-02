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

#include "ErrHndlr.h"

class StatusFileBase : public ErrHndlr {

public:

  static const int NumErrs = 5;
  static const int ESuccess = 0;
  static const int EFileOpen = 1;
  static const int EFileRead = 2;
  static const int EFileWrite = 3;
  static const int ENoFile = 4;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "Status file open failure" },
    { "Status file read failure" },
    { "Status file write failure" },
    { "Status file not open" }
  };

  std::filebuf readFb;
  std::filebuf writeFb;
  bool isOpenRead = false;
  bool isOpenWrite = false;
  bool hdrRead = false;
  bool hdrWritten = false;
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

  virtual int64_t readHeader ( void );
  virtual int64_t writeHeader ( void );

  virtual void getVersion ( int64_t& major, int64_t& minor, int64_t& release );
  virtual int64_t getRecSize ( void );
  virtual int64_t getNumBytes ( void );
  virtual int64_t getMaxElements ( void );
  virtual std::string getFileType ( void );
  virtual std::string getCccr ( void );
  virtual void getSummaryRecord( int *rec );
  virtual void setVersion ( int64_t& major, int64_t& minor, int64_t& release );
  virtual void setRecSize ( int64_t recSize );
  virtual void setNumBytes ( int64_t numBytes );
  virtual void setFileType ( const std::string& fileType );
  virtual void setCccr ( const std::string& ccr );
  virtual void setSummaryRecord( int *rec );

  virtual int64_t getHeaderSize ( void );

  virtual int getDataFullTimeRange ( double& minTime, double& maxTime );

  virtual int getRecordRangeForTime ( double minTime, double maxTime, int64_t& min, int64_t& max );

  virtual void inputSeekToStartOfData ( int64_t firstDataByte );

  virtual void outputSeekToStartOfData ( int64_t firstDataByte );

  virtual void seekToReadOffset ( int64_t offset );

  virtual int64_t readData ( int *buf, int64_t readSizeInbytes );

  virtual void seekToWriteOffset ( int64_t offset );

  virtual int64_t writeData ( int *buf, int64_t readSizeInbytes );

};


#endif //STATUSREPORT_STATUSFILEBASE_H
