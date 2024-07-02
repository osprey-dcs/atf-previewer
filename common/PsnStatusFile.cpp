//
// Created by jws3 on 6/15/24.
//

#include <string.h> // for memset
#include <exception>

#include <QString>

#include "PsnStatusFile.h"

PsnStatusFile::PsnStatusFile() {

  memset( (char *) &statusHdr, 0, sizeof(PsnStatusHdrType) );
  memset( (char *) &outStatusHdr, 0, sizeof(PsnStatusHdrType) );
  statusHdr.recSize = sizeof(int) * NumStatusFields;
  outStatusHdr.recSize = sizeof(int) * NumStatusFields;

}

PsnStatusFile::~PsnStatusFile() {

}

int PsnStatusFile::openRead(const std::string &name) {

  try {
    auto result = readFb.open( name, std::ios::in | std::ios::binary );
    if ( !result ) {
      return ERRINFO(EFileOpen,name);
    }
  }
  catch ( const std::exception& e ) {
    QString qmsg = QStringLiteral("file name is %s, %s").arg(name.c_str()).arg(e.what());
    return ERRINFO(EFileOpen,qmsg.toStdString());
  }

  isOpenRead = true;

  return 0;

}

int PsnStatusFile::openWrite(const std::string &name) {

  try {
    auto result = writeFb.open( name, std::ios::in | std::ios::binary );
    if ( !result ) {
      return ERRINFO(EFileOpen,name);
    }
  }
  catch ( const std::exception& e ) {
    QString qmsg = QStringLiteral("file name is %s, %s").arg(name.c_str()).arg(e.what());
    return ERRINFO(EFileOpen,qmsg.toStdString());
  }

  isOpenWrite = true;

  return 0;

}

int PsnStatusFile::closeRead(void) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  readFb.close();
  
  return 0;
  
}

int PsnStatusFile::closeWrite ( void ) {

  if ( !isOpenWrite ) {
    return ENoFile;
  }

  writeFb.close();
  
  return 0;
  
}

int64_t PsnStatusFile::readHeader ( void ) {

  if ( !isOpenRead ) {
    return 0;
  }

  int64_t num;
  
  try {
    readFb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
    num = readFb.sgetn( (char *) &statusHdr, sizeof(PsnStatusHdrType) );
    //std::cout << "num = " << num << std::endl;
  }
  catch ( const std::exception& e ) {
    std::cout << "PsnStatusFile::readHeader - " << e.what() << std::endl;
    return 0;
  }

  /*
  std::cout << "version = ";
  for ( int i=0; i<PsnStatusFile::NumStatusVersion; i++ ) {
    std::cout << statusHdr.version[i] << " ";
  }
  std::cout << std::endl;
  std::cout << "fileType = " << statusHdr.fileType << std::endl;
  std::cout << "cccr = " << statusHdr.cccr << std::endl;
  std::cout << "recSize = " << statusHdr.recSize << std::endl;
  std::cout << "numBytes = " << statusHdr.numBytes << std::endl;
  for ( int i=0; i<PsnStatusFile::NumStatusFields; i++ ) {
    std::cout << std::hex << "0x" << statusHdr.summaryValues[i] << " ";
  }
  std::cout << std::dec << std::endl;
  */

  hdrRead = true;
  
  return num;
  
}

int64_t PsnStatusFile::writeHeader(void) {

  if ( !isOpenWrite ) {
    return 0;
  }

  int64_t num;
  
  try {
    readFb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
    num = writeFb.sputn( (char *) &outStatusHdr, sizeof(PsnStatusHdrType) );
  }
  catch ( const std::exception& e ) {
    std::cout << "PsnStatusFile::writeHeader - " << e.what() << std::endl;
    return 0;
  }

  hdrWritten = true;
  
  return num;

}

void PsnStatusFile::getVersion(int64_t &major, int64_t &minor, int64_t &release) {

  if ( !hdrRead ) {
    major = 0;
    minor = 0;
    release = 0;
  }
  else {
    major = statusHdr.version[0];
    minor = statusHdr.version[1];
    release = statusHdr.version[2];
  }

}

int64_t PsnStatusFile::getRecSize(void) {
  
  if ( !hdrRead ) {
    return 0;
  }
  else {
    return statusHdr.recSize;
  }
  
}

int64_t PsnStatusFile::getNumBytes(void) {

  if ( !hdrRead ) {
    return 0;
  }
  else {
    return statusHdr.numBytes;
  }

}

int64_t PsnStatusFile::getMaxElements(void) {

  if ( !hdrRead ) {
    return 0;
  }
  else {
    return statusHdr.numBytes / sizeof(int) / PsnStatusFile::NumStatusFields;
  }

}

std::string PsnStatusFile::getFileType(void) {

  std::string s = "";
  if ( hdrRead ) {
    std::string s = statusHdr.fileType;
  }

  return s;

}

std::string PsnStatusFile::getCccr(void) {
  
  std::string s = "";
  if ( hdrRead ) {
    std::string s = statusHdr.cccr;
  }

  return s;

}

void PsnStatusFile::getSummaryRecord( int *rec ) {

  for ( int i=0; i<NumStatusFields; i++ ) {
    rec[i] = statusHdr.summaryValues[i];
  }

}

void PsnStatusFile::setVersion(int64_t &major, int64_t &minor, int64_t &release) {

    major = outStatusHdr.version[0];
    minor = outStatusHdr.version[1];
    release = outStatusHdr.version[2];

}
  
void PsnStatusFile::setRecSize ( int64_t recSize ) {

  outStatusHdr.recSize = recSize;

}

void PsnStatusFile::setNumBytes ( int64_t numBytes ) {

  outStatusHdr.numBytes = numBytes;

}

void PsnStatusFile::setFileType ( const std::string& fileType ) {

  strncpy( outStatusHdr.fileType, fileType.c_str(), PsnStatusFile::NumStatusFileType );
  outStatusHdr.fileType[PsnStatusFile::NumStatusFileType] = 0;

}

void PsnStatusFile::setCccr ( const std::string& cccr ) {

  strncpy( outStatusHdr.cccr, cccr.c_str(), PsnStatusFile::NumCccr );
  outStatusHdr.cccr[PsnStatusFile::NumCccr] = 0;

}

void PsnStatusFile::setSummaryRecord( int *rec ) {

  for ( int i=0; i<NumStatusFields; i++ ) {
    rec[i] = outStatusHdr.summaryValues[i];
  }
  
}

int64_t PsnStatusFile::getHeaderSize(void) {

  return sizeof(PsnStatusHdrType);

}

int PsnStatusFile::getDataFullTimeRange(double &minTime, double &maxTime) {
  return 0;
}

int PsnStatusFile::getRecordRangeForTime(double minTime, double maxTime, int64_t &min, int64_t &max) {
  return 0;
}

void PsnStatusFile::inputSeekToStartOfData(int64_t firstDataByte) {

  if ( isOpenRead ) {
    int64_t pos = sizeof(PsnStatusHdrType) + firstDataByte;
    readFb.pubseekoff( pos, std::ios::beg, std::ios::in );
  }

}

void PsnStatusFile::outputSeekToStartOfData(int64_t firstDataByte) {

  if ( isOpenWrite ) {
    int64_t pos = sizeof(PsnStatusHdrType) + firstDataByte;
    writeFb.pubseekoff( pos, std::ios::beg, std::ios::out );
  }

}

void PsnStatusFile::seekToReadOffset(int64_t offset) {

  if ( isOpenRead ) {
    readFb.pubseekoff( offset, std::ios::beg, std::ios::in );
  }

}

int64_t PsnStatusFile::readData(int *buf, int64_t readSizeInbytes) {

  int64_t num;
  
  if ( isOpenRead ) {
    try {
      num = readFb.sgetn( (char *) buf, readSizeInbytes );
      return num;
    }
    catch ( const std::exception& e ) {
      std::cout << "PsnStatusFile::readData - " << e.what() << std::endl;
      return 0;
    }
  }
  
  return 0;

}

void PsnStatusFile::seekToWriteOffset ( int64_t offset ) {

  if ( isOpenWrite ) {
    writeFb.pubseekoff( offset, std::ios::beg, std::ios::out );
  }

}

int64_t PsnStatusFile::writeData ( int *buf, int64_t writeSizeInbytes ) {

  int64_t num;
  
  if ( isOpenWrite ) {
    try {
      num = writeFb.sputn( (char *) buf, writeSizeInbytes );
      return num;
    }
    catch ( const std::exception& e ) {
      std::cout << "PsnStatusFile::writeData - " << e.what() << std::endl;
      return 0;
    }
  }
  
  return 0;

}
