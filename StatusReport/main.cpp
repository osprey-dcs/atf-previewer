#include <getopt.h>
#include <cstring>

#include <iostream>
#include <list>
#include <map>

#include <QCoreApplication>
#include <QDebug>
#include <QString>

#include "StatusFileType.h"
#include "StatusFileFac.h"
#include "FileUtil.h"
#include "StatusDisplay.h"

static const char *version = "0.0.3";

static const int NUMOPTIONS = 5;
static struct option long_options[] = {
  { "version",  no_argument,       0, 0 },
  { "verbose",  no_argument,       0, 0 },
  { "summary",  no_argument,       0, 0 },
  { "showkey", no_argument,        0, 0 },
  { "chan",     required_argument, 0, 0 },
  { nullptr,    0,                 0, 0 }
};

int main ( int argc, char **argv ) {

  int i, options, option_index;

  bool versionSet = false;
  bool verboseSet = false;
  bool summarySet = false;
  bool showkeySet = false;
  bool chanSet = false;
  char *chanArg = nullptr;

  option_index = -1;

  for ( i=0; i<NUMOPTIONS; i++ ) {
    options = getopt_long(argc, argv, "", long_options, &option_index);
    if ( ( options == -1 ) || ( option_index == -1 ) ) {
      break;
    }
    if ( !strcmp( long_options[option_index].name, "chan" ) ) {
      chanSet = true;
      if ( optarg ) {
        chanArg = strdup( optarg );
      }
      else {
        chanArg = strdup( "unknown" );
      }
    }
    else if ( !strcmp( long_options[option_index].name, "summary" ) ) {
      summarySet = true;
    }
    else if ( !strcmp( long_options[option_index].name, "showkey" ) ) {
      showkeySet = true;
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

  if ( ( argc < ( optind + 1 ) || ( argc > ( optind + 1 ) ) ) ) {
    std::cout << "Usage: " << argv[0] <<
    " [--summary] [--showkey] [--verbose] [--chan # (1-32)] chassisFileName" << std::endl;
    std::cout << "       " << argv[0] <<
    " --version" << std::endl;
    return -1;
  }

  i = optind;
  char *fileName = strdup( argv[ i++ ] );

  QString statusFile( fileName );
  QString chan( chanArg );
  int chanNum;
  
  if ( chanSet ) {
    
    if ( !chan.toInt() ) {
      std::cout << "Illegal chan argument\n";
      return -1;
    }
    chanNum = chan.toInt();

    if ( ( chanNum < 1 ) || ( chanNum > 32 ) ) {
      std::cout << "Illegal chan argument\n";
      return -1;
    }

  }

  if ( verboseSet && chanSet ) std::cout << "chanNum = " << chanNum << std::endl;

  if ( verboseSet ) {
    std::cout << "input status file = " << statusFile.toStdString() << std::endl;
  }

  int st;
  StatusDisplay sdsp;
  std::string statusLabels;

  if ( showkeySet ) {
    sdsp.displayKey();
  }
  else {
    std::cout << std::endl;
  }
  
  QString statusFileType;
  std::shared_ptr<StatusFileType> sft = std::shared_ptr<StatusFileType>( new StatusFileType() );
  st = sft->getStatusFileType( statusFile, statusFileType );
  if ( st ) {
    sft->dspErrMsg( st );
    return st;
  }
  if ( verboseSet ) std::cout << "statusFileType = " << statusFileType.toStdString() << std::endl;

  StatusFileFac sff;
  std::shared_ptr<StatusFileBase> sf = sff.createStatusFile( statusFileType.toStdString() );
  if ( !sf ) {
    std::cout << "createStatusFile failed" << std::endl;
    return -1;
  }

  st = sf->openRead( statusFile.toStdString() );
  if ( st ) {
    sf->dspErrMsg( st );
    return st;
  }

  st = sf->readHeader();
  if ( st ) {
    sf->dspErrMsg( st );
    return st;
  }

  int64_t major, minor, release;
  
  int64_t recSize = sf->getRecSize();
  int64_t numEle = sf->getMaxElements();
  int buf[PsnStatusFile::NumStatusFields];
  int prev[PsnStatusFile::NumStatusFields];

  if ( statusFileType == "PSN Status" ) {

    sf->getVersion( major, minor, release );
    if ( major < 2 ) {
      std::cout << "Status file version " << major << "." << minor << "." << release <<
        " is not supported" << std::endl;
      return -1;
    }

    if ( summarySet ) {

      sf->getSummaryRecord( buf );

      std::cout << std::setfill(' ') << std::setw(18) << std::right << "status" << "    ";
      std::cout << std::setfill(' ') << std::setw(10) << std::right << "LOLO" << "   ";
      std::cout << std::setfill(' ') << std::setw(10) << std::right << "LO" << "   ";
      std::cout << std::setfill(' ') << std::setw(10) << std::right << "HI" << "   ";
      std::cout << std::setfill(' ') << std::setw(10) << std::right << "HIHI" << "   ";
      std::cout << std::dec << std::endl;

      statusLabels = sdsp.getLabels( buf[PsnStatusFile::STATUS] );
      std::cout << "[" << std::setw(15) << std::right << statusLabels << "]    ";

      std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::LOLO] << "   ";
      std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::LO] << "   ";
      std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::HI] << "   ";
      std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::HIHI] << "   ";
      std::cout << std::dec << std::endl;

      return 0;
        
    }

    bool needFirstDisplay = true;
    double time, rcvTime;
    int64_t n;
    sf->inputSeekToStartOfData( 0 );
    n = sf->readData( buf, PsnStatusFile::NumStatusFields*sizeof(int) );
    double baseTime = (double) buf[PsnStatusFile::SECS] + ( (double) buf[PsnStatusFile::NANOSECS] ) / 1.0e9;
    double baseRcvTime = (double) buf[PsnStatusFile::RCVSECS] + ( (double) buf[PsnStatusFile::RCVNANOSECS] ) / 1.0e9;

    prev[PsnStatusFile::STATUS] = 0;
    prev[PsnStatusFile::LOLO]   = 0;
    prev[PsnStatusFile::LO]     = 0;
    prev[PsnStatusFile::HI]     = 0;
    prev[PsnStatusFile::HIHI]   = 0;
    
    if ( chanSet ) std::cout << "       channel " << chanNum << std::endl;
            
    std::cout << std::setfill(' ') << std::setw(10) << std::right << "rec" << "  ";
    std::cout << std::setfill(' ') << std::setw(15) << std::right << "time" << " ";
    std::cout << std::setfill(' ') << std::setw(15) << std::right << "rcv time" << "   ";
    std::cout << std::setfill(' ') << std::setw(18) << std::right << "status" << "    ";
    std::cout << std::setfill(' ') << std::setw(10) << std::right << "LOLO" << "   ";
    std::cout << std::setfill(' ') << std::setw(10) << std::right << "LO" << "   ";
    std::cout << std::setfill(' ') << std::setw(10) << std::right << "HI" << "   ";
    std::cout << std::setfill(' ') << std::setw(10) << std::right << "HIHI" << "   ";
    std::cout << std::dec << std::endl;
    
    sf->inputSeekToStartOfData( 0 );
    
    for ( int i=0; i<numEle; i++ ) {
      
      n = sf->readData( buf, PsnStatusFile::NumStatusFields*sizeof(int) );

      if ( n == ( PsnStatusFile::NumStatusFields*sizeof(int) ) ) {

        if ( ( buf[PsnStatusFile::STATUS] != prev[PsnStatusFile::STATUS] ) ||
             ( buf[PsnStatusFile::LOLO] != prev[PsnStatusFile::LOLO] ) ||
             ( buf[PsnStatusFile::LO] != prev[PsnStatusFile::LO] ) ||
             ( buf[PsnStatusFile::HI] != prev[PsnStatusFile::HI] ) ||
             ( buf[PsnStatusFile::HIHI] != prev[PsnStatusFile::HIHI] ) ||
             needFirstDisplay ) {

          needFirstDisplay = false;

          prev[PsnStatusFile::STATUS] = buf[PsnStatusFile::STATUS];
          prev[PsnStatusFile::LOLO]   = buf[PsnStatusFile::LOLO];
          prev[PsnStatusFile::LO]     = buf[PsnStatusFile::LO];
          prev[PsnStatusFile::HI]     = buf[PsnStatusFile::HI];
          prev[PsnStatusFile::HIHI]   = buf[PsnStatusFile::HIHI];

          time = (double) buf[PsnStatusFile::SECS] + ( (double) buf[PsnStatusFile::NANOSECS] ) / 1.0e9;
          time -= baseTime;
          rcvTime = (double) buf[PsnStatusFile::RCVSECS] + ( (double) buf[PsnStatusFile::RCVNANOSECS] ) / 1.0e9;
          rcvTime -= baseRcvTime;

          if ( chanSet ) {
            
            uint lolo = ( buf[PsnStatusFile::LOLO] & ( (uint) 1 << (chanNum-1) ) ) > 0;
            uint lo = ( buf[PsnStatusFile::LO] & ( (uint) 1 << (chanNum-1) ) ) > 0;
            uint hi = ( buf[PsnStatusFile::HI] & ( (uint) 1 << (chanNum-1) ) ) > 0;
            uint hihi = ( buf[PsnStatusFile::HIHI] & ( (uint) 1 << (chanNum-1) ) ) > 0;

            std::cout << std::setfill(' ') << std::setw(10) << std::right << i << ": ";
            std::cout << std::dec << std::setw(15) << std::setprecision(5) << std::right << std::fixed << time << " ";
            std::cout << std::dec << std::setw(15) << std::setprecision(5) << std::right << rcvTime << "   " <<
              std::dec << std::setfill(' ');

            statusLabels = sdsp.getLabels( buf[PsnStatusFile::STATUS] );
            std::cout << "[" << std::setw(15) << std::right << statusLabels << "]    ";

            //std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::STATUS] <<
            //  std::dec << std::setfill(' ') <<"   ";
            std::cout << "  " << std::setw(8) << std::right << lolo << "   ";
            std::cout << "  " << std::setw(8) << std::right << lo << "   ";
            std::cout << "  " << std::setw(8) << std::right << hi << "   ";
            std::cout << "  " << std::setw(8) << std::right << hihi << "   ";
            std::cout << std::dec << std::endl;

          }
          else {
            
            uint lolo = buf[PsnStatusFile::LOLO];
            uint lo = buf[PsnStatusFile::LO];
            uint hi = buf[PsnStatusFile::HI];
            uint hihi = buf[PsnStatusFile::HIHI];

            std::cout << std::setfill(' ') << std::setw(10) << std::right << i << ": ";
            std::cout << std::dec << std::setw(15) << std::setprecision(5) << std::right << std::fixed << time << " ";
            std::cout << std::dec << std::setw(15) << std::setprecision(5) << std::right << rcvTime << "   " <<
              std::dec << std::setfill(' ');

            statusLabels = sdsp.getLabels( buf[PsnStatusFile::STATUS] );
            std::cout << "[" << std::setw(15) << std::right << statusLabels << "]    ";

            //std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::STATUS] << "   ";
            std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::LOLO] << "   ";
            std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::LO] << "   ";
            std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::HI] << "   ";
            std::cout << std::hex << "0x" << std::setfill('0') << std::setw(8) << std::right << buf[PsnStatusFile::HIHI] << "   ";
            std::cout << std::dec << std::endl;

          }

        }

      }

    }

  }

  st = sf->closeRead();
  if ( st ) {
    sf->dspErrMsg( st );
    return st;
  }

  return 0;

  QCoreApplication a(argc, argv);
  return QCoreApplication::exec();

}
