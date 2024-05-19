//
// Created by jws3 on 5/1/24.
//

#ifndef FILEREFORMATTER_FILECONVERTERFAC_H
#define FILEREFORMATTER_FILECONVERTERFAC_H

#include "memory"

#include <QString>

#include "PsnFileConverter.h"
#include "DspErr.h"

class FileConverterFac {

public:

  static const int NumErrs = 2;
  static const int ESuccess = 0;
  static const int EUnknownType = 1;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "Unknown file type" }
  };
  
  FileConverterFac();
  std::shared_ptr<FileConverter> getFileConverter ( const QString& fileType );
  FileConverter *getFileConverterPtr ( const QString& fileType );

  int mostRecentError {0};
  int errLine {0};
  std::string errFile;
  int errInfo ( int err, int line=0, std::string file="" ) {
    mostRecentError = err;
    errLine = line;
    errFile = file;
    return err;
  }
  void dspErrMsg ( int err ) {
    DspErr::dspErrMsg( errLine, errFile, NumErrs, err, errMsgs );
  }
  void dspErrMsg ( void ) {
    DspErr::dspErrMsg( errLine, errFile, NumErrs, mostRecentError, errMsgs );
  }

};

#endif //FILEREFORMATTER_FILECONVERTERFAC_H
