//
// Created by jws3 on 5/19/24.
//

#ifndef FILEREFORMATTER2_DSPERR_H
#define FILEREFORMATTER2_DSPERR_H

#define ERRINFO(err,arg) \
  errInfo(err,arg,__LINE__, __FILE__)

#include <iostream>
#include <string>
#include <vector>

class DspErr {

  public:
  static int mostRecentError;
  static void dspErrMsg ( int line, const std::string& file, int n, int err,
                          std::vector<std::string>& msgs, const std::string& arg );

};

#endif //FILEREFORMATTER2_DSPERR_H
