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

class FileConverter {

public:

  static const int E_SUCCESS = 0;
  static const int E_FAIL = 1;
  static const int E_OPEN_FAILURE = 2;
  static const int E_READ_FAILURE = 3;
  static const int E_WRITE_FAILURE = 4;

  FileConverter();
  virtual int convert( int chassisIndex, std::list<int>& chanList, int startingSigIndex,
                       const DataHeader *dh, const QString& rawDataFile,
                       const QString& binDataFileDir, const QString& simpleName, bool verbose=false );
  virtual QString buildOutputFileName( int sigIndex, const QString& binDataFileDir, const QString& simpleName );
  virtual QString buildStatusOutputFileName( int chassisIndex, const QString& binDataFileDir,
                                      const QString& simpleName );
  virtual void show( void );

  std::filebuf fb[Cnst::MaxSignals+1];
  unsigned long fileLoc[Cnst::MaxSignals+1];

  std::filebuf statusFb;
  unsigned long statusFileLoc;

};


#endif //FILEREFORMATTER_FILECONVERTER_H
