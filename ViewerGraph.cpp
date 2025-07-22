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

#include "ViewerGraph.h"

static QColor QColorBlack( "black" );
static QColor QColorWhite( "white" );

ViewerGraph::ViewerGraph( int _id, DataType _dataType, QWidget *_parent ) :
  ViewerGraphBase( _id, _dataType, _parent ) {

  id = _id;
  parent1 = _parent;
  dataType = _dataType;
  
  scaleType = ViewerGraph::ScaleType::YLinear;
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

  axisY = QSharedPointer<QtCharts::QAbstractAxis>( new QtCharts::QValueAxis() );
  axisY->setTitleText( "Mag" );
  dynamic_cast<QtCharts::QValueAxis *>(axisY.data())->setLabelFormat( "%.10g" );
  dynamic_cast<QtCharts::QValueAxis *>(axisY.data())->setTickCount( 2 );
  if ( dataType == ViewerGraph::DataType::TimeSeries ) {
    axisY->setRange( Cnst::InitialMinSig, Cnst::InitialMaxSig );
     axisY->setGridLineColor( QRgb( Cnst::InitialGridRgb ) );
  }
  else if ( dataType == ViewerGraph::DataType::FFT ) {
    axisY->setRange( Cnst::InitialMinFft, Cnst::InitialMaxFft );
    axisY->setGridLineColor( QRgb( Cnst::InitialGridRgb ) );
  }
  chart->addAxis( axisY.data(), Qt::AlignLeft );
  qls->attachAxis( axisY.data() );
   axisX = QSharedPointer<QtCharts::QAbstractAxis>( new QtCharts::QValueAxis() );
  if ( dataType == ViewerGraph::DataType::TimeSeries ) {
    axisX->setTitleText( "Time (sec)" );
    dynamic_cast<QtCharts::QValueAxis *>(axisX.data())->setLabelFormat( "%.10g" );
    dynamic_cast<QtCharts::QValueAxis *>(axisX.data())->setTickCount( 2 );
    axisX->setRange( Cnst::InitialMinTime, Cnst::InitialMaxTime );
  }
  else if ( dataType == ViewerGraph::DataType::FFT ) {
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

  zoomFullScaleAction = QSharedPointer<QAction>( new QAction( "Zoom Full Scale" ) );
  this->popupMenu->addAction( zoomFullScaleAction.data() );
  
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
  
  panUpAction = QSharedPointer<QAction>( new QAction(   "Pan Up" ) );
  this->popupMenu->addAction( panUpAction.data() );
  
  panDownAction = QSharedPointer<QAction>( new QAction( "Pan Down" ) );
  this->popupMenu->addAction( panDownAction.data() );
  
  this->popupMenu->addSeparator();

  resetAction = QSharedPointer<QAction>( new QAction( "&Reset" ) );
  this->popupMenu->addAction( resetAction.data() );

  connect( this->popupMenu.data(), SIGNAL( triggered( QAction * ) ),
           this, SLOT( performAction( QAction * ) ) );

  this->connect( this->chart.data(), SIGNAL( plotAreaChanged( const QRectF & ) ),
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

  lastXMin = 0;
  lastXMax = 0;
  lastYMin = 0;
  lastYMax = 0;

}

ViewerGraph::~ViewerGraph( ) {

}

void ViewerGraph::setYTitle( std::string& s ) {

  this->yTitle = QString( s.c_str() );

}

bool ViewerGraph::eventFilter( QObject *watched, QEvent *event ) {

  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    plotW=0, plotH=0, chartXRange=1, xFact,
    boxX, boxW;
  int plotX;
 
  if ( watched == this->qrb ) {
    //std::cout << __FILE__ << ", line = " << __LINE__ << std::endl;
    //std::cout << "rb event, event = " << (int) event->type() << std::endl;
    //if ( event->type() == QEvent::Resize ) {
    //  plotX = this->qrb->pos().x() - chart->plotArea().x();
    //  plotY = this->qrb->pos().y() - chart->plotArea().y();
    //  std::cout << "rb resize event, x = " << plotX << std::endl;
    //  std::cout << "rb resize event, y = " << plotY << std::endl;
    //  std::cout << "rb resize event, w = " << this->qrb->size().width() << std::endl;
    //  std::cout << "rb resize event, h = " << this->qrb->size().height() << std::endl;
    //  this->getPlotSize( plotW, plotH );
    //  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
    //  if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
    //  if ( ( chartYMax - chartYMin ) != 0 ) chartYRange = chartYMax - chartYMin;
    //  xFact = chartXRange / plotW;
    //  yFact = chartYRange / plotH;
    //  std::cout << "xFact = " << xFact << std::endl;
    //  boxX = plotX * xFact + chartXMin;
    //  boxY = chartYRange - plotY * yFact + chartYMin;
    //  boxW = this->qrb->size().width() * xFact;
    //  boxH = this->qrb->size().height() * yFact;
    //  std::cout << "box x, y, w, h = " << boxX << ", " << boxY << ", " << boxW << ", " << boxH << std::endl;
    //}
    if ( event->type() ==  QEvent::HideToParent ) {
      plotX = this->qrb->pos().x() - chart->plotArea().x();
      this->getPlotSize( plotW, plotH );
      this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
      if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
      xFact = chartXRange / plotW;
      boxX = plotX * xFact + chartXMin;
      boxW = this->qrb->size().width() * xFact;
      emit rubberBandScale( this->id, this->curSigIndex, this->curFileName );
      emit rubberBandRange( this->id, boxX, boxX+boxW );
    }
  }

  if ( watched == this ) {
  }

  return false;

}

void ViewerGraph::getAxesLimits( double& x0, double &y0,
                                  double &x1, double& y1 ) {

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  auto *xAxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  x0 = xAxis->min();
  x1 = xAxis->max();

  auto *yAxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );
  y0 = yAxis->min();
  y1 = yAxis->max();

}

void ViewerGraph::setAxesLimits( double x0, double y0,
                                 double x1, double y1, bool adjScales ) {

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  QtCharts::QValueAxis *xValAxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QValueAxis *yValAxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );

  qls->detachAxis( xAxes[0] );
  qls->detachAxis( yAxes[0] );

  chart->removeSeries( qls );

  chart->addSeries( qls );

  double min, max;
  int ticks;
  getBetterAxesParams( x0, x1, 2, min, max, ticks, adjScales );
  //std::cout << "1   x0 = " << x0 << ", x1 = " << x1 << ", 2 = " << 2 << ", min = " << min
  //          << ", max = " << max << ", ticks = " << ticks << std::endl << std::endl;
  xValAxis->setRange( min, max );
  xValAxis->setTickCount( ticks );
  lastXMin = min;
  lastXMax = max;

  getBetterAxesParams( y0, y1, 2, min, max, ticks, adjScales );

  yValAxis->setRange( min, max );
  yValAxis->setTickCount( ticks );
  lastYMin = min;
  lastYMax = max;
  
  qls->attachAxis( xAxes[0] );
  qls->attachAxis( yAxes[0] );

  parent1->update();

}

//void ViewerGraph::getPlotPos( double& x0, double& y0, double& x1, double& y1 ) {

//  x0 = (double) this->chart->pos().x();
//  y0 = (double) this->chart->pos().y();
//  x1 = (double) this->chart->pos().x() + (double) this->chart->size().width();
//  y1 = (double) this->chart->pos().y() + (double) this->chart->size().height();

//}

//void ViewerGraph::getPlotSize( double& w, double &h ) {

//  w = (double) this->chart->size().width();
//  h = (double) this->chart->size().height();

//}

void ViewerGraph::setSeries ( QtCharts::QLineSeries *newQls, int sigIndex, QString fileName, double minX,
                              double maxX, double minY, double maxY, bool adjScales ) {

  this->isEmpty = false;
  this->curSigIndex = sigIndex;
  this->curFileName = fileName;

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );

  QtCharts::QValueAxis *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QValueAxis *yaxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );

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
  lastXMin = newMinX;
  lastXMax = newMaxX;

  getBetterAxesParams( minY, maxY, 5, newMinY, newMaxY, newYTicks, adjScales );

  yaxis->setRange( newMinY, newMaxY );
  yaxis->setTickCount( newYTicks );
  yaxis->setTitleText( this->yTitle );
  lastYMin = newMinY;
  lastYMax = newMaxY;

  parent1->update();

}

void ViewerGraph::setSeries ( QtCharts::QLineSeries *newQls, int sigIndex, QString fileName, double minX,
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
  QtCharts::QValueAxis *yaxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );

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

  double newMinX, newMaxX;
  int newXTicks;
  getBetterAxesParams( minX, maxX, 5, newMinX, newMaxX, newXTicks, adjScales );

  xaxis->setRange( newMinX, newMaxX );
  xaxis->setTickCount( newXTicks );
  lastXMin = newMinX;
  lastXMax = newMaxX;

  yaxis->setTitleText( this->yTitle );

  parent1->update();

}

void ViewerGraph::setEmptySeries ( QtCharts::QLineSeries *newQls ) {

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

  auto *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  if ( xaxis ) {
    xaxis->setRange( Cnst::InitialMinTime, Cnst::InitialMaxTime );
    xaxis->setTickCount( 2 );
    lastXMin = Cnst::InitialMinTime;
    lastXMax = Cnst::InitialMaxTime;
  }

  auto *yaxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );
  if ( yaxis ) {
    yaxis->setRange( Cnst::InitialMinSig, Cnst::InitialMaxSig );
    yaxis->setTickCount( 2 );
    yaxis->setTitleText( "" );
    lastYMin = Cnst::InitialMinSig;
    lastYMax = Cnst::InitialMaxSig;

  }

  parent1->update();

}

int ViewerGraph::getCurSigIndex ( void ) {

  return this->curSigIndex;

}

QString& ViewerGraph::getCurFileName ( void ) {

  return this->curFileName;

}

void ViewerGraph::clear( void ) {

  auto *emptyQls = new QtCharts::QLineSeries();
  this->setEmptySeries( emptyQls );
  isEmpty = true; // redundant

}

bool ViewerGraph::inside( int x, int y ) {

  if ( ( x >= 0 ) && ( x <= chart->plotArea().width() ) &&
       ( y >= 0 ) && ( y <= ( chart->plotArea().height() ) ) ) {
    return true;
  }
  else {
    return false;
  }

}

void ViewerGraph::enterEvent( QEvent *ev ) {
  setFocus();
  shiftState = false;
  ctrlState = false;
}

void ViewerGraph::wheelEvent( QWheelEvent *ev ) {

  if ( isEmpty ) {
    return;
  }

  QPoint numDegrees = ev->angleDelta();
  bool forward = true;
  if ( numDegrees.y() < 0.0 ) forward = false;

  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    distX=0, distY=0, chartXRange=1, chartYRange=1;
  
  QPointF pt = ev->position();
  int plotX = pt.x() - chart->plotArea().x();
  int plotY = pt.y() - chart->plotArea().y();

  bool in = inside( plotX, plotY );

  this->getAxesLimits( chartXMin, chartYMin, chartXMax, chartYMax );

  auto xAxes = chart->axes(Qt::Horizontal, qls );
  auto yAxes = chart->axes(Qt::Vertical, qls );
  auto *xAxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  auto *yAxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );

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

void ViewerGraph::mousePressEvent( QMouseEvent *ev ) {

  //std::cout << "mousePressEvent" << std::endl;

  if ( isEmpty ) {
    std::cout << "empty" << std::endl;
    return;
  }
  
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    panDistX=0, panDistY=0, zoomDistX=0, zoomDistY=0,
    plotW=0, plotH=0, chartXRange=1, chartYRange=1, xFact, yFact;
  bool openMenu=false;
  int plotX = ev->x() - chart->plotArea().x();
  int plotY = ev->y() - chart->plotArea().y();
  int button = ev->button();

  bool in = inside( plotX, plotY );

  this->getPlotSize( plotW, plotH );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  //std::cout << "plotX = " << plotX << ", plotY = " << plotY << ", plotW = " << plotW << ", plotH = " << plotH << std::endl;
  //std::cout << "chartXMin = " << chartXMin << ", chartXMax = " << chartXMax << ", chartYMin = " << chartYMin << ", chartYMax = " << chartYMax << std::endl;

  if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
  if ( ( chartYMax - chartYMin ) != 0 ) chartYRange = chartYMax - chartYMin;

  xFact = chartXRange / plotW;
  yFact = chartYRange / plotH;

  panDistX = ( chartXMax - chartXMin ) * 0.5;
  panDistY = ( chartYMax - chartYMin ) * 0.5;
  zoomDistX = ( chartXMax - chartXMin ) * 0.25;
  zoomDistY = ( chartYMax - chartYMin ) * 0.25;
  
  if ( !shiftState && ctrlState && ( button & 1 ) && in ) {
    double mouseX = plotX * xFact + chartXMin;
    double mouseY = chartYRange - plotY * yFact + chartYMin;
    //std::cout << "pos:   " << mouseX << ",  " << mouseY << std::endl;
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

void ViewerGraph::mouseReleaseEvent( QMouseEvent *ev ) {
  
  if ( isEmpty ) return;

  if ( ev->button() & 2 ) return;
  if ( shiftState || ctrlState ) return;
  QtCharts::QChartView::mouseReleaseEvent( ev );

}

void ViewerGraph::mouseMoveEvent( QMouseEvent *ev ) {
  
  if ( isEmpty ) return;

  if ( shiftState || ctrlState ) return;
  QtCharts::QChartView::mouseMoveEvent( ev );

}

void ViewerGraph::keyPressEvent( QKeyEvent *ev ) {

  if ( isEmpty ) return;
  
  if ( ev->key() == Qt::Key_Shift ) { // 0x1000020 ) {
    shiftState = true;
  }
  else if ( ev->key() == Qt::Key_Control ) { // 0x1000021 ) {
    ctrlState = true;
  }
  else if ( ev->key() == Qt::Key_Return ) {

    emit scaleToSelection();

  }
  else if ( ev->key() == Qt::Key_Right ) {

    if ( shiftState ) {
    
      if ( ctrlState ) {
        emit rightBigInc( 1 );
      }
      else {
        emit rightInc( 1 );
      }

    }
    else {

      if ( ctrlState ) {
        emit leftBigInc( 1 );
      }
      else {
        emit leftInc( 1 );
      }

    }
    
  }
  else if ( ev->key() == Qt::Key_Left ) {

    if ( shiftState ) {
    
      if ( ctrlState ) {
        emit rightBigInc( -1 );
      }
      else {
        emit rightInc( -1 );
      }

    }
    else {

      if ( ctrlState ) {
        emit leftBigInc( -1 );
      }
      else {
        emit leftInc( -1 );
      }

    }

  }
  
  QtCharts::QChartView::keyPressEvent( ev );

}

void ViewerGraph::keyReleaseEvent( QKeyEvent *ev ) {

  if ( isEmpty ) return;
  
  if ( ev->key() == 0x1000020 ) {
    shiftState = false;
  }
  else if ( ev->key() == 0x1000021 ) {
    ctrlState = false;
  }
  QtCharts::QChartView::keyReleaseEvent( ev );
}

void ViewerGraph::performAction ( QAction *a ) {

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
  else if ( a == this->zoomFullScaleAction.data() ) {
    zoomFullScale();
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
    //std::cout << "ViewerGraph::performAction: Unknown action" << std::endl;
  }

}

void ViewerGraph::zoomFullScale ( ) {
    
  emit fullScale( this->id, this->curSigIndex, this->curFileName );

}

void ViewerGraph::zoomIn ( double zoomDistX, double zoomDistY ) {
    
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0;
  
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  this->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  this->prevViewAction->setEnabled( true );

  newXMin = chartXMin + zoomDistX;
  newXMax = chartXMax - zoomDistX;
  newYMin = chartYMin + zoomDistY;
  newYMax = chartYMax - zoomDistY;
  
  this->setAxesLimits( newXMin, newYMin, newXMax, newYMax );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  emit scale( this->id, this->curSigIndex, this->curFileName,
              chartXMin, chartYMin, chartXMax, chartYMax );

}

void ViewerGraph::zoomOut ( double zoomDistX, double zoomDistY ) {
    
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0;
  
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  this->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  this->prevViewAction->setEnabled( true );

  newXMin = chartXMin - zoomDistX;
  newXMax = chartXMax + zoomDistX;
  newYMin = chartYMin - zoomDistY;
  newYMax = chartYMax + zoomDistY;
  
  this->setAxesLimits( newXMin, newYMin, newXMax, newYMax );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  emit scale( this->id, this->curSigIndex, this->curFileName,
              chartXMin, chartYMin, chartXMax, chartYMax );

}

void ViewerGraph::panHorizontal ( double panDist ) {
    
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

void ViewerGraph::panVertical ( double panDist ) {
    
  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0;
  
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  this->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  this->prevViewAction->setEnabled( true );

  newXMin = chartXMin;
  newXMax = chartXMax;
  newYMin = chartYMin + panDist;
  newYMax = chartYMax + panDist;
  
  this->setAxesLimits( newXMin, newYMin, newXMax, newYMax );
  this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
  emit verticalPan( this->id, this->curSigIndex, this->curFileName,
                    chartXMin, chartYMin, chartXMax, chartYMax );

}

void ViewerGraph::getBetterAxesParams ( double min, double max, int ticks,
                                        double& adj_min, double& adj_max, int& num_label_ticks, bool adjScales ) {

double dmin, dmax, diff, mag;

int imag, imin, imax, inc1, inc2, inc5, imin1, imax1,
 imin2, imax2, imin5, imax5, best, bestMin, bestMax, idiv,
 ok;

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

  if ( ! ok ) {

    imag++;

    /* normalize min & max */
    imin = (int) floor( dmin * pow(10.0,-1.0*imag) );
    imax = (int) ceil( dmax * pow(10.0,-1.0*imag) );

    inc1 = imax - imin;
    imin1 = imin;
    imax1 = imax;
    if ( inc1 < 8 ) ok = 1;

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

  }

  // find best

  best = abs( inc1 - 6 );
  bestMin = imin1;
  bestMax = imax1;
  idiv = inc1;

  if ( abs( inc2 - 6 ) < best ) {
    best = abs( inc2 - 6 );
    bestMin = imin2;
    bestMax = imax2;
    idiv = inc2;
  }

  if ( abs( inc5 - 6 ) < best ) {
    best = abs( inc5 - 6 );
    bestMin = imin5;
    bestMax = imax5;
    idiv = inc5;
  }

  adj_min = (double) bestMin * pow(10.0,imag);
  adj_max = (double) bestMax * pow(10.0,imag);

  num_label_ticks = idiv + 1;

}

// in base class
//void ViewerGraph::dimensionChange( const QRectF &r ) {

//  std::cout << "ViewerGraph::dimensionChange" << std::endl;

//  std::cout << "r.x() = " << r.x() << std::endl;
//  std::cout << "r.y() = " << r.y() << std::endl;
//  std::cout << "r.size().width() = " << r.size().width() << std::endl;
//  std::cout << "r.size().height() = " << r.size().height() << std::endl;

  //double x0Old = x0;
  //double x1Old = x1;
  //double wOld = w;
  //double leftOld = leftPos;
  //double rightOld = rightPos;

  //std::cout << "x0Old = " << x0Old << ", wOld = " << wOld << ", leftOld = " << leftOld << ", rightOld = " <<
  //  rightOld << std::endl;

  //return;
  
  //x0 = r.x();
  //y0 = r.y();
  //w = r.size().width();
  //h = r.size().height();
  //x1 = x0 + w;
  //y1 = y0 + h;

  //yMin = y0;
  //yMax = y1;

  //haveDimensions = true;

  //if ( requestSetInitialStateAtLeastOnce ) {
  
  //  leftPos = w / wOld * ( leftOld - x0Old ) + x0;
    
  //  rightPos = w / wOld * ( rightOld - x1Old ) + x1;

  //  leftIndicator->setLine( leftPos,
  //                          yMin, leftPos, yMax );
  //  leftIndicator->update();

  //  rightIndicator->setLine( rightPos,
  //                           yMin, rightPos, yMax );
  //  rightIndicator->update();  
  
    //////////std::cout << "x0 = " << x0 << ", w = " << w << ", left = " << leftPos << ", right = " <<
    //  rightPos << std::endl;

  //  emit roiRange( graph->id, leftPos, rightPos );

  //}
  //else {

  //  setInitialState();

  //}

  //setInitialState();
  
  //if ( requestSetInitialState ) setInitialState();
  //if ( requestSetActive ) setActive( true );

//}
