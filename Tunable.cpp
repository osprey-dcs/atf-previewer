//
// Created by jws3 on 5/21/24.
//
#include <iostream>

#include "Tunable.h"
#include "Cnst.h"

QString Tunable::getString( const std::string& key, const std::string& cnstName ) {

  QString stringVal;

  char *val = getenv( key.c_str() );
  if ( val ) {
    stringVal = val;
  }
  else {
    stringVal = cnstName.c_str();
  }

  return stringVal;

}
