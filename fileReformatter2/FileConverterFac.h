//
// Created by jws3 on 5/1/24.
//

#ifndef FILEREFORMATTER_FILECONVERTERFAC_H
#define FILEREFORMATTER_FILECONVERTERFAC_H

#include "memory"

#include <QString>

#include "PsnFileConverter.h"
#include "ErrHndlr.h"

class FileConverterFac : public ErrHndlr {

public:

  static const int NumErrs = 2;
  static const int ESuccess = 0;
  static const int EUnknownType = 1;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "Unknown file type: " }
  };
  
  FileConverterFac();
  std::shared_ptr<FileConverter> getFileConverter ( const QString& fileType );
  FileConverter *getFileConverterPtr ( const QString& fileType );

};

#endif //FILEREFORMATTER_FILECONVERTERFAC_H
