//
// Created by jws3 on 5/1/24.
//

#include <byteswap.h>
#include "BinFileType.h"
#include "errno.h"

BinFileType::BinFileType() {}

int BinFileType::getRawBinFileChanList( const QString& rawBinFileName, std::list<int>& chanList ) {

  unsigned int buf;
  std::filebuf fb;

  auto result1 = fb.open( rawBinFileName.toStdString(), std::ios::in | std::ios::binary );
  if ( !result1 ) {
    return -1;
  }

  unsigned long loc = 5 * sizeof(int);
  // read file type
  auto result2 = fb.pubseekoff( loc, std::ios::beg, std::ios::in );
  if ( result2 < 0 ) {
    std::cout << "result2 is " << result2 << std::endl;
  }
  
  auto result3 = fb.sgetn( reinterpret_cast<char *>( &buf ), sizeof(buf) );
  if ( result3 < sizeof(buf) ) {
    std::cout << "result3 is " << result3 << std::endl;
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

  return 0;

}

int BinFileType::getRawBinFileType( const QString& rawBinFileName, QString& rawBinFileType ) {

  unsigned char buf[3];
  std::filebuf fb;

  auto result1 = fb.open( rawBinFileName.toStdString(), std::ios::in | std::ios::binary );
  if ( !result1 ) {
    return -1;
  }

  // read file type
  auto result2 = fb.pubseekoff( 0ul, std::ios::beg, std::ios::in );
  if ( result2 < 0 ) {
    std::cout << "result2 is " << result2 << std::endl;
  }
  
  auto result3 = fb.sgetn( (char *) buf, sizeof(buf) );
  if ( result3 < sizeof(buf) ) {
    std::cout << "result3 is " << result3 << std::endl;
  }

  fb.close();

  rawBinFileType = buf[0];
  rawBinFileType += buf[1];
  rawBinFileType += buf[2];

  return 0;

}
