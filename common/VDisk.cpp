//
// Created by jws3 on 5/4/24.
//

#include "sys/types.h"
#include "sys/stat.h"
#include "string.h"

#include <iostream>

#include "VDisk.h"

VDisk::VDisk() : ErrHndlr( NumErrs, errMsgs ) {

  noFile = true;
  unixError = 0;
  maxSize = 0;
  fileSizeInBytes = 0;
  buf = nullptr;
  map = nullptr;
  
}

void VDisk::setMaxSize( int64_t _maxSize ) {

   maxSize = ( _maxSize / BlockSize ) * BlockSize;

}

int VDisk::setFile ( const std::string& fileName ) {

  noFile = false;
  if ( buf ) {
    buf = nullptr;
  }
  if ( map ) {
    map = nullptr;
  }

  struct stat statBuf;
  int status;
  status = stat( fileName.c_str(), &statBuf );
  if ( status ) return ERRINFO(EStat,fileName);
  fileSizeInBytes = statBuf.st_size;

  //std::cout << "file len = " << fileSizeInBytes << std::endl;

  bufSizeInBytes = static_cast<int64_t>( std::min( maxSize, fileSizeInBytes ) );
  buf = std::shared_ptr<char[]>( new char[bufSizeInBytes] );

  //std::cout << "setFile - buf size is = " << bufSizeInBytes << std::endl;
  //std::cout << "setFile - buf addr = " << std::hex << (int64_t) ((char *) &buf) << std::dec << std::endl;

  mapSizeInBytes = bufSizeInBytes / BlockSize;
  if ( bufSizeInBytes % BlockSize ) mapSizeInBytes += 1;
  map = std::shared_ptr<char[]>( new char[mapSizeInBytes] );

  for ( int i=0; i<mapSizeInBytes; i++ ) {
    map[i] = 0;
  }

  //std::cout << "buf size = " << bufSizeInBytes << std::endl;
  //std::cout << "map size = " << mapSizeInBytes << std::endl;

  return ESuccess;

}

VDisk::~VDisk( void ) {

  //std::cout << " VDisk::~VDisk" << std::endl;

}

int VDisk::readN( std::filebuf *fb, int64_t start, int64_t length, char *outBuf,  bool showParams ) {

  int st=0, n;
  bool memRead = true;
  bool fileRead = true;
  bool blockCheckAndFill = true;
  int64_t end=0, startMem=0, endMem=0, startFile=0, endFile=0, block0=0, block1=0;

  if ( noFile ) return ERRINFO(ENoFile,"");
  
  if ( length < 1 ) return ERRINFO(EValue,"");
  end = start + length - 1;

  block0 = start / BlockSize;
  if ( block0 >= mapSizeInBytes ) {
    blockCheckAndFill = false;
  }
  if ( blockCheckAndFill ) {
    block1 = end / BlockSize;
    if ( end % BlockSize ) block1 += 1;
    if (block1 >= mapSizeInBytes) {
      block1 = mapSizeInBytes - 1;
    }
  }

  startMem = start;
  if ( startMem > bufSizeInBytes ) {
    memRead = false;
    startFile = start;
    endFile = end;
  }

  if ( memRead ) {
    endMem = end;
    if ( endMem > bufSizeInBytes) {
      endMem = bufSizeInBytes - 1;
      startFile = bufSizeInBytes;
      endFile = end;
    }
    else {
      fileRead = false;
    }

  }

  //if ( showParams ) {
  //  std::cout << "block0 = " << block0 << std::endl;
  //  std::cout << "block1 = " << block1 << std::endl;
  //  std::cout << "startMem = " << startMem << std::endl;
  //  std::cout << "endMem = " << endMem << std::endl;
  //  std::cout << "memRead = " << memRead << std::endl;
  //  std::cout << "startFile = " << startFile << std::endl;
  //  std::cout << "endFile = " << endFile << std::endl;
  //  std::cout << "fileRead = " << fileRead << std::endl;
  //  //std::cout << "? = " << ? << std::endl;
  //}

  for ( int64_t i=block0; i<=block1; i++ ) {

    if ( !map[i] ) {
      map[i] = 1;
      char *cbuf = (char *) &((this->buf.get())[i*BlockSize]);
      n = readFile( fb, i*BlockSize, BlockSize, cbuf );
    }

  }

  int64_t outButFileReadStart = startFile;
  int64_t outButFileReadLen = endFile - startFile + 1;

  unsigned int outBufIndex = 0;
  
  if ( memRead ) {

    int64_t len = endMem - startMem + 1;
    memcpy( (void *) &(outBuf[outBufIndex]), (void *) &(buf[startMem]), len );
    outButFileReadStart = endMem;
    outButFileReadLen = endFile - outButFileReadStart + 1;

    outBufIndex = len;

  }

  if ( fileRead ) {

    n = readFile( fb, outButFileReadStart, endFile-outButFileReadStart+1, (char *) &(outBuf[outBufIndex]) );

  }

  return (int) length;

}

int VDisk::readFile( std::filebuf *fb, int64_t start, int64_t sizeInBytes,
                    char *outBuf ) {

  int64_t n;
  try {
    fb->pubseekoff( start, std::ios::beg, std::ios::in );
    n = fb->sgetn( (char *) outBuf, sizeInBytes );
  }
  catch ( const std::exception& e ) {
    int err = ERRINFO(ERead,e.what());
    return 0;
  }

  return n;

}
