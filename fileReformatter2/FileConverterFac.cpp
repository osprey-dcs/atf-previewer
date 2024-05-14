//
// Created by jws3 on 5/1/24.
//
#include "FileConverterFac.h"

FileConverterFac::FileConverterFac () {
}

std::shared_ptr<FileConverter> FileConverterFac::getFileConverter ( const QString& fileType ) {

  if (fileType == "PSN") {
    return std::shared_ptr<FileConverter>(new PsnFileConverter());
  } else {
    return nullptr;
  }

}

FileConverter *FileConverterFac::getFileConverterPtr ( const QString& fileType ) {

  if ( fileType == "PSN" ) {
    return  new PsnFileConverter();
  }
  else {
    return nullptr;
  }

}
