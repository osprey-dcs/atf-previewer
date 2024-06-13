#include <getopt.h>
#include <cstring>

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

static const char *version = "0.0.1";

static struct option long_options[] = {
  { "version", no_argument,       0, 0 },
  { "verbose", no_argument,       0, 0 },
  { "summary", no_argument,       0, 0 },
  { "chassis", required_argument, 0, 0 },
  { nullptr,  0,                  0, 0 }
};

int main ( int argc, char **argv ) {

  int i, options, option_index;

  bool versionSet = false;
  bool verboseSet = false;
  bool summarySet = false;
  bool chassisSet = false;
  char *chassisArg = nullptr;

  option_index = -1;

  for ( i=0; i<3; i++ ) {
    options = getopt_long(argc, argv, "", long_options, &option_index);
    if ( ( options == -1 ) || ( option_index == -1 ) ) {
      break;
    }
    if ( !strcmp( long_options[option_index].name, "chassis" ) ) {
      chassisSet = true;
      if ( optarg ) {
        chassisArg = strdup( optarg );
      }
      else {
        chassisArg = strdup( "unknown" );
      }
    } else if ( !strcmp( long_options[option_index].name, "summary" ) ) {
      summarySet = true;
    }
    else if ( !strcmp( long_options[option_index].name, "verbose" ) ) {
      verboseSet = true;
    }
    else if ( !strcmp( long_options[option_index].name, "version" ) ) {
      versionSet = true;
    }
    else {
      std::cout << "abort\n";
      return -1;
    }
  }

  if ( versionSet ) {
    std::cout << "Version " << version << std::endl;
    return 0;
  }

  if ( !chassisSet || ( argc < ( optind + 1 ) || ( argc > ( optind + 1 ) ) ) ) {
    std::cout << "Usage: " << argv[0] <<
    " --chassis # (1-32) [--summary] [--verbose]" << std::endl;
    std::cout << "       " << argv[0] <<
    " --version" << std::endl;
    return -1;
  }

  i = optind;
  char *dataFile = strdup( argv[ i++ ] );

  std::cout << "dataFile = " << dataFile << std::endl;

  return 0;

  QString inputRawDataFile( dataFile );
  QString chassis( chassisArg );
  if ( !chassis.toInt() ) {
    std::cout << "Illegal chassis argument\n";
    return -1;
  }
  int chassisNum = chassis.toInt();
  QString outputJsonFile( "file.1" );
  QString simpleName = FileUtil::extractFileName( outputJsonFile );
  QString outFileDir = FileUtil::extractDir( outputJsonFile );
  QString outBinFileRoot = simpleName;

  if ( verboseSet ) {
    std::cout << "         output hdr file = " << outputJsonFile.toStdString() << std::endl;
    std::cout << "  input binary data file = " << inputRawDataFile.toStdString() << std::endl;
    std::cout << "output dir and file root = " << outBinFileRoot.toStdString() << std::endl;
  }

  int st;

  std::shared_ptr<DataHeader> dh = std::make_shared<DataHeader>();
  dh = nullptr;

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
    // no outfiledir for the file name that gets written to the new header file
    QString fname = fc->buildOutputFileName( sigIndex, "", simpleName );
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

  return 0;

  QCoreApplication a(argc, argv);
  return QCoreApplication::exec();

}
