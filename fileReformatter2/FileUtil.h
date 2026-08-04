//
// Created by jws3 on 5/2/24.
//

#ifndef FILEREFORMATTER_FILEUTIL_H
#define FILEREFORMATTER_FILEUTIL_H

#include <QString>
#include "Cnst.h"
#include "DataHeader.h"

class FileUtil {

public:
  FileUtil();
  static QString extractFileName( const QString& str );
  static QString extractDir( QString str );

};


#endif //FILEREFORMATTER_FILEUTIL_H
