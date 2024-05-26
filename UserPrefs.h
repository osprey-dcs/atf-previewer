/*
This file is part of viewer.

viewer is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

viewer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with viewer.
If not, see <https://www.gnu.org/licenses/>. 
*/

//
// Created by jws3 on 4/2/24.
//

#ifndef VIEWER_USERPREFS_H
#define VIEWER_USERPREFS_H

#include <string>
#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>
#include <iostream>

#include "ErrHndlr.h"

class UserPrefs : public ErrHndlr {

public:

    static const int NumErrs = 4;
    static const int ESuccess = 0;
    static const int EInFileOpen = 1;
    static const int ETypeD = 2;
    static const int ETypeS = 3;
    inline static const std::string errMsgs[NumErrs] {
      { "Success" },
      { "Input file open failure: " },
      { "Type mismatch, expected double" },
      { "Type mismatch, expected string" }
    };


  inline static const std::string ss {".viewerProps.json"};

  QJsonDocument jd;
  QJsonObject jo;

  QString filename;

  UserPrefs();

  UserPrefs ( UserPrefs&& ) = delete;
  UserPrefs ( UserPrefs& ) = delete;
  UserPrefs ( const UserPrefs& ) = delete;

  UserPrefs& operator= ( UserPrefs&& ) = delete;
  UserPrefs& operator= ( UserPrefs& ) = delete;
  UserPrefs& operator= ( const UserPrefs& ) = delete;

  int getDouble (const QString &s, double& d );
  int setDouble ( const QString &s, double d );

  int getString ( const QString &s, QString &qs );
  int setString ( const QString &s, QString qs );

  int update();
  int readContents ();

};

#endif //VIEWER_USERPREFS_H

