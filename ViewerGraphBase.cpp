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

//  qls = new QtCharts::QLineSeries();
//
//  QPen pen( QRgb( 0x000000 ));
//  pen.setWidth( 1 );
//  qls->setPen( pen );
//
//  chart = QSharedPointer<ViewerChart>( new ViewerChart() );
//  QMargins m( 0, 0, 0, 0 );
//  chart->setMargins( m );
//  chart->legend()->hide();
//  chart->setParent( dynamic_cast<QObject*>(this) );
//  chart->addSeries( qls );
//
//  this->setRenderHint( QPainter::Antialiasing );
//  this->setChart( chart.data() );
//
//  w = 200;
//  h = 100;
//  this->setMinimumSize( w, h );
//  this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
//
//  this->setRubberBand( QtCharts::QChartView::RectangleRubberBand );
//
//  this->curX = this->curY = 0;
//  this->popupMenu = QSharedPointer<QMenu>( new QMenu() );
//  
//  prevViewAction = QSharedPointer<QAction>( new QAction( "&Previous" ) );
//  prevViewAction->setEnabled( false );
//  this->popupMenu->addAction( prevViewAction.data() );
//
//  this->popupMenu->addSeparator();
//
//  zoomInAction = QSharedPointer<QAction>( new QAction( "Zoom In" ) );
//  this->popupMenu->addAction( zoomInAction.data() );
//  
//  zoomOutAction = QSharedPointer<QAction>( new QAction( "Zoom Out" ) );
//  this->popupMenu->addAction( zoomOutAction.data() );
//  
//  zoomXInAction = QSharedPointer<QAction>( new QAction( "Zoom In X" ) );
//  this->popupMenu->addAction( zoomXInAction.data() );
//  
//  zoomXOutAction = QSharedPointer<QAction>( new QAction( "Zoom Out X" ) );
//  this->popupMenu->addAction( zoomXOutAction.data() );
//  
//  zoomYInAction = QSharedPointer<QAction>( new QAction( "Zoom In Y" ) );
//  this->popupMenu->addAction( zoomYInAction.data() );
//  
//  zoomYOutAction = QSharedPointer<QAction>( new QAction( "Zoom Out Y" ) );
//  this->popupMenu->addAction( zoomYOutAction.data() );
//  
//  panRightAction = QSharedPointer<QAction>( new QAction( "Pan Right" ) );
//  this->popupMenu->addAction( panRightAction.data() );
//  
//  panLeftAction = QSharedPointer<QAction>( new QAction( "Pan Left" ) );
//  this->popupMenu->addAction( panLeftAction.data() );
//  
//  panUpAction = QSharedPointer<QAction>( new QAction(   "Pan Up" ) );
//  this->popupMenu->addAction( panUpAction.data() );
//  
//  panDownAction = QSharedPointer<QAction>( new QAction( "Pan Down" ) );
//  this->popupMenu->addAction( panDownAction.data() );
//  
//  this->popupMenu->addSeparator();
//
//  resetAction = QSharedPointer<QAction>( new QAction( "&Reset" ) );
//  this->popupMenu->addAction( resetAction.data() );
//
//  connect( this->popupMenu.data(), SIGNAL( triggered( QAction * ) ),
//           this, SLOT( performAction( QAction * ) ) );
//  
//  isEmpty = true;
//  curSigIndex = -1;
//
//  qrb = this->findChild<QRubberBand *>();
//  if ( !qrb ) {
//    std::cout << "cannot find qrb" << std::endl;
//  }
//  else {
//    qrb->installEventFilter( this );
//  }

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