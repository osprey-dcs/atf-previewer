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
#include <fstream>
#include <cmath>
#include <list>

#include <QString>


class BinFileType {

public:
  BinFileType();
  int getRawBinFileType( const QString& rawBinFileName, QString& rawBinFileType );

};


#endif //FILEREFORMATTER_BINFILETYPE_H
