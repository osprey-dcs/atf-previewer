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

int PsnFileConverter::convert ( int chassisIndex, int startingSigIndex, const DataHeader *dh,
                                const QString &rawDataFile, const QString& binDataFileDir,
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
  
  auto result = fb.open( rawDataFile.toStdString(), std::ios::in | std::ios::binary );
  if ( !result ) {
    std::cout << "Input file open failure " << std::endl;
    return -1;
  }

  stat = createAndOpenOutputFiles( startingSigIndex, dh, simpleName, binDataFileDir, verbose );
  if ( stat ) {
    fb.close();
    std::cout << "Output files open failure " << std::endl;
    return -1;
  }

  stat = createAndOpenStatusOutputFile( chassisIndex, dh, simpleName, binDataFileDir, verbose );
  if ( stat ) {
    fb.close();
    std::cout << "Status file open failure " << std::endl;
    return -1;
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
        writeOutputFiles( ivalue, buf4BytesPerWord );
      }
      if ( istatus ) { // there is unwritten data
        writeStatusOutputFile( istatus, statusArray );
      }
      fb.close();
      closeOutputFiles();
      closeStatusOutputFile();
      return 0;
    }
    if ( stat < 0 ) {
      fb.close();
      closeOutputFiles();
      closeStatusOutputFile();
      std::cout << "Error " << stat << " from readBinHeader" << std::endl;
      return stat;
    }

    bool skip = false;

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

      loc += sizeof(BinHdrPsnaType);
      dataLen = bodyLen - 24;

    }
    else if ( headerType == "PSNB" ) {

      bodyLen = bswap_32( binHeaderPsnb.bodyLen );
      status = bswap_32( binHeaderPsnb.status );
      chanMask = bswap_32( binHeaderPsnb.chanMask );
      seqHigh = bswap_32( binHeaderPsnb.seqHigh );
      seqLow = bswap_32( binHeaderPsnb.seqLow );
      seq = seqHigh;
      seq = ( seq << 32 ) | seqLow;
      seconds = bswap_32( binHeaderPsnb.seconds );
      nanoseconds = bswap_32( binHeaderPsnb.nanoseconds );
      rcvSeconds = bswap_32( binHeaderPsnb.rcvSeconds );
      rcvNanoseconds = bswap_32( binHeaderPsnb.rcvNanoseconds );
      lo = bswap_32( binHeaderPsnb.lo );
      lolo = bswap_32( binHeaderPsnb.lolo );
      hi = bswap_32( binHeaderPsnb.hi );
      hihi = bswap_32( binHeaderPsnb.hihi );

      loc += sizeof(BinHdrPsnbType);
      dataLen = bodyLen - 24;

    }
    else { // for this to work, body len of any unknown record type must always exclude 24 bytes of the header
      
      bodyLen = bswap_32( binHeaderGeneric.bodyLen );
      status = 0;
      chanMask = 0;
      seconds = 0;
      nanoseconds = 0;
      rcvSeconds = 0;
      rcvNanoseconds = 0;
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
      }
    
      stat = readBinData( fb, loc, dataLen, buf3BytesPerWord, numBytesRead, complete );
      if ( complete ) {
        if ( ivalue ) {  // unwritten data exists
          writeOutputFiles( ivalue, buf4BytesPerWord );
        }
        if ( istatus ) { // there is unwritten data
          writeStatusOutputFile( istatus, statusArray );
        }
        fb.close();
        closeOutputFiles();
        closeStatusOutputFile();
        return 0;
      }
      if ( stat < 0 ) {
        fb.close();
        closeOutputFiles();
        closeStatusOutputFile();
        std::cout << "Error " << stat << " from readBinHeader" << std::endl;
        return stat;
      }

      int i=0, iout = 0, numOps = dataLen / 4, numRemaining = ( numOps % 3 );
      if ( numRemaining ) {
        numOps -= numRemaining;
      }

      if ( numOps > Cnst::Max3PerWord ) {
        std::cout << "Unexpected value for numOps: " << numOps << std::endl;
        numOps = Cnst::Max3PerWord;
      }

      unsigned int v1, v2, v3, v4, tmp1, tmp2, tmp3;

      for ( i=0; i<numOps; i+=3 ) {

        //v1 = bswap_32( buf3BytesPerWord[i] >> 8 ) >> 8;
        //v2 = bswap_32( ( ( buf3BytesPerWord[i] & 0xff ) << 16 ) | ( buf3BytesPerWord[i+1] >> 16 ) ) >> 8;
        //v3 = bswap_32( ( ( buf3BytesPerWord[i+1] & 0xffff ) << 8 ) | ( ( buf3BytesPerWord[i+2] & 0xff000000 ) >> 24 ) ) >> 8;
        //v4 = bswap_32( buf3BytesPerWord[i+2] & 0xffffff ) >> 8;

        tmp1 = bswap_32( buf3BytesPerWord[i] );
        tmp2 = bswap_32( buf3BytesPerWord[i+1] );
        tmp3 = bswap_32( buf3BytesPerWord[i+2] );

        if ( i+2 >= Cnst::Max3PerWord ) {
          std::cout << "Unexpected value for 3 word index: " << i+2 << std::endl;
        }

        v1 = tmp1 >> 8;
        iout++;
        v2 = ( ( tmp1 & 0xff ) << 16 ) | ( tmp2 >> 16 );
        iout++;
        v3 = ( ( tmp2 & 0xffff ) << 8 ) | ( tmp3 >> 24 );
        iout++;
        v4 = tmp3 & 0xffffff;
        iout++;

        if ( isignal >= Cnst::MaxSignals ) {
          std::cout << "Unexpected value for isignal: " << isignal << ", ivalue = " << ivalue << std::endl;
        }

        if ( ivalue >= Cnst::Max4PerWord ) {
          std::cout << "Unexpected value for isignal: " << isignal << ", ivalue = " << ivalue << std::endl;
        }

        buf4BytesPerWord[isignal][ivalue]   = v1;
        buf4BytesPerWord[isignal+1][ivalue] = v2;
        buf4BytesPerWord[isignal+2][ivalue] = v3;
        buf4BytesPerWord[isignal+3][ivalue] = v4;

        isignal += 4;
        if ( isignal > Cnst::MaxSignals ) {
          std::cout << "Unexpected value for isignal: " << isignal << ", ivalue = " << ivalue << std::endl;
        }
        if ( isignal >= Cnst::MaxSignals ) {
          isignal = 0;
          ivalue++;
        }
        if ( ivalue >= Cnst::MaxValIndex ) {
          writeOutputFiles( ivalue, buf4BytesPerWord );
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

  return PsnFileConverter::E_SUCCESS;

}

int PsnFileConverter::readHeaderType( std::filebuf& fb, unsigned long loc, QString& headerType, bool& eof ) {

  unsigned long numBytessRead;
  unsigned char buf[4];
  
  try {
    auto result = fb.pubseekoff( loc, std::ios::beg, std::ios::in );
  }
  catch ( const std::exception& e) {
    std::cout << "readHeaderType - pubseekoff: " << e.what() << std::endl;
  }

  try {
    numBytessRead = fb.sgetn( ( (char *) buf ), sizeof(buf) );
    if ( numBytessRead != sizeof(buf) ) { // eof
      eof = true;
    }
    else if ( numBytessRead < sizeof(buf) ) { // incomplete read - treat as end of file
      eof = true;
      std::cout << "eof - readHeaderType - numBytessRead is " << numBytessRead << std::endl;
    }
  } catch (const std::exception& e) { // unknown failure - treat as end of file
    eof = true;
    std::cout << "readHeaderType - sgetn: " << e.what() << std::endl;
  }

  headerType = buf[0];
  headerType += buf[1];
  headerType += buf[2];
  headerType += buf[3];

  return 0;

}

int PsnFileConverter::readBinHeader( std::filebuf& fb, unsigned long loc, unsigned int& numBytessRead,
                                    QString& headerType, bool& complete ) {

  bool eof = false;
  int stat = readHeaderType( fb, loc, headerType, eof );

  complete = false;

  try {
    auto result = fb.pubseekoff( loc, std::ios::beg, std::ios::in );
  }
  catch ( const std::exception& e) {
    std::cout << "readBinHeader - pubseekoff: " << e.what() << std::endl;
  }

  if ( headerType == "PSNA" ) {

    try {
      numBytessRead = fb.sgetn( ( (char *) &binHeaderPsna ), sizeof(BinHdrPsnaType ) );
      if ( numBytessRead == 0 ) { // eof
        complete = true;
      }
      else if ( numBytessRead < sizeof(BinHdrPsnaType ) ) { // incomplete read - treat as end of file
        complete = true;
        std::cout << "eof - readBinHeader - numBytessRead is " << numBytessRead << std::endl;
      }
    } catch (const std::exception& e) { // unknown failure - treat as end of file
      complete = true;
      std::cout << "readBinHeader - sgetn: " << e.what() << std::endl;
    }

  }
  else if ( headerType == "PSNB" ) {

    try {
      numBytessRead = fb.sgetn( ( (char *) &binHeaderPsnb ), sizeof(BinHdrPsnbType ) );
      if ( numBytessRead == 0 ) { // eof
        complete = true;
      }
      else if ( numBytessRead < sizeof(BinHdrPsnbType ) ) { // incomplete read - treat as end of file
        complete = true;
        std::cout << "eof - readBinHeader - numBytessRead is " << numBytessRead << std::endl;
      }
    } catch (const std::exception& e) { // unknown failure - treat as end of file
      complete = true;
      std::cout << "readBinHeader - sgetn: " << e.what() << std::endl;
    }

  }
  else { // read into common header so caller may skip

    headerType = "Generic";

    try {
      numBytessRead = fb.sgetn( ( (char *) &binHeaderGeneric ), sizeof(BinHdrGenericType ) );
      if ( numBytessRead == 0 ) { // eof
        complete = true;
      }
      else if ( numBytessRead < sizeof(BinHdrGenericType ) ) { // incomplete read - treat as end of file
        complete = true;
        std::cout << "eof - readBinHeader - numBytessRead is " << numBytessRead << std::endl;
      }
    } catch (const std::exception& e) { // unknown failure - treat as end of file
      complete = true;
      std::cout << "readBinHeader - sgetn: " << e.what() << std::endl;
    }

  }

  if ( complete ) {
    return E_SUCCESS;
  }

  return PsnFileConverter::E_SUCCESS;

}

int PsnFileConverter::readBinData(std::filebuf& fb, unsigned long loc, unsigned int dataLen,
                                  unsigned int *buf, unsigned int& numBytessRead, bool& complete ) {

  complete = false;
  
  try {
    auto result2 = fb.pubseekoff( loc, std::ios::beg, std::ios::in );
  }
  catch ( const std::exception& e) {
    std::cout << "readBinData - pubseekoff: " << e.what() << std::endl;
  }

  try {
    numBytessRead = fb.sgetn( ( (char *) buf ), dataLen );
    if ( numBytessRead == 0 ) { // eof
      complete = true;
    }
    else if ( numBytessRead < dataLen ) { // incomplete read - treat as end of file
      complete = true;
      std::cout << "eof - readBinData - numBytessRead is " << numBytessRead << std::endl;
    }
  } catch (const std::exception& e) { // unknown failure - treat as end of file
    complete = true;
    std::cout << "readBinData - sgetn: " << e.what() << std::endl;
  }

  if ( complete ) {
    return E_SUCCESS;
  }
  
  return PsnFileConverter::E_SUCCESS;

}

int PsnFileConverter::createAndOpenOutputFiles( int startingSigIndex, const DataHeader *dh,
                                                const QString& simpleName, const QString& binDataFileDir,
                                                bool verbose ) {

  QString fname;
  unsigned long sizeInBytes = 0;

  // open all files
  for ( int i=0; i<Cnst::MaxSignals; i++ ) {

    fileLoc[i] = 0;

    std::stringstream strm;
    strm << binDataFileDir.toStdString() << simpleName.toStdString() << "-Chan" <<
      std::setw(4) << std::setfill( '0' ) << i+startingSigIndex <<
      "." << Cnst::BinExtension;
    
    fname = strm.str().c_str();

    if ( verbose ) {
      std::cout << "  Writing output file: " << fname.toStdString() << std::endl;
    }
    
    auto result = fb[i].open( fname.toStdString(), std::ios::out | std::ios::binary );
    if ( !result ) {
      return -1;
    }

    // write version and zero the size (in bytes) field
    auto num = fb[i].sputn( (char *) dataFileVersion, sizeof(dataFileVersion) );

    num = fb[i].sputn( (char *) &sizeInBytes, sizeof(sizeInBytes) );

    fileLoc[i] = 20;

  }

  return 0;

}

int PsnFileConverter::writeOutputFiles(int numValues, unsigned int array[Cnst::MaxSignals][Cnst::Max4PerWord] ) {

  unsigned long sizeInBytes = numValues * sizeof(unsigned int);

  for ( int i=0; i<Cnst::MaxSignals; i++ ) {
     
     auto num = fb[i].sputn( (char *) &array[i][0], sizeInBytes );
     if ( num <= 0 ) {
       return -1;
     }

     fileLoc[i] += sizeInBytes;
     
  }

  //sizeOfOneFile += sizeInBytes;
 
 return 0;

}

void PsnFileConverter::closeOutputFiles (void ) {

  // write number of data bytes (i.e. don't include the 20 header bytes) to the
  // 64bit unsigned quantity at location 12 and then close file

  for ( int i=0; i<Cnst::MaxSignals; i++ ) {
    
    fileLoc[i] -= 20;
    
    // set pointer
    try {
      auto result2 = fb[i].pubseekoff( 12, std::ios::beg, std::ios::out );
    }
    catch ( const std::exception& e) {
      std::cout << "readBinData - pubseekoff: " << e.what() << std::endl;
    }

    auto num = fb[i].sputn( (char *) &fileLoc[i], sizeof(fileLoc[i]) );
    
    fb[i].close();
    
  }

}

int PsnFileConverter::createAndOpenStatusOutputFile ( int chassisIndex, const DataHeader *dh, const QString& simpleName,
                                                      const QString& binDataFileDir, bool verbose ) {

  QString fname;
  unsigned long sizeInBytes = 0;

  // assume directory exists

  // open file

  statusFileLoc = 0;

  std::stringstream strm;
  strm << binDataFileDir.toStdString() << simpleName.toStdString() << "-Chassis" <<
    std::setw(2) << std::setfill( '0' ) << chassisIndex << "-Status" << "." << Cnst::StatusExtension;

  fname = strm.str().c_str();

  if ( verbose ) {
    std::cout << "  Writing output status file: " << fname.toStdString() << std::endl;
  }

  auto result = statusFb.open( fname.toStdString(), std::ios::out | std::ios::binary );
  if ( !result ) {
    return -1;
  }

  // write version and zero the size (in bytes) field
  auto num = statusFb.sputn( (char *) statusFileVersion, sizeof(statusFileVersion) );

  num = statusFb.sputn( (char *) &sizeInBytes, sizeof(sizeInBytes) );

  statusFileLoc = 20;

  return 0;

}

int PsnFileConverter::writeStatusOutputFile ( int numValues, unsigned int
                                              array[Cnst::MaxStatus][PsnFileConverter::NumStatusFields] ) {

  unsigned long sizeInBytes = numValues * sizeof(unsigned int) * PsnFileConverter::NumStatusFields;

  auto num = statusFb.sputn( (char *) array, sizeInBytes );
  if ( num <= 0 ) {
    return -1;
  }

  statusFileLoc += sizeInBytes;
 
 return 0;

}

void PsnFileConverter::closeStatusOutputFile ( void ) {

  // write number of data bytes (i.e. don't include the 20 header bytes) to the
  // 64bit unsigned quantity at location 12 and then close file

  statusFileLoc -= 20;
    
  // set pointer
  try {
    auto result2 = statusFb.pubseekoff( 12, std::ios::beg, std::ios::out );
  }
  catch ( const std::exception& e) {
    std::cout << "closeStatusOutputFile - pubseekoff: " << e.what() << std::endl;
  }

  auto num = statusFb.sputn( (char *) &statusFileLoc, sizeof(statusFileLoc) );
    
  statusFb.close();

}

void PsnFileConverter::show(void ) {
  std::cout << "Class PsnaFileConverter" << std::endl;
}
