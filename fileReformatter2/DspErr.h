//
// Created by jws3 on 5/19/24.
//

#ifndef FILEREFORMATTER2_DSPERR_H
#define FILEREFORMATTER2_DSPERR_H

#define ERRINFO(err)\
  errInfo(err,__LINE__, __FILE__)

#include <iostream>
#include <string>

class DspErr {

  public:
  static int mostRecentError;
  static void dspErrMsg ( int line, std::string file, int n, int err, const std::string msgs[] );
  static void dspErrMsg ( int line, std::string file, int n, const std::string msgs[] );

};

#endif //FILEREFORMATTER2_DSPERR_H
