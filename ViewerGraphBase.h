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

#ifndef VIEWER_VIEWERGRAPHBASE_H
#define VIEWER_VIEWERGRAPHBASE_H

#include <memory>

#include <Qt>
#include <QPoint>
#include <QRectF>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QPushButton>
#include <QCheckBox>
#include <QPalette>
#include <QDebug>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QGraphicsView>
#include <QRubberBand>
#include <QWheelEvent>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <QSharedPointer>

#include "ViewerChart.h"
#include "ViewStack.h"
#include "Cnst.h"

class ViewerGraphBase : public QtCharts::QChartView {

   Q_OBJECT

public:
  
  enum class ScaleType { YLinear, YLog };
  enum class DataType { TimeSeries, FFT };
  
  explicit ViewerGraphBase( int _id, DataType dataType, QWidget *oneParent = nullptr );
  virtual ~ViewerGraphBase();
  ViewerGraphBase( const ViewerGraphBase& ) = delete;
  ViewerGraphBase( ViewerGraphBase& ) = delete;
  ViewerGraphBase( ViewerGraphBase&& ) = delete;
  virtual ViewerGraphBase& operator=( const ViewerGraphBase& ) = delete;
  virtual ViewerGraphBase& operator=( ViewerGraphBase& ) = delete;
  virtual ViewerGraphBase& operator=( ViewerGraphBase&& ) = delete;
  virtual void setYTitle( std::string& s );
  virtual void enterEvent( QEvent *ev );
  virtual void wheelEvent( QWheelEvent *ev );
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void mouseMoveEvent( QMouseEvent *ev );
  virtual void keyPressEvent( QKeyEvent *ev );
  virtual void keyReleaseEvent( QKeyEvent *ev );
  virtual void getAxesLimits( double& x0, double& y0,
                              double& x1, double& y1 );
  virtual void setAxesLimits( double x0, double y0,
                              double x1, double y1, bool adjScales = true );
  virtual void getPlotSize( double& w, double& h );
  virtual void getPlotPos( double& x0, double& y0, double& x1, double& y1 );
  virtual void setSeries ( QtCharts::QLineSeries *qls, int sigIndex, QString curFileName,
                           double minX, double maxX, double minY, double maxY, bool adjScales = true );
  virtual void setSeries ( QtCharts::QLineSeries *qls, int sigIndex, QString curFileName,
                           double minX, double maxX, bool adjScales = true );
  virtual void setEmptySeries ( QtCharts::QLineSeries *qls );
  virtual int getCurSigIndex ( void );
  virtual QString& getCurFileName ( void );
  virtual void clear();
  virtual bool eventFilter( QObject *watched, QEvent *event ) override;
  virtual void getBetterAxesParams ( double, double, int, double&, double&, int&, bool adjScales = true  );

  int id, x, y, w, h;
  int rubberBandWidth;
  ScaleType scaleType;
  DataType dataType;
  bool shiftState, ctrlState;
  QWidget *parent1;
  QString yTitle;
  QSharedPointer<ViewerChart> chart;
  QSharedPointer<QtCharts::QAbstractAxis> axisX;
  QSharedPointer<QtCharts::QAbstractAxis> axisY;
  QtCharts::QLineSeries *qls; // I manage this memory
  int curX, curY;
  double lastXMin, lastXMax, lastYMin, lastYMax;
  QSharedPointer<QMenu> popupMenu;
  QSharedPointer<QAction> resetAction;
  QSharedPointer<QAction> prevViewAction;
  QSharedPointer<QAction> zoomFullScaleAction;
  QSharedPointer<QAction> zoomInAction;
  QSharedPointer<QAction> zoomOutAction;
  QSharedPointer<QAction> zoomXInAction;
  QSharedPointer<QAction> zoomXOutAction;
  QSharedPointer<QAction> zoomYInAction;
  QSharedPointer<QAction> zoomYOutAction;
  QSharedPointer<QAction> panLeftAction;
  QSharedPointer<QAction> panRightAction;
  QSharedPointer<QAction> panUpAction;
  QSharedPointer<QAction> panDownAction;
  bool isEmpty;
  int curSigIndex;
  QString curFileName;
  QRubberBand *qrb;
  ViewStack views;

//signals:
//  virtual void horizontalPan( int id, int curSigIndex, QString& curFileName,
//                              double minX, double maxX, double minY, double maxY );
//  virtual void verticalPan( int id, int curSigIndex, QString& curFileName,
//                            double minX, double maxX, double minY, double maxY );
//  virtual void scale( int id, int curSigIndex, QString& curFileName,
//                      double minX, double maxX, double minY, double maxY );
//  virtual void reset( int id, int curSigIndex, QString& curFileName );
//  virtual void rubberBandScale( int id, int curSigIndex, QString& curFileName );
//  virtual void rubberBandRange( int id, double xmin, double xmax );
//  virtual void prevView( int id, int curSigIndex, QString& curFileName );
//  virtual void mousePos( int id, double mouseX, double mouseY );
//
private slots:
  virtual void performAction( QAction *a );

private:
  virtual bool inside( int x, int y );
  virtual void zoomFullScale ( void );
  virtual void zoomIn ( double zoomDistX, double zoomDistY );
  virtual void zoomOut ( double zoomDistX, double zoomDistY );
  virtual void panHorizontal ( double panDist );
  virtual void panVertical ( double panDist );

};


#endif //VIEWER_VIEWERGRAPHBASE_H
