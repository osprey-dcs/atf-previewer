//
// Created by jws3 on 5/10/24.
//

#include "CsvExport.h"

CsvExport::CsvExport ( const QString& headerFile ) {

  hdrFile = headerFile;
  dh = dhf.createDataHeader();
  dh->readContents( hdrFile );

}

CsvExport::~CsvExport () {

}

int CsvExport::setOutFile ( const QString& qs ) {
  return 0;
}

int CsvExport::writeHeader( std::ofstream &fb, QString &id, QString &desc,
                            QString &startTime, QString &endTime,
                            QString &inputCsvFileName, QString &headerFileName ) {

  int n;

  std::stringstream strm;

  fb << "Acquistion ID: " << id.toStdString() << "    Start: " << startTime.toStdString() <<
    "    End: " << endTime.toStdString() << std::endl;
  //n = fb.sputn( (const char *) (strm.str().c_str()), strm.str().size() );

  fb << "Description: " << desc.toStdString() << std::endl;
  //n = fb.sputn( (const char *) (strm.str().c_str()), strm.str().size() );

  fb << "Input CSV file: " << inputCsvFileName.toStdString() << std::endl;
  //n = fb.sputn( (const char *) (strm.str().c_str()), strm.str().size() );

  fb << "Header file: " << headerFileName.toStdString() << std::endl;
  //n = fb.sputn( (const char *) (strm.str().c_str()), strm.str().size() );

  return 0;
  
}

int CsvExport::writeSignalNames( std::ofstream &fb, QString *names, int numNames ) {

  int nr;
  std::stringstream strm;

  fb << "Rec, Time, ";
  
  for ( int i=0; i<numNames; i++ ) {

    fb << names[i].toStdString() << ", ";

  }

  fb << std::endl;

  //nr = fb.sputn( (const char *) (strm.str().c_str()), strm.str().size() );

  return nr;

}

int CsvExport::writeData( std::ofstream& fb, unsigned long rec, double time, double *buf, unsigned long n ) {

  int nr;
  std::stringstream strm;

  // write 1 row of data

  fb << std::left << rec << ", ";
  fb << std::left << std::setprecision(8) << time << ", ";
  
  for ( int i=0; i<n; i++ ) {

    fb << std::left << std::setprecision(8) << buf[i] << ", ";

  }

  fb << std::endl;

  //nr = fb.sputn( (const char *) (strm.str().c_str()), strm.str().size() );

  return nr;

}
