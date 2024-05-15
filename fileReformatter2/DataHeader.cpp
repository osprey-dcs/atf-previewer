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
  sigsByIndex.clear();
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

  static void clearJsonObj ( QJsonObject& jobj ) {

    //std::cout << "\nclearJsonObj\n";
    
    QJsonObject::iterator it = jobj.begin();
    while ( it != jobj.end() ) {
      //QString qsk = it.key();
      //QString qsv = it.value().toString();
      //std::cout << qsk.toStdString() << "  " << qsv.toStdString() << std::endl;
      jobj.erase( it );
      it = jobj.begin();
    }

    //std::cout << "\n";

  }

  static bool doCopy ( int recChassis, int recSigNum, int inputChassis, std::map<int,QString>& fileMap ) {

    if ( recChassis != inputChassis ) return false;

    try {
      QString outDataFile = fileMap[recSigNum];
      if ( outDataFile == "" ) return false;
    }
    catch ( const std::exception& e ) {
      return false;
    }

    return true;

  }

  int DataHeader::writeNewHeaderFile ( int chassisNum, std::map<int,QString>& fileMap, const QString& inFile,
                                       const QString& outFile, bool verbose ) {


    if ( verbose ) std::cout << "DataHeader::writeNewHeaderFile, inFile = " << inFile.toStdString() << std::endl;

    // Note that the signal number being written consists of up to 32 items ranging from 1 to 1024 starting
    // from ( chassisNum - 1 ) * 32 + 1. The fileMap key is the signal number which ranges from 1 to 1024.
    //
    // the "Address" object contains the chassis number and the channel number. The channel number ranges
    // from 1 to 32
    //
    // We only copy json records if the record chassis number matches the parameter chassisNum and
    // fileMap[record signal number] is non-blank

    QFile inf( inFile );
    bool result = inf.open( QIODevice::ReadOnly );
    if ( !result ) return -1;
    QString contents = inf.readAll();
    inf.close();

    QJsonDocument jdoc = QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject jobj = jdoc.object();

    QJsonDocument jdocNew;
    QJsonObject jobjNew;
    QJsonArray jvaNew;
    QJsonValue jvNew;
    QString qsKey {"OutDataFile"};

    jobjNew["AcquisitionId"] = jobj["AcquisitionId"];
    jobjNew["AcquisitionId"] = jobj["AcquisitionId"];
    jobjNew["AcquisitionStartDate"] = jobj["AcquisitionStartDate"];
    jobjNew["AcquisitionEndDate"] = jobj["AcquisitionEndDate"];
    jobjNew["AcquisitionStartDate2"] = jobj["AcquisitionStartDate2"];
    jobjNew["AcquisitionEndDate2"] = jobj["AcquisitionEndDate2"];
    jobjNew["Role1Name"] = jobj["Role1Name"];
    jobjNew["SampleRate"] = jobj["SampleRate"];

    QJsonValue jv;
    jv = jobj["Signals"];
    if ( jv.isArray() ) {

      QJsonArray jva = jv.toArray();

      for ( QJsonValue jv1 : jva ) {

        if ( jv1.isObject() ) {

          QJsonObject jobj1New;

          //std::cout << jv1["Name"].toString().toStdString() << std::endl;

          QJsonObject jobj1 = jv1.toObject();
          
          int recSigNum = jobj1["SigNum"].toInt();
          
          QJsonObject qjo = jv1["Address"].toObject();
          int recChassisNum = qjo["Chassis"].toDouble();
          
          QJsonObject::iterator it = jobj1.begin();
          while ( it != jobj1.end() ) {
            
            QString qsk = it.key();

            jobj1New[it.key()] = it.value();
            it++;
            
          }
          
          QString outputDatafileName = fileMap[recSigNum]; // add the file here but the entire record may
                                                        // be discarded below
          jobj1New[qsKey] = (QJsonValue) QString( outputDatafileName );

          if ( verbose ) std::cout << "rec chassis num = " << recChassisNum << std::endl;
          if ( verbose ) std::cout << "rec sig num = " << recSigNum << ", outputDatafileName = " <<
           outputDatafileName.toStdString() << std::endl;
          
          if ( doCopy( recChassisNum, recSigNum, chassisNum, fileMap ) ) {
            jvaNew.append( jobj1New );
          }
          
          clearJsonObj( jobj1New );
          
        }

      }

      jobjNew["Signals"] = jvaNew;

    }

    jdocNew.setObject( jobjNew );

    //qDebug() << jdocNew;

    if ( verbose ) std::cout << "write out file " << outFile.toStdString() << std::endl;

    QFile outf{ outFile };
    result = outf.open( QIODevice::WriteOnly );
    if ( result ) {
        outf.write(jdocNew.toJson(QJsonDocument::Indented));
        outf.close();
    }
    else {
      std::cout << "Viewer header file could not be opened" << std::endl;
      return -1;
    }
    
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
      return -1;
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
    sigsByIndex.clear();

    jd = QJsonDocument::fromJson(contents.toUtf8());
    jo = jd.object();

    QJsonValue jv;
    jv = jo["Signals"];
    if ( jv.isArray() ) {

      QJsonArray jva = jv.toArray();
      for ( QJsonValue jv1 : jva ) {

        if ( jv1.isObject() ) {

          QJsonObject qjo = jv1["Address"].toObject();
          auto tp = std::make_tuple
            (
             jv1["Egu"].toString(),
             jv1["Slope"].toDouble(),
             jv1["Intercept"].toDouble(),
             jv1["SigNum"].toDouble(),
             jv1["Type"].toDouble(),
             jv1["YAxisLabel"].toString(),
             jv1["Desc"].toString(),
             jv1["ResponseNode"].toString(),
             jv1["ResponseDirection"].toDouble(),
             jv1["ReferenceNode"].toString(),
             jv1["ReferenceDirection"].toDouble(),
             jv1["Coupling"].toDouble(),
             qjo["Chassis"].toDouble(),
             qjo["Channel"].toDouble() );

             sigs[jv1["Name"].toString()] = tp;
             sigNameList.push_back( jv1["Name"].toString() );

          qjo = jv1["Address"].toObject();
          auto tp1 = std::make_tuple
            (
             jv1["Egu"].toString(),
             jv1["Slope"].toDouble(),
             jv1["Intercept"].toDouble(),
             jv1["Name"].toString(),
             jv1["Type"].toDouble(),
             jv1["YAxisLabel"].toString(),
             jv1["Desc"].toString(),
             jv1["ResponseNode"].toString(),
             jv1["ResponseDirection"].toDouble(),
             jv1["ReferenceNode"].toString(),
             jv1["ReferenceDirection"].toDouble(),
             jv1["Coupling"].toDouble(),
             qjo["Chassis"].toDouble(),
             qjo["Channel"].toDouble() );
                
             sigsByIndex[jv1["SigNum"].toDouble()] = tp1;

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

int DataHeader::getSigInfoBySigIndex ( int sigIndex, QString& name, QString& egu,
                                       double& slope, double& intercept ) {

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

//const std::list<QString>& DataHeader::getNameList() {
//  return sigNameList;
//}

//const std::map<QString,std::tuple<QString, double, double, double, double, QString, QString, QString, double, QString, double>>& DataHeader::getNameMap() {
//  return sigs;
//};

const DataHeader::DataHeaderListType& DataHeader::getNameList() {
  return sigNameList;
}

const DataHeader::DataHeaderMapType& DataHeader::getNameMap() {
  return sigs;
};

const DataHeader::DataHeaderIndexMapType& DataHeader::getIndexMap() {
  return sigsByIndex;
};