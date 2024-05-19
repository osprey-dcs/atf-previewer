#include <getopt.h>
#include <string.h>

#include <iostream>
#include <list>
#include <map>

#include <QCoreApplication>
#include <QDebug>
#include <QString>

#include "DataHeader.h"
#include "BinFileType.h"
#include "FileConverterFac.h"
#include "FileUtil.h"
#include "Cnst.h"
#include "DspErr.h"

static const char *version = "0.0.3";

static struct option long_options[] = {
  { "version", no_argument,       0, 0 },
  { "verbose", no_argument,       0, 0 },
  { "chassis", required_argument, 0, 0 },
  { "output", required_argument,  0, 0 },
  { 0,         0,                 0, 0 }
};

int main ( int argc, char **argv ) {

  int options;
  int option_index;

  bool chassisSet = false;
  char *chassisArg = nullptr;
  bool outputSet = false;
  char *outputArg = nullptr;
  bool verboseSet = false;
  bool versionSet = false;

  for ( int i=0; i<4; i++ ) {
    options = getopt_long(argc, argv, "", long_options, &option_index);
    if (options == -1) {
      break;
    }
    if (long_options[option_index].name == "chassis") {
      chassisSet = true;
      if ( optarg ) {
        chassisArg = strdup( optarg );
      }
      else {
        chassisArg = strdup( "unknown" );
      }
    } else if (long_options[option_index].name == "output") {
      outputSet = true;
      if ( optarg ) {
        outputArg = strdup( optarg );
      }
      else {
        outputArg = strdup( "unknown" );
      }
    }
    else if (long_options[option_index].name == "verbose") {
      verboseSet = true;
    }
    else if (long_options[option_index].name == "version") {
      versionSet = true;
    }
  }

  if ( versionSet ) {
    std::cout << "Version " << version << std::endl;
    return 0;
  }

  if ( !chassisSet || !outputSet || ( argc < ( optind + 2 ) || ( argc > ( optind + 2 ) ) ) ) {
    std::cout << "Usage: " << argv[0] <<
    " --chassis # (1-32) --output {json output hdr file} {json config file} {input data file}" << std::endl;
    return -1;
  }

  int i = optind;
  char *config = strdup( argv[ i++ ] );
  char *dataFile = strdup( argv[ i++ ] );

  QString inputRawDataFile( dataFile );
  QString configJsonFile( config );
  QString chassis( chassisArg );
  if ( !chassis.toInt() ) {
    std::cout << "Illegal chassis argument\n";
    return -1;
  }
  int chassisNum = chassis.toInt();
  QString outputJsonFile( outputArg );
  QString simpleName = FileUtil::extractFileName( outputJsonFile );
  QString outFileDir = FileUtil::extractDir( outputJsonFile );
  QString outBinFileRoot = outFileDir + simpleName;

  if ( verboseSet ) {
    std::cout << "         output hdr file = " << outputJsonFile.toStdString() << std::endl;
    std::cout << "          input cfg file = " << configJsonFile.toStdString() << std::endl;
    std::cout << "  input binary data file = " << inputRawDataFile.toStdString() << std::endl;
    std::cout << "output dir and file root = " << outBinFileRoot.toStdString() << std::endl;
  }

  std::shared_ptr<DataHeader> dh = std::make_shared<DataHeader>();

  int st;
  QString inputRawDataFileType;
  std::shared_ptr<BinFileType> bft = std::shared_ptr<BinFileType>( new BinFileType() );
  st = bft->getRawBinFileType( inputRawDataFile, inputRawDataFileType );
  if ( st ) {
    bft->dspErrMsg( st );
    return st;
  }
  if ( verboseSet ) std::cout << "inputRawDataFileType = " << inputRawDataFileType.toStdString() << std::endl;

  FileConverterFac fcf;
  std::shared_ptr<FileConverter> fc = fcf.getFileConverter( inputRawDataFileType );
  if ( !fc ) {
    fcf.dspErrMsg();
    return fcf.mostRecentError;
  }

  std::list<int> chanList;
  st = fc->getRawBinFileChanList( inputRawDataFile, chanList );
  if ( st ) {
    fc->dspErrMsg( st );
    return st;
  }

  if ( verboseSet ) {
    std::cout << "channels: ";
    for ( int ival : chanList ) {
      std::cout << ival << " ";
    }
    std::cout << std::endl;
  }

  if ( verboseSet ) std::cout << "fileMap:" << std::endl;
  std::map<int,QString> fileMap;
  for ( int ch : chanList ) {
    int sigIndex = ( chassisNum - 1 ) * 32 + ch;
    QString fname = fc->buildOutputFileName( sigIndex, outFileDir, simpleName );
    fileMap[sigIndex] = fname;
    if ( verboseSet ) std::cout << "fileMap[" << sigIndex << "] = " << fname.toStdString() << std::endl;
  }
  
  int startingChanIndex = ( chassisNum - 1 ) * 32 + 1;
  st = fc->convert ( chassisNum, chanList, startingChanIndex, dh.get(), inputRawDataFile, outFileDir,
                     simpleName, verboseSet );
  if ( st ) {
    fc->dspErrMsg( st );
    return st;
  }

  st = dh->writeNewHeaderFile( chassisNum, fileMap,
                               configJsonFile, outputJsonFile, verboseSet );
  if ( st ) {
    dh->dspErrMsg( st );
    return st;
  }

  return 0;


  QCoreApplication a(argc, argv);
  qDebug() << "Hello World";
  return QCoreApplication::exec();

}
