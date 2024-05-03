#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <utility>
#include <memory>

#include <QCoreApplication>
#include <QDebug>
#include <QString>

#include "DataHeader.h"
#include "BinFileType.h"
#include "FileConverterFac.h"
#include "FileUtil.h"
#include "Cnst.h"

#include <iomanip>
#include <byteswap.h>
#include <errno.h>
#include <string.h>

static const char* version = "0.0.1";

int main( int argc, char **argv ) {

  QString rawDataFileName, rawDataFileType, hdrFileName;
  int stat;

  if ( ( argc == 2 ) && ( ( !strcmp( argv[1], "-v" ) ) || ( !strcmp( argv[1], "--version" ) ) ) ) {
    std::cout << "Version " << version << std::endl;
    return 0;
  }
  else if ( argc != 3 ) {
    std::cout << "usage: " << argv[0] << "{raw data filename} {header filename}" << std::endl;
    return -1;
  }

  rawDataFileName = argv[2];

  std::shared_ptr<DataHeader> dh = std::make_shared<DataHeader>();

  QString dir = QString( Cnst::HdrRoot.c_str() );
  hdrFileName = FileUtil::getHeaderFileName( dir, argv[1] );

  QString simpleName = FileUtil::extractFileName( argv[1] );

  stat = dh->readContents( hdrFileName );
  if ( stat ) {
    std::cout << "Error from readContents - abort" << std::endl;
    return stat;
  }

  QString dateAsFilePath;
  stat = dh->getString( "AcquisitionStartDate2", dateAsFilePath );
  if ( stat ) {
    std::cout << "Error from getString - abort" << std::endl;
    return stat;
  }

  QString binFileDir = FileUtil::getBinDir( Cnst::BinRoot.c_str(), dateAsFilePath );

  std::shared_ptr<BinFileType> bft = std::shared_ptr<BinFileType>( new BinFileType() );
  stat = bft->getRawBinFileType( rawDataFileName, rawDataFileType );

  FileConverterFac fcf;
  std::shared_ptr<FileConverter> fc = fcf.getFileConverter( rawDataFileType );

  //fc->show();

  stat = fc->convert ( dh.get(), rawDataFileName, binFileDir, simpleName );
  if ( stat ) {
    std::cout << "Error " << stat << " from convert" << std::endl;
    return stat;
  }

  return 0;

  QCoreApplication a(argc, argv);
  qDebug() << "Hello World";
  return QCoreApplication::exec();
  
}
