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

//namespace osp {

  class UserPrefs {

  public:

    QJsonDocument jd;
    QJsonObject jo;

   static QString filename;

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

//} // osp

#endif //VIEWER_USERPREFS_H

