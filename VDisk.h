//
// Created by jws3 on 5/4/24.
//

#ifndef UNTITLED_VDISK_H
#define UNTITLED_VDISK_H

#include <string.h> // for memcpy

#include <string>
#include <memory>
#include <fstream>

#include "ErrHndlr.h"

class VDisk : public ErrHndlr {

public:

  static const int NumErrs = 4;
  static const int ESuccess = 0;
  static const int EStat = 1;
  static const int ENoFile = 2;
  static const int EValue = 3;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    { "File stat failure: " },
    { "No file specified" },
    { "Zero file length" }
  };
  
  VDisk ();
  ~VDisk ( void );
  void setMaxSize( int64_t _maxSize );
  int setFile ( const std::string& fileName );
  int readN ( std::filebuf *fb, int64_t start, int64_t end, char *buf, bool showParams = false );

  static const unsigned int BlockSize = 8192;

  int64_t maxSize;
  int64_t fileSizeInBytes;
  int64_t maxBufSizeInButes;
  int64_t bufSizeInBytes;
  int64_t mapSizeInBytes;
  int64_t mapSizeInBlocks;
  std::shared_ptr<char[]> map; // 0=not filled, 1=filled by file data,
                               // represents one unit of storage
  std::shared_ptr<char[]> buf;
  bool noFile;
  
private:
  int readFile( std::filebuf *fb, int64_t start, int64_t length,
                char *outBuf );
  int unixError;

};

#endif //UNTITLED_VDISK_H
