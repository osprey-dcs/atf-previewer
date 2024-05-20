//
// Created by jws3 on 5/1/24.
//

#ifndef FILEREFORMATTER_BINFILETYPE_H
#define FILEREFORMATTER_BINFILETYPE_H

// this class takes a raw binary file name and returns the file type as a QString

#include <memory>
#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cmath>
#include <list>

#include <QString>
#include "DspErr.h"

class BinFileType {

public:

  static const int NumErrs = 3;
  static const int ESuccess = 0;
  static const int EFileOpen = 1;
  static const int EFileRead = 2;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "File open failure" },
    { "File read failure" }
  };

  BinFileType();
  BinFileType( BinFileType&& ) = delete;
  BinFileType( BinFileType& ) = delete;
  BinFileType( const BinFileType& ) = delete;
  BinFileType& operator=( BinFileType&& ) = delete;
  BinFileType& operator=( BinFileType& ) = delete;
  BinFileType& operator=( const BinFileType& ) = delete;
  int getRawBinFileType( const QString& rawBinFileName, QString& rawBinFileType );

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


#endif //FILEREFORMATTER_BINFILETYPE_H
