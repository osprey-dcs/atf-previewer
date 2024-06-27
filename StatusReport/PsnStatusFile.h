//
// Created by jws3 on 6/15/24.
//

#ifndef STATUSREPORT_PSNSTATUSFILE_H
#define STATUSREPORT_PSNSTATUSFILE_H


#include <cstdint>

#include "StatusFileBase.h"

class PsnStatusFile : public StatusFileBase {

public:

  static const int NumStatusVersion {3};
  static const int NumStatusFileType {16};
  static const int NumCccr {80};
  static const unsigned int NumStatusFields {10};

  // for interpreting status record
  static const int STATUS      = 0;
  static const int CHANMASK    = 1;
  static const int SECS        = 2;
  static const int NANOSECS    = 3;
  static const int RCVSECS     = 4;
  static const int RCVNANOSECS = 5;
  static const int LO          = 6;
  static const int LOLO        = 7;
  static const int HI          = 8;
  static const int HIHI        = 9;

  #pragma pack(push, 1)
  typedef struct PsnStatusHdrTag {
    unsigned int version[NumStatusVersion];
    char fileType[NumStatusFileType];
    char cccr[NumCccr];
    uint64_t recSize;
    uint64_t numBytes;
    unsigned int summaryValues[PsnStatusFile::NumStatusFields];
  } PsnStatusHdrType, PsnStatusHdrPtr;

  PsnStatusHdrType statusHdr;
  PsnStatusHdrType outStatusHdr;

  // declared in base class
  //std::filebuf readFb;
  //std::filebuf writeFb;
  //bool isOpenRead = false;
  //bool isOpenWrite = false;
  //bool hdrRead = false;
  //int64_t readOffset = 0l;
  //int64_t writeOffset = 0l;

  PsnStatusFile ();
  ~PsnStatusFile ();
  PsnStatusFile ( const PsnStatusFile& ) = delete;
  PsnStatusFile ( PsnStatusFile& ) = delete;
  PsnStatusFile ( PsnStatusFile&& ) = delete;
  PsnStatusFile& operator= ( const PsnStatusFile& ) = delete;
  PsnStatusFile& operator= ( PsnStatusFile& ) = delete;
  PsnStatusFile& operator= ( PsnStatusFile&& ) = delete;

  int openRead ( const std::string& name );
  int openWrite ( const std::string& name );

  int closeRead ( void );
  int closeWrite ( void );

  int64_t readHeader ( void );
  int64_t writeHeader(void);

  void getVersion ( int64_t& major, int64_t& minor, int64_t& release );
  int64_t getRecSize ( void );
  int64_t getNumBytes ( void );
  int64_t getMaxElements ( void );
  std::string getFileType ( void );
  std::string getCccr ( void );
  void getSummaryRecord( int *rec );

  void setVersion(int64_t &major, int64_t &minor, int64_t &release);
  void setRecSize ( int64_t recSize );
  void setNumBytes ( int64_t numBytes );
  void setMaxElements ( int64_t maxEle );
  void setFileType ( const std::string& fileType );
  void setCccr ( const std::string& cccr );
  void setSummaryRecord( int *rec );

  int64_t getHeaderSize ( void );

  int getDataFullTimeRange ( double& minTime, double& maxTime );

  int getRecordRangeForTime ( double minTime, double maxTime, int64_t& min, int64_t& max );

  void inputSeekToStartOfData ( int64_t firstDataByte );

  void outputSeekToStartOfData ( int64_t firstDataByte );

  void seekToReadOffset ( int64_t offset );

  int64_t readData ( int *buf, int64_t readSizeInbytes );

  void seekToWriteOffset ( int64_t offset );

  int64_t writeData ( int *buf, int64_t readSizeInbytes );

};


#endif //STATUSREPORT_PSNSTATUSFILE_H
