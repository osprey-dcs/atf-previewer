//
// Created by jws3 on 5/10/24.
//

#include "Uff58bExport.h"

Uff58bExport::Uff58bExport ( const QString& headerFile ) {

  hdrFile = headerFile;
  dh = dhf.createDataHeader();
  dh->readContents( hdrFile );

}

Uff58bExport::~Uff58bExport () {

}

int Uff58bExport::setOutFile ( const QString &qs ) {
  return 0;
}

int Uff58bExport::set58bHeader(unsigned long numBytes) {

  int endian = LITTLE;
  short sval = 1;
  char *cval = reinterpret_cast<char *>( &sval );
  if ( *cval != 1 ) endian = BIG;

  int floatFormat = IEEE_754;

  char lf = 10;
  char cr = 13;

  std::stringstream strm;
  strm << std::setw(6) << std::right << "58" << "b" <<
    std::setw(6) << std::right << endian <<
    std::setw(6) << std::right << floatFormat <<
    std::setw(12) <<std::right << NUM_ASCII_LINES <<
    std::setw(12) <<std::right << numBytes <<
    std::setw(6) << std::right << 0 <<
    std::setw(6) << std::right << 0 <<
    std::setw(12) << std::right << 0 <<
    std::setw(12) << std::right << 0 <<
    lf;

  uff58bIdLine = strm.str().c_str();

  //std::cout << "[         1         2         3         4         5         6         7         8]\n";
  //std::cout << "[12345678901234567890123456789012345678901234567890123456789012345678901234567890]\n";
  //std::cout << "[" << uff58bIdLine.toStdString() << "]\n";

  return 0;
  
}

int Uff58bExport::set80CharRec ( int recNum, QString line ) {

  if ( line.isEmpty() ) {
    line = "NONE";
  }

  if ( recNum < 1 ) recNum == 1;
  if ( recNum > 5 ) recNum = 5;

  recNum -= 1;

  char lf = 10;
  char cr = 13;

  std::stringstream strm;
  strm << std::setw(78) << std::left << std::setfill( ' ' ) << line.toStdString() << lf;

  headerLines1thru11[recNum] = strm.str().c_str();
  
  //std::cout << "[         1         2         3         4         5         6         7         8]\n";
  //std::cout << "[12345678901234567890123456789012345678901234567890123456789012345678901234567890]\n";
  //std::cout << "[" << headerLines1thru11[recNum].toStdString() << "]\n";
  
  return 0;
  
}

int Uff58bExport::setDofIdentification( int funcType, int funcIdNum,
  int versNum, int ldCaseIdNum, QString rspEntityName, int rspNode, int rspDir,
  QString refEntityName, int refNode, int refDir ) {

  if ( rspEntityName.isEmpty() ) rspEntityName = "NONE";
  if ( refEntityName.isEmpty() ) refEntityName = "NONE";
  
  char lf = 10;
  char cr = 13;

  std::stringstream strm;
  strm << std::setw(5) << std::right << funcType <<
    std::setw(10) << std::right << funcIdNum <<
    std::setw(5) << std::right << versNum <<
    std::setw(10) <<std::right << ldCaseIdNum <<
    " " <<
    std::setw(10) << std::left << std::setfill( ' ' ) << rspEntityName.toStdString() <<
    std::setw(10) << std::right << rspNode <<
    std::setw(4) << std::right << rspDir <<
    " " <<
    std::setw(10) << std::left << std::setfill( ' ' ) << refEntityName.toStdString() <<
    std::setw(10) << std::right << refNode <<
    std::setw(4) << std::right << refDir <<
    lf;

  int recNum = 6 - 1;
  headerLines1thru11[recNum] = strm.str().c_str();

  //std::cout << "[         1         2         3         4         5         6         7         8]\n";
  //std::cout << "[12345678901234567890123456789012345678901234567890123456789012345678901234567890]\n";
  //std::cout << "[" << headerLines1thru11[recNum].toStdString() << "]\n";

  return 0;

}

int Uff58bExport::setDataForm( int dataType, int numEle, int xSpacing,
                               double xMin, double xInc, double zAxisVal ) {

  char lf = 10;
  char cr = 13;

  std::stringstream strm;
  strm << std::setw(10) << std::right << dataType <<
    std::setw(10) << std::right << numEle <<
    std::setw(10) << std::right << xSpacing <<
    std::setw(13) << std::setprecision(7) << std::right << xMin <<
    std::setw(13) << std::setprecision(7) << std::right << xInc <<
    std::setw(13) << std::setprecision(7) << std::right << zAxisVal <<
    "         " << lf;

  int recNum = 7 - 1;
  headerLines1thru11[recNum] = strm.str().c_str();
  
  //std::cout << "[         1         2         3         4         5         6         7         8]\n";
  //std::cout << "[12345678901234567890123456789012345678901234567890123456789012345678901234567890]\n";
  //std::cout << "[" << headerLines1thru11[recNum].toStdString() << "]\n";
  
  return 0;

}

int Uff58bExport::setDataCharacteristics( int recNum, int dataType, int lengthUnitsExponent,
                                          int forceUnitsExponent, int tempUnitsExponent,
                                          QString axisLabel, QString axisUnitsLabel ) {

  if ( recNum < 8 ) recNum == 8;
  if ( recNum > 11 ) recNum = 11;

  recNum -= 1;

  if ( axisLabel.isEmpty() ) axisLabel = "NONE";
  if ( axisUnitsLabel.isEmpty() ) axisUnitsLabel = "NONE";
  
  char lf = 10;
  char cr = 13;

  std::stringstream strm;
  strm << std::setw(10) << std::right << dataType <<
    std::setw(5) << std::right << lengthUnitsExponent <<
    std::setw(5) << std::right << forceUnitsExponent <<
    std::setw(5) << std::right << tempUnitsExponent <<
    " " <<
    std::setw(20) << std::left << std::setfill( ' ' ) << axisLabel.toStdString() <<
    " " <<
    std::setw(20) << std::left << std::setfill( ' ' ) << axisUnitsLabel.toStdString() <<
    "           " << lf;

  headerLines1thru11[recNum] = strm.str().c_str();
  
  //std::cout << "[         1         2         3         4         5         6         7         8]\n";
  //std::cout << "[12345678901234567890123456789012345678901234567890123456789012345678901234567890]\n";
  //std::cout << "[" << headerLines1thru11[recNum].toStdString() << "]\n";
  
  return 0;

}

int Uff58bExport::writeSpacer( std::filebuf &fb ) {

  std::string s;
  std::stringstream strm;

  char lf = 10;
  char cr = 13;

  strm << std::setw(6) << std::right << -1 << lf;

  s = strm.str();
  
  int n = writeBinary( fb, s.c_str(), 7 );
  //std::cout << "Uff58bExport::writeSpacer - n = " << n << std::endl;

  return 0;
  
}

int Uff58bExport::writeHeader( std::filebuf &fb ) {

  int n;

  // write spacer: -1 as I6
  n = writeSpacer( fb );
  //std::cout << "1 Uff58bExport::writeHeader - spacer n = " << n << std::endl;

  // write uff58bIdLine
  //int n = fb.sputn( uff58bIdLine.toStdString().c_str(), uff58bIdLine.size() );
  n = writeBinary( fb, uff58bIdLine.toStdString().c_str(), uff58bIdLine.size() );
  //std::cout << "1 Uff58bExport::writeHeader - n = " << n << std::endl;

  for ( int i = 0; i<11; i++ ) {
    //write headerLines1thru11[i]
    int n = writeBinary( fb, headerLines1thru11[i].toStdString().c_str(), headerLines1thru11[i].size() );
    //std::cout << "header " << i << ", n = " << n << std::endl;
  }
  
  return 0;
  
}

int Uff58bExport::writeBinary( std::filebuf &fb, double *buf, unsigned long numBytes ) {

  // write binary file in chunks
  
  int n = fb.sputn( (char *) buf, numBytes );

  return n;

}

int Uff58bExport::writeBinary( std::filebuf &fb, float *buf, unsigned long numBytes ) {

  int n = fb.sputn( (char *) buf, numBytes );

  return n;

}

int Uff58bExport::writeBinary( std::filebuf &fb, char *buf, unsigned long numBytes ) {

  int n = fb.sputn( (char *) buf, numBytes );
  
  return n;

}

int Uff58bExport::writeBinary( std::filebuf &fb, const char *buf, unsigned long numBytes ) {

  int n = fb.sputn( (char *) buf, numBytes );

  return n;

}

bool Uff58bExport::isTranslation( int val ) {

  return ( ( val >= -3 ) && ( val <= 3 ) && ( val != 0 ) );

}

bool Uff58bExport::isRotation( int val ) {

  return ( ( ( val >= -6 ) && ( val <= -4 ) ) || ( ( val >= 4 ) && ( val <= 6 ) ) );

}

void Uff58bExport::getExponents( int dataType, int direction, int &lenExp, int &forceExp, int &tempExp ) {

  if ( isTranslation( direction ) ) {

    try {
      ExponentTplType tpl = translationExponentMap[dataType];
      lenExp = std::get<LENGTH_EXP>( tpl );
      forceExp = std::get<FORCE_EXP>( tpl );
      tempExp = std::get<TEMP_EXP>( tpl );
    }
    catch ( const std::exception& e ) {
      lenExp = forceExp = tempExp = 0;
    }

  }
  else if ( isRotation( direction ) ) {

    try {
      ExponentTplType tpl = rotationExponentMap[dataType];
      lenExp = std::get<LENGTH_EXP>( tpl );
      forceExp = std::get<FORCE_EXP>( tpl );
      tempExp = std::get<TEMP_EXP>( tpl );
    }
    catch ( const std::exception& e ) {
      lenExp = forceExp = tempExp = 0;
    }

  }
  else {

    lenExp = forceExp = tempExp = 0;
    
  }

}
