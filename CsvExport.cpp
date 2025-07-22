//
// Created by jws3 on 5/10/24.
//

#include "CsvExport.h"

CsvExport::CsvExport ( const QString& headerFile ) :
  ErrHndlr ( DataHeader::NumErrs, DataHeader::errMsgs ) {

  hdrFile = headerFile;
  dh = dhf.createDataHeader();
  dh->readContents( hdrFile );

}

CsvExport::~CsvExport () {

}

int CsvExport::setOutFile ( const QString& qs ) {
  return 0;
}

int CsvExport::writeHeader( FILE *f, std::ofstream &fb, QString &id, QString &desc,
                            QString &startTime, QString &endTime,
                            QString &inputCsvFileName, QString &headerFileName ) {

  try {

    if ( f ) {
      
      fprintf( f, "\"Acquistion ID\", \"%s\"\n", id.toStdString().c_str() );
      fprintf( f, "\"StartTime\", \"%s\"\n", startTime.toStdString().c_str() );
      fprintf ( f, "\"EndTime\", \"%s\"\n", endTime.toStdString().c_str() );
      fprintf( f, "\"Description\", \"%s\"\n", desc.toStdString().c_str() );
      fprintf( f, "\"InputCSVFile\", \"%s\"\n", inputCsvFileName.toStdString().c_str() );
      fprintf( f, "\"HeaderFile\", \"%s\"\n", headerFileName.toStdString().c_str() );

    }
    else {

      fb << "\"Acquistion ID\", \"" << id.toStdString() << "\"" << std::endl;
      fb << "\"StartTime\", \"" << startTime.toStdString() << "\"" << std::endl;
      fb << "\"EndTime\", \"" << endTime.toStdString() << "\"" << std::endl;
      fb << "\"Description\", \"" << desc.toStdString() << "\"" << std::endl;
      fb << "\"InputCSVFile\", \"" << inputCsvFileName.toStdString()<< "\"" << std::endl;
      fb << "\"HeaderFile\", \"" << headerFileName.toStdString()<< "\"" << std::endl;

    }
    
  }
  catch ( const std::exception& e ) {
    return ERRINFO(EWriteFailure,e.what());
  }

  return 0;
  
}

int CsvExport::writeSignalProperties( FILE *f, std::ofstream &fb, int *signalIndices, int numSignals ) {

 DataHeader::DataHeaderIndexMapType indexMap = dh->getIndexMap();

 try {
   
   for ( int i=0; i<numSignals; i++ ) {

     if ( f ) {
       
       fprintf( f, "\"NumSignals\", \"%d\"\n", numSignals );

       fprintf( f, "\"Signal\", \"%d\"\n", signalIndices[i] );
       fprintf( f, "\"Name\", \"%s\"\n", std::get<DataHeader::SIGNAME>( indexMap[signalIndices[i]] ).toStdString().c_str() );
       fprintf( f, "\"Chassis\", \"%g\"\n", std::get<DataHeader::ADDRESS_CHASSIS >( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"Channel\", \"%g\"\n", std::get<DataHeader::ADDRESS_CHANNEL >( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"Desc\", \"%s\"\n", std::get<DataHeader::DESC>( indexMap[signalIndices[i]] ).toStdString().c_str() );

       fprintf( f,"\"Units\", \"%s\"\n", std::get<DataHeader::EGU>( indexMap[signalIndices[i]] ).toStdString().c_str() );
       fprintf( f, "\"Slope\", \"%.8f\"\n", std::get<DataHeader::SLOPE>( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"Intercept\", \"%.8f\"\n", std::get<DataHeader::INTERCEPT>( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"Type\", \"%g\"\n", std::get<DataHeader::TYPE>( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"YLabel\", \"%s\"\n", std::get<DataHeader::YAXISLABEL>( indexMap[signalIndices[i]] ).toStdString().c_str() );
     
       fprintf( f,"\"RespNode\", \"%g\"\n", std::get<DataHeader::RESPONSENODE>( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"RespDir\", \"%g\"\n", std::get<DataHeader::RESPONSEDIRECTION>( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"RefNode\", \"%g\"\n", std::get<DataHeader::REFERENCENODE>( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"RefDir\", \"%g\"\n", std::get<DataHeader::REFERENCEDIRECTION >( indexMap[signalIndices[i]] ) );
       fprintf( f, "\"Coupling\", \"%g\"\n", std::get<DataHeader::COUPLING>( indexMap[signalIndices[i]] ) );

       fprintf( f,"\"DataFileName\", \"%s\"\n", std::get<DataHeader::DATA_FILENAME >( indexMap[signalIndices[i]] ).toStdString().c_str() );

     }
     else {
     
       fb << "\"NumSignals\", " << "\"" << numSignals << "\"" << std::endl;

       fb << "\"Signal\", " << "\"" << signalIndices[i] << "\"" << std::endl;
       fb <<  "\"Name\", " << "\"" << std::get<DataHeader::SIGNAME>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
       fb <<  "\"Chassis\", " << "\"" << std::get<DataHeader::ADDRESS_CHASSIS >( indexMap[signalIndices[i]] ) << "\"" << std::endl;
       fb <<  "\"Channel\", " << "\"" << std::get<DataHeader::ADDRESS_CHANNEL >( indexMap[signalIndices[i]] ) << "\"" << std::endl;
       fb <<  "\"Desc\", " << "\"" << std::get<DataHeader::DESC>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
     
       fb << "\"Units\", " << "\"" << std::get<DataHeader::EGU>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
       fb <<  "\"Slope\", " << "\"" << std::left << std::fixed << std::setprecision(8)<< std::get<DataHeader::SLOPE>( indexMap[signalIndices[i]] ) << "\"" << std::defaultfloat << std::endl;
       fb <<  "\"Intercept\", " << "\"" << std::left << std::fixed << std::setprecision(8) << std::get<DataHeader::INTERCEPT>( indexMap[signalIndices[i]] ) << "\"" << std::defaultfloat << std::endl;
       fb <<  "\"Type\", " << "\"" << std::get<DataHeader::TYPE>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
       fb <<  "\"YLabel\", " << "\"" << std::get<DataHeader::YAXISLABEL>( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;
     
       fb << "\"RespNode\", " << "\"" << std::get<DataHeader::RESPONSENODE>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
       fb <<  "\"RespDir\", " << "\"" << std::get<DataHeader::RESPONSEDIRECTION>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
       fb <<  "\"RefNode\", " << "\"" << std::get<DataHeader::REFERENCENODE>( indexMap[signalIndices[i]] ) << "\"" << std::endl;
       fb <<  "\"RefDir\", " << "\"" << std::get<DataHeader::REFERENCEDIRECTION >( indexMap[signalIndices[i]] ) << "\"" << std::endl;
       fb <<  "\"Coupling\", " << "\"" << std::get<DataHeader::COUPLING>( indexMap[signalIndices[i]] ) << "\"" << std::endl;

       fb << "\"DataFileName\", " << "\"" << std::get<DataHeader::DATA_FILENAME >( indexMap[signalIndices[i]] ).toStdString() << "\"" << std::endl;

     }

   }
 }
 catch ( const std::exception& e ) {
   return ERRINFO(EWriteFailure,e.what());
 }

 return 0;

}

int CsvExport::writeSignalNames( FILE *f, std::ofstream &fb, QString *names, int numNames ) {

  try {
    if ( f ) {
      fprintf( f, "Rec, Time, " );
    }
    else {
      fb << "Rec, Time, ";
    }
  
    for ( int i=0; i<numNames; i++ ) {

      if ( f ) {
        fprintf( f, "%s, ", names[i].toStdString().c_str() );
      }
      else {
        fb << names[i].toStdString() << ", ";
      }

    }

    if ( f ) {
      fprintf( f, "\n" );
    }
    else {
      fb << std::endl;
    }

  }
  catch ( const std::exception& e ) {
    return ERRINFO(EWriteFailure,e.what());
  }
  return 0;

}

int CsvExport::writeData( FILE *f, std::ofstream& fb, int64_t rec, double time, double *buf, int64_t n ) {
  // write 1 row of data

  try {

    if ( f ) {
      
      fprintf( f, "%ld, ", rec );
      fprintf( f, "%.8f, ", time );

    }
    else {
  
      fb << std::left << rec << ", ";
      fb << std::left << std::fixed << std::setprecision(8) << time << ", ";

    }

    for ( int i=0; i<n; i++ ) {

      if ( f ) {
        fprintf( f, "%.8f, ", buf[i] );
      }
      else {
        fb << std::left << std::fixed << std::setprecision(8) << buf[i] << ", ";
      }

    }

    if ( f ) {
      fprintf( f, "\n" );
    }
    else {
      fb << std::endl;
    }

  }
  catch ( const std::exception& e ) {
    return ERRINFO(EWriteFailure,e.what());
  }
  return 0;

}
