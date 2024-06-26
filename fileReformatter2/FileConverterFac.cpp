//
// Created by jws3 on 5/1/24.
//
#include "FileConverterFac.h"

FileConverterFac::FileConverterFac () : ErrHndlr ( FileConverterFac::NumErrs, FileConverterFac::errMsgs ) {
}

std::shared_ptr<FileConverter> FileConverterFac::getFileConverter ( const QString& fileType ) {

  if (fileType == "PSN") {
    return std::shared_ptr<FileConverter>(new PsnFileConverter());
  }
  else {
    int dummy = ERRINFO(1,fileType.toStdString());
    return nullptr;
  }

}

FileConverter *FileConverterFac::getFileConverterPtr ( const QString& fileType ) {

  if ( fileType == "PSN" ) {
    return  new PsnFileConverter();
  }
  else {
    int dummy = ERRINFO(1,fileType.toStdString());
    return nullptr;
  }

}
