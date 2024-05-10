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

#include "ViewerGraphAreaBase.h"

static QColor qcolorBlack( "black" );
static QColor qcolorWhite( "white" );

ViewerGraphAreaBase::ViewerGraphAreaBase() {
}

ViewerGraphAreaBase::ViewerGraphAreaBase( int _id, QWidget *_parent ) {

  //std::cout << "ViewerGraphAreaBase created" << std::endl;

  id = _id;
  vlayout = nullptr;
  hlayout = nullptr;
  graph = nullptr;

  curSigIndex = 0;
  curFileName = "";

  //this->setMouseTracking( true );

}

ViewerGraphAreaBase::~ViewerGraphAreaBase() {

}

void ViewerGraphAreaBase::setCurInfo( QString _curFileName, int _curSigIndex ) {
}

void ViewerGraphAreaBase::getCurInfo( QString& _curFileName, int& _curSigIndex ) {
}

void ViewerGraphAreaBase::setInitialState( void ) {
  graph->views.clear();
  graph->prevViewAction->setEnabled( false );
}

void ViewerGraphAreaBase::clear ( void ) {
  if ( graph ) graph->clear();
}

void ViewerGraphAreaBase::enableFftButton( bool ena ) {
}

void ViewerGraphAreaBase::updateMousePosition( double x, double y ) {
}

void ViewerGraphAreaBase::updatePeakInfo( double x, double y ) {
}
