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

ViewerGraph::ViewerGraph( int _id, ScaleType _scaleType, DataType _dataType, QWidget *_parent ) {

  id = _id;
  parent1 = _parent;
  shiftState = false;
  ctrlState = false;

  scaleType = _scaleType;
  dataType = _dataType;

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

  if ( scaleType == ViewerGraph::ScaleType::Linear ) {
  
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
  
  }
  else if ( scaleType == ViewerGraph::ScaleType::Log ) {
  
    axisY = QSharedPointer<QtCharts::QAbstractAxis>( new QtCharts::QLogValueAxis() );
    if ( dataType == ViewerGraph::DataType::TimeSeries ) {
      axisY->setTitleText( "Mag" );
      dynamic_cast<QtCharts::QLogValueAxis *>(axisY.data())->setLabelFormat( "%.10g" );
      axisY->setRange( Cnst::InitialMinSigLog, Cnst::InitialMaxSigLog );
    }
    else if ( dataType == ViewerGraph::DataType::FFT ) {
      axisY->setTitleText( "Mag)" );
      dynamic_cast<QtCharts::QLogValueAxis *>(axisY.data())->setLabelFormat( "%.10g" );
      axisY->setRange( Cnst::InitialMinFftLog, Cnst::InitialMaxFftLog );
    }
    axisY->setGridLineColor( QRgb( Cnst::InitialGridRgb ) );
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
  
  }
  
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
  
  panUpAction = QSharedPointer<QAction>( new QAction(   "Pan Up" ) );
  this->popupMenu->addAction( panUpAction.data() );
  
  panDownAction = QSharedPointer<QAction>( new QAction( "Pan Down" ) );
  this->popupMenu->addAction( panDownAction.data() );
  
  this->popupMenu->addSeparator();

  resetAction = QSharedPointer<QAction>( new QAction( "&Reset" ) );
  this->popupMenu->addAction( resetAction.data() );

  connect( this->popupMenu.data(), SIGNAL( triggered( QAction * ) ),
           this, SLOT( performAction( QAction * ) ) );
  
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

ViewerGraph::~ViewerGraph( ) {

}

void ViewerGraph::setYTitle( std::string& s ) {

  this->yTitle = QString( s.c_str() );

}

bool ViewerGraph::eventFilter( QObject *watched, QEvent *event ) {

  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
    panDistX=0, panDistY=0, zoomDistX=0, zoomDistY=0,
    newXMin=0, newXMax=0, newYMin=0, newYMax=0,
    plotW=0, plotH=0, chartXRange=1, chartYRange=1, xFact, yFact,
    x0, x1, y0, y1, mouseX, mouseY, boxX, boxY, boxW, boxH;
  int plotX, plotY;
 
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
      plotY = this->qrb->pos().y() - chart->plotArea().y();
      this->getPlotSize( plotW, plotH );
      this->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
      if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
      if ( ( chartYMax - chartYMin ) != 0 ) chartYRange = chartYMax - chartYMin;
      xFact = chartXRange / plotW;
      yFact = chartYRange / plotH;
      boxX = plotX * xFact + chartXMin;
      boxY = chartYRange - plotY * yFact + chartYMin;
      boxW = this->qrb->size().width() * xFact;
      boxH = this->qrb->size().height() * yFact;
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

  QtCharts::QValueAxis *xAxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  QtCharts::QValueAxis *yAxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );

  x0 = xAxis->min();
  x1 = xAxis->max();
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

  getBetterAxesParams( y0, y1, 2, min, max, ticks, adjScales );
  //std::cout << "1   y0 = " << y0 << ", y1 = " << y1 << ", 2 = " << 2
  //          << ", min = " << min << ", max = " << max << ", ticks = " << ticks << std::endl << std::endl;
  yValAxis->setRange( min, max );
  yValAxis->setTickCount( ticks );

  //xValAxis->setRange( x0, x1 );
  //xValAxis->setTickCount( 2 );

  //yValAxis->setRange( y0, y1 );
  //yValAxis->setTickCount( 2 );
  
  qls->attachAxis( xAxes[0] );
  qls->attachAxis( yAxes[0] );

  parent1->update();

}

void ViewerGraph::getPlotSize( double& w, double &h ) {

  w = (double) this->chart->plotArea().width();
  h = (double) this->chart->plotArea().height();

}

void ViewerGraph::setSeries ( QtCharts::QLineSeries *newQls, int sigIndex, QString fileName, double minX,
                              double maxX, double minY, double maxY, bool adjScales ) {

  //std::cout << "ViewerGraph::setSeries 7" << std::endl;

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

  //xaxis->setRange( minX, maxX );
  //yaxis->setRange( minY, maxY );

  qls->attachAxis( xaxis );
  qls->attachAxis( yaxis );

  //xaxis->setRange( minX, maxX );
  //yaxis->setRange( minY, maxY );
  
  double newMinX, newMaxX, newMinY, newMaxY;
  int newXTicks, newYTicks;
  getBetterAxesParams( minX, maxX, 5, newMinX, newMaxX, newXTicks, adjScales );
  //std::cout << "2   minX = " << minX << ", maxX = " << maxX << ", 5 = " << 5
  //  << ", newMinX = " << newMinX << ", newMaxX = " << newMaxX
  //<< ", newXTicks = " << newXTicks << std::endl  << std::endl;
  //QtCharts::QValueAxis *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  xaxis->setRange( newMinX, newMaxX );
  xaxis->setTickCount( newXTicks );

  getBetterAxesParams( minY, maxY, 5, newMinY, newMaxY, newYTicks, adjScales );
  //std::cout << "2   minY = " << minY << ", maxY = " << maxY << ", 5 = " << 5
  //          << ", newMinY = " << newMinY << ", newMaxY = " << newMaxY
  //          << ", newYTicks = " << newYTicks << std::endl << std::endl;
  //QtCharts::QValueAxis *yaxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );
  yaxis->setRange( newMinY, newMaxY );
  yaxis->setTickCount( newYTicks );
  yaxis->setTitleText( this->yTitle );
  //std::cout << "this->yTitle = [" << this->yTitle.toStdString() << "]" << std::endl;


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

  double newMinX, newMaxX, newMinY, newMaxY;
  int newXTicks, newYTicks;
  getBetterAxesParams( minX, maxX, 5, newMinX, newMaxX, newXTicks, adjScales );
  //std::cout << "3   minX = " << minX << ", maxX = " << maxX << ", 5 = " << 5
  //  << ", newMinX = " << newMinX << ", newMaxX = " << newMaxX << ", newXTicks = " << newXTicks << std::endl  << std::endl;
  //QtCharts::QValueAxis *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  xaxis->setRange( newMinX, newMaxX );
  xaxis->setTickCount( newXTicks );

  //std::cout << "this->yTitle = " << this->yTitle.toStdString() << std::endl;
  yaxis->setTitleText( this->yTitle );
  
  //xAxes[0]->setRange( minX, maxX );

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

  QtCharts::QValueAxis *xaxis = dynamic_cast<QtCharts::QValueAxis *>( xAxes[0] );
  if ( xaxis ) {
    xaxis->setRange( Cnst::InitialMinTime, Cnst::InitialMaxTime );
    xaxis->setTickCount( 2 );
  }

  QtCharts::QValueAxis *yaxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );
  if ( yaxis ) {
    yaxis->setRange( Cnst::InitialMinSig, Cnst::InitialMaxSig );
    yaxis->setTickCount( 2 );
    yaxis->setTitleText( "" );
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

  QtCharts::QLineSeries *emptyQls = new QtCharts::QLineSeries();
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
  QtCharts::QValueAxis *yAxis = dynamic_cast<QtCharts::QValueAxis *>( yAxes[0] );

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
  
  int plotX = ev->x() - chart->plotArea().x();
  int plotY = ev->y() - chart->plotArea().y();
  if ( ev->button() & 2 ) return;
  if ( shiftState || ctrlState ) return;
  QtCharts::QChartView::mouseReleaseEvent( ev );

}

void ViewerGraph::mouseMoveEvent( QMouseEvent *ev ) {
  
  if ( isEmpty ) return;
  
  int plotX = ev->x() - chart->plotArea().x();
  int plotY = ev->y() - chart->plotArea().y();
  if ( shiftState || ctrlState ) return;
  QtCharts::QChartView::mouseMoveEvent( ev );

}

void ViewerGraph::keyPressEvent( QKeyEvent *ev ) {

  if ( isEmpty ) return;
  
  if ( ev->key() == 0x1000020 ) {
    shiftState = true;
  }
  else if ( ev->key() == 0x1000021 ) {
    ctrlState = true;
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

//void ViewerGraph::getBetterAxesParams ( double min, double max, int ticks,
//                                        double& newMin, double& newMax, int& newTicks ) {
//
//  double diff;
//  double min1;
//  double max1;
//  double ticks1;
//
//  std::cout << "getBetterAxesParams" << std::endl;
//  std::cout << "min = " << min << std::endl;
//  std::cout << "max = " << max << std::endl;
//  //std::cout << "ticks = " << ticks << std::endl;
//
//
//  diff = log10( max - min );
//  std::cout << "diff = " << diff << std::endl;
//  diff = floor( log10( max - min ) );
//  std::cout << "floor diff = " << diff << std::endl;
//  
//  min1 = floor( min / pow( 10.0, diff ) ) * pow( 10.0, diff );
//  std::cout << "min1 = " << min1 << std::endl;
//  if ( min1 > min ) {
//    min1 = min1 - pow( 10.0, diff );
//  }
//
//  max1 = ceil( max / pow( 10.0, diff ) ) * pow( 10.0, diff );
//  std::cout << "max1 = " << max1 << std::endl;
//  if ( max1 < max ) {
//    max1 = max1 + pow( 10.0, diff );
//  }
//
//  double fact =  pow( 10.0, diff );
//  double div = ( max1 - min1 ) / fact;
//
//  std::cout << "fact = " << fact << std::endl;
//  std::cout << "div = " << div << std::endl;
//
//  if ( div >= 9 ) {
//    max1 = min1 + 10.0 * fact;
//    ticks1 = 5;
//  }
//  else if ( div > 8 ) {
//    max1 = min1 + 10.0 * fact;
//    ticks1 = 5;
//  }
//  else if ( div == 8 ) {
//    max1 = min1 + 8.0 * fact;
//    ticks1 = 4;
//  }
//  else if ( div >= 7 ) {
//    max1 = min1 + 8.0 * fact;
//    ticks1 = 4;
//  }
//  else if ( div > 6 ) {
//    max1 = min1 + 8.0 * fact;
//    min1 = min1 - fact;
//    ticks1 = 6;
//  }
//  else if ( div > 5 ) {
//    max1 = min1 + 6.0 * fact;
//    ticks1 = 6;
//  }
//  else if ( div > 4 ) {
//    max1 = min1 + 5.0 * fact;
//    ticks1 = 5;
//  }
//  else if ( div > 3 ) {
//    max1 = min1 + 4.0 * fact;
//    ticks1 = 4;
//  }
//  else if ( div > 2 ) {
//    max1 = min1 + 3.0 * fact;
//    ticks1 = 3;
//  }
//  else if ( div > 1 ) {
//    max1 = min1 + 2.0 * fact;
//    ticks1 = 2;
//  }
//  else {  
//    max1 = min1 + 1.0 * fact;
//    ticks1 = 5;
//  }
//
//  std::cout << "final min1 = " << min1 << std::endl;
//  std::cout << "final max1 = " << max1 << std::endl;
//  std::cout << "final ticks1 = " << ticks1 << std::endl;
//
//  ticks1++;
//
//  double inc = ( max1 - min1 ) / (ticks1-1);
//  double val = min1;
//  for ( int i=0; i<ticks1; i++ ) {
//    std::cout << val << "   ";
//    val += inc;
//  }
//  std::cout << std::endl;
//    
//
//  newMin = min1;
//  newMax = max1;
//  newTicks = ticks1;
//  
//}
