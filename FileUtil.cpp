//
// Created by jws3 on 5/2/24.
//

#include <sstream>
#include <iomanip>

#include <QFileInfo>
#include <QDir>

#include "FileUtil.h"

FileUtil::FileUtil() {}

bool FileUtil::hasExtension( QString str ) {

  /* Search backwards for a ".". If we find "/" first, then there is no extension.
   * If we find a "." before finding a "/" and the "." is not the first character,
   * then there is an extension.
   */

  // don't include 1st character in test
  for ( int i=str.length()-1; i>0; i-- ) {
    if ( str.at(i).toLatin1() == '/' ) return false;
    if ( str.at(i).toLatin1() == '.' ) return true;
  }

  return false;

}

QString FileUtil::getHeaderFileName (
  const QString& rootDir,
  const QString& name ) {

  /* Combine rootDir and name to get fullPath.
   *
   * If rootDir does not end with "/", append one.
   * If name starts with a "./" or "/", then ignore rootDir.
   * If name does not include an extension, append the default one.
   */

  QString path = "";

  QString rootDir1 = rootDir.trimmed();
  if ( !( rootDir1.endsWith( "/" ) ) ) {
    rootDir1 += "/";
  }

  QString name1 = name.trimmed();
  if ( !hasExtension( name1 ) ) {

    if ( name1.endsWith( "." ) ) {
      name1 += Cnst::HdrExtension.c_str();
    }
    else {
      name1 += ".";
      name1 += Cnst::HdrExtension.c_str();
    }

  }

  if ( name1.startsWith( "./" ) || name1.startsWith( "/" ) ) {
    path = name1;
  }
  else {
    path = rootDir1 + name1;
  }

  return path;

}

QString FileUtil::extractFileName( QString str ) {

  QString tmp = str;

  if ( hasExtension( str ) ) {

    int extensionStart = 0;

    for ( int i=str.length()-1; i>0; i-- ) {
      if ( str.at(i).toLatin1() == '.' ) {
        extensionStart = i;
        break;
      }
    }

    if ( extensionStart > 0 ) {
      tmp = str.mid( 0, extensionStart );
    }

  }

  int dirEnd = 0;
  for ( int i=tmp.length()-1; i>=0; i-- ) {
    if ( tmp.at(i).toLatin1() == '/' ) {
      dirEnd = i;
      break;
    }
  }

  if ( dirEnd && ( dirEnd != tmp.length()-1 ) ) {
    tmp = tmp.mid( dirEnd+1 );
  }

  return tmp;

}

QString FileUtil::extractDir( QString str ) {

  QString tmp = str;

  int dirEnd = -1;
  for ( int i=tmp.length()-1; i>=0; i-- ) {
    if ( tmp.at(i).toLatin1() == '/' ) {
      dirEnd = i;
      break;
    }
  }

  return tmp.left( dirEnd+1 );

}

QString FileUtil::getBinDir( const QString& binRoot, const QString& subDir ) {

  QString binRoot1 = binRoot.trimmed();

  if ( !( binRoot1.endsWith( "/" ) ) ) {
    binRoot1 += "/";
  }
  binRoot1 += subDir.trimmed() + "/";

  return binRoot1;

}

QString FileUtil::makeBinFileName( DataHeader *dh, const QString& hdrName, int sigIndex ) {

  const auto& indexMap(dh->getIndexMap());
  auto it(indexMap.find(sigIndex));
  if(it!=indexMap.end()) {
    QFileInfo binName(QFileInfo(hdrName).dir(), std::get<DataHeader::DATA_FILENAME>(it->second));
    qDebug()<<__func__<<"header"<<hdrName<<"sig"<<sigIndex<<binName.filePath();
    return binName.filePath();

  } else {
    qWarning()<<__func__<<"No meta for signal"<<sigIndex;
    return QString();
  }
}
