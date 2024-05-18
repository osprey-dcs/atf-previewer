//
// Created by jws3 on 5/17/24.
//

#ifndef VIEWER_ROISELECTER_H
#define VIEWER_ROISELECTER_H

#include <iostream>
#include <iomanip>

#include <QObject>
#include <QGraphicsItem>

#include "ViewerGraph.h"

class RoiSelecter : public QObject {

  Q_OBJECT

public:
  explicit RoiSelecter( ViewerGraph *graph );

  virtual ~RoiSelecter();

public slots:
  void dimensionChange( const QRectF & );

public:
  ViewerGraph *graph;

};


#endif //VIEWER_ROISELECTER_H
