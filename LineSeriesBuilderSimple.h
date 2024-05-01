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

#ifndef __LineSeriesBuilderSimple_h
#define __LineSeriesBuilderSimple_h 1

#include <iostream>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <string>
#include <string.h>

#include <QtCharts/QLineSeries>

#include "LineSeriesBuilderBase.h"

class LineSeriesBuilderSimple : public LineSeriesBuilderBase {

public:

  static const int ESuccess = 0; // error numeric codes must start with zero and increment by one
  static const int EVal = 1;
  static const int ERange = 2;
  static const int ENotIncr = 3;
  static const int ENull = 4;

  static const int ErrLast = 4; // where N = is the largest error number (from the above example, 3

  inline static const std::string ErrMsg[ErrLast+1] = {
  "Success",
  "Pixel width value cannot be 0",
  "X scale range cannot be 0",
  "Data is not strictly increasing",
  "Null pointer detected" };

  explicit LineSeriesBuilderSimple ();
  ~LineSeriesBuilderSimple ();
  const std::string getErrMsg ( int st );
  int setXPixelWidth ( int );
  int setXAxisLimits ( double, double );
  void setLineSeries ( QtCharts::QLineSeries *_qls ); 
  void startNewSeries ( bool clearLineSeries=false );
  int addPoint ( double, double );
  int processLastPoint ( void );
  double minX  ( void );
  double maxX ( void );
  double minY ( void );
  double maxY  ( void );
  void show();
  double xPixelWidth;
  double xMin;
  double xMax;
  double prevX;
  double xRange;
  bool init;
  double finalMinX, finalMaxX;
  double finalMinY, finalMaxY;
  QtCharts::QLineSeries *qls;

};

#endif
