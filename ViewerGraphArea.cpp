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
  graph = QSharedPointer<ViewerGraphBase>( new ViewerGraph( id, ViewerGraph::DataType::TimeSeries, this ) );
  mousePos = QSharedPointer<QLabel>( new QLabel( "          " ) );
  exportSelection = QSharedPointer<QLabel>( new QLabel( " " ) );
  selectionXMin = QSharedPointer<QLabel>( new QLabel( " " ) );
  selectionXMax = QSharedPointer<QLabel>( new QLabel( " " ) );
  sigLabel = QSharedPointer<QLabel>( new QLabel( "Signal" ) );
  sigName = QSharedPointer<Combo>( new Combo( this ) );
  sigName->setEditable( true );
  sigName->clear();
  curSigIndex = 0;
  curFileName = "";
  calcFft = QSharedPointer<ViewerCtlrPushButton>( new ViewerCtlrPushButton(this, "FFT" ) );
  lockTimeScale = QSharedPointer<QPushButton>( new QPushButton( "Lock Time Scale" ) );
  lockTimeScale->setCheckable( true );
  lockTimeScale->setDown( false );
  lockTimeScaleState = false;
  enableFftButton( false );
  curSliderValue = 0;

  sigName->addItem( QString( "                    " ), -1 );

  w = 200;
  h = 100;
  this->setMinimumSize( w, h );
  this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  vlayout->addLayout( hlayout.data() );
  hlayout->addWidget( sigLabel.data() );
  hlayout->addWidget( sigName.data() );
  hlayout->addWidget( calcFft.data() );
  hlayout->addWidget( lockTimeScale.data() );
  
  hlayout->addSpacing( 100 );
  hlayout->addWidget( exportSelection.data() );
  hlayout->addSpacing( 20 );
  hlayout->addWidget( selectionXMin.data() );
  hlayout->addSpacing( 20 );
  hlayout->addWidget( selectionXMax.data() );
  hlayout->addStretch( 1 );
  hlayout->addWidget( mousePos.data() );
  
  vlayout->addLayout( hlayout2.data() );
  hlayout2->addWidget( slider.data() );
  
  vlayout->addWidget( graph.data() );
  this->setLayout( vlayout.data() );

  curSliderValue = 0;
  slider->setValue( 0 );

  connect( lockTimeScale.data(), SIGNAL( clicked( bool ) ),
           this, SLOT( performAction( bool ) ) );
    
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

  if ( graph ) {
    graph->views.clear();
    graph->prevViewAction->setEnabled( false );
  }
  curSliderValue = 0;
  slider->setValue( 0 );

}

void ViewerGraphArea::clear ( void ) {

  curSigIndex = 0;
  curFileName = "";
  if ( graph ) graph->clear();

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

void ViewerGraphArea::updateSelectionRange( double xMin, double xMax ) {

  selectionXMinValue = xMin;
  selectionXMaxValue = xMax;
  
  std::stringstream strm1, strm2;

  strm1 << "[ XMin: " << std::setw(9) << std::left << std::setprecision(8) << xMin;
  QString text( strm1.str().c_str() );
  text = text.simplified();
  selectionXMin->setText( text );
  selectionXMin->update();

  strm2 << " XMax: " << std::setw(9) << std::left << std::setprecision(8) << xMax << " ]";
  text = strm2.str().c_str();
  text = text.simplified();
  selectionXMax->setText( text );
  selectionXMax->update();

  text = "Export Selection";
  exportSelection->setText( text );
  exportSelection->update();

}

void ViewerGraphArea::performAction ( bool flag ) {

  QPushButton *b = qobject_cast<QPushButton *>(sender());

  lockTimeScaleState = !lockTimeScaleState;

  if ( lockTimeScaleState ) {
    b->setDown( true );
    emit enableLockTimeScale( true );
  }
  else {
    b->setDown( false );
    emit enableLockTimeScale( false );
  }
     
}
