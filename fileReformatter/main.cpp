#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <utility>
#include <memory>
#include <sstream>
#include <iomanip>

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

  QString runName, rawDataFileType, hdrFileName;
  int stat;
  bool verbose = false;
  char *run, *hdrFile;

  if ( ( argc == 2 ) && ( ( !strcmp( argv[1], "-v" ) ) || ( !strcmp( argv[1], "--version" ) ) ) ) {
    std::cout << "Version " << version << std::endl;
    return 0;
  }
  else if ( ( argc == 4 ) && ( !strcmp( argv[1], "--verbose" ) ) ) {
    verbose = true;
    run = argv[2];
    hdrFile = argv[3];
  }
  else if ( argc != 3 ) {
    std::cout << "usage: " << argv[0] << "{run name} {header filename}" << std::endl;
    return -1;
  }

  if ( argc == 3 ) {
    run = argv[1];
    hdrFile = argv[2];
  }

  runName = run;

  std::shared_ptr<DataHeader> dh = std::make_shared<DataHeader>();

  hdrFileName = hdrFile;

  QString simpleName = FileUtil::extractFileName( hdrFile );

  QString binFileDir = FileUtil::extractDir( hdrFileName );

  stat = dh->readContents( hdrFileName );
  if ( stat ) {
    std::cout << "Error from readContents - abort" << std::endl;
    return stat;
  }

  int startingChanIndex = 1;
  for ( int i=1; i<=Cnst::MaxChassis; i++ ) {

    QString rawDataFileName = binFileDir + runName + "-Chassis";

    std::stringstream strm;
    strm << rawDataFileName.toStdString() << std::setw(2) << std::setfill( '0' ) << i << "." << Cnst::RawFileExtension;
    rawDataFileName = strm.str().c_str();

    if ( verbose ) {
      std::cout << "Processing " << rawDataFileName.toStdString() << std::endl;
    }

    std::shared_ptr<BinFileType> bft = std::shared_ptr<BinFileType>( new BinFileType() );
    stat = bft->getRawBinFileType( rawDataFileName, rawDataFileType );
    if ( stat ) {
      std::cout << "Error " << stat << " from getRawBinFileType" << std::endl;
      return stat;
    }
    
    FileConverterFac fcf;
    std::shared_ptr<FileConverter> fc = fcf.getFileConverter( rawDataFileType );
    if ( !fc ) {
      std::cout << "Error from getFileConverter - no type found" << std::endl;
    }
    
    //fc->show();
    
    stat = fc->convert ( i, startingChanIndex, dh.get(), rawDataFileName, binFileDir, simpleName, verbose );
    if ( stat ) {
      std::cout << "Error " << stat << " from convert" << std::endl;
      return stat;
    }

    startingChanIndex += Cnst::MaxSignals;

  }

  return 0;

  QCoreApplication a(argc, argv);
  return QCoreApplication::exec();
  
}
