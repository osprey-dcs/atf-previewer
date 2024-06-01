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

#include "ViewerGraphLogY.h"

static QColor QColorBlack( "black" );
static QColor QColorWhite( "white" );

ViewerGraphLogY::ViewerGraphLogY( int _id, DataType _dataType, QWidget *_parent ) :
  ViewerGraphBase( _id, _dataType, _parent ) {

  id = _id;
  parent1 = _parent;
  dataType = _dataType;
  
  scaleType = ViewerGraphLogY::ScaleType::YLog;
  shiftState = false;
  ctrlState = false;

  qls = new QtCharts::QLineSeries();
  
  QPen pen( QRgb( 0x000000 ));
  pen.setWidth( 1 );
  qls->setPen( pen );

  chart = QSharedPointer<ViewerChart>( new ViewerChart() );

  QMargins m( 0, 0, 0, 0 );
  chart->setMargins( m );
  chart->legend()->hide();
  chart->setParent( dynamic_cast<QObject*>(this) );
  chart->addSeries( qls );

  axisY = QSharedPointer<QtCharts::QAbstractAxis>( new QtCharts::QLogValueAxis() );
  if ( dataType == ViewerGraphLogY::DataType::TimeSeries ) {
    axisY->setTitleText( "Mag" );
    dynamic_cast<QtCharts::QLogValueAxis *>(axisY.data())->setLabelFormat( "%.10g" );
    axisY->setRange( Cnst::InitialMinSigLog, Cnst::InitialMaxSigLog );
  }
  else if ( dataType == ViewerGraphLogY::DataType::FFT ) {
    axisY->setTitleText( "Mag)" );
    dynamic_cast<QtCharts::QLogValueAxis *>(axisY.data())->setLabelFormat( "%.10g" );
    axisY->setRange( Cnst::InitialMinFftLog, Cnst::InitialMaxFftLog );
  }
  axisY->setGridLineColor( QRgb( Cnst::InitialGridRgb ) );
  chart->addAxis( axisY.data(), Qt::AlignLeft );
  qls->attachAxis( axisY.data() );

  axisX = QSharedPointer<QtCharts::QAbstractAxis>( new QtCharts::QValueAxis() );
  if ( dataType == ViewerGraphLogY::DataType::TimeSeries ) {
    axisX->setTitleText( "Time (sec)" );
    dynamic_cast<QtCharts::QValueAxis *>(axisX.data())->setLabelFormat( "%.10g" );
    dynamic_cast<QtCharts::QValueAxis *>(axisX.data())->setTickCount( 2 );
    axisX->setRange( Cnst::InitialMinTime, Cnst::InitialMaxTime );
  }
  else if ( dataType == ViewerGraphLogY::DataType::FFT ) {
    axisX->setTitleText( "Freq (Hz)" );
    dynamic_cast<QtCharts::QValueAxis *>(axisX.data())->setLabelFormat( "%.10g" );
    dynamic_cast<QtCharts::QValueAxis *>(axisX.data())->setTickCount( 2 );
    axisX->setRange( Cnst::InitialMinFreq, Cnst::InitialMaxFreq );
  }
  axisX->setGridLineColor( QRgb( Cnst::InitialGridRgb ) );
  chart->addAxis( axisX.data(), Qt::AlignBottom );
  qls->attachAxis( axisX.data() );
  
  this->setRenderHint( QPainter::Antialiasing );
  this->setChart( chart.data() );

  w = 200;
  h = 100;
  this->setMinimumSize( w, h );
  this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  this->setRubberBand( QtCharts::QChartView::RectangleRubberBand );

  this->curX = this->curY = 0;
  this->popupMenu = QSharedPointer<QMenu>( new QMenu() );
  
  prevViewAction = QSharedPointer<QAction>( new QAction( "&Previous" ) );
  prevViewAction->setEnabled( false );
  this->popupMenu->addAction( prevViewAction.data() );

  this->popupMenu->addSeparator();

  zoomInAction = QSharedPointer<QAction>( new QAction( "Zoom In" ) );
  this->popupMenu->addAction( zoomInAction.data() );
  
  zoomOutAction = QSharedPointer<QAction>( new QAction( "Zoom Out" ) );
  this->popupMenu->addAction( zoomOutAction.data() );
  
  zoomXInAction = QSharedPointer<QAction>( new QAction( "Zoom In X" ) );
  this->popupMenu->addAction( zoomXInAction.data() );
  
  zoomXOutAction = QSharedPointer<QAction>( new QAction( "Zoom Out X" ) );
  this->popupMenu->addAction( zoomXOutAction.data() );
  
  zoomYInAction = QSharedPointer<QAction>( new QAction( "Zoom In Y" ) );
  this->popupMenu->addAction( zoomYInAction.data() );
  
  zoomYOutAction = QSharedPointer<QAction>( new QAction( "Zoom Out Y" ) );
  this->popupMenu->addAction( zoomYOutAction.data() );
  
  panRightAction = QSharedPointer<QAction>( new QAction( "Pan Right" ) );
  this->popupMenu->addAction( panRightAction.data() );
  
  panLeftAction = QSharedPointer<QAction>( new QAction( "Pan Left" ) );
  this->popupMenu->addAction( panLeftAction.data() );
  
  panUpAction = QSharedPointer<QAction>( new QAction( "Pan Up" ) );
  this->popupMenu->addAction( panUpAction.data() );
  
  panDownAction = QSharedPointer<QAction>( new QAction( "Pan Down" ) );
  this->popupMenu->addAction( panDownAction.data() );
  
  this->popupMenu->addSeparator();

  resetAction = QSharedPointer<QAction>( new QAction( "&Reset" ) );
  this->popupMenu->addAction( resetAction.data() );

  connect( this->popupMenu.data(), SIGNAL( triggered( QAction * ) ),
           this, SLOT( performAction( QAction * ) ) );

  connect( this->chart.data(), SIGNAL( plotAreaChanged( const QRectF & ) ),
           this, SLOT( dimensionChange( const QRectF & ) ) );
  
  isEmpty = true;
  curSigIndex = -1;

  qrb = this->findChild<QRubberBand *>();
  if ( !qrb ) {
    std::cout << "cannot find qrb" << std::endl;
  }
  else {
    qrb->installEventFilter( this );
  }

}

ViewerGraphLogY::~ViewerGraphLogY( ) {

}

void ViewerGraphLogY::setYTitle( std::string& s ) {

  this->yTitle = QString( s.c_str() );

}

bool ViewerGraphLogY::eventFilter( QObject *watched, QEvent *event ) {

  if ( watched == this->qrb ) {
    if ( event->type() ==  QEvent::HideToParent ) {
      emit rubberBandScale( this->id, this->curSigIndex, this->curFileName );
    }
  }

  if ( watched == this ) {
  }

  return false;

}

void ViewerGraphLogY::getAxesLimits( double& x0, double &y0,
                                  double &x1, double& y1 ) {

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  QtCharts::QValueAxis *xAxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QLogValueAxis *yAxis = dynamic_cast<QtCharts::QLogValueAxis *>( yAxes[0] );

  x0 = xAxis->min();
  x1 = xAxis->max();
  y0 = yAxis->min();
  y1 = yAxis->max();

}

void ViewerGraphLogY::setAxesLimits( double x0, double y0,
                                 double x1, double y1, bool adjScales ) {

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  QtCharts::QValueAxis *xValAxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QLogValueAxis *yValAxis = dynamic_cast<QtCharts::QLogValueAxis *>( yAxes[0] );

  qls->detachAxis( xAxes[0] );
  qls->detachAxis( yAxes[0] );

  chart->removeSeries( qls );

  chart->addSeries( qls );

  double min, max;
  int ticks;
  getBetterAxesParams( x0, x1, 2, min, max, ticks, adjScales );
  xValAxis->setRange( min, max );
  xValAxis->setTickCount( ticks );

  yValAxis->setRange( y0, y1 );
  
  qls->attachAxis( xAxes[0] );
  qls->attachAxis( yAxes[0] );

  parent1->update();

}

//void ViewerGraphLogY::getPlotSize( double& w, double &h ) {
//
//  w = (double) this->chart->plotArea().width();
//  h = (double) this->chart->plotArea().height();
//
//}

void ViewerGraphLogY::setSeries ( QtCharts::QLineSeries *newQls, int sigIndex, QString fileName, double minX,
                              double maxX, double minY, double maxY, bool adjScales ) {

  //std::cout << "ViewerGraphLogY::setSeries 7" << std::endl;

  // There's a memory leak somewhere in the following

  // remove and delete existing data - causes axes text from mutilple sources - investigate
  // chart->removeAllSeries();

  this->isEmpty = false;
  this->curSigIndex = sigIndex;
  this->curFileName = fileName;

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  QtCharts::QValueAxis *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QLogValueAxis *yaxis = dynamic_cast<QtCharts::QLogValueAxis *>( yAxes[0] );

  qls->detachAxis( xaxis );
  qls->detachAxis( yaxis );

  chart->removeSeries( qls );

  qls->clear();
  delete qls;
  qls = nullptr;

  qls = newQls;
  QPen pen( QColorBlack );
  pen.setWidth( 1 );
  qls->setPen( pen );

  chart->addSeries( qls );

  //xaxis->setRange( minX, maxX );
  //yaxis->setRange( minY, maxY );

  qls->attachAxis( xaxis );
  qls->attachAxis( yaxis );

  yaxis->setRange( minY, maxY );
  
  double newMinX, newMaxX, newMinY, newMaxY;
  int newXTicks, newYTicks;
  getBetterAxesParams( minX, maxX, 5, newMinX, newMaxX, newXTicks, adjScales );
  xaxis->setRange( newMinX, newMaxX );
  xaxis->setTickCount( newXTicks );

  parent1->update();

}

void ViewerGraphLogY::setSeries ( QtCharts::QLineSeries *newQls, int sigIndex, QString fileName, double minX,
                              double maxX, bool adjScales ) {

  // There's a memory leak somewhere in the following

  // remove and delete existing data - causes axes text from mutilple sources - investigate
  // chart->removeAllSeries();

  this->isEmpty = false;
  this->curSigIndex = sigIndex;
  this->curFileName = fileName;

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  QtCharts::QValueAxis *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QLogValueAxis *yaxis = dynamic_cast<QtCharts::QLogValueAxis *>( yAxes[0] );

  qls->detachAxis( xaxis );
  qls->detachAxis( yaxis );

  chart->removeSeries( qls );

  qls->clear();
  delete qls;
  qls = nullptr;

  qls = newQls;
  QPen pen( QColorBlack );
  pen.setWidth( 1 );
  qls->setPen( pen );

  chart->addSeries( qls );

  qls->attachAxis( xaxis );
  qls->attachAxis( yaxis );

  double newMinX, newMaxX, newMinY, newMaxY;
  int newXTicks, newYTicks;
  getBetterAxesParams( minX, maxX, 5, newMinX, newMaxX, newXTicks, adjScales );
  xaxis->setRange( newMinX, newMaxX );
  xaxis->setTickCount( newXTicks );

  yaxis->setTitleText( this->yTitle );
  
  parent1->update();

}

void ViewerGraphLogY::setEmptySeries ( QtCharts::QLineSeries *newQls ) {

  // There's a memory leak somewhere in the following

  // remove and delete existing data - causes axes text from mutilple sources - investigate
  // chart->removeAllSeries();

  isEmpty = true;
  this->curSigIndex = -1;
  this->curFileName = "";

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  qls->detachAxis( xAxes[0] );
  qls->detachAxis( yAxes[0] );

  chart->removeSeries( qls );

  qls->clear();
  delete qls;
  qls = nullptr;

  qls = newQls;
  QPen pen( QColorBlack );
  pen.setWidth( 1 );
  qls->setPen( pen );

  chart->addSeries( qls );

  qls->attachAxis( xAxes[0] );
  qls->attachAxis( yAxes[0] );

  QtCharts::QValueAxis *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  if ( xaxis ) {
    xaxis->setRange( Cnst::InitialMinTime, Cnst::InitialMaxTime );
    xaxis->setTickCount( 2 );
  }

  QtCharts::QLogValueAxis *yaxis = dynamic_cast<QtCharts::QLogValueAxis *>( yAxes[0] );
  if ( yaxis ) {
    yaxis->setRange( Cnst::InitialMinSig, Cnst::InitialMaxSig );
    //yaxis->setTickCount( 2 );
    yaxis->setTitleText( "" );
  }

  parent1->update();

}

int ViewerGraphLogY::getCurSigIndex ( void ) {

  return this->curSigIndex;

}

QString& ViewerGraphLogY::getCurFileName ( void ) {

  return this->curFileName;

}

void ViewerGraphLogY::clear( void ) {

  QtCharts::QLineSeries *emptyQls = new QtCharts::QLineSeries();
  this->setEmptySeries( emptyQls );
  isEmpty = true; // redundant

}

bool ViewerGraphLogY::inside( int x, int y ) {

  if ( ( x >= 0 ) && ( x <= chart->plotArea().width() ) &&
       ( y >= 0 ) && ( y <= ( chart->plotArea().height() ) ) ) {
    return true;
  }
  else {
    return false;
  }

}

void ViewerGraphLogY::enterEvent( QEvent *ev ) {
  setFocus();
  shiftState = false;
  ctrlState = false;
}

void ViewerGraphLogY::wheelEvent( QWheelEvent *ev ) {

  if ( isEmpty ) {
    return;
  }

  QPoint numDegrees = ev->angleDelta();
  bool forward = true;
  if ( numDegrees.y() < 0.0 ) forward = false;

  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    distX=0, distY=0, newXMin=0, newXMax=0, newYMin=0, newYMax=0,
    plotW=0, plotH=0, chartXRange=1, chartYRange=1, xFact, yFact,
    x0, y0, x1, y1;
  
  QPointF pt = ev->position();
  int plotX = pt.x() - chart->plotArea().x();
  int plotY = pt.y() - chart->plotArea().y();

  bool in = inside( plotX, plotY );

  this->getAxesLimits( chartXMin, chartYMin, chartXMax, chartYMax );

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );
  QtCharts::QValueAxis *xAxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QLogValueAxis *yAxis = dynamic_cast<QtCharts::QLogValueAxis *>( yAxes[0] );

  if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
  if ( ( chartYMax - chartYMin ) != 0 ) chartYRange = chartYMax - chartYMin;

  double xticks = std::fmax( 2.0, xAxis->tickCount() );
  distX = chartXRange / ( xticks - 1);
  double yticks = std::fmax( 2.0, yAxis->tickCount() );
  distY = chartYRange / ( yticks - 1);

  if ( !shiftState && ( plotY < 0 ) ) {
    if ( forward ) {
      //pan right
    }
    else {
      //pan left
      distX *= -1.0;
    }
    panHorizontal( distX );
  }
  else if ( !shiftState && ( plotY > chart->plotArea().height() ) ) {
    if ( forward ) {
      //pan right
    }
    else {
      //pan left
      distX *= -1.0;
    }
    panHorizontal( distX );
  }
  else if ( !shiftState && ( plotX > chart->plotArea().width() ) ) {
    if ( forward ) {
      //pan up
    }
    else {
      //pan down
      distY *= -1.0;
    }
    panVertical( distY );
  }
  else if ( !shiftState && ( plotX < 0 ) ) {
    if ( forward ) {
      //pan down
    }
    else {
      //pan up
      distY *= -1.0;
    }
    panVertical( distY );
  }
  else if ( shiftState && ( plotX < 0 ) ) {
    double dist = distY;
    if ( forward ) {
      zoomIn( 0.0, dist );
    }
    else {
      zoomOut( 0.0, dist );
    }
  }
  else if ( shiftState && ( plotY > chart->plotArea().height() ) ) {
    double dist = distX;
    if ( forward ) {
      zoomIn( dist, 0.0 );
    }
    else {
      zoomOut( dist, 0.0 );
    }
  }
  else if ( shiftState && in ) {
    if ( forward ) {
      zoomIn( distX, distY );
    }
    else {
      zoomOut( distX, distY );
    }

  }
  
  QtCharts::QChartView::wheelEvent( ev );

}

void ViewerGraphLogY::mousePressEvent( QMouseEvent *ev ) {

  if ( isEmpty ) {
    //std::cout << "empty" << std::endl;
    return;
  }
  
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    panDistX=0, panDistY=0, zoomDistX=0, zoomDistY=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0,
    plotW=0, plotH=0, chartXRange=1, chartYRange=1, xFact, yFact,
    x0, x1, y0, y1;
  bool openMenu=false;
  int plotX = ev->x() - chart->plotArea().x();
  int plotY = ev->y() - chart->plotArea().y();
  int button = ev->button();

  bool in = inside( plotX, plotY );

  this->getPlotSize( plotW, plotH );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  //std::cout << "plotX = " << plotX << ", plotY = " << plotY << ", plotW = " << plotW << ", plotH = " << plotH << std::endl;
  //std::cout << "chartXMin = " << chartXMin << ", chartXMax = " << chartXMax << ", chartYMin = " << chartYMin << ", chartYMax = " << chartYMax << std::endl;
  //std::cout << " = " << x << ", x = " << x << ", x = " << x << ", x = " << x << std::endl;

  if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
  if ( ( chartYMax - chartYMin ) != 0 ) chartYRange = log10( chartYMax ) - log10( chartYMin );

  xFact = chartXRange / plotW;
  yFact = chartYRange / plotH;

  panDistX = ( chartXMax - chartXMin ) * 0.5;
  panDistY = ( chartYMax - chartYMin ) * 0.5;
  zoomDistX = ( chartXMax - chartXMin ) * 0.25;
  zoomDistY = ( chartYMax - chartYMin ) * 0.25;
  
  if ( !shiftState && ctrlState && ( button & 1 ) && in ) {
    double mouseX = plotX * xFact + chartXMin;
    double mouseY = chartYRange - plotY * yFact + log10( chartYMin );
    mouseY = pow(10.0, mouseY );
    emit mousePos( this->id, mouseX, mouseY );
  }
  // pan right
  else if ( !shiftState && !ctrlState && ( button & 1 ) && ( plotX > chart->plotArea().width() ) ) {
    panHorizontal( panDistX ); 
  }
  // pan left
  else if ( !shiftState && !ctrlState && ( button & 1 ) && ( plotX < 0 ) ) {
    panDistX *= -1.0;
    panHorizontal( panDistX ); 
  }
  // pan up
  else if ( !shiftState && !ctrlState && ( button & 1 ) && ( plotY < 0 ) ) {
    panVertical( panDistY );
  }
  // pan down
  else if ( !shiftState && !ctrlState && ( button & 1 ) && ( plotY > chart->plotArea().height() ) ) {
    panDistY *= -1.0;
    panVertical( panDistY );
  }
  // zoom in y axis only
  else if ( shiftState && !ctrlState && ( button & 1 ) && ( plotX < 0 ) ) {
    zoomIn( 0.0, zoomDistY );
  }
  // zoom in x axis only
  else if ( shiftState && !ctrlState && ( button & 1 ) && ( plotY > chart->plotArea().height() ) ) {
    zoomIn( zoomDistX, 0.0 );
  }
  // zoom out y axis only
  else if ( shiftState && !ctrlState && ( button & 2 ) && ( plotX < 0 ) ) {
    zoomOut( 0.0, zoomDistY );
  }
  // zoom out x axis only
  else if ( shiftState && !ctrlState && ( button & 2 ) && ( plotY > chart->plotArea().height() ) ) {
    zoomOut( zoomDistX, 0.0 );
  }
  // zoom in
  else if ( ( button & 1 ) && !ctrlState && shiftState && in ) {
    zoomIn( zoomDistX, zoomDistY );
  }
  // zoom out
  else if ( ( button & 2 ) && !ctrlState && shiftState && in ) {
    zoomOut( zoomDistX, zoomDistY );
  }
  else if ( ( button & 2 ) && !ctrlState && !shiftState && in ) {
    openMenu = true;
  }

  if ( openMenu ) {
    QPoint p( ev->x(), ev->y() );
    this->popupMenu->popup( this->mapToGlobal( p ) );
  }

  if ( button & 2 ) return;
  if ( shiftState || ctrlState ) return;

  QtCharts::QChartView::mousePressEvent( ev );

}

void ViewerGraphLogY::mouseReleaseEvent( QMouseEvent *ev ) {
  
  if ( isEmpty ) return;
  
  int plotX = ev->x() - chart->plotArea().x();
  int plotY = ev->y() - chart->plotArea().y();
  if ( ev->button() & 2 ) return;
  if ( shiftState || ctrlState ) return;
  QtCharts::QChartView::mouseReleaseEvent( ev );

}

void ViewerGraphLogY::mouseMoveEvent( QMouseEvent *ev ) {
  
  if ( isEmpty ) return;
  
  int plotX = ev->x() - chart->plotArea().x();
  int plotY = ev->y() - chart->plotArea().y();
  if ( shiftState || ctrlState ) return;
  QtCharts::QChartView::mouseMoveEvent( ev );

}

void ViewerGraphLogY::keyPressEvent( QKeyEvent *ev ) {

  if ( isEmpty ) return;
  
  if ( ev->key() == 0x1000020 ) {
    shiftState = true;
  }
  else if ( ev->key() == 0x1000021 ) {
    ctrlState = true;
  }
  QtCharts::QChartView::keyPressEvent( ev );
}

void ViewerGraphLogY::keyReleaseEvent( QKeyEvent *ev ) {

  if ( isEmpty ) return;
  
  if ( ev->key() == 0x1000020 ) {
    shiftState = false;
  }
  else if ( ev->key() == 0x1000021 ) {
    ctrlState = false;
  }
  QtCharts::QChartView::keyReleaseEvent( ev );
}

void ViewerGraphLogY::performAction ( QAction *a ) {

  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0;

  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  double zoomDistX = ( chartXMax - chartXMin ) * 0.25;
  double zoomDistY = ( chartYMax - chartYMin ) * 0.25;
  double panDistX = ( chartXMax - chartXMin ) * 0.5;
  double panDistY = ( chartYMax - chartYMin ) * 0.5;

  if ( a == this->resetAction.data() ) {
    emit reset( this->id, this->curSigIndex, this->curFileName );
  }
  else if ( a == this->prevViewAction.data() ) {
    emit prevView( this->id, this->curSigIndex, this->curFileName );
  }
  else if ( a == this->zoomInAction.data() ) {
    zoomIn( zoomDistX, zoomDistY );
  }
  else if ( a == this->zoomOutAction.data() ) {
    zoomOut( zoomDistX, zoomDistY );
  }
  else if ( a == this->zoomXInAction.data() ) {
    zoomIn( zoomDistX, 0.0 );
  }
  else if ( a == this->zoomXOutAction.data() ) {
    zoomOut( zoomDistX, 0.0 );
  }
  else if ( a == this->zoomYInAction.data() ) {
    zoomIn( 0.0, zoomDistY );
  }
  else if ( a == this->zoomYOutAction.data() ) {
    zoomOut( 0.0, zoomDistY );
  }
  else if ( a == this->panRightAction.data() ) {
    panHorizontal( panDistX );
  }
  else if ( a == this->panLeftAction.data() ) {
    panHorizontal( -1.0 * panDistX );
  }
  else if ( a == this->panUpAction.data() ) {
    panVertical( panDistY );
  }
  else if ( a == this->panDownAction.data() ) {
    panVertical( -1.0 * panDistY );
  }
  else {
    //std::cout << "ViewerGraphLogY::performAction: Unknown action" << std::endl;
  }

}

void ViewerGraphLogY::zoomIn ( double zoomDistX, double zoomDistY ) {
    
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0;
  
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  this->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  this->prevViewAction->setEnabled( true );

  newXMin = chartXMin + zoomDistX;
  newXMax = chartXMax - zoomDistX;
  if ( zoomDistY > 0.0 ) {
    newYMin = pow( 10.0, log10( chartYMin ) + 1 );
    newYMax = pow( 10.0, log10( chartYMax ) - 1 );
  }
  
  this->setAxesLimits( newXMin, newYMin, newXMax, newYMax );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  emit scale( this->id, this->curSigIndex, this->curFileName,
              chartXMin, chartYMin, chartXMax, chartYMax );

}

void ViewerGraphLogY::zoomOut ( double zoomDistX, double zoomDistY ) {
    
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0;
  
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  this->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  this->prevViewAction->setEnabled( true );

  newXMin = chartXMin - zoomDistX;
  newXMax = chartXMax + zoomDistX;
  if ( zoomDistY > 0.0 ) {
    newYMin = pow( 10.0, log10( chartYMin ) - 1 );
    newYMax = pow( 10.0, log10( chartYMax ) + 1 );
  }
  
  this->setAxesLimits( newXMin, newYMin, newXMax, newYMax );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  emit scale( this->id, this->curSigIndex, this->curFileName,
              chartXMin, chartYMin, chartXMax, chartYMax );

}

void ViewerGraphLogY::panHorizontal ( double panDist ) {
    
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0;
  
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  this->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  this->prevViewAction->setEnabled( true );

  newXMin = chartXMin + panDist;
  newXMax = chartXMax + panDist;
  newYMin = chartYMin;
  newYMax = chartYMax;
  
  this->setAxesLimits( newXMin, newYMin, newXMax, newYMax );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  emit horizontalPan( this->id, this->curSigIndex, this->curFileName,
                      chartXMin, chartYMin, chartXMax, chartYMax );

}

void ViewerGraphLogY::panVertical ( double panDist ) {
    
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0;

  //std::cout << "ViewerGraphLogY::panVertical" << std::endl;
  
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  this->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  this->prevViewAction->setEnabled( true );

  //std::cout << "chartXMin = " << chartXMin << ", chartXMax = " << chartXMax << ", chartYMin = " << chartYMin << ", chartYMax = " << chartYMax << std::endl;

  if ( panDist > 0 ) {
    
    newXMin = chartXMin;
    newXMax = chartXMax;
    newYMin = pow( 10.0, log10( chartYMin ) + 1 );
    newYMax = pow( 10.0, log10( chartYMax ) + 1 );

  }
  else {

    newXMin = chartXMin;
    newXMax = chartXMax;
    newYMin = pow( 10.0, log10( chartYMin ) - 1 );
    newYMax = pow( 10.0, log10( chartYMax ) - 1 );

  }

   //std::cout << "newXMin = " << newXMin << ", newXMax = " << newXMax << ", newYMin = " << newYMin << ", newYMax = " << newYMax << std::endl;
  
  this->setAxesLimits( newXMin, newYMin, newXMax, newYMax );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  emit verticalPan( this->id, this->curSigIndex, this->curFileName,
                    chartXMin, chartYMin, chartXMax, chartYMax );

}

void ViewerGraphLogY::getBetterAxesParams ( double min, double max, int ticks,
                                        double& adj_min, double& adj_max, int& num_label_ticks, bool adjScales ) {

double dmin, dmax, diff, mag, norm;

int imag, inorm, imin, imax, inc1, inc2, inc5, imin1, imax1,
 imin2, imax2, imin5, imax5, best, bestInc, bestMin, bestMax, idiff, idiv,
 choice, ok;

  if ( !adjScales ) {
    adj_min = min;
    adj_max = max;
    num_label_ticks = ticks;
    return;
  }

  dmin = min;
  dmax = max;

  if ( dmax <= dmin ) dmax = dmin + 1.0;

  diff = dmax - dmin;

  /* fprintf( stderr, "dmin = %-g, dmax = %-g, diff =  %-g\n", dmin, dmax, diff ); */

  mag = log10( diff );
  imag = (int ) floor( mag ) - 1;

  norm = diff * pow(10.0,-1.0*imag);
  inorm = (int) ceil( norm );

  //fprintf( stderr, "mag = %-g, imag = %-d\n", mag, imag );
  //fprintf( stderr, "norm = %-d\n", inorm );


  /* normalize min & max */
  imin = (int) floor( dmin * pow(10.0,-1.0*imag) );
  imax = (int) ceil( dmax * pow(10.0,-1.0*imag) );

  ok = 0;

  inc1 = imax - imin;
  imin1 = imin;
  imax1 = imax;
  if ( inc1 < 8 ) ok = 1;

  //fprintf( stderr, "1st adj min 1 = %-d, 1st adj max 1 = %-d\n", imin1, imax1 );

  if ( imin < 0 ) {
    if ( imin % 2 )
      imin2 = imin - 2 - ( imin % 2 );
    else
      imin2 = imin;
  }
  else {
    imin2 = imin - ( imin % 2 );
  }

  if ( imax < 0 ) {
    imax2 = imax + 2 + ( imax % 2 );
  }
  else {
    if ( imax % 2 )
      imax2 = imax + 2 - ( imax % 2 );
    else
      imax2 = imax;
  }

  inc2 = ( imax2 - imin2 ) / 2;
  if ( inc2 < 8 ) ok = 1;

  //fprintf( stderr, "1st adj min 2 = %-d, 1st adj max 2 = %-d\n", imin2, imax2 );

  if ( imin < 0 ) {
    if ( imin % 5 )
      imin5 = imin - 5 - ( imin % 5 );
    else
      imin5 = imin;
  }
  else {
    imin5 = imin - ( imin % 5 );
  }

  if ( imax < 0 ) {
    imax5 = imax + 5 + ( imax % 5 );
  }
  else {
    if ( imax % 5 )
      imax5 = imax + 5 - ( imax % 5 );
    else
      imax5 = imax;
  }

  inc5 = ( imax5 - imin5 ) / 5;
  if ( inc5 < 8 ) ok = 1;

  //fprintf( stderr, "1st adj min 5 = %-d, 1st adj max 5 = %-d\n", imin5, imax5 );

  //fprintf( stderr, "1 inc1 = %-d\n", inc1 );
  //fprintf( stderr, "1 inc2 = %-d\n", inc2 );
  //fprintf( stderr, "1 inc5 = %-d\n", inc5 );

  if ( ! ok ) {

    imag++;

    /* normalize min & max */
    imin = (int) floor( dmin * pow(10.0,-1.0*imag) );
    imax = (int) ceil( dmax * pow(10.0,-1.0*imag) );

    inc1 = imax - imin;
    imin1 = imin;
    imax1 = imax;
    if ( inc1 < 8 ) ok = 1;

    //fprintf( stderr, "1st adj min 1 = %-d, 1st adj max 1 = %-d\n", imin1, imax1 );

    if ( imin < 0 ) {
      if ( imin % 2 )
        imin2 = imin - 2 - ( imin % 2 );
      else
        imin2 = imin;
    }
    else {
      imin2 = imin - ( imin % 2 );
    }

    if ( imax < 0 ) {
      imax2 = imax + 2 + ( imax % 2 );
    }
    else {
      if ( imax % 2 )
        imax2 = imax + 2 - ( imax % 2 );
      else
        imax2 = imax;
    }

    inc2 = ( imax2 - imin2 ) / 2;
    if ( inc2 < 8 ) ok = 1;

    //fprintf( stderr, "1st adj min 2 = %-d, 1st adj max 2 = %-d\n", imin2, imax2 );

    if ( imin < 0 ) {
      if ( imin % 5 )
        imin5 = imin - 5 - ( imin % 5 );
      else
        imin5 = imin;
    }
    else {
      imin5 = imin - ( imin % 5 );
    }

    if ( imax < 0 ) {
      imax5 = imax + 5 + ( imax % 5 );
    }
    else {
      if ( imax % 5 )
        imax5 = imax + 5 - ( imax % 5 );
      else
        imax5 = imax;
    }

    inc5 = ( imax5 - imin5 ) / 5;
    if ( inc5 < 8 ) ok = 1;

    //fprintf( stderr, "1st adj min 5 = %-d, 1st adj max 5 = %-d\n", imin5, imax5 );

    //fprintf( stderr, "2 inc1 = %-d\n", inc1 );
    //fprintf( stderr, "2 inc2 = %-d\n", inc2 );
    //fprintf( stderr, "2 inc5 = %-d\n", inc5 );

  }

  // find best

  best = abs( inc1 - 6 );
  bestInc = inc1;
  bestMin = imin1;
  bestMax = imax1;
  idiv = inc1;
  choice = 1;

  if ( abs( inc2 - 6 ) < best ) {
    best = abs( inc2 - 6 );
    bestInc = inc2;
    bestMin = imin2;
    bestMax = imax2;
    idiv = inc2;
    choice = 2;
  }

  if ( abs( inc5 - 6 ) < best ) {
    best = abs( inc5 - 6 );
    bestInc = inc5;
    bestMin = imin5;
    bestMax = imax5;
    idiv = inc5;
    choice = 5;
  }

  idiff = ( bestMax - bestMin );
  adj_min = (double) bestMin * pow(10.0,imag);
  adj_max = (double) bestMax * pow(10.0,imag);

  num_label_ticks = idiv + 1;

}
