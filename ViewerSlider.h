//
// Created by jws3 on 4/30/24.
//

#ifndef VIEWER_VIEWERSLIDER_H
#define VIEWER_VIEWERSLIDER_H

#include <iostream>

#include <QObject>
#include <QWidget>
#include <QSlider>

//#include "ViewerGraphArea.h"

class ViewerSlider : public QSlider {

  Q_OBJECT

  public:

  explicit ViewerSlider ( QWidget *w, QWidget *parent=nullptr );
  ViewerSlider( const ViewerSlider& ) = delete;
  ViewerSlider( ViewerSlider& ) = delete;
  ViewerSlider( ViewerSlider&& ) = delete;
  ViewerSlider& operator=( const ViewerSlider& ) = delete;
  ViewerSlider& operator=( ViewerSlider& ) = delete;
  ViewerSlider& operator=( ViewerSlider&& ) = delete;
  
  QWidget *callbackW;
  QWidget *vga;

  public slots:
  void newValue( int value );

  signals:
  void valueChanged1( QWidget *w, int value );

};


#endif //VIEWER_VIEWERSLIDER_H
