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
  if ( bufSizeInBytes % BlockSize ) mapSizeInBytes += 1;
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

int VDisk::readN( std::filebuf *fb, unsigned long start, unsigned long length, char *outBuf,  bool showParams ) {

  //if ( showParams ) {
  //  std::cout << "readN - start = " << start << ", length = " << length << std::endl;
  //}
  
  int st=0, n;
  bool memRead = true;
  bool fileRead = true;
  bool blockCheckAndFill = true;
  unsigned long end=0, startMem=0, endMem=0, startFile=0, endFile=0, block0=0, block1=0;

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

  //if ( blockCheckAndFill ) {

    for ( unsigned long i=block0; i<=block1; i++ ) {

      if ( !map[i] ) {
        map[i] = 1;

        //char *zbuf = this->buf.get();
        //std::cout << "zbuf addr = " << std::hex << (unsigned long) zbuf << std::dec << std::endl;
        //zbuf = (char *) &((this->buf.get())[i*BlockSize]);
        //std::cout << "zbuf addr 2 = " << std::hex << (unsigned long) zbuf << std::dec << std::endl;

        char *cbuf = (char *) &((this->buf.get())[i*BlockSize]);
        int n = readFile( fb, i*BlockSize, BlockSize, cbuf );
        //std::cout << "read " << n << " bytes from file to fill block" << i << std::endl;
      }

  //}

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

  unsigned int outBufIndex = 0;
  
  if ( memRead ) {

    //std::cout << "memRead, start ele = " << startMem/4 << std::endl;

    unsigned long len = endMem - startMem + 1;
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

int VDisk::readFile( std::filebuf *fb, unsigned long start, unsigned long sizeInBytes,
                    char *outBuf ) {

  //std::cout << "read file - start = " << start <<
  // ", end = " << start+sizeInBytes-1 <<
  // ", len = " << sizeInBytes << std::endl;

  fb->pubseekoff( start, std::ios::beg, std::ios::in );
  unsigned long n = fb->sgetn( (char *) outBuf, sizeInBytes );

  return n;

}

