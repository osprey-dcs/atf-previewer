#include "ErrHndlr.h"

ErrHndlr::ErrHndlr( int n, const std::string messages[] ) {

  numErrs = n;
  for ( int i=0; i<n; i++ ) {
    errMsgs.push_back( messages[i] );
  }

}
  
int ErrHndlr::errInfo ( int err, const std::string& a,
                      int line, const std::string& file ) {
  mostRecentError = err;
  errLine = line;
  errFile = file;
  arg = a;
  return err;
}

void ErrHndlr::dspErrMsg ( int err ) {
  DspErr::dspErrMsg( errLine, errFile, numErrs, err, errMsgs, arg );
}

void ErrHndlr::dspErrMsg ( void ) {
  DspErr::dspErrMsg( errLine, errFile, numErrs, mostRecentError, errMsgs, arg );
}
