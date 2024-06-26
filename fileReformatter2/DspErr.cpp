//
// Created by jws3 on 5/19/24.
//

#include "DspErr.h"

void DspErr::dspErrMsg ( int line, const std::string& file, int n, int err,
                         std::vector<std::string>& msgs, const std::string& arg ) {

  std::string str;

  if ( ( err < 0 ) || ( err >= n ) ) {
    str = "Unknown error";
  }
  else {
    str = msgs[err];
  }

  if ( ( file.empty() ) && ( line == 0 ) ) {
    std::cout << str << " in " << __FILE__ << std::endl;
  }
  else if ( file.empty() ) {
    std::cout << str << std::endl;
  }

  if ( arg.empty() ) {
    std::cout << str << " at line " << line << " in " << file << std::endl;
  }
  else {
    std::cout << str << arg << " at line " << line << " in " << file << std::endl;
  }

}
