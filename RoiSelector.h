//
// Created by jws3 on 5/17/24.
//

#ifndef VIEWER_ROISELECTOR_H
#define VIEWER_ROISELECTOR_H

#include <iostream>
#include <iomanip>

#include <QObject>
#include <QGraphicsLineItem>

#include "ViewerGraph.h"

class RoiSelector : public QObject {

  Q_OBJECT

public:
  explicit RoiSelector( ViewerGraphBase *gr );

  RoiSelector ( RoiSelector&& ) = delete;
  RoiSelector ( RoiSelector& ) = delete;
  RoiSelector ( const RoiSelector& ) = delete;

  RoiSelector& operator=( RoiSelector&& other ) = delete;
  RoiSelector& operator=( RoiSelector& other ) = delete;
  RoiSelector& operator=( const RoiSelector& other ) = delete;
  
  virtual ~RoiSelector();
  void setInitialState( void );
  bool isVisible( void );
  void setVisible( bool v );
  void setLeft( int pos );
  void setRight( int pos );
  void setYMin( int pos );
  void setYMax( int pos );
  int getLeft( void );
  int getRight( void );
  int getYMin( void );
  int getYMax( void );
  void setLeft( double pos, double min, double max );
  void setRight( int pos, double max, double min );
  bool isActive( void );
  void setActive( bool a );
  void incRight( int dir );
  void incRightBig( int dir );
  void incLeft( int dir );
  void incLeftBig( int dir );

  ViewerGraphBase *graph;
  QGraphicsScene *scene;
  QGraphicsLineItem *leftIndicator;
  QGraphicsLineItem *rightIndicator;
  QRectF plotArea;
  int leftPos;
  int rightPos;
  int yMin;
  int yMax;
  bool visible;
  bool active;
  bool requestSetActive;
  bool requestSetInitialStateAtLeastOnce;
  bool requestSetInitialState;
  bool haveDimensions;
  double x0;
  double y0;
  double x1;
  double y1;
  double w;
  double h;

public slots:
  void dimensionChange( const QRectF & );

signals:
  void roiRange( int id, int xmin, int xmax );

};


#endif //VIEWER_ROISELECTOR_H
