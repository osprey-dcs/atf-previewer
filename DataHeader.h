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

#ifndef VIEWER_DataHeader_H
#define VIEWER_DataHeader_H

#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>
#include <iostream>
#include <map>
#include <tuple>
#include <list>

#include <QString>

class DataHeader {

  public:

    static const int EGU {0};
    static const int SLOPE {1};
    static const int INTERCEPT {2};
    static const int SIGINDEX {3};
  
    QJsonDocument jd;
    QJsonObject jo;
    //                          0        1       2          3
    //                          egu      slope   intercept  sig num
    std::map<QString,std::tuple<QString, double, double,    double>> sigs;
    std::list<QString> sigNameList;

    DataHeader();
    ~DataHeader();
  
    DataHeader( DataHeader&& ) = delete;
    DataHeader( DataHeader& ) = delete;
    DataHeader( const DataHeader& ) = delete;

    DataHeader& operator=( DataHeader&& ) = delete;
    DataHeader& operator=( DataHeader& ) = delete;
    DataHeader& operator=( const DataHeader& ) = delete;

    int getDouble(const QString &s, double& d );
    int setDouble(const QString &s, const double& d );
    int getString(const QString &s, QString& ss );
    int getString(const QString &s, std::string& ss );
    int setString(const QString &s, const QString& ss );
    int update( QString filename );
    int readContents ( QString filename );
    int getSigInfoBySigIndex ( int sigIndex, QString& name, QString& egu, double& slope, double& intercept );

    const std::map<QString,std::tuple<QString, double, double, double>>& getNameMap();
    const std::list<QString>& getNameList();

  };

#endif //VIEWER_DataHeader_H
