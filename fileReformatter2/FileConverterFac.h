//
// Created by jws3 on 5/1/24.
//

#ifndef FILEREFORMATTER_FILECONVERTERFAC_H
#define FILEREFORMATTER_FILECONVERTERFAC_H

#include "memory"

#include <QString>

#include "PsnFileConverter.h"

class FileConverterFac {

public:
  FileConverterFac();
  std::shared_ptr<FileConverter> getFileConverter ( const QString& fileType );
  FileConverter *getFileConverterPtr ( const QString& fileType );
};

#endif //FILEREFORMATTER_FILECONVERTERFAC_H
