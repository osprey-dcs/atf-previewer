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
  static bool hasExtension( QString str );
  static QString getHeaderFileName (
    const QString& rootDir,
    const QString& name );
  static QString extractFileName( QString str );
  static QString extractDir( QString str );
  static QString getBinDir(
     const QString& binRoot,
     const QString& subDir );
  static QString makeBinFileName( DataHeader *dh, const QString& hdrName, int sigIndex );

};


#endif //FILEREFORMATTER_FILEUTIL_H
