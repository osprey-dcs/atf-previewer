//
// Created by jws3 on 5/10/24.
//


// to increase max file handles, do ulimit -Sn {max}


#ifndef VIEWER_CSVEXPORT_H
#define VIEWER_CSVEXPORT_H

#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>

#include <QString>

#include "ExportDialog.h"
#include "DataHeaderFac.h"
#include "Cnst.h"

class CsvExport {

public:

  explicit CsvExport ( const QString& qs );

  virtual ~CsvExport ();

  int setOutFile ( const QString& qs );

  int writeHeader( std::ofstream &fb, QString &id, QString &desc,
                   QString &startTime, QString &endTime,
                   QString &inputCsvFileName, QString &headerFileName );

  int writeSignalProperties( std::ofstream &fb, int *signalIndices, int numSignals );
  
  int writeSignalNames( std::ofstream &fb, QString *names, int numNames );
  
  int writeData( std::ofstream &fb, uint64_t rec, double time, double *buf, uint64_t numBytes );
  
  DataHeaderFac dhf;
  std::shared_ptr<DataHeader> dh;
  QString hdrFile;
  QString outFile;


};


#endif //VIEWER_CSVEXPORT_H
