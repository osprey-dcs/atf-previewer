//
// Created by jws3 on 5/1/24.
//

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <iomanip>
#include <byteswap.h>

#include "PsnFileConverter.h"

PsnFileConverter::PsnFileConverter ( ) {}

int PsnFileConverter::convert ( int chassisIndex, std::list<int>& chanList, int startingSigIndex,
                                const DataHeader *dh, const QString &rawDataFile, const QString& binDataFileDir,
                                const QString& simpleName, bool verbose ) {

  // first cut will be single threaded

  // Open rawDataFile

  // Read raw data header.
  // Use signal bit field to determine how many signals are present.
  // Construct a vector of signal numbers.

  // Open all binary signal files (up to 32). Construct a vector
  // of binary data file handles.

  // N will be set to the number of signals.

  // Allocate a raw data file buffer that will accommodate some number, MAXRECS, records of data
  // and N binary signal file buffers of MAXRECS entries for moving data into for writing the binary
  // signal files.

  //sizeOfOneFile = 0;

  unsigned int buf3BytesPerWord[Cnst::Max3PerWord];
  unsigned int buf4BytesPerWord[Cnst::MaxSignals][Cnst::Max4PerWord];
  unsigned int statusArray[Cnst::MaxStatus][PsnFileConverter::NumStatusFields];
  unsigned int numBytesRead;
  unsigned long dataLen, loc = 0;
  int isignal = 0;
  int ivalue = 0;
  int istatus = 0;
  bool complete = false;
  std::filebuf fb;
  int stat;
  QString headerType;
  firstSeqNum = true;

  auto result = fb.open( rawDataFile.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    return ERRINFO( 1 );
  }

  stat = createAndOpenOutputFiles( chanList, startingSigIndex, binDataFileDir, simpleName, verbose );
  if ( stat ) {
    fb.close();
    return ERRINFO( stat );
  }

  stat = createAndOpenStatusOutputFile( chassisIndex, binDataFileDir, simpleName, verbose );
  if ( stat ) {
    fb.close();
    dspErrMsg( stat );
    return ERRINFO( stat );
  }
  
  unsigned int bodyLen;
  unsigned int status;
  unsigned int chanMask;
  unsigned int seqHigh;
  unsigned int seqLow;
  unsigned int seconds;
  unsigned int nanoseconds;
  unsigned int rcvSeconds;
  unsigned int rcvNanoseconds;
  int lo;
  int lolo;
  int hi;
  int hihi;
  unsigned long seq;

  do {

    stat = readBinHeader( fb, loc, numBytesRead, headerType, complete );
    if ( complete ) {
      if ( ivalue ) { // there is unwritten data
        int st = writeOutputFiles( chanList, ivalue, buf4BytesPerWord );
        if ( st ) {
          dspErrMsg( st );
          return ERRINFO(st);
        }
      }
      if ( istatus ) { // there is unwritten data
        int st = writeStatusOutputFile( istatus, statusArray );
        if ( st ) {
          dspErrMsg( st );
          return ERRINFO(st);
        }
      }
      fb.close();
      closeOutputFiles( chanList );
      closeStatusOutputFile();
      return ESuccess;
    }
    if ( stat ) {
      fb.close();
      closeOutputFiles( chanList );
      closeStatusOutputFile();
      dspErrMsg( stat );
      return ERRINFO( stat );
    }

    bool skip = false;

    //std::cout << "headerType = " << headerType.toStdString() << std::endl;

    if ( headerType == "PSNA" ) {
      
      bodyLen = bswap_32( binHeaderPsna.bodyLen );
      status = bswap_32( binHeaderPsna.status );
      chanMask = bswap_32( binHeaderPsna.chanMask );
      seqHigh = bswap_32( binHeaderPsna.seqHigh );
      seqLow = bswap_32( binHeaderPsna.seqLow );
      seq = seqHigh;
      seq = ( seq << 32 ) | seqLow;
      seconds = bswap_32( binHeaderPsna.seconds );
      nanoseconds = bswap_32( binHeaderPsna.nanoseconds );
      rcvSeconds = 0;
      rcvNanoseconds = 0;
      lo = 0;
      lolo = 0;
      hi = 0;
      hihi = 0;

      if ( !firstSeqNum ) {
        if ( seq != prevSeqNum+1 ) {
          return ERRINFO(ESequence);
        }
        prevSeqNum = seq;
      }
      else {
        firstSeqNum = false;
        prevSeqNum = seq;
      }

      loc += sizeof(BinHdrPsnaType);
      dataLen = bodyLen - 24;

    }
    else if ( headerType == "PSNB" ) {

      bodyLen = bswap_32( binHeaderPsnb.bodyLen );
      rcvSeconds = bswap_32( binHeaderPsnb.rcvSeconds );
      rcvNanoseconds = bswap_32( binHeaderPsnb.rcvNanoseconds );
      status = bswap_32( binHeaderPsnb.status );
      chanMask = bswap_32( binHeaderPsnb.chanMask );
      seqHigh = bswap_32( binHeaderPsnb.seqHigh );
      seqLow = bswap_32( binHeaderPsnb.seqLow );
      seq = seqHigh;
      seq = ( seq << 32 ) | seqLow;

      if ( !firstSeqNum ) {
        if ( seq != prevSeqNum+1 ) {
          return ERRINFO(ESequence);
        }
        prevSeqNum = seq;
      }
      else {
        firstSeqNum = false;
        prevSeqNum = seq;
      }

      seconds = bswap_32( binHeaderPsnb.seconds );
      nanoseconds = bswap_32( binHeaderPsnb.nanoseconds );
      lo = bswap_32( binHeaderPsnb.lo );
      lolo = bswap_32( binHeaderPsnb.lolo );
      hi = bswap_32( binHeaderPsnb.hi );
      hihi = bswap_32( binHeaderPsnb.hihi );

      loc += sizeof(BinHdrPsnbType);

      dataLen = bodyLen - 40;
      
    }
    else { // for this to work, body len of any unknown record type must always exclude 24 bytes of the header
      
      bodyLen = bswap_32( binHeaderGeneric.bodyLen );
      rcvSeconds = 0;
      rcvNanoseconds = 0;
      status = 0;
      chanMask = 0;
      seconds = 0;
      nanoseconds = 0;
      lo = 0;
      lolo = 0;
      hi = 0;
      hihi = 0;

      loc += sizeof(BinHdrGenericType);
      dataLen = bodyLen - 24;
      skip = true;

    }

    if ( !skip ) {

      if ( dataLen >= Cnst::Max3PerWord ) {
        std::cout << "unexpected dataLen value: " << dataLen << std::endl;
        return ERRINFO(EInternal);
      }
    
      stat = readBinData( fb, loc, dataLen, buf3BytesPerWord, numBytesRead, complete );
      if ( complete ) {
        if ( ivalue ) {  // unwritten data exists
          int st = writeOutputFiles( chanList, ivalue, buf4BytesPerWord );
          if ( st ) {
            dspErrMsg( st );
            return ERRINFO(st);
          }
        }
        if ( istatus ) { // there is unwritten data
          int st = writeStatusOutputFile( istatus, statusArray );
          if ( st ) {
            dspErrMsg( st );
            return ERRINFO(st);
          }
        }
        fb.close();
        closeOutputFiles( chanList );
        closeStatusOutputFile();
        return ESuccess;
      }
      if ( stat < 0 ) {
        fb.close();
        closeOutputFiles( chanList );
        closeStatusOutputFile();
        dspErrMsg( stat );
        return ERRINFO( stat );
      }

      int i=0, iout = 0, numOps = dataLen / 4, numRemaining = ( numOps % 3 );
      if ( numRemaining ) {
        numOps -= numRemaining;
      }

      if ( numOps > Cnst::Max3PerWord ) {
        std::cout << "Unexpected value for numOps: " << numOps << std::endl;
        return  ERRINFO(EInternal);
      }

      unsigned int v1, v2, v3, v4, tmp1, tmp2, tmp3;

      for ( i=0; i<numOps; i+=3 ) {

        tmp1 = bswap_32( buf3BytesPerWord[i] );
        tmp2 = bswap_32( buf3BytesPerWord[i+1] );
        tmp3 = bswap_32( buf3BytesPerWord[i+2] );

        if ( i+2 >= Cnst::Max3PerWord ) {
          std::cout << "Unexpected value for 3 word index: " << i+2 << std::endl;
          return ERRINFO(EInternal);
        }

        v1 = tmp1 >> 8;
        if ( v1 & 0x800000 ) v1 |= 0xff000000;
        iout++;
        v2 = ( ( tmp1 & 0xff ) << 16 ) | ( tmp2 >> 16 );
        if ( v2 & 0x800000 ) v2 |= 0xff000000;
        iout++;
        if ( v3 & 0x800000 ) v3 |= 0xff000000;
        v3 = ( ( tmp2 & 0xffff ) << 8 ) | ( tmp3 >> 24 );
        iout++;
        v4 = tmp3 & 0xffffff;
        if ( v4 & 0x800000 ) v4 |= 0xff000000;
        iout++;

        if ( isignal > Cnst::MaxSignals ) {
          std::cout << "Unexpected value for isignal: " << isignal << ", ivalue = " << ivalue << std::endl;
          return ERRINFO(EInternal);
        }

        if ( ivalue >= Cnst::Max4PerWord ) {
          std::cout << "Unexpected value for isignal: " << isignal << ", ivalue = " << ivalue << std::endl;
           return ERRINFO(EInternal);
       }

        buf4BytesPerWord[isignal][ivalue]   = v1;
        buf4BytesPerWord[isignal+1][ivalue] = v2;
        buf4BytesPerWord[isignal+2][ivalue] = v3;
        buf4BytesPerWord[isignal+3][ivalue] = v4;

        isignal += 4;
        if ( isignal > Cnst::MaxSignals ) {
          std::cout << "Unexpected value for isignal: " << isignal << ", ivalue = " << ivalue << std::endl;
          return ERRINFO(EInternal);
        }
        if ( isignal >= Cnst::MaxSignals ) {
          isignal = 0;
          ivalue++;
        }
        if ( ivalue >= Cnst::MaxValIndex ) {
          writeOutputFiles( chanList, ivalue, buf4BytesPerWord );
          ivalue = 0;
        }
      
      }

      // update status records
      statusArray[istatus][PsnFileConverter::STATUS] = status;
      statusArray[istatus][PsnFileConverter::CHANMASK] = chanMask;
      statusArray[istatus][PsnFileConverter::SECS] = seconds;
      statusArray[istatus][PsnFileConverter::NANOSECS] = nanoseconds;
      statusArray[istatus][PsnFileConverter::RCVSECS] = rcvSeconds;
      statusArray[istatus][PsnFileConverter::RCVNANOSECS] = rcvNanoseconds;
      statusArray[istatus][PsnFileConverter::LO] = lo;
      statusArray[istatus][PsnFileConverter::LOLO] = lolo;
      statusArray[istatus][PsnFileConverter::HI] = hi;
      statusArray[istatus][PsnFileConverter::HIHI] = hihi;

      istatus++;
      if ( istatus >= Cnst::MaxStatusIndex ) {
        writeStatusOutputFile( istatus, statusArray );
        istatus = 0;
      }

    }

    loc += dataLen;
  
  } while ( 1 );

  // Execution cannot get here

  return ESuccess;

}

int PsnFileConverter::readHeaderType( std::filebuf& fb, unsigned long loc, QString& headerType, bool& eof ) {

  unsigned long numBytessRead;
  unsigned char buf[4];
  
  auto result = fb.pubseekoff( loc, std::ios::beg, std::ios::in );

  numBytessRead = fb.sgetn( ( (char *) buf ), sizeof(buf) );
  if ( numBytessRead != sizeof(buf) ) { // eof
    eof = true;
  }

  headerType = buf[0];
  headerType += buf[1];
  headerType += buf[2];
  headerType += buf[3];

  return ESuccess;

}

int PsnFileConverter::readBinHeader( std::filebuf& fb, unsigned long loc, unsigned int& numBytessRead,
                                    QString& headerType, bool& complete ) {

  bool eof = false;
  int stat = readHeaderType( fb, loc, headerType, eof );

  complete = false;

  auto result = fb.pubseekoff( loc, std::ios::beg, std::ios::in );

  if ( headerType == "PSNA" ) {

    numBytessRead = fb.sgetn( ( (char *) &binHeaderPsna ), sizeof(BinHdrPsnaType ) );
    if ( numBytessRead == 0 ) {
      complete = true;
    }
    else if ( numBytessRead != sizeof(BinHdrPsnaType ) ) { // incomplete read - treat as end of file
      std::cout << "eof - readBinHeader - numBytessRead is " << numBytessRead << std::endl;
      return ERRINFO(EReadFailure);
    }

  }
  else if ( headerType == "PSNB" ) {

    numBytessRead = fb.sgetn( ( (char *) &binHeaderPsnb ), sizeof(BinHdrPsnbType ) );
    if ( numBytessRead == 0 ) {
      complete = true;
    }
    else if ( numBytessRead != sizeof(BinHdrPsnbType ) ) { // incomplete read - treat as end of file
      std::cout << "eof - readBinHeader - numBytessRead is " << numBytessRead << std::endl;
      return ERRINFO(EReadFailure);
    }

  }
  else { // read into common header so caller may skip

    headerType = "Generic";

    numBytessRead = fb.sgetn( ( (char *) &binHeaderGeneric ), sizeof(BinHdrGenericType ) );
    if ( numBytessRead == 0 ) {
      complete = true;
    }
    else if ( numBytessRead != sizeof(BinHdrGenericType ) ) { // incomplete read - treat as end of file
      std::cout << "eof - readBinHeader - numBytessRead is " << numBytessRead << std::endl;
      return ERRINFO(EReadFailure);
    }

  }

  if ( complete ) {
    return ESuccess;
  }

  return ESuccess;

}

int PsnFileConverter::readBinData(std::filebuf& fb, unsigned long loc, unsigned int dataLen,
                                  unsigned int *buf, unsigned int& numBytessRead, bool& complete ) {

  complete = false;
  
  auto result2 = fb.pubseekoff( loc, std::ios::beg, std::ios::in );

  numBytessRead = fb.sgetn( ( (char *) buf ), dataLen );
  if ( numBytessRead == 0 ) { // eof
    complete = true;
  }
  else if ( numBytessRead != dataLen ) { // incomplete read - treat as end of file
    std::cout << "eof - readBinData - numBytessRead is " << numBytessRead << std::endl;
    return ERRINFO(EReadFailure);
  }

  if ( complete ) {
    return ESuccess;
  }
  
  return ESuccess;

}

QString PsnFileConverter::buildOutputFileName( int sigIndex, const QString& binDataFileDir,
                                                const QString& simpleName ) {

  std::stringstream strm;
  
  strm << binDataFileDir.toStdString() << simpleName.toStdString() << "-Chan" <<
    std::setw(4) << std::setfill( '0' ) << sigIndex <<
    "." << Cnst::BinExtension;
    
  QString fname;
  fname = strm.str().c_str();
  
  return fname;
  
}

int PsnFileConverter::createAndOpenOutputFiles( std::list<int>& chanList, int startingSigIndex,
                                                const QString& binDataFileDir, const QString& simpleName, 
                                                bool verbose ) {

  QString fname;
  unsigned long sizeInBytes = 0;

  // init
  for ( int i=0; i<=Cnst::MaxSignals; i++ ) {
    fileLoc[i] = 0;
  }
  
  // open all files
  for ( int i : chanList ) {

    fname = buildOutputFileName( i - 1 + startingSigIndex, binDataFileDir, simpleName );

    if ( verbose ) {
      std::cout << "  Writing output file: " << fname.toStdString() << std::endl;
    }
    
    auto result = fb[i].open( fname.toStdString(), std::ios::out | std::ios::binary );
    if ( !result ) {
      return ERRINFO(EOutFileOpen);
    }

    // write version and zero the size (in bytes) field
    auto num = fb[i].sputn( (char *) dataFileVersion, sizeof(dataFileVersion) );
    if ( num != sizeof(dataFileVersion) ) {
      return ERRINFO(EWriteFailure);
    }

    num = fb[i].sputn( (char *) &sizeInBytes, sizeof(sizeInBytes) );
    if ( num != sizeof(sizeInBytes) ) {
      return ERRINFO(EWriteFailure);
    }

    fileLoc[i] = 20;

  }

  return ESuccess;

}

int PsnFileConverter::writeOutputFiles( std::list<int>& chanList, int numValues,
                                       unsigned int array[Cnst::MaxSignals+1][Cnst::Max4PerWord] ) {

  unsigned long sizeInBytes = numValues * sizeof(unsigned int);

  //for ( int i=0; i<Cnst::MaxSignals; i++ ) {
  for ( int i : chanList ) {

    auto num = fb[i].sputn( (char *) &array[i][0], sizeInBytes );
    if ( num == 0 ) {
      return ESuccess;
    }
    else if ( num != sizeInBytes ) {
      return ERRINFO(EWriteFailure);
    }

    fileLoc[i] += sizeInBytes;
     
  }

  //sizeOfOneFile += sizeInBytes;
 
 return ESuccess;

}

int PsnFileConverter::closeOutputFiles ( std::list<int>& chanList ) {

  // write number of data bytes (i.e. don't include the 20 header bytes) to the
  // 64bit unsigned quantity at location 12 and then close file

  //for ( int i=0; i<Cnst::MaxSignals; i++ ) {
  for ( int i : chanList ) {
    
    fileLoc[i] -= 20;
    
    // set pointer
    auto result2 = fb[i].pubseekoff( 12, std::ios::beg, std::ios::out );

    auto num = fb[i].sputn( (char *) &fileLoc[i], sizeof(fileLoc[i]) );
    if ( num != sizeof(fileLoc[i]) ) {
      return ERRINFO(EWriteFailure);
    }
    
    fb[i].close();
    
  }

  return ESuccess;

}

QString PsnFileConverter::buildStatusOutputFileName( int chassisIndex, const QString& binDataFileDir,
                                        const QString& simpleName ) {

  std::stringstream strm;

  strm << binDataFileDir.toStdString() << simpleName.toStdString() << "-Chassis" <<
    std::setw(2) << std::setfill( '0' ) << chassisIndex << "-Status" << "." << Cnst::StatusExtension;

  QString fname;
  fname = strm.str().c_str();

  return fname;

}

int PsnFileConverter::createAndOpenStatusOutputFile ( int chassisIndex, const QString& binDataFileDir,
                                                      const QString& simpleName, bool verbose ) {

  QString fname;
  unsigned long sizeInBytes = 0;

  // assume directory exists

  // open file

  statusFileLoc = 0;

  fname = buildStatusOutputFileName( chassisIndex, binDataFileDir, simpleName );
  
  if ( verbose ) {
    std::cout << "  Writing output status file: " << fname.toStdString() << std::endl;
  }

  auto result = statusFb.open( fname.toStdString(), std::ios::out | std::ios::binary );
  if ( !result ) {
    return ERRINFO(EStatFileOpen);
  }

  // write version and zero the size (in bytes) field
  auto num = statusFb.sputn( (char *) statusFileVersion, sizeof(statusFileVersion) );
  if ( num != sizeof(statusFileVersion) ) {
    return ERRINFO(EWriteFailure);
  }

  num = statusFb.sputn( (char *) &sizeInBytes, sizeof(sizeInBytes) );
  if ( num != sizeof(sizeInBytes) ) {
    return ERRINFO(EWriteFailure);
  }

  statusFileLoc = 20;

  return ESuccess;

}

int PsnFileConverter::writeStatusOutputFile ( int numValues, unsigned int
                                              array[Cnst::MaxStatus][PsnFileConverter::NumStatusFields] ) {

  unsigned long sizeInBytes = numValues * sizeof(unsigned int) * PsnFileConverter::NumStatusFields;

  auto num = statusFb.sputn( (char *) array, sizeInBytes );
  if ( num == 0 ) {
    return ESuccess;
  }
  else if ( num != sizeInBytes ) {
    return ERRINFO(EWriteFailure);
  }

  statusFileLoc += sizeInBytes;
 
 return ESuccess;

}

int PsnFileConverter::closeStatusOutputFile ( void ) {

  // write number of data bytes (i.e. don't include the 20 header bytes) to the
  // 64bit unsigned quantity at location 12 and then close file

  statusFileLoc -= 20;
    
  // set pointer
  auto result2 = statusFb.pubseekoff( 12, std::ios::beg, std::ios::out );

  auto num = statusFb.sputn( (char *) &statusFileLoc, sizeof(statusFileLoc) );
  if ( num != sizeof(statusFileLoc) ) {
    return ERRINFO(EWriteFailure);
  }
    
  statusFb.close();

  return ESuccess;

}

void PsnFileConverter::show(void ) {
  std::cout << "Class PsnFileConverter" << std::endl;
}

int PsnFileConverter::getRawBinFileChanList( const QString& rawBinFileName, std::list<int>& chanList ) {

  unsigned int buf;
  std::filebuf fb;

  auto result1 = fb.open( rawBinFileName.toStdString(), std::ios::in | std::ios::binary );
  if ( !result1 ) {
    return ERRINFO(EInFileOpen);
  }

  unsigned long loc = 5 * sizeof(int);
  // read chan mask (active adc channels)
  auto result2 = fb.pubseekoff( loc, std::ios::beg, std::ios::in );
  
  auto num = fb.sgetn( reinterpret_cast<char *>( &buf ), sizeof(buf) );
  if ( num != sizeof(buf) ) {
    return ERRINFO(EReadFailure);
  }
  buf = bswap_32( buf );

  fb.close();

  chanList.clear();
  unsigned int bit = 1;
  for ( int i=0; i<32; i++ ) {
    if ( buf & bit ) {
      int ival = i + 1;
      chanList.push_back( ival );
    }
    bit = bit << 1;
  }

  return ESuccess;

}
