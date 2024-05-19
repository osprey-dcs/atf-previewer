//
// Created by jws3 on 5/19/24.
//

#include "DspErr.h"

void DspErr::dspErrMsg ( int line, std::string file, int n, int err, const std::string msgs[] ) {

  std::string str;

  if ( ( err < 0 ) || ( err >= n ) ) {
    str = "Unknown error";
  }
  else {
    str = msgs[err];
  }

  if ( ( file == "" ) && ( line = 0 ) ) {
    std::cout << str << " in " << __FILE__ << std::endl;
  }
  else if ( file == "" ) {
    std::cout << str << std::endl;
  }

  std::cout << str << " at line " << line << " in " << file << std::endl;

}
