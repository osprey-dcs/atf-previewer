//
// Created by jws3 on 5/1/24.
//

#ifndef FILEREFORMATTER_PSNFILECONVERTER_H
#define FILEREFORMATTER_PSNFILECONVERTER_H

#include "errno.h"

#include "memory"
#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <list>

#include <QString>

#include "FileConverter.h"

class PsnFileConverter : public FileConverter {

public:

  static constexpr unsigned int dataFileVersion[] { 1,0,0 };

  static const int NumStatusVersion {3};
  static const int NumStatusFileType {16};
  static const int NumCccr {80};
  static const unsigned int NumStatusFields {10};

  // for writing status file
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

  PsnFileConverter();
  int convert(  int chassisIndex, std::list<int>& chanList, int startingSigIndex, DataHeader *dh,
                const QString& rawDataFile, const QString& binDataFileDir, const QString& simpleName,
                bool verbose=false );
  
  void show( void );

  std::filebuf rawDataFileBuf, binDataFileBuf[Cnst::MaxRecs+1], statusFileBuf;

private:

  int readBinHeader (std::filebuf& fb, size_t loc, size_t &numBytesRead,
                      QString& headerType, bool& complete );
  int readHeaderType(std::filebuf& fb, size_t loc, QString& headerType, bool& eof );
  int readBinData (std::filebuf& fb, size_t loc, unsigned int dataLen,
                   unsigned int *buf, size_t &numBytesRead, bool& complete );
  QString buildOutputFileName( int sigIndex, const QString& binDataFileDir, const QString& simpleName );
  int createAndOpenOutputFiles ( std::list<int>& chanList, int startingSigIndex,
                                 const QString& binDataFileDir, const QString& simpleName, 
                                 bool verbose=false );
  int writeOutputFiles (std::list<int>& chanList, unsigned int numValues,
                         unsigned int array[Cnst::MaxSignals+1][Cnst::Max4PerWord] );
  int closeOutputFiles ( std::list<int>& chanList );

  QString buildStatusOutputFileName( int chassisIndex, const QString& binDataFileDir,
                                      const QString& simpleName );
  int createAndOpenStatusOutputFile ( int chassisIndex, const QString& binDataFileDir, 
                                      const QString& simpleName, bool verbose=false );
  void statusHdrSetCccr( QString& cccr );
  void statusHdrSetNumBytes( const int64_t numBytes );
  int writeStatusHdr ( void );
  void initStatusOutputFile( void );
  int writeStatusOutputFile ( const int64_t numValues,
                              const unsigned int array[Cnst::MaxStatus][NumStatusFields] );
  int closeStatusOutputFile ( void );
  int getRawBinFileChanList( const QString& rawBinFileName, std::list<int>& chanList );

  #pragma pack(push, 1)
  typedef struct BinHdrPsnaTag {
    char id[4];
    unsigned int bodyLen;
    unsigned int rcvSeconds;
    unsigned int rcvNanoseconds;
    unsigned int status;
    unsigned int chanMask;
    unsigned int seqHigh;
    unsigned int seqLow;
    unsigned int seconds;
    unsigned int nanoseconds;
  } BinHdrPsnaType, *BinHdrPsnaPtr;
  #pragma pack(pop)

  #pragma pack(push, 1)
  typedef struct BinHdrPsnbTag {
    char id[4];
    unsigned int bodyLen;
    unsigned int rcvSeconds;
    unsigned int rcvNanoseconds;
    unsigned int status;
    unsigned int chanMask;
    unsigned int seqHigh;
    unsigned int seqLow;
    unsigned int seconds;
    unsigned int nanoseconds;
    int lolo;
    int lo;
    int hi;
    int hihi;
  } BinHdrPsnbType, *BinHdrPsnbPtr;
  #pragma pack(pop)

  #pragma pack(push, 1)
  typedef struct BinHdrGenericTag {
    char id[4];
    unsigned int bodyLen;
    unsigned int rcvSeconds;
    unsigned int rcvNanoseconds;
  } BinHdrGenericType, *BinHdrGenericPtr;
  #pragma pack(pop)

  static constexpr unsigned int statusFileVersion[] { 2,0,0 };

  #pragma pack(push, 1)
  typedef struct StatusHdrTag {
    unsigned int version[NumStatusVersion];
    char fileType[NumStatusFileType];
    char cccr[NumCccr];
    uint64_t recSize;
    uint64_t numBytes;
    unsigned int summaryValues[PsnFileConverter::NumStatusFields];
  } StatusHdrType, StatusHdrPtr;

  BinHdrPsnaType binHeaderPsna;
  BinHdrPsnbType binHeaderPsnb;
  BinHdrGenericType binHeaderGeneric;

  DataHeader *dh = nullptr;
  bool firstSeqNum = true;
  bool firstStatusTime = true;
  double baseTime = 0.0;
  uint64_t prevSeqNum;
  StatusHdrType statusHdr;

};

#endif //FILEREFORMATTER_PSNFILECONVERTER_H
