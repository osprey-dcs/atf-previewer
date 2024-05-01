//
// Created by jws3 on 4/30/24.
//

#include "ViewerSlider.h"

ViewerSlider::ViewerSlider ( QWidget *w, QWidget *parent ) : QSlider( Qt::Horizontal, parent ) {

  vga = w;

  connect( this, SIGNAL( valueChanged( int ) ),
           this, SLOT( newValue( int ) ) );

}

void ViewerSlider::newValue( int value ) {

  emit( valueChanged1( vga, value ) );

}



