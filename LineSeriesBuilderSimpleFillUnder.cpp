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

#include "LineSeriesBuilderSimpleFillUnder.h"

LineSeriesBuilderSimpleFillUnder::LineSeriesBuilderSimpleFillUnder () {
  
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

LineSeriesBuilderSimpleFillUnder::~LineSeriesBuilderSimpleFillUnder () {

}

int LineSeriesBuilderSimpleFillUnder::setXPixelWidth (int w ) {
 
  if ( w != 0 ) {
    this->xPixelWidth = static_cast<double>( w );
    //std::cout << "this->xPixelWidth = " << this->xPixelWidth << std::endl;
  }
  else {
    return ERRINFO(EVal,"");
  }

  return ESuccess;

}

int LineSeriesBuilderSimpleFillUnder::setXAxisLimits (double min, double max ) {
  
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

void LineSeriesBuilderSimpleFillUnder::setLineSeries(QtCharts::QLineSeries *_qls ) {

  qls = _qls;

}

void LineSeriesBuilderSimpleFillUnder::startNewSeries (bool clearLineSeries ) {
  
  init = true;
  if ( qls && clearLineSeries ) {
    qls->clear();
  }
  
}

// x must be strictly increasing
int LineSeriesBuilderSimpleFillUnder::addPoint (double x, double y ) {

  if ( !qls ) return ENull;
  if ( xPixelWidth == 0 ) return EVal;
  if ( xRange == 0 ) return ERange;
  
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

  qls->append( x , 0 );
  qls->append( x , y );
  qls->append( x , 0 );

  return ESuccess;
  
}

int LineSeriesBuilderSimpleFillUnder::processLastPoint(void ) {

  if ( !qls ) return ENull;
  return ESuccess;
  
}

double LineSeriesBuilderSimpleFillUnder::minX (void ) {
  return finalMinX;
}

double LineSeriesBuilderSimpleFillUnder::maxX (void ) {
  return finalMaxX;
}

double LineSeriesBuilderSimpleFillUnder::minY (void ) {
  return finalMinY;
}

double LineSeriesBuilderSimpleFillUnder::maxY (void ) {
  return finalMaxY;
}

void LineSeriesBuilderSimpleFillUnder::show() {

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
