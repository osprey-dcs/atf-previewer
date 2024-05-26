//
// Created by jws3 on 5/1/24.
//

#include "BinFileType.h"

BinFileType::BinFileType() : ErrHndlr ( BinFileType::NumErrs, BinFileType::errMsgs ) {
}

// : ErrHndlr ( ?::NumErrs, ?::errMsgs ) {

int BinFileType::getRawBinFileType( const QString& rawBinFileName, QString& rawBinFileType ) {

  char buf[3];
  std::filebuf fb;

  auto result = fb.open( rawBinFileName.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return ERRINFO(EFileOpen,rawBinFileName.toStdString());
  }

  // read file type
  fb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
  
  int num = fb.sgetn( (char *) buf, sizeof(buf) );
  if ( num < sizeof(buf) ) {
    return ERRINFO(EFileRead,rawBinFileName.toStdString());
  }

  fb.close();

  rawBinFileType = buf[0];
  rawBinFileType += buf[1];
  rawBinFileType += buf[2];

  return ESuccess;

}
