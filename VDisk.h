//
// Created by jws3 on 5/4/24.
//

#ifndef UNTITLED_VDISK_H
#define UNTITLED_VDISK_H


#include <string>
#include <memory>
#include <fstream>

class VDisk {

public:
  VDisk ();
  ~VDisk ( void );
  int getUnixError ( void );
  void setMaxSize( unsigned long _maxSize );
  int setFile ( std::string fileName );
  int readN ( std::filebuf *fb, unsigned long start, unsigned long end, char *buf );

  static const unsigned int NumErrs = 5;
  static const unsigned int E_Success = 0;
  static const unsigned int E_NoFile = 1;
  static const unsigned int E_UnixError = 2;
  static const unsigned int E_Value = 3;
  static const unsigned int E_4 = 4;

  inline static const std::string ErrMsg[NumErrs] {
    "Success",
    "Error_1",
    "Error 2",
    "Error 3",
    "Error 4"
  };

  static const unsigned int BlockSize = 8192;

  unsigned long maxSize;
  unsigned long fileSizeInBytes;
  unsigned long maxBufSizeInButes;
  unsigned long bufSizeInBytes;
  unsigned long mapSizeInBytes;
  unsigned long mapSizeInBlocks;
  std::shared_ptr<char[]> map; // 0=not filled, 1=filled by file data,
                               // represents one unit of storage
  std::shared_ptr<char[]> buf;
  bool noFile;
  
private:
  int readFile( std::filebuf *fb, unsigned long start, unsigned long length,
                char *outBuf );
  int unixError;

};


#endif //UNTITLED_VDISK_H
