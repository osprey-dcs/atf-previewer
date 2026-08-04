//
// Created by jws3 on 5/2/24.
//

#include "FileUtil.h"

#include <QDir>
#include <QFileInfo>

FileUtil::FileUtil() = default;

QString FileUtil::extractFileName( const QString& str ) {
  // "path/file.ext" -> "file"
  return QFileInfo(str).completeBaseName();
}

QString FileUtil::extractDir( QString str ) {
  // "path/file.ext" -> "path"
  return QFileInfo(str).dir().dirName() + "/";
}
