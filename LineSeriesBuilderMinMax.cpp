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

#include "LineSeriesBuilderMinMax.h"

LineSeriesBuilderMinMax::LineSeriesBuilderMinMax () {
  
  xPixelWidth = 1.0;
  xMin = 1.0;
  xMax = 2.0;
  xRange = 1.0;
  xRes = 1.0;
  xFact = 1.0;
  finalMinX = 0.0;
  finalMaxX = 0.0;
  finalMinY = 0.0;
  finalMaxY = 0.0;
  init = false;
  qls = nullptr;

}

LineSeriesBuilderMinMax::~LineSeriesBuilderMinMax () {
  
}

int LineSeriesBuilderMinMax::setXPixelWidth ( int w ) {

  if ( w != 0 ) {
    this->xPixelWidth = static_cast<double>( w );
  }
  else {
    return ERRINFO(EVal,"");
  }

  return ESuccess;

}

int LineSeriesBuilderMinMax::setXAxisLimits ( double min, double max ) {
  
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

void LineSeriesBuilderMinMax::setLineSeries( QtCharts::QLineSeries *_qls ) {

  qls = _qls;

}

void LineSeriesBuilderMinMax::startNewSeries ( bool clearLineSeries ) {
  
  init = true;
  xRes = this->xRange / this->xPixelWidth;
  //std::cout << "xRes = " << xRes << std::endl;
  xFact = xPixelWidth / xRange;
  //std::cout << "xFact = " << xFact << std::endl;
  if ( qls && clearLineSeries ) {
    qls->clear();
  }

}

// x must be strictly increasing
int LineSeriesBuilderMinMax::addPoint ( double x, double y ) {

  if ( !qls ) return ERRINFO(ENull,"");
  if ( xPixelWidth == 0 ) return ERRINFO(EVal,"");
  if ( xRange == 0 ) return ERRINFO(ERange,"");
  
  if ( init ) {
    init = false;
    noPrev = true;
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

  double xWithPixRes = round ( ( x - xMin ) * xFact );
  xWithPixRes = xWithPixRes / xPixelWidth * xRange + xMin;

  if ( noPrev ) {
    yfirst = y;
    ylast = y;
    ymin = y;
    ymax = y;
    prevXWithPixRes = xWithPixRes;
    noPrev = false;
    //std::cout << "noPrev" << std::endl;
  }
  else if ( xWithPixRes == prevXWithPixRes ) {
    ylast = y;
    if ( y < ymin ) ymin = y;
    if ( y > ymax ) ymax = y;
    //std::cout << "==" << std::endl;
  }
  else {
    qls->append( prevXWithPixRes, yfirst );
    qls->append( prevXWithPixRes, ymin );
    qls->append( prevXWithPixRes, ymax );
    qls->append( prevXWithPixRes, ylast );
    yfirst = y;
    ylast = y;
    ymin = y;
    ymax = y;
    prevXWithPixRes = xWithPixRes;
    //std::cout << "!=" << std::endl;
  }

  return ESuccess;
  
}

int LineSeriesBuilderMinMax::processLastPoint ( void ) {
  
  if ( !qls ) return ERRINFO(ENull,"");
  
  qls->append( prevXWithPixRes, yfirst );
  qls->append( prevXWithPixRes, ymin );
  qls->append( prevXWithPixRes, ymax  );
  qls->append( prevXWithPixRes, ylast  );

  return ESuccess;
  
}

double LineSeriesBuilderMinMax::minX ( void ) {
  return finalMinX;
}

double LineSeriesBuilderMinMax::maxX ( void ) {
  return finalMaxX;
}

double LineSeriesBuilderMinMax::minY ( void ) {
  return finalMinY;
}

double LineSeriesBuilderMinMax::maxY ( void ) {
  return finalMaxY;
}

void LineSeriesBuilderMinMax::show () {

  int count = 0;

  if ( !qls ) return;

  //qDebug() << "";

  int i = 0;
  for ( auto p : qls->points() ) {
    //qDebug() << i << ", p = " << p;
    i++;
  }
  
}
