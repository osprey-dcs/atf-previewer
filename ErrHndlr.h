#ifndef __ErrHndlr_h
#define __ErrHndlr_h 1

#include <string>
#include <iostream>
#include <vector>

#include "DspErr.h"

class ErrHndlr {

 public:

  int numErrs;
  std::vector<std::string> errMsgs;

  int mostRecentError {0};
  int errLine {0};
  std::string errFile;
  std::string arg;

  ErrHndlr( int n, const std::string messages[] );
  
  int errInfo ( int err, const std::string& a = "",
                int line=0, const std::string& file = "" );
  
  void dspErrMsg ( int err );
  
  void dspErrMsg ( void );

};

#endif
