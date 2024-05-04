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
    QString getString(const QString &s );
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
