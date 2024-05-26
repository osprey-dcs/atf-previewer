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

#ifndef __LineSeriesBuilderBase_h
#define __LineSeriesBuilderBase_h 1

#include <QtCharts/QLineSeries>

#include "ErrHndlr.h"

class LineSeriesBuilderBase : public ErrHndlr {

public:

  static const int NumErrs = 5;
  static const int ESuccess = 0;
  static const int EVal = 1;
  static const int ERange = 2;
  static const int ENotIncr =3;
  static const int ENull = 4;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
    {"Pixel width value cannot be 0"},
    {"X scale range cannot be 0"},
    {"Data is not strictly increasing"},
    {"Null pointer detected"}
  };

  LineSeriesBuilderBase ();
  virtual ~LineSeriesBuilderBase () { };
  virtual int setXPixelWidth ( int ) = 0;
  virtual int setXAxisLimits ( double, double ) = 0;
  virtual void setLineSeries ( QtCharts::QLineSeries *_qsl ) = 0;
  virtual void startNewSeries ( bool clearLineSeries=false ) = 0;
  virtual int addPoint ( double, double ) = 0;
  virtual int processLastPoint ( void ) = 0;
  virtual double minX ( void ) = 0;
  virtual double maxX ( void ) = 0;
  virtual double minY ( void ) = 0;
  virtual double maxY  ( void ) = 0;
  virtual void show() = 0;

};

#endif
