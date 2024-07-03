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
#include "ErrHndlr.h"

class BinFileType : public ErrHndlr {

public:

  static const int NumErrs = 3;
  static const int ESuccess = 0;
  static const int EFileOpen = 1;
  static const int EFileRead = 2;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "File open failure: " },
    { "File read failure: " }
  };

  BinFileType();
  BinFileType( BinFileType&& ) = delete;
  BinFileType( BinFileType& ) = delete;
  BinFileType( const BinFileType& ) = delete;
  BinFileType& operator=( BinFileType&& ) = delete;
  BinFileType& operator=( BinFileType& ) = delete;
  BinFileType& operator=( const BinFileType& ) = delete;
  int getRawBinFileType( const QString& rawBinFileName, QString& rawBinFileType );

};


#endif //FILEREFORMATTER_BINFILETYPE_H
