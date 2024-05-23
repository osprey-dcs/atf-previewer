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

#include <iostream>
#include <string>
#include <cstdlib>

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "UserPrefs.h"

UserPrefs::UserPrefs() {

    QJsonDocument jd;
    QJsonObject jo;

    filename = getenv( "HOME" );
    filename += "/";
    filename += ss.c_str();

    int ok = readContents();
    //std::cout << "after readContents, ok = " << ok << std::endl;

  }

  int UserPrefs::getDouble(const QString &s, double& d ) {

    //std::cout << "UserPrefs::getDouble" << std::endl;

    QJsonValue jv = jo[s];

    if ( jv.isDouble() ) {
      d = jv.toDouble();
      // need to get these offsets using some reliable method
      if ( s == "x" ) d -= 5;
      if ( s == "y" ) d -= 29;
    }
    else {
      d = 0.0;
      return -1;
    }

    return 0;

  }

  int UserPrefs::setDouble(const QString &s, double d ) {

    jo[s] = d;

    return 0;

  }

  int UserPrefs::getString(const QString &s, QString &qs ) {

    QJsonValue jv = jo[s];

    if ( jv.isString() ) {
      qs = jv.toString();
    }
    else {
      qs = "";
      return -1;
    }

    return 0;

  }

  int UserPrefs::setString(const QString &s, QString qs ) {

    jo[s] = qs;

    return 0;

  }

  int UserPrefs::update() {

    jd.setObject(jo);

    QFile outf{ filename };
    bool result = outf.open( QIODevice::WriteOnly );
    if ( result ) {
        outf.write(jd.toJson(QJsonDocument::Indented));
        outf.close();
    }
    else {
      std::cout << "Viewer property file could not be opened for update" << std::endl;
      return -1;
    }

    return 0;

  }

  int UserPrefs::readContents () {

    QString contents;

    QFile inf( QString( UserPrefs::filename ) );
    bool result = inf.open( QIODevice::ReadOnly );
    if ( result ) {
      contents = inf.readAll();
      inf.close();
    }
    else {
      std::cout << "Viewer property file could not be opened for read" << std::endl;
      return -1;
    }

    jd = QJsonDocument::fromJson(contents.toUtf8());
    jo = jd.object();

    return 0;

  }
