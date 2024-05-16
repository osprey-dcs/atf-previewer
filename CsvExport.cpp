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

  fb << "\"Acquistion ID\", \"" << id.toStdString() << "\"" << std::endl;
  fb << "\"StartTime\", \"" << startTime.toStdString() << "\"" << std::endl;
  fb << "\"EndTime\", \"" << endTime.toStdString() << "\"" << std::endl;
  fb << "\"Description\", \"" << desc.toStdString() << "\"" << std::endl;
  fb << "\"InputCSVFile\", \"" << inputCsvFileName.toStdString()<< "\"" << std::endl;
  fb << "\"HeaderFile\", \"" << headerFileName.toStdString()<< "\"" << std::endl;

  return 0;
  
}

int CsvExport::writeSignalProperties( std::ofstream &fb, int *signalIndices, int numSignals ) {

 DataHeader::DataHeaderIndexMapType indexMap = dh->getIndexMap();

 for ( int i=0; i<numSignals; i++ ) {

   fb << "\"NumSignals\", " << "\"" << numSignals << "\"" << std::endl;

   fb << "\"Signal\", " << "\"" << signalIndices[i] << "\"" << std::endl;
   fb <<  "\"Name\", " << "\"" << std::get<DataHeader::SIGNAME>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
   fb <<  "\"Chassis\", " << "\"" << std::get<DataHeader::ADDRESS_CHASSIS >( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"Channel\", " << "\"" << std::get<DataHeader::ADDRESS_CHANNEL >( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"Desc\", " << "\"" << std::get<DataHeader::DESC>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
     
   fb << "\"Units\", " << "\"" << std::get<DataHeader::EGU>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
   fb <<  "\"Slope\", " << "\"" << std::get<DataHeader::SLOPE>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"Intercept\", " << "\"" << std::get<DataHeader::INTERCEPT>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"Type\", " << "\"" << std::get<DataHeader::TYPE>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"YLabel\", " << "\"" << std::get<DataHeader::YAXISLABEL>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
     
   fb << "\"RespNode\", " << "\"" << std::get<DataHeader::RESPONSENODE>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"RespDir\", " << "\"" << std::get<DataHeader::RESPONSEDIRECTION>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"RefNode\", " << "\"" << std::get<DataHeader::REFERENCENODE>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"RefDir\", " << "\"" << std::get<DataHeader::REFERENCEDIRECTION >( indexMap[signalIndices[i]] ) << "\"" << std::endl;
   fb <<  "\"Coupling\", " << "\"" << std::get<DataHeader::COUPLING>( indexMap[signalIndices[i]] ) << "\"" << std::endl;

   fb << "\"DataFileName\", " << "\"" << std::get<DataHeader::DATA_FILENAME >( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;

 }

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
