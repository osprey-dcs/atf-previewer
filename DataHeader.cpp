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

#include <tuple>
#include <map>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <iostream>
#include <string>
#include <QDebug>

#include "DataHeader.h"
#include "DataHeaderFac.h"

DataHeader::DataHeader() {

  //int ok = readContents();
  //std::cout << "after readContents, ok = " << ok << std::endl;

}

DataHeader::~DataHeader() {

  sigs.clear();
  sigNameList.clear();
  
}

int DataHeader::getDouble(const QString &s, double& d ) {

  QJsonValue jv = jo[s];

  if ( jv.isDouble() ) {
    d = jv.toDouble();
  }
  else {
    d = 0.0;
    return -1;
  }

  return 0;

}

int DataHeader::setDouble(const QString &s, const double& d ) {

  jo[s] = d;

  return 0;

}

  int DataHeader::getString(const QString &s, QString& qs ) {

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

  int DataHeader::getString(const QString &s, std::string& ss ) {

  QJsonValue jv = jo[s];

    if ( jv.isString() ) {
      QString qs = jv.toString();
      ss = qs.toStdString();
    }
    else {
      ss = "";
      return -1;
    }

    return 0;

  }

  int DataHeader::setString(const QString &s, const QString &ss ) {

    jo[s] = ss;

    return 0;

  }

  int DataHeader::update( QString filename ) {

    jd.setObject(jo);

    QFile outf{ filename };
    bool result = outf.open( QIODevice::WriteOnly );
    if ( result ) {
        outf.write(jd.toJson(QJsonDocument::Indented));
        outf.close();
    }
    else {
      std::cout << "Viewer header file could not be opened" << std::endl;
    }

    return 0;

  }

  int DataHeader::readContents ( QString filename ) {

    QFile inf( filename );
    bool result = inf.open( QIODevice::ReadOnly );
    if ( !result ) return -1;
    QString contents = inf.readAll();
    inf.close();

    sigNameList.clear();
    sigs.clear();

    jd = QJsonDocument::fromJson(contents.toUtf8());
    jo = jd.object();

    QJsonValue jv;
    jv = jo["Signals"];
    if ( jv.isArray() ) {

      QJsonArray jva = jv.toArray();
      for ( QJsonValue jv1 : jva ) {

        if ( jv1.isObject() ) {
          auto tp = std::make_tuple(
             jv1["Egu"].toString(),
             jv1["Slope"].toDouble(),
             jv1["Intercept"].toDouble(),
             jv1["SigNum"].toDouble() );

             sigs[jv1["Name"].toString()] = tp;

             sigNameList.push_back( jv1["Name"].toString() );

        }

      }

    }
    
    sigNameList.sort();

    // uesr of this class does the following:
    /*DataHeaderFac dhf;
      DataHeader *dh = dhf.createDataHeader();
      int stat = dh->readContents( "/home/osprey/viewer/data/test2.hdr" );*/
    /*for( auto const& [key,val] : sigs ) {
      qDebug() << "key = " << key << " : ";
      qDebug() << "  Egu       = " << std::get<EGU>(val);
      qDebug() << "  Slope     = " << std::get<SLOPE>(val);
      qDebug() << "  Intercept = " << std::get<INTERCEPT>(val);
      qDebug() << "  SigIndex  = " << std::get<SIGINDEX>(val);
    }

    qDebug() << "";

    for ( auto& nm : sigNameList ) {
      qDebug() << nm;
    }

    qDebug() << "";*/

    return 0;

  }

int DataHeader::getSigInfoBySigIndex ( int sigIndex, QString& name, QString& egu, double& slope, double& intercept ) {

  name = "";
  egu = "";
  slope = 1.0;
  intercept = 0.0;
  
  for ( auto it = sigs.begin(); it != sigs.end(); it++ ) {

    QString qs = it->first;
    
    if ( sigIndex == ( std::get<SIGINDEX>( it->second ) ) ) {

      name = qs;
      egu = std::get<EGU>( it->second );
      slope  = std::get<SLOPE>( it->second );
      intercept = std::get<INTERCEPT>( it->second );

      return 0;

    }

  }

  return -1;

}

const std::list<QString>& DataHeader::getNameList() {
  return sigNameList;
}

const std::map<QString,std::tuple<QString, double, double, double>>& DataHeader::getNameMap() {
  return sigs;
};
