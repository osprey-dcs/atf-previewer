//
// Created by jws3 on 5/10/24.
//


// to increase max file handles, do ulimit -Sn {max}


#ifndef VIEWER_CSVEXPORT_H
#define VIEWER_CSVEXPORT_H

#include <stdio.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>

#include <QString>

#include "ExportDialog.h"
#include "DataHeaderFac.h"
#include "ErrHndlr.h"
#include "Cnst.h"

class CsvExport : public ErrHndlr {

public:

  static const int NumErrs = 2;
  static const int ESuccess = 0;
  static const int EWriteFailure = 1;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "Output file write failure: " }
  };

  explicit CsvExport ( const QString& qs );

  virtual ~CsvExport ();

  int setOutFile ( const QString& qs );

  int writeHeader( FILE *f, std::ofstream &fb, QString &id, QString &desc,
                   QString &startTime, QString &endTime,
                   QString &inputCsvFileName, QString &headerFileName );

  int writeSignalProperties( FILE *f, std::ofstream &fb, int *signalIndices, int numSignals );
  
  int writeSignalNames( FILE *f, std::ofstream &fb, QString *names, int numNames );
  
  int writeData( FILE *f, std::ofstream &fb, int64_t rec, double time, double *buf, int64_t numBytes );
  
  DataHeaderFac dhf;
  std::shared_ptr<DataHeader> dh;
  QString hdrFile;
  QString outFile;


};


#endif //VIEWER_CSVEXPORT_H
