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
// Created by jws3 on 4/5/24.
//
#include <iostream>
#include <iomanip>
#include <cmath>

#include <QDebug>

#include "ViewerGraphBase.h"

static QColor QColorBlack( "black" );
static QColor QColorWhite( "white" );

ViewerGraphBase::ViewerGraphBase( int _id, DataType _dataType, QWidget *_parent ) {

  id = _id;
  parent1 = _parent;
  dataType = _dataType;
  haveDimensions = false;
  curX0 = curY0 = curX1 = curY1 = curW = curH = 0;

}

ViewerGraphBase::~ViewerGraphBase( ) {

}

void ViewerGraphBase::setYTitle( std::string& s ) {

}

bool ViewerGraphBase::eventFilter( QObject *watched, QEvent *event ) {

  return false;
  
}

void ViewerGraphBase::getAxesLimits( double& x0, double &y0,
                                  double &x1, double& y1 ) {

}

void ViewerGraphBase::setAxesLimits( double x0, double y0,
                                 double x1, double y1, bool adjScales ) {

}

void ViewerGraphBase::getPlotSize( double& w, double &h ) {

  if ( !haveDimensions ) {
    w = (double) this->chart->size().width();
    h = (double) this->chart->size().height();
  }
  else {
    w = curW;
    h = curH;
  }

}

void ViewerGraphBase::getPlotPos( double& x0, double& y0, double& x1, double& y1 ) {

  if ( !haveDimensions ) {
    x0 = (double) this->chart->pos().x();
    y0 = (double) this->chart->pos().y();
    x1 = (double) this->chart->pos().x() + (double) this->chart->size().width();
    y1 = (double) this->chart->pos().y() + (double) this->chart->size().height();
  }
  else {
    x0 = curX0;
    y0 = curY0;
    x1 = curX1;
    y1 = curY1;
  }

}

void ViewerGraphBase::setSeries ( QtCharts::QLineSeries *newQls, int sigIndex, QString fileName, double minX,
                              double maxX, double minY, double maxY, bool adjScales ) {

}

void ViewerGraphBase::setSeries ( QtCharts::QLineSeries *newQls, int sigIndex, QString fileName, double minX,
                              double maxX, bool adjScales ) {

}

void ViewerGraphBase::setEmptySeries ( QtCharts::QLineSeries *newQls ) {

}

int ViewerGraphBase::getCurSigIndex ( void ) {

  return this->curSigIndex;

}

QString& ViewerGraphBase::getCurFileName ( void ) {

  return this->curFileName;

}

void ViewerGraphBase::clear( void ) {

  QtCharts::QLineSeries *emptyQls = new QtCharts::QLineSeries();
  this->setEmptySeries( emptyQls );
  isEmpty = true; // redundant

}

bool ViewerGraphBase::inside( int x, int y ) {

  if ( ( x >= 0 ) && ( x <= chart->plotArea().width() ) &&
       ( y >= 0 ) && ( y <= ( chart->plotArea().height() ) ) ) {
    return true;
  }
  else {
    return false;
  }

}

void ViewerGraphBase::enterEvent( QEvent *ev ) {
  setFocus();
  shiftState = false;
  ctrlState = false;
}

void ViewerGraphBase::wheelEvent( QWheelEvent *ev ) {

}

void ViewerGraphBase::mousePressEvent( QMouseEvent *ev ) {

}

void ViewerGraphBase::mouseReleaseEvent( QMouseEvent *ev ) {
  
}

void ViewerGraphBase::mouseMoveEvent( QMouseEvent *ev ) {
  
}

void ViewerGraphBase::keyPressEvent( QKeyEvent *ev ) {

}

void ViewerGraphBase::keyReleaseEvent( QKeyEvent *ev ) {

}

void ViewerGraphBase::performAction ( QAction *a ) {

}

void ViewerGraphBase::zoomFullScale ( void ) {
    
}

void ViewerGraphBase::zoomIn ( double zoomDistX, double zoomDistY ) {
    
}

void ViewerGraphBase::zoomOut ( double zoomDistX, double zoomDistY ) {
    
}

void ViewerGraphBase::panHorizontal ( double panDist ) {
    
}

void ViewerGraphBase::panVertical ( double panDist ) {
    
}

void ViewerGraphBase::getBetterAxesParams ( double min, double max, int ticks,
                                        double& adj_min, double& adj_max, int& num_label_ticks, bool adjScales ) {

}

void ViewerGraphBase::dimensionChange( const QRectF &r ) {

  //std::cout << "ViewerGraphBase::dimensionChange" << std::endl;

  //std::cout << "r.x() = " << r.x() << std::endl;
  //std::cout << "r.y() = " << r.y() << std::endl;
  //std::cout << "r.size().width() = " << r.size().width() << std::endl;
  //std::cout << "r.size().height() = " << r.size().height() << std::endl;

  curX0 = r.x();
  curY0 = r.y();
  curW = r.size().width();
  curH = r.size().height();
  curX1 = curX0 + curW;
  curY1 = curY0 + curH;

  haveDimensions = true;

}
