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

#include "ViewerGraphAreaFft.h"

static QColor qcolorBlack( "black" );
static QColor qcolorWhite( "white" );

ViewerGraphAreaFft::ViewerGraphAreaFft( int _id, QWidget *_parent ) : ViewerGraphAreaBase( _id, _parent ) {

  vlayout = QSharedPointer<QVBoxLayout>( new QVBoxLayout() );
  hlayout = QSharedPointer<QHBoxLayout>( new QHBoxLayout() );
  mousePos = QSharedPointer<QLabel>( new QLabel( "" ) );
  peakInfo = QSharedPointer<QLabel>( new QLabel( "" ) );
  graph = QSharedPointer<ViewerGraphBase>( new ViewerGraphLogY( id, ViewerGraph::DataType::FFT, this ) );
  w = 200;
  h = 100;
  this->setMinimumSize( w, h );
  this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  vlayout->addLayout( hlayout.data() );
  hlayout->addStretch( 1 );
  hlayout->addWidget( mousePos.data() );
  //hlayout->addSpacing( 30 );
  hlayout->addWidget( peakInfo.data() );
  vlayout->addWidget( graph.data() );
  this->setLayout( vlayout.data() );

}

ViewerGraphAreaFft::~ViewerGraphAreaFft( ) {

}

void ViewerGraphAreaFft::clear ( void ) {
  if ( graph ) graph->clear();
}

void ViewerGraphAreaFft::updateMousePosition( double x, double y ) {

  std::stringstream strm;
  strm << "X, Y: [ " << std::left << std::setprecision(8) << x << ", " << std::left << std::setprecision(8) << y << " ]";
  QString text( strm.str().c_str() );
  mousePos->setText( text );
  mousePos->update();

}

void ViewerGraphAreaFft::updatePeakInfo( double x, double y ) {

  std::stringstream strm;
  strm << "Peak: [ " << std::left << std::setprecision(8) << x << ", " <<
    std::left << std::setprecision(8) << y << " ]";
  QString text( strm.str().c_str() );
  peakInfo->setText( text );
  peakInfo->update();

}
