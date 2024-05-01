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
#include <sstream>
#include <iomanip>

#include "ViewerGraphArea.h"

static QColor qcolorBlack( "black" );
static QColor qcolorWhite( "white" );

ViewerGraphArea::ViewerGraphArea( int _id, QWidget *_parent ) : ViewerGraphAreaBase( _id, _parent ) {

  //std::cout << "ViewerGraphArea created" << std::endl;
  
  vlayout = QSharedPointer<QVBoxLayout>( new QVBoxLayout() );
  hlayout = QSharedPointer<QHBoxLayout>( new QHBoxLayout() );
  hlayout2 = QSharedPointer<QHBoxLayout> ( new QHBoxLayout() );
  slider = QSharedPointer<ViewerSlider>( new ViewerSlider( this ) );
  graph = QSharedPointer<ViewerGraph>( new ViewerGraph( id, ViewerGraph::ScaleType::Linear,
                                                        ViewerGraph::DataType::FFT, this ) );
  mousePos = QSharedPointer<QLabel>( new QLabel( "          " ) );
  sigLabel = QSharedPointer<QLabel>( new QLabel( "Signal" ) );
  sigName = QSharedPointer<Combo>( new Combo( this ) );
  sigName->setEditable( true );
  sigName->clear();
  curSigIndex = 0;
  curFileName = "";
  calcFft = QSharedPointer<ViewerCtlrPushButton>( new ViewerCtlrPushButton(this, "FFT" ) );
  enableFftButton( false );

  sigName->addItem( QString( "                    " ), -1 );

  w = 200;
  h = 100;
  this->setMinimumSize( w, h );
  this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  vlayout->addLayout( hlayout.data() );
  hlayout->addWidget( sigLabel.data() );
  hlayout->addWidget( sigName.data() );
  hlayout->addWidget( calcFft.data() );
  hlayout->addStretch( 1 );
  hlayout->addWidget( mousePos.data() );
  
  vlayout->addLayout( hlayout2.data() );
  hlayout2->addWidget( slider.data() );
  
  vlayout->addWidget( graph.data() );
  this->setLayout( vlayout.data() );

}

ViewerGraphArea::~ViewerGraphArea( ) {

}

void ViewerGraphArea::setCurInfo( QString _curFileName, int _curSigIndex ) {
  curFileName = _curFileName;
  curSigIndex = _curSigIndex;
  //qDebug() << "vga id: " << this->id << "  -  " << curFileName << "  -  " << curSigIndex;
}

void ViewerGraphArea::getCurInfo( QString& _curFileName, int& _curSigIndex ) {
  _curFileName = curFileName;
  _curSigIndex = curSigIndex;
}

void ViewerGraphArea::setInitialState( void ) {

  slider->setValue( 0 );

}

void ViewerGraphArea::clear ( void ) {

  curSigIndex = 0;
  curFileName = "";
  graph->clear();

}

void ViewerGraphArea::enableFftButton( bool ena ) {
  calcFft->setEnabled( ena );
}

void ViewerGraphArea::updateMousePosition( double x, double y ) {

  std::stringstream strm;
  strm << "X, Y: [ " << std::left << std::setprecision(8) << x << ", " << std::left << std::setprecision(8) << y << " ]";
  QString text( strm.str().c_str() );
  mousePos->setText( text );
  mousePos->update();

}
