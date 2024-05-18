//
// Created by jws3 on 5/17/24.
//

#include "RoiSelecter.h"

RoiSelecter::RoiSelecter( ViewerGraph *gr ) {

  graph = gr;

  this->connect( graph, SIGNAL( plotAreaChanged( const QRectF & ) ),
          this, SLOT( dimensionChange( const QRectF & ) ) );

}

RoiSelecter::~RoiSelecter() {

}

void RoiSelecter::dimensionChange( const QRectF & ) {

  std::cout << "RoiSelecter::dimensionChange" << std::endl;

}
