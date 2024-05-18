//
// Created by jws3 on 5/17/24.
//

#include <cmath>

#include "RoiSelector.h"

RoiSelector::RoiSelector( ViewerGraphBase *gr ) {

  //std::cout << "RoiSelector::RoiSelector" << std::endl;

  graph = gr;
  scene = graph->scene();
  
  x0 = 0;
  y0 = 0;
  x1 = 0;
  y1 = 0;
  w = 0;
  h = 0;

  yMin = y0;
  yMax = y1;
  leftPos = x0;
  rightPos = x1;
  
  leftIndicator = new  QGraphicsLineItem( leftPos,
                                          yMin, leftPos, yMax );
  
  QPen pen1( QColor(Qt::magenta) );
  pen1.setWidth( 1 );
  leftIndicator->setPen( pen1 );
  leftIndicator->setVisible( false );
  scene->addItem( leftIndicator );

  rightIndicator = new  QGraphicsLineItem( rightPos,
                                           yMin, rightPos, yMax );
  
  QPen pen2( QColor(Qt::magenta) );
  pen2.setWidth( 1 );
  rightIndicator->setPen( pen2 );
  rightIndicator->setVisible( false );
  scene->addItem( rightIndicator );

  haveDimensions = false;
  active = false;
  requestSetActive = false;
  requestSetInitialState = true;
  requestSetInitialStateAtLeastOnce = false;
  
  this->connect( graph->chart.data(), SIGNAL( plotAreaChanged( const QRectF & ) ),
                 this, SLOT( dimensionChange( const QRectF & ) ) );

}

RoiSelector::~RoiSelector() {

}

void RoiSelector::setInitialState ( void ) {

  if ( !haveDimensions ) {
    requestSetInitialState = true;
    return;
  }

  //std::cout << "RoiSelector::setInitialState" << std::endl;

  yMin = y0;
  yMax = y0 + h;
  leftPos = x0 + 50;
  rightPos = x0 + w - 50;
  
  leftIndicator->setLine( leftPos,
    yMin, leftPos, yMax );

  rightIndicator->setLine( rightPos,
     yMin, rightPos, yMax );

  requestSetInitialStateAtLeastOnce = true;
  requestSetInitialState = false;

  emit roiRange( graph->id, leftPos, rightPos );

}

bool RoiSelector::isVisible( void ) {

  return visible;

}

void RoiSelector::setVisible( bool v ) {

  visible = v;
  leftIndicator->setVisible( visible );
  rightIndicator->setVisible( visible );
  
}

void RoiSelector::setLeft( int pos ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  leftPos = pos;
  
  leftIndicator->setLine( leftPos,
    yMin, leftPos, yMax );

  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::setRight( int pos ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  rightPos = pos;
  
  rightIndicator->setLine( rightPos,
    yMin, rightPos, yMax );

}

void RoiSelector::setYMin( int pos ) {

  if ( !active || !haveDimensions ) return;

  yMin = pos;

  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::setYMax( int pos ) {

  if ( !active || !haveDimensions ) return;

  yMax = pos;
  
}

int RoiSelector::getLeft( void ) {

  return leftPos;

}

int RoiSelector::getRight( void ) {

  return rightPos;

}

int RoiSelector::getYMin( void ) {

  return yMin;
  
}

int RoiSelector::getYMax( void ) {

  return yMax;
  
}

void RoiSelector::setLeft( double pos, double min, double max ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  leftPos = round( pos * ( max - min ) / w + min );
  
  leftIndicator->setLine( leftPos,
    yMin, leftPos, yMax );
  leftIndicator->update();

  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::setRight( int pos, double max, double min ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  rightPos = round( pos * ( max - min ) / h + min );
  
  rightIndicator->setLine( rightPos,
    yMin, rightPos, yMax );
  rightIndicator->update();

  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::setActive( bool a ) {

  if ( !haveDimensions ) {
    requestSetActive = true;
    return;
  }

  active = a;
  if ( active ) {

    setVisible( true );
    requestSetActive = false;
    
  }
  else {
    
    setVisible( false );
    
  }

}

bool RoiSelector::isActive ( void ) {

  return active;

}

void RoiSelector::incRight( int dir ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  int r;
  
  if ( dir >= 0 ) {
    r = rightPos + 1;
    if ( r >= x1 ) r = x1;
  }
  else {
    r = rightPos - 1;
    if ( r < leftPos ) r = leftPos + 1;
  }

  rightPos = r;
  
  rightIndicator->setLine( rightPos,
    yMin, rightPos, yMax );
  rightIndicator->update();
  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::incRightBig( int dir ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  int r;
  int inc = w / 20;

  if ( dir >= 0 ) {
    r = rightPos + inc;
    if ( r >= x1 ) r = x1;
  }
  else {
    r = rightPos - inc;
    if ( r < leftPos ) r = leftPos + 1;
  }

  rightPos = r;
  
  rightIndicator->setLine( rightPos,
    yMin, rightPos, yMax );
  rightIndicator->update();  

  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::incLeft( int dir ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  int l;
  
  if ( dir >= 0 ) {
    l = leftPos + 1;
    if ( l >= rightPos ) l = rightPos - 1;
  }
  else {
    l = leftPos - 1;
    if ( l < x0 ) l = x0;
  }

  leftPos = l;
  
  leftIndicator->setLine( leftPos,
    yMin, leftPos, yMax );
  leftIndicator->update();
  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::incLeftBig( int dir ) {

  if ( !active || !haveDimensions ) return;

  yMin = y0;
  yMax = y0 + h;

  int l;
  int inc = w / 20;

  //std::cout << "RoiSelector::incLeftBig - inc = " << inc << std::endl;

  if ( dir >= 0 ) {
    l = leftPos + inc;
    if ( l >= rightPos ) l = rightPos - 1;
  }
  else {
    l = leftPos - inc;
    if ( l < x0 ) l = x0;
  }

  leftPos = l;
  
  leftIndicator->setLine( leftPos,
    yMin, leftPos, yMax );
  leftIndicator->update();

  emit roiRange( graph->id, leftPos, rightPos );

}

void RoiSelector::dimensionChange( const QRectF &r ) {

  //std::cout << "RoiSelecter::dimensionChange" << std::endl;

  double x0Old = x0;
  double x1Old = x1;
  double wOld = w;
  double leftOld = leftPos;
  double rightOld = rightPos;

  //std::cout << "x0Old = " << x0Old << ", wOld = " << wOld << ", leftOld = " << leftOld << ", rightOld = " <<
  //  rightOld << std::endl;
  
  x0 = r.x();
  y0 = r.y();
  w = r.size().width();
  h = r.size().height();
  x1 = x0 + w;
  y1 = y0 + h;

  yMin = y0;
  yMax = y1;

  haveDimensions = true;

  if ( requestSetInitialStateAtLeastOnce ) {
  
    leftPos = w / wOld * ( leftOld - x0Old ) + x0;
    
    rightPos = w / wOld * ( rightOld - x1Old ) + x1;

    leftIndicator->setLine( leftPos,
                            yMin, leftPos, yMax );
    leftIndicator->update();

    rightIndicator->setLine( rightPos,
                             yMin, rightPos, yMax );
    rightIndicator->update();  
  
    //std::cout << "x0 = " << x0 << ", w = " << w << ", left = " << leftPos << ", right = " <<
    //  rightPos << std::endl;

    emit roiRange( graph->id, leftPos, rightPos );

  }
  else {

    setInitialState();

  }

  //setInitialState();
  
  if ( requestSetInitialState ) setInitialState();
  if ( requestSetActive ) setActive( true );

}
