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

#ifndef VIEWER_VIEWERGRAPH_H
#define VIEWER_VIEWERGRAPH_H

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

class ViewerGraph : public QtCharts::QChartView {

   Q_OBJECT

public:
  
  enum class ScaleType { Linear, Log };
  enum class DataType { TimeSeries, FFT };
  
  explicit ViewerGraph( int _id, ScaleType scaleType, DataType dataType, QWidget *oneParent = nullptr );
  virtual ~ViewerGraph();
  ViewerGraph( const ViewerGraph& ) = delete;
  ViewerGraph( ViewerGraph& ) = delete;
  ViewerGraph( ViewerGraph&& ) = delete;
  ViewerGraph& operator=( const ViewerGraph& ) = delete;
  ViewerGraph& operator=( ViewerGraph& ) = delete;
  ViewerGraph& operator=( ViewerGraph&& ) = delete;
  void setYTitle( std::string& s );
  void enterEvent( QEvent *ev );
  void wheelEvent( QWheelEvent *ev );
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent( QMouseEvent *ev );
  void keyPressEvent( QKeyEvent *ev );
  void keyReleaseEvent( QKeyEvent *ev );
  void getAxesLimits( double& x0, double& y0,
                      double& x1, double& y1 );
  void setAxesLimits( double x0, double y0,
                      double x1, double y1, bool adjScales = true );
  void getPlotSize( double& w, double& h );
  void setSeries ( QtCharts::QLineSeries *qls, int sigIndex, QString curFileName,
                   double minX, double maxX, double minY, double maxY, bool adjScales = true );
  void setSeries ( QtCharts::QLineSeries *qls, int sigIndex, QString curFileName,
                   double minX, double maxX, bool adjScales = true );
  void setEmptySeries ( QtCharts::QLineSeries *qls );
  int getCurSigIndex ( void );
  QString& getCurFileName ( void );
  void clear();
  bool eventFilter( QObject *watched, QEvent *event ) override;
  void getBetterAxesParams ( double, double, int, double&, double&, int&, bool adjScales = true  );

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
  QSharedPointer<QMenu> popupMenu;
  QSharedPointer<QAction> resetAction;
  QSharedPointer<QAction> prevViewAction;
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

signals:
  void horizontalPan( int id, int curSigIndex, QString& curFileName,
                      double minX, double maxX, double minY, double maxY );
  void verticalPan( int id, int curSigIndex, QString& curFileName,
                    double minX, double maxX, double minY, double maxY );
  void scale( int id, int curSigIndex, QString& curFileName,
              double minX, double maxX, double minY, double maxY );
  void reset( int id, int curSigIndex, QString& curFileName );
  void rubberBandScale( int id, int curSigIndex, QString& curFileName );
  void prevView( int id, int curSigIndex, QString& curFileName );
  void mousePos( int id, double mouseX, double mouseY );

private slots:
  void performAction( QAction *a );

private:
  bool inside( int x, int y );
  void zoomIn ( double zoomDistX, double zoomDistY );
  void zoomOut ( double zoomDistX, double zoomDistY );
  void panHorizontal ( double panDist );
  void panVertical ( double panDist );

};


#endif //VIEWER_VIEWERGRAPH_H
