//
// Created by jws3 on 5/1/24.
//

#ifndef FILEREFORMATTER_FILECONVERTER_H
#define FILEREFORMATTER_FILECONVERTER_H

#include "memory"
#include <fstream>
#include <list>

#include <QString>

#include "Cnst.h"
#include "DataHeader.h"
#include "ErrHndlr.h"

class FileConverter : public ErrHndlr {

public:
  
  static const int NumErrs = 8;
  static const int ESuccess = 0;
  static const int EInFileOpen = 1;
  static const int EOutFileOpen = 2;
  static const int EStatFileOpen = 3;
  static const int EInternal = 4;
  static const int EReadFailure = 5;
  static const int EWriteFailure = 6;
  static const int ESequence = 7;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "Input file open failure: " },
    { "Output file open failure: " },
    { "Status file open failure: " },
    { "Unexpected quantity, internal error failure" },
    { "File read failure: " },
    { "File write failure: " },
    { "Sequence number failure" }
  };

  FileConverter();
  virtual int convert( int chassisIndex, std::list<int>& chanList, int startingSigIndex,
                       const DataHeader *dh, const QString& rawDataFile,
                       const QString& binDataFileDir, const QString& simpleName, bool verbose=false );
  virtual QString buildOutputFileName( int sigIndex, const QString& binDataFileDir, const QString& simpleName );
  virtual QString buildStatusOutputFileName( int chassisIndex, const QString& binDataFileDir,
                                      const QString& simpleName );
  virtual int getRawBinFileChanList( const QString& rawBinFileName, std::list<int>& chanList );
  virtual void show( void );

  std::filebuf fb[Cnst::MaxSignals+1];
  unsigned long fileLoc[Cnst::MaxSignals+1]{};

  std::filebuf statusFb;
  unsigned long statusFileLoc;

};


#endif //FILEREFORMATTER_FILECONVERTER_H
