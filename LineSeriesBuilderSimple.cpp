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

#include <QList>
#include <QPointF>
#include <QDebug>

#include "LineSeriesBuilderSimple.h"

LineSeriesBuilderSimple::LineSeriesBuilderSimple () {
  
  xPixelWidth = 1.0;
  xMin = 1.0;
  xMax = 2.0;
  xRange = 1.0;
  finalMinX = 0.0;
  finalMaxX = 0.0;
  finalMinY = 0.0;
  finalMaxY = 0.0;
  init = false;
  qls = nullptr;

}

LineSeriesBuilderSimple::~LineSeriesBuilderSimple () {

}

int LineSeriesBuilderSimple::setXPixelWidth (int w ) {
  
  int stat;
 
  if ( w != 0 ) {
    this->xPixelWidth = static_cast<double>( w );
    //std::cout << "this->xPixelWidth = " << this->xPixelWidth << std::endl;
  }
  else {
    return ERRINFO(EVal,"");
  }

  return ESuccess;

}

int LineSeriesBuilderSimple::setXAxisLimits (double min, double max ) {
  
  int stat;
  
  double r = max - min;
  xMin = min;
  xMax = max;
  if ( r != 0 ) {
    this->xRange = r;
    //std::cout << "r = " << r << std::endl;
  }
  else {
    return ERRINFO(ERange,"");
  }
  
  return ESuccess;

}

void LineSeriesBuilderSimple::setLineSeries(QtCharts::QLineSeries *_qls ) {

  qls = _qls;

}

void LineSeriesBuilderSimple::startNewSeries (bool clearLineSeries ) {
  
  init = true;
  if ( qls && clearLineSeries ) {
    qls->clear();
  }
  
}

// x must be strictly increasing
int LineSeriesBuilderSimple::addPoint (double x, double y ) {

  if ( !qls ) return ERRINFO(ENull,"");
  if ( xPixelWidth == 0 ) return ERRINFO(EVal,"");
  if ( xRange == 0 ) return ERRINFO(ERange,"");
  
  if ( init ) {
    init = false;
    finalMaxX = x;
    finalMinX = x;
    finalMaxY = y;
    finalMinY = y;
  }
  else {
    finalMaxX = std::max( x, finalMaxX );
    finalMinX = std::min( x, finalMinX );
    finalMaxY = std::max( y, finalMaxY );
    finalMinY = std::min( y, finalMinY );
    if ( x < prevX ) {
      return ERRINFO(ENotIncr,"");
    }
  }
  prevX = x;

  qls->append( x , y );

  return ESuccess;
  
}

int LineSeriesBuilderSimple::processLastPoint(void ) {

  if ( !qls ) return ERRINFO(ENull,"");
  return ESuccess;
  
}

double LineSeriesBuilderSimple::minX (void ) {
  return finalMinX;
}

double LineSeriesBuilderSimple::maxX (void ) {
  return finalMaxX;
}

double LineSeriesBuilderSimple::minY (void ) {
  return finalMinY;
}

double LineSeriesBuilderSimple::maxY (void ) {
  return finalMaxY;
}

void LineSeriesBuilderSimple::show() {

  if ( !qls ) return;
  
  //qDebug() << "";

  int i = 0;
  for ( auto p : qls->points() ) {
    //qDebug() << i << ", p = " << p;
    i++;
  }

  //qDebug() << "";
  
  //qDebug() << "finalMinX = " << finalMinX << ", finalMaxX = " << finalMaxX << ", finalMinY = "
  //          << finalMinY << ", finalMaxY = " << finalMaxY;

  //qDebug() << "";

}
