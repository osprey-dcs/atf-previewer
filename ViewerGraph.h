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

#include "ViewerGraphBase.h"

class ViewerGraph : public ViewerGraphBase {

   Q_OBJECT

public:
  
  explicit ViewerGraph( int _id, DataType dataType, QWidget *oneParent = nullptr );
  virtual ~ViewerGraph();
  ViewerGraph( const ViewerGraph& ) = delete;
  ViewerGraph( ViewerGraph& ) = delete;
  ViewerGraph( ViewerGraph&& ) = delete;
  ViewerGraph& operator=( const ViewerGraph& ) = delete;
  ViewerGraph& operator=( ViewerGraph& ) = delete;
  ViewerGraph& operator=( ViewerGraph&& ) = delete;
  void setYTitle( std::string& s ) override final;
  void enterEvent( QEvent *ev ) override final;
  void wheelEvent( QWheelEvent *ev ) override final;
  void mousePressEvent(QMouseEvent *event) override final;
  void mouseReleaseEvent(QMouseEvent *event) override final;
  void mouseMoveEvent( QMouseEvent *ev ) override final;
  void keyPressEvent( QKeyEvent *ev ) override final;
  void keyReleaseEvent( QKeyEvent *ev ) override final;
  void getAxesLimits( double& x0, double& y0,
                      double& x1, double& y1 ) override final;
  void setAxesLimits( double x0, double y0,
                      double x1, double y1, bool adjScales = true ) override final;
  //void getPlotSize( double& w, double& h );
  //void getPlotPos( double& x0, double& y0, double& x1, double& y1 );
  void setSeries ( QtCharts::QLineSeries *qls, int sigIndex, QString curFileName,
                   double minX, double maxX, double minY, double maxY, bool adjScales = true ) override final;
  void setSeries ( QtCharts::QLineSeries *qls, int sigIndex, QString curFileName,
                   double minX, double maxX, bool adjScales = true ) override final;
  void setEmptySeries ( QtCharts::QLineSeries *qls ) override final;
  int getCurSigIndex ( void ) override final;
  QString& getCurFileName ( void ) override final;
  void clear() override final;
  bool eventFilter( QObject *watched, QEvent *event ) override final;
  void getBetterAxesParams ( double, double, int, double&, double&, int&, bool adjScales = true  ) override final;

signals:
  void horizontalPan( int id, int curSigIndex, QString& curFileName,
                      double minX, double maxX, double minY, double maxY );
  void verticalPan( int id, int curSigIndex, QString& curFileName,
                    double minX, double maxX, double minY, double maxY );
  void fullScale( int id, int curSigIndex, QString& curFileName );
  void scale( int id, int curSigIndex, QString& curFileName,
              double minX, double maxX, double minY, double maxY );
  void reset( int id, int curSigIndex, QString& curFileName );
  void rubberBandScale( int id, int curSigIndex, QString& curFileName );
  void rubberBandRange( int id, double xmin, double xmax );
  void prevView( int id, int curSigIndex, QString& curFileName );
  void mousePos( int id, double mouseX, double mouseY );
  void scaleToSelection( void );
  void leftInc ( int dir );
  void leftBigInc ( int dir );
  void rightInc ( int dir );
  void rightBigInc ( int dir );

private slots:
  void performAction( QAction *a ) override final;
  //void dimensionChange( const QRectF &r );  // in base class

private:
  bool inside( int x, int y ) override final;
  void zoomFullScale ( void ) override final;
  void zoomIn ( double zoomDistX, double zoomDistY ) override final;
  void zoomOut ( double zoomDistX, double zoomDistY ) override final;
  void panHorizontal ( double panDist ) override final;
  void panVertical ( double panDist ) override final;

};


#endif //VIEWER_VIEWERGRAPH_H
