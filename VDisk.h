//
// Created by jws3 on 5/4/24.
//

#ifndef UNTITLED_VDISK_H
#define UNTITLED_VDISK_H


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
  void setMaxSize( uint64_t _maxSize );
  int setFile ( std::string fileName );
  int readN ( std::filebuf *fb, uint64_t start, uint64_t end, char *buf, bool showParams = false );

  static const unsigned int BlockSize = 8192;

  uint64_t maxSize;
  uint64_t fileSizeInBytes;
  uint64_t maxBufSizeInButes;
  uint64_t bufSizeInBytes;
  uint64_t mapSizeInBytes;
  uint64_t mapSizeInBlocks;
  std::shared_ptr<char[]> map; // 0=not filled, 1=filled by file data,
                               // represents one unit of storage
  std::shared_ptr<char[]> buf;
  bool noFile;
  
private:
  int readFile( std::filebuf *fb, uint64_t start, uint64_t length,
                char *outBuf );
  int unixError;

};

#endif //UNTITLED_VDISK_H
