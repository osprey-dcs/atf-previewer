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
  
  static const int BaseNumErrs = 1;
  static const int ESuccess = 0;
  inline static const std::string BaseErrMsgs[BaseNumErrs] {
    { "Success" }
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
