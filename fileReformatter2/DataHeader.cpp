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
//#include "DataHeaderFac.h"

DataHeader::DataHeader() {

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

QString DataHeader::getString(const QString &s ) {

  QString qs;
  QJsonValue jv = jo[s];

    if ( jv.isString() ) {
      qs = jv.toString();
    }
    else {
      qs = "";
    }

    return qs;

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

  int DataHeader::updateAll( QString filename ) {

    jd1.setObject(jo1);

    QFile outf{ filename };
    bool result = outf.open( QIODevice::WriteOnly );
    if ( result ) {
        outf.write(jd1.toJson(QJsonDocument::Indented));
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

  int DataHeader::readAllContents ( QString filename ) {


  QJsonArray ja;
  QJsonObject *jo;

  jo = new QJsonObject;
  jo->insert("one","111");
  jo->insert("twp", 222);

  ja.push_back( *jo );

  delete jo;
  jo = new QJsonObject;
  jo->insert("one1","111");
  jo->insert("twp1", 222);

  ja.push_back( *jo );

  delete jo;
  jo1.insert( "signals", ja );

  return 0;

    QFile inf( filename );
    bool result = inf.open( QIODevice::ReadOnly );
    if ( !result ) return -1;
    QString contents = inf.readAll();
    inf.close();

    jd1 = QJsonDocument::fromJson(contents.toUtf8());
    jo1 = jd1.object();

    QJsonValue jv;
    jv = jo1["Signals"];
    if ( jv.isArray() ) {

      QJsonArray jva = jv.toArray();
      for ( QJsonValue jv1 : jva ) {

        if ( jv1.isObject() ) {

          QJsonObject jvo1 = jv1.toObject();
          jvo1.insert( "111", "222" );
          QJsonObject::iterator it = jvo1.begin();
          while ( it != jvo1.end() ) {

            qDebug() << it.key() << "  " << *it;
            it++;

          }

        }

        std::cout << "-------------------------------------\n";

      }

    }
    
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
