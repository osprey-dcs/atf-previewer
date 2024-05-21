//
// Created by jws3 on 5/21/24.
//

#ifndef VIEWER_TUNABLE_H
#define VIEWER_TUNABLE_H

#include <cstdlib>
#include <string>

#include <QString>

class Tunable {

public:
  static QString getString( const std::string& key, const std::string& defaultVal );

};


#endif //VIEWER_TUNABLE_H
