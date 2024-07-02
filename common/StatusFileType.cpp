//
// Created by jws3 on 5/1/24.
//

#include "StatusFileType.h"
#include <exception>

StatusFileType::StatusFileType() : ErrHndlr (StatusFileType::NumErrs, StatusFileType::errMsgs ) {
}

int StatusFileType::getStatusFileType(const QString& statusFileName, QString& statusFileType ) {

  char buf[15+1];
  std::filebuf fb;

  try {
    auto result = fb.open( statusFileName.toStdString(), std::ios::in | std::ios::binary );
    if ( !result ) {
      return ERRINFO(EFileOpen,statusFileName.toStdString());
    }
  }
  catch ( const std::exception& e ) {
    QString qmsg = QString("file name is %s, %s").arg(statusFileName).arg(e.what());
    return ERRINFO(EFileOpen,qmsg.toStdString());
  }

  try {
    
    // read file type - version is 12 bytes the 16 bytes of file type
    fb.pubseekoff( 12ul, std::ios::beg, std::ios::in );
  
    int num = fb.sgetn( (char *) buf, 16 );
    if ( num < sizeof(buf) ) {
      return ERRINFO(EFileRead,statusFileName.toStdString());
    }
    buf[15] = 0;

  }
  catch ( const std::exception& e ) {
    QString qmsg = QStringLiteral("file name is %1, %2").arg(statusFileName).arg(e.what());
    return ERRINFO(EFileRead,qmsg.toStdString());
  }

  fb.close();

  statusFileType = buf;

  return ESuccess;

}
