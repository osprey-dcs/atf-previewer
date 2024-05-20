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

#include "DspErr.h"

class DataHeader {

  public:

    static const int NumErrs = 6;
    static const int ESuccess = 0;
    static const int EInFileOpen = 1;
    static const int ETypeD = 2;
    static const int ETypeS = 3;
    static const int EOutFileOpen = 4;
    static const int ESigIndex = 5;
    inline static const std::string errMsgs[NumErrs] {
      { "Success" },
      { "Input file open failure" },
      { "Type mismatch, expected double" },
      { "Type mismatch, expected string" },
      { "Output file open failure" },
      { "Unknown signal index failure" }
    };
  
    typedef std::map<QString,std::tuple<QString, double, double, double,
                                        double, QString, QString, double, double, double, double,
                                        double, double, double, QString>> DataHeaderMapType;

    typedef std::map<double,std::tuple<QString, double, double, QString,
                                       double, QString, QString, double, double, double, double,
                                       double, double, double, QString>> DataHeaderIndexMapType;
  
    typedef std::list<QString> DataHeaderListType;

    static const int EGU {0};
    static const int SLOPE {1};
    static const int INTERCEPT {2};
    static const int SIGINDEX {3};
    static const int SIGNAME {3};
    static const int TYPE {4};
    static const int YAXISLABEL {5};
    static const int DESC {6};
    static const int RESPONSENODE {7};
    static const int RESPONSEDIRECTION {8};
    static const int REFERENCENODE {9};
    static const int REFERENCEDIRECTION {10};
    static const int COUPLING {11};
    static const int ADDRESS_CHASSIS {12};
    static const int ADDRESS_CHANNEL {13};
    static const int DATA_FILENAME {14};
  
    QJsonDocument jd;
    QJsonObject jo;
    //                            0        1       2          3        4       5        6        7         8        9        10        11        12            13            14
    //                            egu      slope   intercept  sig num  type    Y label  desc     rsp node  rsp dir  ref node  ref dir  coupling  addr chassis  addr channel  data filename
    //std::map<QString,std::tuple<QString, double, double,    double,  double, QString, QString, QString,  double,  QString,  double,  double,   double,       double,       QString>> sigs;
    //std::list<QString> sigNameList;

    DataHeaderMapType sigs;
    DataHeaderIndexMapType sigsByIndex;
    DataHeaderListType sigNameList;

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
    QString getString(const QString &s );
    int getString(const QString &s, QString& ss );
    int getString(const QString &s, std::string& ss );
    int setString(const QString &s, const QString& ss );
    int writeNewHeaderFile ( int chassisNum, std::map<int,QString>& fileMap, const QString& inFile,
                             const QString& outFile, bool verbose=false );
    int update( const QString& filename );
    int readContents ( const QString& filename );
    int getSigInfoBySigIndex ( int sigIndex, QString& name, QString& egu,
                               double& slope, double& intercept );

    const DataHeaderListType& getNameList() const;

    const DataHeaderMapType& getNameMap() const;
  
    const DataHeaderIndexMapType& getIndexMap() const;
  
    int mostRecentError {0};
    int errLine {0};
    std::string errFile;
    int errInfo ( int err, int line=0, std::string file="" ) {
      mostRecentError = err;
      errLine = line;
      errFile = file;
      return err;
    }
    void dspErrMsg ( int err ) {
      DspErr::dspErrMsg( errLine, errFile, NumErrs, err, errMsgs );
    }
    void dspErrMsg ( void ) {
      DspErr::dspErrMsg( errLine, errFile, NumErrs, mostRecentError, errMsgs );
    }
  
  };

#endif //VIEWER_DataHeader_H
