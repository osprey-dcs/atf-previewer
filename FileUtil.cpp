//
// Created by jws3 on 5/2/24.
//

#include <QDir>
#include <QFileInfo>
#include "FileUtil.h"

FileUtil::FileUtil() {}

QString FileUtil::extractFileName( const QString& str ) {
  // "path/file.ext" -> "file"
  return QFileInfo(str).completeBaseName();
}

QString FileUtil::extractDir( QString str ) {
    // "path/file.ext" -> "path"
  return QFileInfo(str).dir().dirName() + "/";
}

QString FileUtil::makeBinFileName( DataHeader *dh, const QString& hdrName, int sigIndex ) {

  // binary file path is directory of header file + DATA_FILENAME element of tuple retrieved by sigIndex

  QString binDir = FileUtil::extractDir( hdrName );

  DataHeader::DataHeaderIndexMapType indexMap = dh->getIndexMap();
  QString binName = std::get<DataHeader::DATA_FILENAME>( indexMap[sigIndex] );

  QString binFile;
  if ( binName.isEmpty() ) {
    binFile = "";
  }
  else {
    binFile = binDir + binName;
  }

  return binFile;

}
