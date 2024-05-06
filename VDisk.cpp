//
// Created by jws3 on 5/4/24.
//

#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "string.h"
#include "errno.h"

#include <iostream>
#include <cstring>

#include "VDisk.h"

VDisk::VDisk() {

  noFile = true;
  unixError = 0;
  maxSize = 0;
  fileSizeInBytes = 0;
  buf = nullptr;
  map = nullptr;
  
}

int VDisk::getUnixError ( void ) {

  return unixError;
  
}

void VDisk::setMaxSize( unsigned long _maxSize ) {

   maxSize = ( _maxSize / BlockSize ) * BlockSize;

}

int VDisk::setFile ( std::string fileName ) {

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
  if ( status ) {
    unixError = errno;
    return E_UnixError;
  }
  fileSizeInBytes = statBuf.st_size;

  //std::cout << "file len = " << fileSizeInBytes << std::endl;

  bufSizeInBytes = static_cast<unsigned long>( std::min( maxSize, fileSizeInBytes ) );
  buf = std::shared_ptr<char[]>( new char[bufSizeInBytes] );

  //std::cout << "setFile - buf size is = " << bufSizeInBytes << std::endl;
  //std::cout << "setFile - buf addr = " << std::hex << (unsigned long) ((char *) &buf) << std::dec << std::endl;

  mapSizeInBytes = bufSizeInBytes / BlockSize;
  map = std::shared_ptr<char[]>( new char[mapSizeInBytes] );

  for ( int i=0; i<mapSizeInBytes; i++ ) {
    map[i] = 0;
  }

  //std::cout << "buf size = " << bufSizeInBytes << std::endl;
  //std::cout << "map size = " << mapSizeInBytes << std::endl;

  return E_Success;

}

VDisk::~VDisk(void) {

  //std::cout << " VDisk::~VDisk" << std::endl;

}

int VDisk::readN( std::filebuf *fb, unsigned long start, unsigned long length, char *outBuf ) {

  //std::cout << "readN - start = " << start << ", length = " << length << std::endl;
  
  int st;
  bool memRead = true;
  bool fileRead = true;
  bool blockCheckAndFill = true;
  unsigned long end, startMem, endMem, startFile, endFile, block0, block1;

  if ( noFile ) return E_NoFile;
  
  //std::cout << std::endl << std::endl;

  if ( length < 1 ) return E_Value;
  end = start + length - 1;

  block0 = start / BlockSize;
  if ( block0 >= mapSizeInBytes ) {
    blockCheckAndFill = false;
  }
  if ( blockCheckAndFill ) {
    block1 = end / BlockSize;
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

  if ( blockCheckAndFill ) {

    for ( unsigned long i=block0; i<=block1; i++ ) {

      if ( !map[i] ) {
        map[i] = 1;
        char *cbuf = (char *) &((this->buf.get())[i*8192]);
        //std::cout << "read file to fill block" << std::endl;
        st = readFile( fb, i*BlockSize, BlockSize, cbuf );
      }

    }

  }

  unsigned long outButFileReadStart = startFile;
  unsigned long outButFileReadLen = endFile - startFile + 1;


  //std::cout << "start = " << start << std::endl;
  //std::cout << "end = " << end << std::endl;
  //std::cout << "memRead = " << memRead << std::endl;
  //std::cout << "startMem = " << startMem << std::endl;
  //std::cout << "endMem = " << endMem << std::endl;
  //std::cout << "fileRead = " << fileRead << std::endl;
  //std::cout << "startFile = " << startFile << std::endl;
  //std::cout << "endFile = " << endFile << std::endl;
  
  if ( memRead ) {

    unsigned long len = endMem - startMem + 1;
    memcpy( (void *) outBuf, (void *) &(buf[startMem]), len );
    outButFileReadStart = endMem;
    outButFileReadLen = endFile - outButFileReadStart + 1;

  }

  if ( fileRead ) {

    st = readFile( fb, outButFileReadStart, endFile-outButFileReadStart+1, outBuf );

  }

  return (int) length;

}

int VDisk::readFile( std::filebuf *fb, unsigned long start, unsigned long sizeInBytes,
                    char *outBuf ) {

  //std::cout << "read file - start = " << start <<
  // ", end = " << start+sizeInBytes-1 <<
  // ", len = " << sizeInBytes << std::endl;

  fb->pubseekoff( start, std::ios::beg, std::ios::in );
  unsigned long n = fb->sgetn( (char *) outBuf, sizeInBytes );

  return 0;

}

