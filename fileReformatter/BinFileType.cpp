//
// Created by jws3 on 5/1/24.
//

#include "BinFileType.h"
#include "errno.h"

BinFileType::BinFileType() {}

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
